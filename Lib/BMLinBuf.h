#if !defined(BMLINBUF_H)
#define BMLINBUF_H
#include "BMPoolBase.h"
#include "BMQBase.h"
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

/*!
\brief Clear the buffer.
*/
#define BMLinBuf_CLEAR(_buf) (_buf).filled = (_buf).crunched = 0

/*!
\brief Get the pointer pointing the byte to write.
*/
#define BMLinBuf_WRPTR(_buf) ((_buf).buffer + (_buf).filled)

/*!
\brief Get the pointer pointing the byte to read.
*/
#define BMLinBuf_RDPTR(_buf) ((_buf).buffer + (_buf).crunched)

/*!
\brief Available byte count in the buffer.
*/
#define BMLinBuf_AVAILABLE(_buf) ((_buf).size - (_buf).filled)

#define BMLinBuf_DECL(_varname, _size) \
    uint8_t _varname ## _buf[_size]; \
    BMLinBuf_t _varname = { _varname ## _buf, _size, 0, 0 }
#pragma endregion DECLARE_BMLinBuf_t

#pragma region DECLARE_BMLinBufQ_t
typedef struct {
    BMQBase_t base;
    BMLinBuf_pt bufs;
} BMLinBufQ_t, *BMLinBufQ_pt;
typedef const BMLinBufQ_t *BMLinBufQ_cpt;

#define BMLinBufQ_DECL(_varname, _qsize, _bufsize) \
    uint8_t _varname ## _buf[_qsize * _bufsize]; \
    BMLineBuf_t _varname ## _linbufs[_qsize] = { \
        { _varname ## _buf, _bufsize, 0, 0 }, \
    }; \
    BMLineBufQ_t _varname = { BMQBase(_qsize), \
        _varname ## _linbufs }

#define BMLinBufQ_SDECL(_varname, _qsize, _bufsize) \
    static uint8_t _varname ## _buf[_qsize * _bufsize]; \
    static BMLineBuf_t _varname ## _linbufs[_qsize] = { \
        { _varname ## _buf, _bufsize, 0, 0 }, \
    }; \
    static BMLineBufQ_t _varname = { BMQBase(_qsize), \
        _varname ## _linbufs }

#define BMLinBufQ_INIT(_varptr) { \
    BMQBase_INIT(&(_varptr)->base); \
    uint16_t _qsize = (_varptr)->base.count; \
    uint16_t _bufsize = (_varptr)->bufs[0].size; \
    BMLinBuf_t _buftemplate; \
    size_t _copysize = sizeof(BMLinBuf_t); \
    memcpy(&_buftemplate, (_varptr)->bufs, _copysize); \
    BMLinBuf_pt _iter = (_varptr)->bufs; \
    BMLinBuf_cpt const _end = _iter + _qsize; \
    for (; _iter != _end; _iter++) \
    { \
        memcpy(_iter, &_buftemplate, _copysize); \
        _buftemplate.buf += _bufsize; \
    } \
}

#define BMLinBufQ_DEINIT(_varptr) BMQBase_DEINIT(&(_varptr)->base)


#pragma endregion DECLARE_BMLinBufQ_t
#endif /* BMLINBUF_H */