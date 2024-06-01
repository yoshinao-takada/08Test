#include "BMDLLayer.h"
#include "BMTest.h"
#include "BMDLLayerDbg.h"

static const uint8_t TEST_BYTES0[] = "0123456789ABCDEF";
static const uint8_t HDRMK[] = BMDLLayer_HDRMK;

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
    BMDLDecoder_t decoder = BMDLLayer_INI;
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
        if (decoder.state != BMDLLayer_StateWHMK)
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
            if (BMDLLayer_StateWHMK != decoder.state)
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
    } while (0);
    BMLinBufPool_SDeinit();
    BMTest_ENDFUNC(status);
    return status;
}