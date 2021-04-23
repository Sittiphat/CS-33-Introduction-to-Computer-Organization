/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "fTeam",
    /* First member's full name */
    "Sittiphat Narkmanee",
    /* First member's email address */
    "sittiphat@g.ucla.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};


// Seg Free, Header Blocks and Footer for free, and only header for allocated
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
/*

// Most Macros and Global Constants were taken under inspiration of
// Computer Systems: A Programmer's Perspective by Randal E. Bryant (pp. 875-900)
/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ //line:vm:mm:beginconst
#define DSIZE       8       /* Double word size (bytes) */
#define ZSIZE       0       /* epilogue block, which is a zero-size allocated block */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */  //line:vm:mm:endconst

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc)) //line:vm:mm:pack

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            //line:vm:mm:get
#define PUT(p, val)  (*(unsigned int *)(p) = (val))    //line:vm:mm:put

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)                   //line:vm:mm:getsize
#define GET_ALLOC(p) (GET(p) & 0x1)                    //line:vm:mm:getalloc

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)                      //line:vm:mm:hdrp
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //line:vm:mm:ftrp

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //line:vm:mm:nextblkp
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //line:vm:mm:prevblkp
/* $end mallocmacros */

// Global Variables
// The allocator uses a global static variable heap_list_ptr that
// always points to the prologue block. TODO: For optimization point to next block
// instead of pointer
static char* heap_list_ptr = 0;  // Pointer to prologue block

// Next Fit Pointer Implementation to save previous spot
// this static char variable
static char* saved_ptr;

// This entire helper function to help coalese free blocks
// under four cases was taken from the textbook's source code example
// on (pp. 896)
static void* coalesce(void* bp) {
  // In this case the size of the previous and next allocation blocks
  // are initialized through getting the address of the next/previous block, through
  // the current free block's footer to previous that can help point to the previous block
  // and the current header to point to the next block
  // GET_ALLOC will use bitwise to get the LSB which encodes if that block is alocated or not
  // and save it to our local variables
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  // Saving the size of the current free block
  size_t size = GET_SIZE(HDRP(bp));

  // In case 1, if both previous and next block LSB are allocated with 1 encoding,
  // then we do nothing and return the current base pointer
  if (prev_alloc && next_alloc) {
    return bp;
  }

  // In case 2, if the previous block is allocated and the next block is free,
  // we add the size of our current free block to the size of the next block
  // Then we accordingly initialize our new header and footers of the coalesced free block
  else if (prev_alloc && !next_alloc) {
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size,0));
  }
  // In case 3, if the previous block is free  and the next block is allocated,
  // we add the size of our current free block to the size of the previous block
  // Then we accordingly initialize our new header and footers of the coalesced free block
  // and our base pointer will be at the previous block's top location
  else if (!prev_alloc && next_alloc) {
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }
  // In case 4, if the previous block is free  and the next block is also free,
  // we add the size of our current free block to the size of the next and previous block
  // Then we accordingly initialize our new header and footers of the coalesced free block
  // and our base pointer will be at the previous block's top location
  else {
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }

  // Next Fit Implementation has a check if our static next fit variable
  // is larger that the current base pointer and less than the next block's base
  // pointer our search is saved here and our next search will start there
  // instead of the beginning
  if ((saved_ptr > (char*)bp) && (saved_ptr < NEXT_BLKP(bp)))
    saved_ptr = bp;

  // Return location of newly/unchanged base pointer location
  return bp;
}

// Helper Function to extend heap by given size is taken under inspiration from
// the psudocode on (pp. 893, 895) and some of its source code examples on (pp. 894)
static void* extend_heap(size_t words) {
    char* bp;
    size_t size;

    // To maintain 2-word alignment, extend_heap rounds up the requested size to the nearest
    // 8 byte multiple to then request that space in (size).
    if (words % 2) { // Test if number of words is even for alignment
      size = (words + 1) * WSIZE;
    } else {
      size = WSIZE * words;
    }

    // We now call mem_brk() to allocate aligned size
    // where if there is an error it returns -1 and we later return null
    bp = mem_sbrk(size);

    // Casting to create integer type;
    if ((long)bp == -1) {
      return NULL;
    }

    // Taken from the textbook's MACROS (pp. 894), given our new base pointer after valid 2-word
    // allignment we create our new free header and footer blocks
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    // Move epilogue header after new free header and footers
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    // If the previous block is free then we coalesce using the coalese helper function
    return coalesce(bp);
}

 /* mm_init - initialize the malloc package.*/
int mm_init(void) {

  // sbrk() returns the previous program break when successful. If the break was increased,
  // then (heap_list_ptr) is now pointing to start of the newly allocated memory.
  heap_list_ptr = mem_sbrk(4 * WSIZE);

  // If erroroneous, sbrk() will return (void*)-1 and then mm_init() will return -1.
  if (heap_list_ptr == (void*)-1) {
    return -1;
  }

  // Initialization of padding, prologue, and epilogue header/footers taken
  // under inspiration from (pp. 891 and 895)
  PUT(heap_list_ptr, 0); // The first word is an unused special padding word aligned to a double-word boundary.

  // A special prologue block, which is an 8-byte allocated
  // block consisting of only a header and a footer.
  PUT(heap_list_ptr + (1 * WSIZE), PACK(DSIZE, 1)); // Prologue header
  PUT(heap_list_ptr + (2 * WSIZE), PACK(DSIZE, 1)); // Prologue footer

  // The heap always ends with a special epilogue block,
  // which is a zero-size allocated block
  PUT(heap_list_ptr + (3 * WSIZE), PACK(0, 1)); // Epilogue header

  // As a minor optimization, we make (heap_list_ptr) point to the next
  // block instead of the prologue block.
  heap_list_ptr += (2 * WSIZE);

  saved_ptr = heap_list_ptr; // Next Fit saved_ptring pointer of last used is initially assigned


  // Extend Heap when the heap is initialized and when mm_malloc() is unable
  // to find a suitable fit. In this case this is after initialization where we
  // extend the heap using extend_heap() of CHUNKSIZE bytes.
  if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
    return -1;
  }

  return 0; // After initilization is successful, we return 0
}

// Next Fit search algorithm influenced on (pp. 885), which generally is faster
// but has lower utilization than first fit
static void* next_fit(size_t asize) {
    // Save the current static most previous location ptr
    char* curr_saved_ptr = saved_ptr;

    // Starting from our saved pointed position, search until the end of the free list
    while (GET_SIZE(HDRP(saved_ptr)) > 0) {
      if (!GET_ALLOC(HDRP(saved_ptr)) && (asize <= GET_SIZE(HDRP(saved_ptr)))) {
        return saved_ptr;
      }
      saved_ptr = NEXT_BLKP(saved_ptr);
    }

    // If nothing is found the first time, we implement first_fit
    saved_ptr = heap_list_ptr;
    while (saved_ptr < curr_saved_ptr) {
      if (!GET_ALLOC(HDRP(saved_ptr)) && (asize <= GET_SIZE(HDRP(saved_ptr)))) {
        return saved_ptr;
      }
      saved_ptr = NEXT_BLKP(saved_ptr);
    }

    // If there is no fit, then return null (external fragmentation)
    return NULL;
}

// Place the requested block at the beginning of the free
// block, splitting only if the size of the remainder would equal or exceed the minimum
// block size. This algorithm was taken under influence of the author's reccomendations
// on pp. 897 and his source code on his official website.
static void place(void* bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2 * DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */

// mm_malloc() allocates a block as large its size parameter and accordingly increments the brk pointer.
// Most of this code was taken under inspiration from pseudocode on (pp. 895)
// and source code supplied on (pp. 896)
void *mm_malloc(size_t size) {
  size_t asize; // Actual block size after alignment and header/footer
  size_t extendsize; // If there is not enough room on heap this is the size we must extend
  char* bp;

  // If the heap list pointer is uninitialize, initialize its blocks
  if (heap_list_ptr == 0) {
      mm_init();
  }

  // Cannot allocate 0 size
  if (size == 0) {
    return NULL;
  }

  if (size <= DSIZE) { // Adding 16 bytes for alignment + overhard of header/footer
    asize = 2 * DSIZE;
  }
  else { // If over 8 bytes we again add overhead but round to nearest multiple of 8
    asize = DSIZE * ((size + DSIZE + (DSIZE-1)) / DSIZE);
  }

  // If fit is found in free list we return the base pointer of the top
  // of our newly allocated block as per specs
  bp = next_fit(asize);
  if (bp != NULL) {
      place(bp, asize);
      return bp;
  } else { // If no fit found, (external frag.) and must extend heap to get more memory
    extendsize = MAX(asize,CHUNKSIZE); // Returns greater size to extend heap by per WSIZE
    bp = extend_heap(extendsize/WSIZE); // New location of bp after extension of heap

    if (bp == NULL) { // If there is not enough room we return null
      return NULL;
    }

    place(bp, asize);
    return bp;
  }
}

// mm_free() frees a block and uses boundary-tags through to merge it
// with any adjacent free blocks by calling coalesce()
// Macros and majority of algorithm taken under inspiration from pseudocode
// and source code in textbook on (pp. 895-895)
void mm_free(void* bp) {

  if (bp == 0) {
    return; // We cannot free a base pointer that is non-existent
  }

  // If we have not yet initialized the heap, then we call mm_init to initiliaze
  if (heap_list_ptr == 0){
      mm_init();
  }

  // We reassign our newly free header and footers by using MACROS from the textbook
  // that to give them new values of the no longer allocated (LSB is 0) and its size
  // To get the the size of our allocated block by seeing its calling a MACRO that reads
  // the header size located at indicated base pointer
  PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 0));
  PUT(FTRP(bp), PACK(GET_SIZE(HDRP(bp)), 0));

  coalesce(bp); // We then coalesce if needed using 4 cases mentioned on (pp. 889)
}

// mm_realloc() gets the block passed in by the ptr parameter and returns a new block
// that is realocatted based on this function's passed in size
void *mm_realloc(void* ptr, size_t size) {
  void* old_ptr = ptr;
  size_t old_size;
  void* new_ptr;
  size_t new_size;

  // If the size is 0, we just free at the location of the passed in pointer
  // then we return null as said in specs
  if (size == 0) {
    mm_free(old_ptr);
    return NULL;
  }

  // If the passed in pointer is not pointing to anything
  // then we just call malloc at indicated passed through size as said in specs
  if (old_ptr == NULL) {
    return mm_malloc(size);
  }

  // We create a new allocated block and indicate its location with (new_ptr)
  new_ptr = mm_malloc(size);

  // If allocation fails, we return null
  if (new_ptr == NULL) {
    return NULL;
  }

  // We get the size of the old block by using a MACRO supplied on (pp. 892)
  // so that we can later compare with the passed in (new_size)
  old_size = GET_SIZE(HDRP(old_ptr));

  // If the old size is greater or equal to the new size,
  if (old_size >= size) {
    // (new_ptr) is the dest and (old_ptr) is the source while the third parameter is the size
    memcpy(new_ptr, old_ptr, size);
  } else {
    // (new_ptr) is the dest and (old_ptr) is the source while the third parameter is the size
    memcpy(new_ptr, old_ptr, old_size);
  }

  // Now we free the old block
  mm_free(ptr);

  // Return the new pointer that points to our newly reallocated block
  return new_ptr;
}
