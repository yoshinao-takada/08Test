#define BMTEST_MAIN_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "BMDefs.h"
#include "BMTest.h"

BMStatus_t BMDefsUT();
BMStatus_t BMQBaseUT();
BMStatus_t BMPoolBaseUT();
BMStatus_t BMEvUT();
BMStatus_t BMEvQUT();
BMStatus_t BMTickUT();
BMStatus_t BMRingBufUT();
BMStatus_t BMDLNodeUT();

int HasFlag(int argc, const char* argv[], const char* strFlag)
{
    return (argc > 1) && (!strcasecmp(argv[1], strFlag)) ? 1: 0;
}

#define HAS_TICK(_argc, _argv) HasFlag(_argc, _argv, "TICK")

int main(int argc, const char* argv[])
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMTest_START;
    do {
        if (HAS_TICK(argc, argv))
        {
            status = BMTickUT();
            if (status) BMTest_ERRLOGBREAKEX("Fail in BMTickUT()");
            break;
        }
        if (BMStatus_SUCCESS != (status = BMDefsUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMDefsUT()");
        }
        if (BMStatus_SUCCESS != (status = BMQBaseUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMQBaseUT()");
        }
        if (BMStatus_SUCCESS != (status = BMPoolBaseUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMPoolBaseUT()");
        }
        if (BMStatus_SUCCESS != (status = BMEvUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMEvUT()");
        }
        if (BMStatus_SUCCESS != (status = BMEvQUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMEvQUT()");
        }
        if (BMStatus_SUCCESS != (status = BMRingBufUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMRingBufUT()");
        }
        if (BMStatus_SUCCESS != (status = BMDLNodeUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMDLNodeUT()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    BMTest_END;
    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}