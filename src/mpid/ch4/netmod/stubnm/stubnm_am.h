/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2006 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 *  Portions of this code were written by Intel Corporation.
 *  Copyright (C) 2011-2016 Intel Corporation.  Intel provides this material
 *  to Argonne National Laboratory subject to Software Grant and Corporate
 *  Contributor License Agreement dated February 8, 2012.
 */
#ifndef NETMOD_STUBNM_AM_H_INCLUDED
#define NETMOD_STUBNM_AM_H_INCLUDED

#include "stubnm_impl.h"

static inline int MPIDI_NM_am_reg_handler(int handler_id,
                                          MPIDI_NM_am_origin_handler_fn origin_handler_fn,
                                          MPIDI_NM_am_target_handler_fn target_handler_fn)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_send_hdr(int rank,
                                       MPIR_Comm * comm,
                                       int handler_id,
                                       const void *am_hdr,
                                       size_t am_hdr_sz, MPIR_Request * sreq, void *netmod_context)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_send(int rank,
                                   MPIR_Comm * comm,
                                   int handler_id,
                                   const void *am_hdr,
                                   size_t am_hdr_sz,
                                   const void *data,
                                   MPI_Count count,
                                   MPI_Datatype datatype, MPIR_Request * sreq, void *netmod_context)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_sendv(int rank,
                                    MPIR_Comm * comm,
                                    int handler_id,
                                    struct iovec *am_hdr,
                                    size_t iov_len,
                                    const void *data,
                                    MPI_Count count,
                                    MPI_Datatype datatype,
                                    MPIR_Request * sreq, void *netmod_context)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_sendv_hdr(int rank,
                                        MPIR_Comm * comm,
                                        int handler_id,
                                        struct iovec *am_hdr,
                                        size_t iov_len, MPIR_Request * sreq, void *netmod_context)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_send_hdr_reply(MPIR_Context_id_t context_id, int src_rank,
                                             int handler_id,
                                             const void *am_hdr,
                                             size_t am_hdr_sz, MPIR_Request * sreq)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_send_reply(MPIR_Context_id_t context_id, int src_rank,
                                         int handler_id,
                                         const void *am_hdr,
                                         size_t am_hdr_sz,
                                         const void *data,
                                         MPI_Count count,
                                         MPI_Datatype datatype, MPIR_Request * sreq)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_sendv_reply(MPIR_Context_id_t context_id, int src_rank,
                                          int handler_id,
                                          struct iovec *am_hdr,
                                          size_t iov_len,
                                          const void *data,
                                          MPI_Count count,
                                          MPI_Datatype datatype, MPIR_Request * sreq)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline size_t MPIDI_NM_am_hdr_max_sz(void)
{
    MPIR_Assert(0);
    return 0;
}

static inline int MPIDI_NM_am_inject_hdr(int rank,
                                         MPIR_Comm * comm,
                                         int handler_id,
                                         const void *am_hdr, size_t am_hdr_sz, void *netmod_context)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_inject_hdr_reply(MPIR_Context_id_t context_id, int src_rank,
                                               int handler_id, const void *am_hdr, size_t am_hdr_sz)
{
    MPIR_Assert(0);
    return MPI_SUCCESS;
}

static inline int MPIDI_NM_am_recv(MPIR_Request * req)
{
    MPIR_Assert(0);
    return 0;
}

#endif /* NETMOD_STUBNM_AM_H_INCLUDED */