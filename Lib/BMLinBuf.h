#if !defined(BMLINBUF_H)
#define BMLINBUF_H
#include "BMPoolBase.h"
#include <memory.h>
#define BMLinBuf_STATIC_POOL_SIZE   4
#define BMLinBuf_STATIC_BUF_SIZE    32
#pragma region DECLARE_BMLinBuf_t
/*!
\brief Linear buffer struct
*/
typedef struct {
    uint8_t *buf;
    uint16_t size, filled, crunched;
} BMLinBuf_t, *BMLinBuf_pt;
typedef const BMLinBuf_t *BMLinBuf_cpt;

#define BMLinBuf_DECL(_varname, _size) \
    uint8_t _varname ## _buf[_size]; \
    BMLinBuf_t _varname = { _varname ## _buf, _size, 0, 0 }

#define BMLinBuf_SDECL(_varname, _size) \
    static uint8_t _varname ## _buf[_size]; \
    static BMLinBuf_t _varname = { _varname ## _buf, _size, 0, 0 }
#pragma endregion DECLARE_BMLinBuf_t

#pragma region DECLARE_BMLinBufPool_t
typedef struct {
    BMPoolBase_t base;
    BMLinBuf_pt bufs;
} BMLinBufPool_t, *BMLinBufPool_pt;
typedef const BMLinBufPool_t *BMLinBufPool_cpt;

#define BMLinBufPool_DECL(_varname, _poolsize, _bufsize) \
    uint8_t _varname ## _bytebuf[(_poolsize) * (_bufsize)]; \
    uint16_t _varname ## _used[BMAlign_BC2SWC(_poolsize)]; \
    BMLinBuf_t _varname ## _linbufs[_poolsize] = { \
        { _varname ## _bytebuf, _bufsize, 0, 0 } \
    }; \
    BMLinBufPool_t _varname = { \
        { _varname ## _used, 0, _poolsize }, \
        _varname ## _linbufs \
    }

#define BMLinBufPool_SDECL(_varname, _poolsize, _bufsize) \
    static uint8_t _varname ## _bytebuf[(_poolsize) * (_bufsize)]; \
    static uint16_t _varname ## _used[BMAlign_BC2SWC(_poolsize)]; \
    static BMLinBuf_t _varname ## _linbufs[_poolsize] = { \
        { _varname ## _bytebuf, _bufsize, 0, 0 } \
    }; \
    static BMLinBufPool_t _varname = { \
        { _varname ## _used, 0, _poolsize }, \
        _varname ## _linbufs \
    }

#define BMLinBufPool_INIT(_varptr) BMPoolBase_INIT(&(_varptr)->base); \
{ \
    size_t _copysize = sizeof(BMLinBuf_t); \
    BMLinBuf_pt _iter = (_varptr)->bufs; \
    BMLinBuf_cpt const _end = _iter + (_varptr)->base.count; \
    BMLinBuf_t _template; \
    memcpy(&_template, _iter, _copysize); \
    for (; _iter != _end; _iter++) \
    { \
        memcpy(_iter, &_template, _copysize); \
        _template.buf += _template.size; \
    } \
}

#define BMLinBufPool_DEINIT(_varptr) BMPoolBase_DEINIT(&(_varptr)->base)

BMLinBuf_pt BMLinBufPool_Get(BMLinBufPool_pt pool);

BMStatus_t BMLinBufPool_Return(BMLinBufPool_pt pool, BMLinBuf_pt linbuf);

BMLinBuf_pt BMLinBufPool_SGet();

BMStatus_t BMLinBufPool_SReturn(BMLinBuf_pt linbuf);
#pragma endregion DECLARE_BMLinBufPool_t
#endif /* BMLINBUF_H */