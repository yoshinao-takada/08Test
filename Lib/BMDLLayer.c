#include "BMDLLayer.h"
#include "BMDLLayerDbg.h"

#define PUSH_BYTE(_buf, _byte) \
    _buf[0] = _buf[1]; \
    _buf[1] = _byte

static const uint8_t HDRMK[] = BMDLDecoder_HDRMK;

#define MatchHDRMK(_tocompare) \
    ((HDRMK[0] == _tocompare[0]) && (HDRMK[1] == _tocompare[1]))

static BMStateResult_t StateGFRL0(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateGFRL1(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateRDPL(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateCCRC0(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateCCRC1(BMDLDecoder_pt decoder, uint8_t byte);
static BMStateResult_t StateFRCMP(BMDLDecoder_pt decoder, uint8_t byte);

static BMDLLayerState_f const STATES[] =
{
    BMDLDecoder_StateWHMK,
    StateGFRL0,
    StateGFRL1,
    StateRDPL,
    StateCCRC0,
    StateCCRC1,
    StateFRCMP,
};

BMDLLayerState_f BMDLLayerDBG_State(BMDLLayerDBG_StateID_t stateIndex)
{
    return STATES[(int)stateIndex];
}

BMLinBuf_pt BMDLDecoder_Reset(BMDLDecoder_pt obj, BMLinBuf_pt payloadbuf)
{
    BMLinBuf_pt retval = obj->payload;
    obj->payload = payloadbuf;
    obj->crc.Shifter = BMDLDecoder_CRCSEED;
    obj->hm[0] = obj->hm[1] = 0;
    obj->payload_len = 0;
    obj->state = BMDLDecoder_StateWHMK;
    if (obj->payload)
    {
        obj->payload->crunched = obj->payload->filled = 0;
    }
    return retval;
}

/*!
\brief put a character into the decoder.
*/
BMStatus_t BMDLDecoder_Putc(BMDLDecoder_pt obj, uint8_t byte)
{
    BMStatus_t status = BMStatus_SUCCESS;
    if (obj->payload->filled == obj->payload->size)
    { // decoder internal buffer is full.
        return BMStatus_NORESOURCE;
    }
    // decode 1 byte.
    BMStateResult_t result = obj->state(obj, byte);
    if (result == BMStateResult_ERROR) 
    {
        status = BMStatus_FAILURE;
    }
    else if (obj->state == StateFRCMP)
    {
        status = BMStatus_END;
    }
    return status;
}

/*!
\brief put a byte sequence into the decoder.
*/
BMStatus_t BMDLDecoder_Puts(
    BMDLDecoder_pt obj, uint8_t* bytes, uint16_t* bytecount)
{
    BMStatus_t status = BMStatus_SUCCESS;
    for (uint16_t i = 0; i < (*bytecount); i++)
    {
        if (BMStatus_SUCCESS != (status = BMDLDecoder_Putc(obj, bytes[i])))
        {
            *bytecount = i + 1;
            break;
        }
    }
    return status;
}

/*!
\brief put a byte sequence from the ring buffer into the decoder.
*/
BMStatus_t BMDLDecoder_RbPuts(BMDLDecoder_pt obj, BMRingBuf_pt rb)
{
    BMStatus_t status = BMStatus_SUCCESS;
    uint8_t bytebuf;
    for (; BMStatus_SUCCESS == BMRingBuf_Get(rb, &bytebuf); )
    {
        status = BMDLDecoder_Putc(obj, bytebuf);
    }
    return status;
}

#pragma region DECODER_STATE_HANDLER_IMPL
BMStateResult_t BMDLDecoder_StateWHMK(BMDLDecoder_pt decoder, uint8_t byte)
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
    BMStateResult_t result = BMStateResult_TRANSIT;
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
    else if (decoder->payload_len > BMDLDeoder_MAX_PAYLOAD_LEN)
    {
        decoder->state = BMDLDecoder_StateWHMK;
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
        result = BMStateResult_ERROR;
    }
    else
    {
        decoder->state = StateFRCMP;
        result = BMStateResult_TRANSIT;
    }
    return result;
}

// Frame decoding completion state
static BMStateResult_t StateFRCMP(BMDLDecoder_pt decoder, uint8_t byte)
{
    BMStateResult_t result = BMStateResult_IGNORE;
    return result;
}
#pragma endregion DECODER_STATE_HANDLER_IMPL

#pragma region DATALINK_LAYER_CONTROLLER_IMPL
BMStateResult_t BMDLLayer_State0(BMFSM_pt *fsm, BMEv_pt ev)
{
    BMStateResult_t result = BMStateResult_IGNORE;
    return result;
}
#pragma endregion DATALINK_LAYER_CONTROLLER_IMPL
