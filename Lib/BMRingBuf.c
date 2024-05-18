#include "BMRingBuf.h"


BMStatus_t BMRingBuf_Put(BMRingBuf_pt rb, const uint8_t* byteptr)
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMQBase_LOCK(&rb->base);
    do {
        uint16_t next = BMQBase_NextWridx(&rb->base);
        if (next == rb->base.rdidx)
        {
            status = BMStatus_NORESOURCE;
            break;
        }
        rb->buf[rb->base.wridx] = *byteptr;
        rb->base.wridx = next;
    } while (0);
    BMQBase_UNLOCK(&rb->base);
    return status;
}

BMStatus_t BMRingBuf_Get(BMRingBuf_pt rb, uint8_t* byteptr)
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMQBase_LOCK(&rb->base);
    do {
        if (rb->base.rdidx == rb->base.wridx)
        {
            status = BMStatus_NORESOURCE;
            break;
        }
        *byteptr = rb->buf[rb->base.rdidx];
        rb->base.rdidx = BMQBase_NextRdidx(&rb->base);
    } while (0);
    BMQBase_UNLOCK(&rb->base);
    return status;
}

BMRingBuf_pt BMRingBufPool_Get(BMRingBufPool_pt poolptr)
{
    BMRingBuf_pt rbptr = NULL;
    BMPoolBase_LOCK(&poolptr->base);
    int16_t idx = BMPoolBase_FindAvailable(&poolptr->base);
    if (idx >= 0)
    {
        rbptr = poolptr->rbs + idx;
        BMRingBuf_INIT(rbptr);
    }
    BMPoolBase_UNLOCK(&poolptr->base);
    return rbptr;
}

BMStatus_t BMRingBufPool_Return(BMRingBufPool_pt poolptr, BMRingBuf_pt rbptr)
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMPoolBase_LOCK(&poolptr->base);
    do {
        ptrdiff_t offs = rbptr - poolptr->rbs;
        status = BMPoolBase_Return(&poolptr->base, offs);
        if (status) break;
        // clear the ring buffer.
        rbptr->base.rdidx = rbptr->base.wridx = 0;
        BMRingBuf_DEINIT(rbptr);
    } while (0);
    BMPoolBase_UNLOCK(&poolptr->base);
    return status;
}

BMRingBufPool_SDECL
(rbpool, BMRingBuf_STATIC_POOL_SIZE, BMRingBuf_RB_SIZE);


BMRingBuf_pt BMRingBufPool_SGet()
{
    return BMRingBufPool_Get(&rbpool);
}

BMStatus_t BMRingBufPool_SReturn(BMRingBuf_pt rbptr)
{
    return BMRingBufPool_Return(&rbpool, rbptr);
}

void BMRingBufPool_SInit()
{
    BMRingBufPool_INIT(&rbpool);
}

void BMRingBufPool_SDeinit()
{
    BMRingBufPool_DEINIT(&rbpool);
}
