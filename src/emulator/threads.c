#include "threads.h"
#include "xmalloc.h"
#include "stacks.h"
#include "loop.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#ifdef THREADS
int next_index = 0;
pthread_key_t index_key;
pthread_mutex_t gc_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t alloc_lock = PTHREAD_MUTEX_INITIALIZER;
bool gc_pending = false;
int gc_ready[MAX_THREAD_COUNT];
register_set_t* register_array[MAX_THREAD_COUNT];
stack_t *value_stack_array[MAX_THREAD_COUNT];
stack_t *cntxt_stack_array[MAX_THREAD_COUNT];
#endif

static u_int16_t tail_recurse_instruction = (22 << 2);

typedef struct {
    ref_t start_operation;
    int parent_index;
} start_info_t;

static void *init_thread(void *info_p);

int create_thread(ref_t start_operation)
{
#ifdef THREADS
    pthread_t new_thread;
    start_info_t *info_p = (start_info_t *)malloc(sizeof(start_info_t));
    info_p->start_operation = start_operation;
    info_p->parent_index = *((int *)pthread_getspecific(index_key));
    pthread_create(&new_thread, NULL,
		   (void *)init_thread, (void *)info_p);
    return 1;
#else
    return 0;
#endif
}

static void *init_thread (void *info_p)
{
#ifdef THREADS
   int my_index;
   int *my_index_p;
   start_info_t info;
   my_index_p = (int *)xmalloc (sizeof (int));
   /*Retrieve the next index in the thread arrays and lock it so
     another starting thread cannot get the same index*/
   *my_index_p = lock_next_index();
   my_index = *my_index_p;
   pthread_setspecific(index_key, (void *)my_index_p);  
   gc_ready[my_index] = 0;
   /*Increment also releases the gc lock on next_index so another
     starting thread can get the lock, or a thread that is gc'ing can
     get the lock*/
   inc_next_index();

   /* Shouldn't get interrupted for gc until after stacks are
      created.  This is below here in the vm not checking intterupts
      until after we get to the loop */
   info = *((start_info_t *)info_p);
   free(info_p);
   value_stack_array[my_index] = (stack_t*)xmalloc (sizeof (stack_t));
   cntxt_stack_array[my_index] = (stack_t*)xmalloc(sizeof (stack_t));

   value_stack_array[my_index]->size = value_stack_array[0]->size;
   value_stack_array[my_index]->filltarget = value_stack_array[0]->filltarget;
   cntxt_stack_array[my_index]->size = cntxt_stack_array[0]->size;
   cntxt_stack_array[my_index]->filltarget = cntxt_stack_array[0]->filltarget;
 
   init_stacks ();
   register_array[my_index] = (register_set_t*)xmalloc(sizeof (register_set_t));

 /*
   e_current_method = (ref_t)start_method;
   e_env = REF_TO_PTR (REF_SLOT (e_current_method, METHOD_ENV_OFF));
   e_code_segment = REF_SLOT (e_current_method, METHOD_CODE_OFF);
   e_pc = CODE_SEG_FIRST_INSTR (e_code_segment);
   e_bp = e_env;
   e_nargs = 0;
 */
   memcpy(register_array[my_index], register_array[info.parent_index],
	  sizeof(register_set_t));
   e_pc = &tail_recurse_instruction;
   *++value_stack.sp = info.start_operation;
   e_nargs = 0;

   /* Big virtual machine interpreter loop */
   loop();

#endif
   return 0;
}

void set_gc_flag (bool flag) 
{
#ifdef THREADS
  if (flag == true) {
    pthread_mutex_lock (&gc_lock);
    gc_pending = flag;
  }
  else {
    gc_pending = flag;
    pthread_mutex_unlock (&gc_lock);
  }
#endif
}

/*Increment uses the gc lock
  since we must be sure that a new thread does not
  get started and begin processing while the gc is
  already running.  The get_next_index additionally
  ensures that no two threads get the same index when
  starting*/
void inc_next_index ()
{
#ifdef THREADS
  next_index++;
  pthread_mutex_unlock (&gc_lock);
#endif
}

int lock_next_index ()
{
  int ret = -1;
#ifdef THREADS
  pthread_mutex_lock (&gc_lock);
  ret = next_index;
#endif
  return (ret);
}

void free_registers ()
{
}
