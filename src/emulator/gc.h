/**********************************************************************
 *     Copyright (c) by Barak Pearlmutter and Kevin Lang, 1987-99.    *
 *     Copyright (c) by Alex Stuebinger, 1998-99.                     *
 *     Distributed under the GNU General Public License v2 or later   *
 **********************************************************************/


#ifndef _GC_H_INCLUDED
#define _GC_H_INCLUDED

#include "config.h"
#include "data.h"

extern bool full_gc;
extern void printref(FILE * fd, ref_t refin);

extern void gc(bool pre_dump, bool full_gc, char *reason,
	       size_t amount);

#define GC_MEMORY(v) \
{*gc_examine_ptr++ = (v);}
		/*
		   assert(gc_examine_ptr < &gc_examine_buffer[GC_EXAMINE_BUFFER_SIZE]);\
		   } */

#define GC_RECALL(v) \
{(v) = *--gc_examine_ptr;}
		/*
		   assert(gc_examine_ptr >= gc_examine_buffer);\
		   } */

#ifdef THREADS
extern int gc_ready[];
extern bool gc_pending;
extern pthread_mutex_t gc_lock;
#endif

extern void set_gc_flag (bool flag);
extern int get_next_index();
extern void free_registers();
extern void wait_for_gc();

#endif
