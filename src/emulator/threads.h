#ifndef _THREADS_H_INCLUDED
#define _THREADS_H_INCLUDED

#include <pthread.h>
#include "config.h"
#include "data.h"

#ifdef THREADS
extern int next_index;
extern pthread_key_t index_key;
extern pthread_mutex_t gc_lock;
extern pthread_mutex_t alloc_lock;
extern int gc_ready[];
extern bool gc_pending;
extern register_set_t* register_array[];
extern stack_t *value_stack_array[];
extern stack_t *cntxt_stack_array[];
#endif

int create_thread(ref_t start_method);
void set_gc_flag (bool flag);
void inc_next_index();

#endif /*_THREADS_H_INCLUDED*/
