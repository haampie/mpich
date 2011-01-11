/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#if !defined(MPICH_MPIDPOST_H_INCLUDED)
#define MPICH_MPIDPOST_H_INCLUDED

/* FIXME: mpidpost.h is included by mpiimpl.h .  However, mpiimpl.h should 
   refer only to the ADI3 prototypes and should never include prototypes 
   specific to any particular device.  Factor the include files to maintain
   better modularity by providing mpiimpl.h with only the definitions that it
   needs */
/*
 * Channel API prototypes
 */

/*@
  MPIDI_CH3_Request_destroy - Release resources in use by an existing request 
  object.

  Input Parameters:
. req - pointer to the request object

  IMPLEMENTORS:
  MPIDI_CH3_Request_destroy() must call MPIDI_CH3U_Request_destroy() before 
  request object is freed.
@*/
void MPIDI_CH3_Request_destroy(MPID_Request * req);


/*@
  MPIDI_CH3_Progress_start - Mark the beginning of a progress epoch.

  Input Parameters:
. state - pointer to a MPID_Progress_state object

  Return value:
  An MPI error code.
  
  NOTE:
  This routine need only be called if the code might call 
  MPIDI_CH3_Progress_wait().  It is normally used as follows example:
.vb
      if (*req->cc_ptr != 0)
      {
          MPID_Progress_state state;
          
          MPIDI_CH3_Progress_start(&state);
          {
              while(*req->cc_ptr != 0)
              {
                  MPIDI_CH3_Progress_wait(&state);
              }
          }
          MPIDI_CH3_Progress_end(&state);
      }
.ve

  IMPLEMENTORS:
  A multi-threaded implementation might save the current value of a request 
  completion counter in the state.
@*/
void MPIDI_CH3_Progress_start(MPID_Progress_state * state);


/*@
  MPIDI_CH3_Progress_wait - Give the channel implementation an opportunity to 
  make progress on outstanding communication requests.

  Input Parameters:
. state - pointer to the same MPID_Progress_state object passed to 
  MPIDI_CH3_Progress_start

  Return value:
  An MPI error code.
  
  NOTE:
  MPIDI_CH3_Progress_start/end() need to be called.
  
  IMPLEMENTORS:
  A multi-threaded implementation would return immediately if the a request 
  had been completed between the call to
  MPIDI_CH3_Progress_start() and MPIDI_CH3_Progress_wait().  This could be 
  implemented by checking a request completion counter
  in the progress state against a global counter, and returning if they did 
  not match.
@*/
int MPIDI_CH3_Progress_wait(MPID_Progress_state * state);


/*@
  MPIDI_CH3_Progress_end - Mark the end of a progress epoch.
  
  Input Parameters:
. state - pointer to the same MPID_Progress_state object passed to 
  MPIDI_CH3_Progress_start

  Return value:
  An MPI error code.
@*/
void MPIDI_CH3_Progress_end(MPID_Progress_state * state);


/*@
  MPIDI_CH3_Progress_test - Give the channel implementation an opportunity to 
  make progress on outstanding communication requests.

  Return value:
  An MPI error code.
  
  NOTE:
  This function implicitly marks the beginning and end of a progress epoch.
@*/
int MPIDI_CH3_Progress_test(void);


/*@
  MPIDI_CH3_Progress_poke - Give the channel implementation a moment of 
  opportunity to make progress on outstanding communication.

  Return value:
  An mpi error code.
  
  IMPLEMENTORS:
  This routine is similar to MPIDI_CH3_Progress_test but may not be as 
  thorough in its attempt to satisfy all outstanding
  communication.
@*/
int MPIDI_CH3_Progress_poke(void);



int MPIDI_CH3_Open_port(char *port_name);

int MPIDI_CH3_Comm_spawn_multiple(int count, char ** commands, char *** argvs,
				  int * maxprocs, MPID_Info ** info_ptrs, 
				  int root,
                                  MPID_Comm * comm_ptr, MPID_Comm ** intercomm,
				  int * errcodes);

int MPIDI_CH3_Comm_accept(char * port_name, int root, MPID_Comm * comm_ptr, 
			  MPID_Comm ** newcomm); 

int MPIDI_CH3_Comm_connect(char * port_name, int root, MPID_Comm * comm_ptr, 
			   MPID_Comm ** newcomm);


/*@
  MPIDI_CH3U_Request_destroy - Free resources associated with the channel 
  device (ch3) component of a request.

  Input Parameters:
. req - pointer to the request object

  IMPLEMENTORS:
  This routine must be called by MPIDI_CH3_Request_destroy().
@*/
void MPIDI_CH3U_Request_destroy(MPID_Request * req);

/* Include definitions from the channel which require items defined by this 
   file (mpidimpl.h) or the file it includes
   (mpiimpl.h). */
#include "mpidi_ch3_post.h"

#include "mpid_datatype.h"

/*
 * Request utility macros (public - can be used in MPID macros)
 */

/* SHMEM: In the case of a single-threaded shmem channel sharing requests 
   between processes, a write barrier must be performed
   before decrementing the completion counter.  This ensures that other fields
   in the req structure are updated before the
   completion is signalled.  How should that be incorporated into this code 
   from the channel level? */
/* The above comment is accurate, although we do not currently have any channels
 * that do this.  Memory barriers are included in fine-grained multithreaded
 * versions of the MPID_cc_incr/decr macros. */
#define MPIDI_CH3U_Request_decrement_cc(req_, incomplete_)   \
    MPID_cc_decr((req_)->cc_ptr, incomplete_)
#define MPIDI_CH3U_Request_increment_cc(req_, was_incomplete_)   \
    MPID_cc_incr((req_)->cc_ptr, was_incomplete_)

/*
 * Device level request management macros
 */

/* We only export release and set completed on requests, since 
 * other uses (such as incrementing the ref count) are done solely 
 * by the device */

#define MPID_Request_release(req_)			\
{							\
    int inuse;					        \
							\
    MPIR_Request_release_ref((req_), &inuse);	        \
    if (inuse == 0)					\
    {							\
	MPIDI_CH3_Request_destroy(req_);		\
    }							\
}

/* MT note: The following order of operations is _essential_ for correct
 * operation of the fine-grained multithreading code.  Assume that
 * _signal_completion() acquires and releases a mutex in order to update the
 * global completion counter (it does for fine-grained ch3:nemesis).  Further,
 * assume the following standard pattern is used by the request consumer to wait
 * for completion:
 *
 *   if (req is not complete (req->cc!=0)) {
 *     // progress_enter:
 *     acquire mutex;
 *     my_count = global_count;
 *     release mutex;
 *
 *     while (req is not complete (req->cc!=0)) {
 *       progress_wait(&my_count);
 *     }
 *   }
 *
 * Where progress_wait will attempt to make progress forever as long as
 * (my_count==global_count).  If it is possible for the consumer to see the
 * global completion count before seeing the request's completion counter drop
 * to zero, the consumer could spin in progress_wait forever without a chance to
 * retest the request.
 *
 * If the mutex approach is dropped in favor of atomic access, additional memory
 * barriers must be inserted.  The mutex acquire/release currently enforces
 * sufficient ordering constraints provided the statement order below is not
 * accidentally inverted.
 *
 * See also the note above the MSGQUEUE CS macros and request completion in
 * mpiimplthread.h.
 */
/* MPID_Request_set_completed (the function) is defined in ch3u_request.c */
#define MPID_REQUEST_SET_COMPLETED(req_)	\
{						\
    MPID_cc_set((req_)->cc_ptr, 0);             \
    /* MT do not reorder! see note above*/      \
    MPIDI_CH3_Progress_signal_completion();	\
}


/*
 * Device level progress engine macros
 */
#define MPID_Progress_start(progress_state_) MPIDI_CH3_Progress_start(progress_state_)
#define MPID_Progress_wait(progress_state_)  MPIDI_CH3_Progress_wait(progress_state_)
#define MPID_Progress_end(progress_state_)   MPIDI_CH3_Progress_end(progress_state_)
#define MPID_Progress_test()                 MPIDI_CH3_Progress_test()
#define MPID_Progress_poke()		     MPIDI_CH3_Progress_poke()

/* Dynamic process support */
int MPID_GPID_GetAllInComm( MPID_Comm *comm_ptr, int local_size, 
			    int local_gpids[], int *singlePG );
int MPID_GPID_Get( MPID_Comm *comm_ptr, int rank, int gpid[] );
int MPID_GPID_ToLpidArray( int size, int gpid[], int lpid[] );
int MPID_VCR_CommFromLpids( MPID_Comm *newcomm_ptr, 
			    int size, const int lpids[] );
int MPID_PG_ForwardPGInfo( MPID_Comm *peer_ptr, MPID_Comm *comm_ptr, 
			   int nPGids, const int gpids[], 
			   int root );
/* PG_ForwardPGInfo is used as the implementation of the intercomm-create
   hook that is needed with dynamic processes because of limitations
   in the current definition of PMI */
#define MPID_ICCREATE_REMOTECOMM_HOOK(_p,_c,_np,_gp,_r) \
     MPID_PG_ForwardPGInfo(_p,_c,_np,_gp,_r)

#endif /* !defined(MPICH_MPIDPOST_H_INCLUDED) */
