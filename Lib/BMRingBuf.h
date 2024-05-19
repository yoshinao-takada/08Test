#if !defined(BMRINGBUF_H)
#define BMRINGBUF_H
#include "BMDefs.h"
#include "BMQBase.h"
#include "BMPoolBase.h"
#include <memory.h>

#define BMRingBuf_STATIC_POOL_SIZE      2
#define BMRingBuf_RB_SIZE               64

#pragma region BMRingBuf_DECL
typedef struct {
    BMQBase_t base;
    uint8_t* buf;
} BMRingBuf_t, *BMRingBuf_pt;

typedef const BMRingBuf_t *BMRingBuf_cpt;

#define BMRingBuf_DECL(_varname, _bufsize) \
    uint8_t _varname ## _buf[_bufsize]; \
    BMRingBuf_t _varname = { BMQBase(_bufsize), _varname ## _buf }


#define BMRingBuf_SDECL(_varname, _bufsize) \
    static uint8_t _varname ## _buf[_bufsize]; \
    static BMRingBuf_t _varname = { BMQBase(_bufsize), _varname ## _buf }

#define BMRingBuf_INIT(_varptr) BMQBase_INIT(&(_varptr)->base)

#define BMRingBuf_DEINIT(_varptr) BMQBase_DEINIT(&(_varptr)->base)

/*!
\brief put a byte into a ring buffer.
*/
BMStatus_t BMRingBuf_Put(BMRingBuf_pt rb, const uint8_t* byteptr);

/*!
\brief get a byte from a ring buffer.
*/
BMStatus_t BMRingBuf_Get(BMRingBuf_pt rb, uint8_t* byteptr);
#pragma endregion BMRingBuf_DECL

#pragma region BMRingBufPool_DECL
typedef struct {
    BMPoolBase_t base;
    BMRingBuf_pt rbs;
} BMRingBufPool_t, *BMRingBufPool_pt;

#define BMRingBufPool_DECL(_varname, _poolsize, _rbsize) \
    uint16_t _varname ## _used[BMAlign_BC2SWC(_poolsize)]; \
    uint8_t _varname ## _buf[_poolsize * _rbsize]; \
    BMRingBuf_t _varname ## _rbs[_poolsize] = { \
        { BMQBase(_rbsize), _varname ## _buf }, \
    }; \
    BMRingBufPool_t _varname = { \
        { _varname ## _used, 0, _poolsize }, _varname ## _rbs }

#define BMRingBufPool_SDECL(_varname, _poolsize, _rbsize) \
    static uint16_t _varname ## _used[BMAlign_BC2SWC(_poolsize)]; \
    static uint8_t _varname ## _buf[_poolsize * _rbsize]; \
    static BMRingBuf_t _varname ## _rbs[_poolsize] = { \
        { BMQBase(_rbsize), _varname ## _buf }, \
    }; \
    static BMRingBufPool_t _varname = { \
        { _varname ## _used, 0, _poolsize }, _varname ## _rbs }

#define BMRingBufPool_INIT(_poolptr) \
BMPoolBase_INIT(&(_poolptr)->base); \
{ \
    uint16_t _used_size = BMAlign_BC2SWC((_poolptr)->base.count); \
    const uint16_t* _used_end = (_poolptr)->base.used; \
    uint16_t* _used = (_poolptr)->base.used; \
    for (; _used != _used_end; _used++) *_used = 0; \
} { \
    size_t _copysize = sizeof(BMRingBuf_t); \
    BMRingBuf_t _rbtemplate; \
    memcpy(&_rbtemplate, (_poolptr)->rbs, _copysize); \
    BMRingBuf_pt _rbs = (_poolptr)->rbs; \
    BMRingBuf_cpt const _rbs_end = _rbs + (_poolptr)->base.count; \
    uint16_t _rbsize = _rbtemplate.base.count; \
    for (; _rbs != _rbs_end; _rbs++) \
    { \
        memcpy(_rbs, &_rbtemplate, _copysize); \
        _rbtemplate.buf += _rbsize; \
    } \
}

#define BMRingBufPool_DEINIT(_poolptr) BMPoolBase_DEINIT(&(_poolptr)->base)

BMRingBuf_pt BMRingBufPool_Get(BMRingBufPool_pt poolptr);

BMStatus_t 
BMRingBufPool_Return(BMRingBufPool_pt poolptr, BMRingBuf_pt rbptr);


BMRingBuf_pt BMRingBufPool_SGet();

BMStatus_t BMRingBufPool_SReturn(BMRingBuf_pt rbptr);

void BMRingBufPool_SInit();

void BMRingBufPool_SDeinit();
#pragma endregion BMRingBufPool_DECL
#endif /* BMRINGBUF_H */