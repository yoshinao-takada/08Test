#if !defined(BMDLLAYER_H)
#define BMDLLAYER_H
#include "BMCRC.h"
#include "BMLinBuf.h"
#include "BMRingBuf.h"
#include "BMFSM.h"
#include <stdint.h>
#define BMDLDeoder_MAX_PAYLOAD_LEN  BMLinBuf_STATIC_BUF_SIZE

#pragma region FRAME_DECODER
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

BMStateResult_t BMDLDecoder_StateWHMK(BMDLDecoder_pt decoder, uint8_t byte);

#define BMDLDecoder_CRCSEED   BMCRC_SEED16
#define BMDLDecoder_HDRMK { 0xaa, 0x55 }
#define BMDLDecoder_INI   { \
    BMCRC_CCITT16_INIT(BMDLDecoder_CRCSEED), \
    BMDLDecoder_HDRMK, \
    NULL, \
    0u, \
    BMDLDecoder_StateWHMK }

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

/*!
\brief put a byte sequence from the ring buffer into the decoder.
*/
BMStatus_t BMDLDecoder_RbPuts(BMDLDecoder_pt obj, BMRingBuf_pt rb);
#pragma endregion FRAME_DECODER

#pragma region DATA_LINK_LAYER_STATE_MACHINE
    /*!
    \brief initial state of data link layer frame reader
    */
    BMStateResult_t BMDLLayer_State0(BMFSM_pt* fsm, BMEv_pt ev);

#define BMDLLayer_DECL(_varname) \
    BMDLDecoder_t _varname ## _decoder = BMDLDecoder_INI; \
    BMFSM_DECL(_varname)

#define BMDLLayer_SDECL(_varname) \
    static BMDLDecoder_t _varname ## _decoder = BMDLDecoder_INI; \
    BMFSM_SDECL(_varname)

#pragma endregion DATA_LINK_LAYER_STATE_MACHINE
#endif /* BMDLLayer_H */