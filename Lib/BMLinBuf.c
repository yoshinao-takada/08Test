#include "BMLinBuf.h"
BMLinBufPool_SDECL(linbufpool, 
    BMLinBuf_STATIC_POOL_SIZE, BMLinBuf_STATIC_BUF_SIZE);


BMLinBuf_pt BMLinBufPool_Get(BMLinBufPool_pt pool)
{
    BMLinBuf_pt linbuf = NULL;
    BMPoolBase_LOCK(&pool->base);
    do {
        int16_t idx = BMPoolBase_FindAvailable(&pool->base);
        if (idx < 0) break;
        linbuf = pool->bufs + idx;
        linbuf->crunched = linbuf->filled = 0;
    } while (0);    
    BMPoolBase_UNLOCK(&pool->base);
    return linbuf;
}

BMStatus_t BMLinBufPool_Return(BMLinBufPool_pt pool, BMLinBuf_pt linbuf)
{
    BMStatus_t status;
    BMPoolBase_LOCK(&pool->base);
    ptrdiff_t offs = linbuf - pool->bufs;
    status = BMPoolBase_Return(&linbufpool.base, offs);
    BMPoolBase_UNLOCK(&pool->base);
    return status;
}

BMLinBuf_pt BMLinBufPool_SGet()
{
    return BMLinBufPool_Get(&linbufpool);
}

BMStatus_t BMLinBufPool_SReturn(BMLinBuf_pt linbuf)
{
    return BMLinBufPool_Return(&linbufpool, linbuf);
}

void BMLinBufPool_SInit()
{
    BMLinBufPool_INIT(&linbufpool);
}

void BMLinBufPool_SDeinit()
{
    BMLinBufPool_DEINIT(&linbufpool);
}