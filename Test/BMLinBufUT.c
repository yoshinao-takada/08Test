#include "BMLinBuf.h"
#include "BMTest.h"

#define BMLinBufUT_SBUFSIZE 64
BMLinBuf_SDECL(linbuf, BMLinBufUT_SBUFSIZE);

/*!
\brief Static variable linbuf initialization check
*/
BMStatus_t BMLinBufUT_SCheck()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        if (linbuf.buf != linbuf_buf)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("(linbuf.buf != linbuf_buf)");
        }
        if ((sizeof(linbuf_buf) != BMLinBufUT_SBUFSIZE) ||
            (linbuf.size != BMLinBufUT_SBUFSIZE))
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("Size mismatch");
        }
        if (linbuf.crunched != 0 || linbuf.filled != 0)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("Not initialized");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

/*!
\brief Confirm BMLinBufPool_SGet and _SReturn
*/
BMStatus_t BMLinBufPool_SGetReturn()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMLinBuf_pt linbufs[BMLinBuf_STATIC_POOL_SIZE * 2];
    do {
        int i = 0;
        for (; linbufs[i] = BMLinBufPool_SGet(); i++) ;
        if (i != BMLinBuf_STATIC_POOL_SIZE)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("Fail in BMLinBufPool_SGet()");
        }
        do {
            status = BMLinBufPool_SReturn(linbufs[--i]);
            if (status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMLinBufPool_SReturn()");
            }
        } while (i);
        if (status) break;
        if (BMStatus_SUCCESS == BMLinBufPool_SReturn(linbufs[0]))
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX(
                "BMLinBufPool_SReturn() ignored dupulicated return");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMLinBufUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMLinBufPool_SInit();
    do {
        if (BMStatus_SUCCESS != (status = BMLinBufUT_SCheck()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMLinBufUT_SCheck()");
        }

        if (BMStatus_SUCCESS != (status = BMLinBufPool_SGetReturn()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMLinBufPool_SGetReturn()");
        }
    } while (0);
    BMLinBufPool_SDeinit();
    BMTest_ENDFUNC(status);
    return status;
}