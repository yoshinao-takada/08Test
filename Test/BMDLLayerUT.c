#include "BMDLLayer.h"
#include "BMTest.h"

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
    do {
        if (!(linbuf = BMLinBufPool_SGet()))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Fail in BMLinBufPool_SGet()");
        }
        PutBytes(&crc, linbuf, HDRMK, 2);
        PutBytes(&crc, linbuf, TEST_BYTES0, strlen(TEST_BYTES0));
        BMCRC_Put0s(&crc, 2);
        BMENDIAN_REV16(shifterBytes);
        BMLinBuf_COPY(linbuf, shifterBytes, 2);
        crc.Shifter = BMCRC_SEED16;
        BMCRC_Puts(&crc, linbuf->buf, linbuf->filled);
        if (0 != *(BMCRC_SHIFTER_BYTES(&crc)))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Fail in encode/decode a byte sequence");
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
    do {
        if (!(linbuf = BMLinBufPool_SGet()))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Fail in BMLinBufPool_SGet()");
        }
        PutBytes(&crc, linbuf, HDRMK, 2);
        PutBytes(&crc, linbuf, TEST_BYTES0, strlen(TEST_BYTES0));
        BMCRC_Put0s(&crc, 2);
        BMENDIAN_REV16(shifterBytes);
        BMLinBuf_COPY(linbuf, shifterBytes, 2);
        /*
        * To be implemented decoding frame detection
        */
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