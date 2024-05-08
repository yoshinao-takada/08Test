#define BMTEST_MAIN_C
#include <stdio.h>
#include <stdlib.h>
#include "BMDefs.h"
#include "BMTest.h"

BMStatus_t BMDefsUT();
BMStatus_t BMQBaseUT();
BMStatus_t BMPoolBaseUT();
BMStatus_t BMEvUT();
BMStatus_t BMEvQUT();


int main(int argc, const char* argv[])
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMTest_START;
    do {
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
    } while (0);
    BMTest_ENDFUNC(status);
    BMTest_END;
    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}