#include "BMDLLayer.h"

#define PUSH_BYTE(_buf, _byte) \
    _buf[1] = _buf[0]; \
    _buf[0] = _byte

static const uint8_t HDRMK[] = BMDLLayer_HDRMK;

static int MatchHDRMK(const char* tocompare)
{
    return (HDRMK[0] == tocompare[0]) && (HDRMK[1] == tocompare[1]);
}

static BMStateResult_t StateWHMK(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateGFRL0(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateGFRL1(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateRDPL(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateCCRC0(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateCCRC1(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t
StateFRCMP(BMDLDecoder_pt decoder, BMLinBuf_pt newbuf, BMLinBuf_pt* ppbuf);

void BMDLDecoder_Reset(BMDLDecoder_pt obj, BMLinBuf_pt payloadbuf)
{
    obj->payload = payloadbuf;
    obj->crc.Shifter = BMDLLayer_CRCSEED;
    obj->hm[0] = obj->hm[1] = 0;
    obj->payload_len = -1;
    obj->state = StateWHMK;
    obj->payload->crunched = obj->payload->filled = 0;
}

/*!
\brief put a character into the decoder.
*/
BMStatus_t BMDLDecoder_Putc(BMDLDecoder_pt obj, uint8_t byte);

/*!
\brief put a byte sequence into the decoder.
*/
BMStatus_t BMDLDecoder_Puts(
    BMDLDecoder_pt obj, uint8_t* bytes, uint16_t bytecount);

BMLinBuf_pt BMDLDecoder_GetPayload(BMDLDecoder_pt obj)
{
    BMLinBuf_pt retval = obj->payload;
    obj->payload = NULL;
    return retval;
}


static BMStateResult_t StateWHMK(BMDLDecoder_pt decoder, uint8_t byte)
{
    BMStateResult_t result = BMStateResult_IGNORE;
    PUSH_BYTE(decoder->hm, byte);
    if (MatchHDRMK(decoder->hm))
    {
        decoder->state = StateGFRL0;
        BMCRC_Puts(&decoder->crc, decoder->hm, sizeof(decoder->hm));
        result = BMStateResult_TRANSIT;
    }
    return result;
}

static BMStateResult_t StateGFRL0(BMDLDecoder_pt decoder, uint8_t byte)
{
    BMStateResult_t result = BMStateResult_ACCEPT;
    PUSH_BYTE(decoder->hm, byte);
    BMCRC_Put(&decoder->crc, byte);
    decoder->state = StateGFRL1;
    return result;
}

static BMStateResult_t StateGFRL1(BMDLDecoder_pt decoder, uint8_t byte)
{
    BMStateResult_t result = BMStateResult_TRANSIT;
    PUSH_BYTE(decoder->hm, byte);
    BMCRC_Put(&decoder->crc, byte);
    decoder->payload_len = *(uint16_t*)decoder->hm;
    if (decoder->payload_len == 0)
    {
        decoder->state = StateCCRC0;
    }
    else if (decoder->payload_len > decoder->payload->size)
    {
        decoder->state = StateWHMK;
        BMDLDecoder_Reset(decoder, decoder->payload);
    }
    else
    {
        decoder->state = StateRDPL;
    }
    return result;
}

static BMStateResult_t StateRDPL(BMDLDecoder_pt decoder, uint8_t byte)
{
    BMStateResult_t result = BMStateResult_ACCEPT;
    BMCRC_Put(&decoder->crc, byte);
    decoder->payload->buf[decoder->payload->filled++] = byte;
    if (decoder->payload->filled == decoder->payload_len)
    {
        decoder->state = StateCCRC0;
        BMStateResult_TRANSIT;
    }
    return result;
}

static BMStateResult_t StateCCRC0(BMDLDecoder_pt decoder, uint8_t byte)
{
    BMStateResult_t result = BMStateResult_TRANSIT;
    BMCRC_Put(&decoder->crc, byte);
    decoder->state = StateCCRC1;
    return result;
}


static BMStateResult_t StateCCRC1(BMDLDecoder_pt decoder, uint8_t byte)
{
    BMStateResult_t result = BMStateResult_TRANSIT;
    BMCRC_Put(&decoder->crc, byte);
    if (decoder->crc.Shifter)
    { // error
        result = BMStateResult_ERR;
    }
    return result;
}

static BMStateResult_t 
StateFRCMP(BMDLDecoder_pt decoder, BMLinBuf_pt newbuf, BMLinBuf_pt* ppbuf)
{
    BMStateResult_t result = BMStateResult_TRANSIT;
    *ppbuf = decoder->payload;
    BMDLDecoder_Reset(decoder, newbuf);
    decoder->state = StateWHMK;
    return result;
}