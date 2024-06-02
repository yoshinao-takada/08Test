#include "BMDLLayer.h"
#include "BMTest.h"
#include "BMDLLayerDbg.h"
#include <assert.h>

static const uint8_t TEST_BYTES0[] = "0123456789ABCDEF";
static const uint8_t TEST_BYTES1[] = { 0, 1, '\r', '\n', 0xfe, 0xff, 0x80, 0x81, 188, 195 };
static const uint8_t HDRMK[] = BMDLDecoder_HDRMK;

static void PutBytes
(BMCRC_pt crc, BMLinBuf_pt linbuf, const uint8_t* bytes, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++)
    {
        BMCRC_Put(crc, *bytes);
        BMLinBuf_COPY(linbuf, bytes++, 1);
    }
}

/*!
\brief Confirm operations of BMCRC module.
*/
static BMStatus_t BMDLLayer_ConfirmCRC()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMLinBuf_pt linbuf = NULL;
    BMCRC_t crc = BMCRC_CCITT16_INIT(BMCRC_SEED16);
    uint8_t* shifterBytes = BMCRC_SHIFTER_BYTES(&crc);
    uint16_t payloadLength = strlen(TEST_BYTES0);
    do {
        if (!(linbuf = BMLinBufPool_SGet()))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Fail in BMLinBufPool_SGet()");
        }
        PutBytes(&crc, linbuf, HDRMK, 2);
        PutBytes(&crc, linbuf, (uint8_t*)&payloadLength, sizeof(payloadLength));
        PutBytes(&crc, linbuf, TEST_BYTES0, payloadLength);
        BMCRC_Put0s(&crc, 2);
        BMENDIAN_REV16(shifterBytes);
        BMLinBuf_COPY(linbuf, shifterBytes, 2);
        if (linbuf->filled != 6 + strlen(TEST_BYTES0))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Invalid encoded length");
        }
        crc.Shifter = BMCRC_SEED16;
        BMCRC_Puts(&crc, linbuf->buf, linbuf->filled);
        if (0 != crc.Shifter)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Fail in encode/decode a byte sequence");
        }
        linbuf->buf[8] = (0x10 ^ linbuf->buf[8]);
        crc.Shifter = BMCRC_SEED16;
        BMCRC_Puts(&crc, linbuf->buf, linbuf->filled);
        if (0 == crc.Shifter)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Fail to detect bit error");
        }
    } while (0);
    BMLinBufPool_SReturn(linbuf);
    BMTest_ENDFUNC(status);
    return status;
}

/*!
\brief confirm operations of BMDLLayer decode operations
*/
static BMStatus_t BMDLLayer_DecodeUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMLinBuf_pt linbuf = NULL;
    BMCRC_t crc = BMCRC_CCITT16_INIT(BMCRC_SEED16);
    uint8_t* shifterBytes = BMCRC_SHIFTER_BYTES(&crc);
    BMDLDecoder_t decoder = BMDLDecoder_INI;
    uint16_t payloadLength = strlen(TEST_BYTES0);
    do {
        if (!(linbuf = BMLinBufPool_SGet()))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Fail in BMLinBufPool_SGet()");
        }
        PutBytes(&crc, linbuf, HDRMK, 2);
        PutBytes(&crc, linbuf, (uint8_t*)&payloadLength, sizeof(payloadLength));
        PutBytes(&crc, linbuf, TEST_BYTES0, strlen(TEST_BYTES0));
        BMCRC_Put0s(&crc, 2);
        BMENDIAN_REV16(shifterBytes);
        BMLinBuf_COPY(linbuf, shifterBytes, 2);
        /*
        * To be implemented decoding frame detection
        */
#pragma region send_byte_sequence_to_ignore
        if (decoder.state != BMDLDecoder_StateWHMK)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("Fail to init decoder");
        }
        for (int i = 0; i < sizeof(TEST_BYTES0); i++)
        {
            if (BMStatus_SUCCESS != 
            (status = BMDLDecoder_Putc(&decoder, TEST_BYTES0[i])))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDLDecoder_Putc()");
            }
            if (BMDLDecoder_StateWHMK != decoder.state)
            {
                status = BMStatus_INVALID;
                BMTest_ERRLOGBREAKEX("Invalid state transition");
            }
        }
        if (status) break;
#pragma endregion send_byte_sequence_to_ignore
#pragma region send_header_mark
        for (; linbuf->crunched < 2; linbuf->crunched++)
        {
            status = BMDLDecoder_Putc(&decoder, linbuf->buf[linbuf->crunched]);
            if (BMStatus_SUCCESS != status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDLDecoder_Putc()");
            }
        }
        if (status) break;
        if (BMDLLayerDBG_State(BMDLLayerDBG_GFRL0) != decoder.state)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("Invalid state transition");
        }
#pragma endregion send_header_mark
#pragma region send_frame_length
        for (; linbuf->crunched < 4; linbuf->crunched++)
        {
            status = BMDLDecoder_Putc(&decoder, linbuf->buf[linbuf->crunched]);
            if (BMStatus_SUCCESS != status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDLDecoder_Putc()");
            }
        }
        if (status) break;
        if (BMDLLayerDBG_State(BMDLLayerDBG_RDPL) != decoder.state)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("Invalid state transition");
        }
#pragma endregion send_frame_length
#pragma region send_payload
        for (; linbuf->crunched < (4 + payloadLength); linbuf->crunched++)
        {
            status = BMDLDecoder_Putc(&decoder, linbuf->buf[linbuf->crunched]);
            if (BMStatus_SUCCESS != status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDLDecoder_Putc()");
            }
        }
        if (status) break;
        if (BMDLLayerDBG_State(BMDLLayerDBG_CCRC0) != decoder.state)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("Invalid state transition");
        }
#pragma endregion send_payload
#pragma region send_CRC
        for (; linbuf->crunched < (6 + payloadLength); linbuf->crunched++)
        {
            status = BMDLDecoder_Putc(&decoder, linbuf->buf[linbuf->crunched]);
            if (
                ((linbuf->crunched < (5 + payloadLength)) &&
                (BMStatus_SUCCESS != status)) ||
                ((linbuf->crunched == (5 + payloadLength)) &&
                (BMStatus_END != status)))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDLDecoder_Putc()");
            }
        }
        if (status == BMStatus_END) { status = BMStatus_SUCCESS; }
        if (status) break;
        if (BMDLLayerDBG_State(BMDLLayerDBG_FRCMP) != decoder.state)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("Invalid state transition");
        }
#pragma endregion send_CRC
#pragma region Check_Rx_Payload
        if ((decoder.payload_len != strlen(TEST_BYTES0)) ||
            (decoder.payload->filled != strlen(TEST_BYTES0)))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Invalid payload length");
        }
        else if (memcmp(TEST_BYTES0, decoder.payload->buf, decoder.payload_len))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Invalid payload contents");
        }
#pragma endregion Check_Rx_Payload
#pragma region Return_The_Payload_Resource_To_The_Pool
        status = BMLinBufPool_SReturn(BMDLDecoder_Reset(&decoder, NULL));
        if (status)
        {
            BMTest_ERRLOGBREAKEX("Fail in BMDLDecoder_Reset() "
                "or in BMLinBufPool_SReturn()");
        }
#pragma endregion Return_The_Payload_Resource_To_The_Pool

    } while (0);
    BMLinBufPool_SReturn(linbuf);
    BMTest_ENDFUNC(status);
    return status;
}

/*!
\brief confirm recovery from CRC error
Step 1: Create two frames; The 1st one has bit errors. The 2nd one
    has no bit error.</br>
Step 2: Initialize a decoder.
Step 3: Put the 1st frame into the decoder and confirm error.
Step 4: Put the 2nd frame into the decoder and confirm no error.
*/
static void MkFrame(BMLinBuf_pt buf, const uint8_t* payload, uint16_t payloadLen)
{
    // Initialize CRC encoder
    BMCRC_t crc = BMCRC_CCITT16_INIT(BMCRC_SEED16);
    uint8_t* shifterBytes = BMCRC_SHIFTER_BYTES(&crc);
    // Copy and CRC encode the physical layer byte sequence into buf.
    PutBytes(&crc, buf, HDRMK, sizeof(HDRMK));
    PutBytes(&crc, buf, (uint8_t*)&payloadLen, sizeof(uint16_t));
    PutBytes(&crc, buf, payload, payloadLen);
    // Create CRC error checking code in buf.
    BMCRC_Put0s(&crc, 2);
    BMENDIAN_REV16(shifterBytes);
    BMLinBuf_COPY(buf, shifterBytes, 2);
}

static BMStatus_t BMDLLayer_ErrRecoveryUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMLinBuf_pt rbuf0 = BMLinBufPool_SGet(), rbuf1 = BMLinBufPool_SGet();
    BMLinBuf_pt sbuf0 = BMLinBufPool_SGet(), sbuf1 = BMLinBufPool_SGet();
    assert(rbuf0); assert(rbuf1); assert(sbuf0); assert(sbuf1);
    MkFrame(sbuf0, TEST_BYTES0, sizeof(TEST_BYTES0));
    MkFrame(sbuf1, TEST_BYTES1, sizeof(TEST_BYTES1));
    // apply a bit error in payload length part in the frame
    sbuf0->buf[3] ^= 0x04;
    // init the frame decoder
    BMDLDecoder_t decoder = BMDLDecoder_INI;
    BMDLDecoder_Reset(&decoder, rbuf0);
    do {
        uint16_t bytecount = sbuf0->filled;
        status = BMDLDecoder_Puts(&decoder, sbuf0->buf, &bytecount);
        if (status || (bytecount != sbuf0->filled) ||
            (decoder.state != BMDLDecoder_StateWHMK) ||
            (decoder.payload_len) || (decoder.payload->filled))
        {
            BMTest_ERRLOGBREAKEX("Unknown error in BMDLDecoder_Puts()");
        }
        bytecount = sbuf1->filled;
        status = BMDLDecoder_Puts(&decoder, sbuf1->buf, &bytecount);
        if ((status != BMStatus_END) ||
            (decoder.payload_len != sizeof(TEST_BYTES1)))
        {
            BMTest_ERRLOGBREAKEX("Unknown error in BMDLDecoder_Puts()");
        }
        status = BMStatus_SUCCESS;
        BMLinBuf_pt payload = BMDLDecoder_Reset(&decoder, NULL);
        if ((payload->filled != sizeof(TEST_BYTES1)) ||
            memcmp(payload->buf, TEST_BYTES1, payload->filled))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Unknown error in BMDLDecoder_Puts() result payload");
        }
    } while (0);
    assert(BMStatus_SUCCESS == BMLinBufPool_SReturn(rbuf0));
    assert(BMStatus_SUCCESS == BMLinBufPool_SReturn(rbuf1));
    assert(BMStatus_SUCCESS == BMLinBufPool_SReturn(sbuf0));
    assert(BMStatus_SUCCESS == BMLinBufPool_SReturn(sbuf1));
    BMTest_ENDFUNC(status);
    return status;
}

#pragma region BMDLLayer_FSM_Tests
//BMDLLayer_SDECL(dllayer);
#pragma endregion BMDLLayer_FSM_Tests

BMStatus_t BMDLLayerUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMLinBufPool_SInit();
    do {
        if (BMStatus_SUCCESS != (status = BMDLLayer_ConfirmCRC()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMDLLayer_ConfirmCRC()");
        }
        if (BMStatus_SUCCESS != (status = BMDLLayer_DecodeUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMDLLayer_DecodeUT()");
        }
        if (BMStatus_SUCCESS != (status = BMDLLayer_ErrRecoveryUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMDLLayer_ErrRecoveryUT()");
        }
    } while (0);
    BMLinBufPool_SDeinit();
    BMTest_ENDFUNC(status);
    return status;
}