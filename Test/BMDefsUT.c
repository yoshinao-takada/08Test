#include "BMDefs.h"
#include "BMTest.h"

static BMStatus_t BMAlign()
{
    BMStatus_t status = BMStatus_FAILURE;
    do {
        if ((0 != BMAlign_TO_ARCH(0)) || (96 != BMAlign_TO_ARCH(96)))
        {
            BMTest_ERRLOGBREAKEX("Fail in "
                "(0 != BMAlign_TO_ARCH(0)) || "
                "(8 != BMAlign_TO_ARCH(8))");
        }
        if ((8 != BMAlign_TO_ARCH(1)) || (8 != BMAlign_TO_ARCH(7)))
        {
            BMTest_ERRLOGBREAKEX("Fail in "
                "(8 != BMAlign_TO_ARCH(1)) || "
                "(8 != BMAlign_TO_ARCH(7))");
        }
        if ((32 != BMAlign_TO_ARCH(25)) || (32 != BMAlign_TO_ARCH(31)))
        {
            BMTest_ERRLOGBREAKEX("Fail in "
                "(32 != BMAlign_TO_ARCH(25)) || "
                "(32 != BMAlign_TO_ARCH(31))");
        }
        status = BMStatus_SUCCESS;
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

static BMStatus_t BMAlign16()
{
    BMStatus_t status = BMStatus_FAILURE;
    do {
        if ((0 != BMAlign_TO16(0)) || (80 != BMAlign_TO16(80)))
        {
            BMTest_ERRLOGBREAKEX("Fail in "
                "(0 != BMAlign_TO16(0)) || "
                "(80 != BMAlign_TO16(80))");
        }
        if ((16 != BMAlign_TO16(1)) || (16 != BMAlign_TO16(15)))
        {
            BMTest_ERRLOGBREAKEX("Fail in "
                "(16 != BMAilgn_TO16(1)) || "
                "(16 != BMAlign_TO16(15))");
        }
        if ((64 != BMAlign_TO16(49)) || (64 != BMAlign_TO16(63)))
        {
            BMTest_ERRLOGBREAKEX("Fail in "
                "(64 != BMAlign_TO16(49))"
                "(64 != BMAlign_TO16(63))");
        }
        status = BMStatus_SUCCESS;
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMDefsUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        if (BMStatus_SUCCESS != (status = BMAlign()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMAlign()");
        }
        if (BMStatus_SUCCESS != (status = BMAlign16()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMAlign16()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}