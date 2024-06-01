#if !defined(BMDLLAYER_H)
#define BMDLLAYER_H
#include "BMCRC.h"
#include "BMLinBuf.h"
#include "BMFSM.h"
#include <stdint.h>
#define BMDLDeoder_MAX_PAYLOAD_LEN  BMLinBuf_STATIC_BUF_SIZE

/*!
\brief Data Link Layer decoder accepts characters in
    byte-by-byte manner. The decoder detects a header mark and
    a payload length. And then read and store the payload.
    Finally check the CRC code to determin to discard or accept
    the payload.
*/
typedef struct BMDLDecoder {
    BMCRC_t crc; // crc endec
    uint8_t hm[2]; // Header mark buffer
    BMLinBuf_pt payload; // payload buffer
    uint16_t payload_len; // payload length
    // state
    BMStateResult_t (*state)(struct BMDLDecoder* decoder, uint8_t byte);
} BMDLDecoder_t, *BMDLDecoder_pt;
typedef const BMDLDecoder_t *BMDLDecoder_cpt;

BMStateResult_t BMDLLayer_StateWHMK(BMDLDecoder_pt decoder, uint8_t byte);

#define BMDLLayer_CRCSEED   BMCRC_SEED16
#define BMDLLayer_HDRMK { 0xaa, 0x55 }
#define BMDLLayer_INI   { \
    BMCRC_CCITT16_INIT(BMDLLayer_CRCSEED), \
    BMDLLayer_HDRMK, \
    BMLinBufPool_SGet(), \
    0u, \
    BMDLLayer_StateWHMK }

/*!
\brief set the decoder in the initial state.
*/
BMLinBuf_pt BMDLDecoder_Reset(BMDLDecoder_pt obj, BMLinBuf_pt payloadbuf);

/*!
\brief put a character into the decoder.
*/
BMStatus_t BMDLDecoder_Putc(BMDLDecoder_pt obj, uint8_t byte);

/*!
\brief put a byte sequence into the decoder.
*/
BMStatus_t BMDLDecoder_Puts(
    BMDLDecoder_pt obj, uint8_t* bytes, uint16_t* bytecount);
#endif /* BMDLLayer_H */