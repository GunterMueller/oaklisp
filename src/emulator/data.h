/**********************************************************************
 *     Copyright (c) by Barak Pearlmutter and Kevin Lang, 1987-99.    *
 *     Copyright (c) by Alex Stuebinger, 1998-99.                     *
 *     Distributed under the GNU General Public License v2 or later   *
 **********************************************************************/


#ifndef _DATA_H_INCLUDED
#define _DATA_H_INCLUDED

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"

/* Version and greeting */
extern const char *version, *compilation_date, *compilation_time;


#ifndef bool
typedef int bool;
#endif

#ifndef false
#define false	0
#define true	1
#endif


/* byte gender */

#define little_endian 0
#define big_endian    1

extern int byte_gender;

/* reference type */

typedef u_int32_t ref_t;

/* space type */

typedef struct
  {
    ref_t *start;
    ref_t *end;
    size_t size;		/* in size reference_t */
#ifdef UNALIGNED_MALLOC
    size_t displacement;
#endif
  }
space_t;


extern space_t new_space, old_space, spatic;
extern ref_t *free_point;

/* Size of first newspace, in K */
#define DEFAULT_NEWSPACE 128

/* The following is for stack debugging */
#define PATTERN 0x0a0b0c0d


#define CONTEXT_FRAME_SIZE 3	/* not a tunable parameter */


/* Garbage collection */

#define GC_EXAMINE_BUFFER_SIZE 16
extern ref_t gc_examine_buffer[GC_EXAMINE_BUFFER_SIZE];
extern ref_t *gc_examine_ptr;

/* Virtual Machine registers */

extern ref_t
* e_bp, *e_env, e_t, e_nil, e_fixnum_type, e_loc_type, e_cons_type, e_env_type,
 *e_subtype_table, e_object_type, e_segment_type, e_boot_code, e_code_segment,
 *e_arged_tag_trap_table, *e_argless_tag_trap_table, e_current_method,
  e_uninitialized, e_method_type, e_operation_type;

#define e_false e_nil

extern size_t e_next_newspace_size, original_newspace_size;

extern u_int16_t *e_pc;

extern unsigned e_nargs;

extern char *world_file_name;
extern char *dump_file_name;
extern int dump_base;

extern bool dump_flag;
extern bool gc_before_dump;

extern int trace_gc;
extern bool trace_traps;

#ifndef FAST

extern bool trace_insts;
extern bool trace_segs;
extern bool trace_valcon;
extern bool trace_cxtcon;
extern bool trace_stks;
extern bool trace_meth;

#ifdef OP_TYPE_METH_CACHE
extern bool trace_mcache;
#endif

#endif

extern bool trace_files;


/* miscellanous */

#ifndef ISATTY
#define ISATTY(stream) (isatty(fileno(stream)))
#endif


#define READ_MODE "r"
#define WRITE_MODE "w"
#define APPEND_MODE "a"
#define READ_BINARY_MODE "rb"
#define WRITE_BINARY_MODE "wb"



/* Tag Scheme */

#define SIGN_16BIT_ARG(x)	((int16_t)(x))

#define TAGSIZE 2

#define TAG_MASK	3
#define TAG_MASKL	3l
#define SUBTAG_MASK	0xff
#define SUBTAG_MASKL	0xffl

#define INT_TAG		0
#define IMM_TAG		1
#define LOC_TAG 	2
#define PTR_TAG		3

#define PTR_MASK	2

#define CHAR_SUBTAG	 IMM_TAG

#define TAG_IS(X,TAG)		(((X)&TAG_MASK)==(TAG))
#define SUBTAG_IS(X,SUBTAG)	(((X)&SUBTAG_MASK)==(SUBTAG))

/* #define OR_TAG */

#define REF_TO_INT(r)   ((int32_t)r>>TAGSIZE)


#define REF_TO_PTR(r)	((ref_t*)((r)-PTR_TAG))
/*
   #define REF_TO_PTR(r)   ((ref_t*)((r)&~3ul))
 */
/* This maybe used in slot calculations, where tag corrections
   can be done by the address calculation unit */
#define REF_TO_PTR_ADDR(r) ((ref_t*)((r) - PTR_TAG))


#define LOC_TO_PTR(r)	((ref_t*)((r) - LOC_TAG))
#define ANY_TO_PTR(r)	((ref_t*)((r) & ~TAG_MASKL))

#ifndef OR_TAG
#define PTR_TO_LOC(p)	((ref_t)((ref_t)(p) + LOC_TAG))
#define PTR_TO_REF(p)	((ref_t)((ref_t)(p) + PTR_TAG))
#else
#define PTR_TO_LOC(p)   ((ref_t)((ref_t)(p) | LOC_TAG))
#define PTR_TO_REF(p)   ((ref_t)((ref_t)(p) | PTR_TAG))
#endif

/* Put q's tag onto p */
#define PTR_TO_TAGGED(p,q) ((ref_t)((ref_t)(p) + ((q) & TAG_MASK)))

#define REF_TO_CHAR(r)	((char)((r)>>8))
#ifndef OR_TAG
#define CHAR_TO_REF(c)	(((ref_t)(c)<<8) + CHAR_SUBTAG)
#else
#define CHAR_TO_REF(c)  (((ref_t)(c)<<8) | IMM_TAG)
#endif

#ifndef OR_TAG
#define INT_TO_REF(i)	((ref_t)(((int32_t)(i)<<TAGSIZE) + INT_TAG))
#else
#define INT_TO_REF(i)   ((ref_t)(((int32_t)(i)<<TAGSIZE) | INT_TAG))
#endif

#define BOOL_TO_REF(x)   ( (x) ? e_t : e_false )

/* MIN_REF is the most negative fixnum.  There is no corresponding
   positive fixnum, an asymmetry inherent in a twos complement
   representation. */

#define MIN_REF     ((ref_t)(1<<(WORDSIZE-1)))
#define MAX_REF ((ref_t)-((int32_t)MIN_REF+1))

/* Check if high three bits are equal. */

/*
#define OVERFLOWN_INT(i,code)					\
{ register int highcrap						\
	= ((u_int32_t)(i)) >> (WORDSIZE-(TAGSIZE+1));		\
if ((highcrap != 0x0) && (highcrap != 0x7)) {code;} }
*/

/* The following is for 32-bit ref_t only */

#define OVERFLOWN_INT(i,code)					\
{ u_int32_t highcrap = (i) & 0xe0000000;			\
if ((highcrap) && (highcrap != 0xe0000000)) {code;}}


/*
 * Offsets for wired types.  Offset includes type and
 * optional length fields when present.
 */

/* CONS-PAIR: */
#define CONS_PAIR_CAR_OFF	1
#define CONS_PAIR_CDR_OFF	2

/* TYPE: */
#define TYPE_LEN_OFF		1
#define TYPE_VAR_LEN_P_OFF	2
#define TYPE_SUPER_LIST_OFF	3
#define TYPE_IVAR_LIST_OFF	4
#define TYPE_IVAR_COUNT_OFF	5
#define TYPE_TYPE_BP_ALIST_OFF	6
#define TYPE_OP_METHOD_ALIST_OFF 7
#define TYPE_WIRED_P_OFF	8

/* METHOD: */
#define METHOD_CODE_OFF		1
#define METHOD_ENV_OFF		2

/* CODE-VECTOR: */
#define CODE_IVAR_MAP_OFF	2
#define CODE_CODE_START_OFF	3

/* OPERATION: */
#define OPERATION_LAMBDA_OFF		1
#define OPERATION_CACHE_TYPE_OFF	2
#define OPERATION_CACHE_METH_OFF	3
#define OPERATION_CACHE_TYPE_OFF_OFF	4
#define OPERATION_LENGTH		5

/* ESCAPE-OBJECT */
#define ESCAPE_OBJECT_VAL_OFF	1
#define ESCAPE_OBJECT_CXT_OFF	2

/* Continuation Objects */
#define CONTINUATION_VAL_SEGS	1
#define CONTINUATION_VAL_OFF	2
#define CONTINUATION_CXT_SEGS	3
#define CONTINUATION_CXT_OFF	4

#define SPACE_PTR(s,p)	((s).start<=(p) && (p)<(s).end)
#define NEW_PTR(r)      SPACE_PTR(new_space,(r))
#define SPATIC_PTR(r)	SPACE_PTR(spatic,(r))
#define OLD_PTR(r) (SPACE_PTR(old_space,(r))||(full_gc&&SPACE_PTR(spatic,(r))))




/* Leaving r unsigned lets us checks for negative and too big in one shot: */
#define wp_to_ref(r)					\
  ( (u_int32_t)REF_TO_INT(r) >= (u_int32_t) wp_index ?	\
   e_nil : wp_table[1+(u_int32_t)REF_TO_INT((r))] )


/* This is used to allocate some storage.  It calls gc when necessary. */

#define ALLOCATE(p, words, reason)			\
  ALLOCATE_PROT(p, words, reason,; ,; )

/* This is used to allocate some storage */

#define ALLOCATE_SS(p, words, reason)			\
  ALLOCATE_PROT(p, words, reason,			\
		{ value_stack.sp = local_value_sp;	\
          context_stack.sp = local_context_sp;		\
		  e_pc = local_epc; },			\
		{ local_epc = e_pc;			\
          local_context_sp = context_stack.sp;		\
		  local_value_sp = value_stack.sp; })


/* This allocates some storage, assuming that v must be protected from gc. */

#define ALLOCATE1(p, words, reason, v)			\
  ALLOCATE_PROT(p, words, reason,			\
		{ GC_MEMORY(v);				\
		  value_stack.sp = local_value_sp;	\
          context_stack.sp = local_context_sp;		\
		  e_pc = local_epc; },			\
		{ local_epc = e_pc;			\
          local_context_sp = context_stack.sp;		\
		  local_value_sp = value_stack.sp;	\
		  GC_RECALL(v); })



#define ALLOCATE_PROT(p, words, reason, before, after)	\
{							\
  if (free_point + (words) >= new_space.end)            \
    {							\
      before;						\
      gc(false, false, (reason), (words));		\
      after;						\
    }							\
  (p) = free_point;					\
  free_point += (words);				\
}


/* These get slots out of Oaklisp objects, and may be used as lvalues. */

#define SLOT(p,s)	(*((p)+(s)))
#define REF_SLOT(r,s)	SLOT(REF_TO_PTR(r),s)


/* This is for the warmup code */

#define CODE_SEG_FIRST_INSTR(seg) \
  ( (u_int16_t *)(REF_TO_PTR((seg)) + CODE_CODE_START_OFF) )

#endif
