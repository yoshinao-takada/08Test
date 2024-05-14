#include "BMTick.h"
#include "BMTest.h"

BMStatus_t BMDispatcherUT();
BMStatus_t BMDispatchersUT();
BMStatus_t BMTick_IntegratedUT();

BMStatus_t BMTickUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        if (BMStatus_SUCCESS != (status = BMDispatcherUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMDispatcherUT()");
        }
        if (BMStatus_SUCCESS != (status = BMDispatchersUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMDispatchersUT()");
        }
        if (BMStatus_SUCCESS != (status = BMTick_IntegratedUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMTick_IntegratedUT()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

static void* TestHandler0(void* par) 
{
    int* ipar = (int*)par;
    (*ipar)++;
    return (*ipar == 4) ? NULL : par; // return error if (*ipar == 4)
}

BMStatus_t BMDispatcherUT()
{
    static const int DISP_IT = 3, DISP_COUNT = 5;
    static const uint16_t EXP_CT0[] = { 5, 4, 3, 2, 1, 0, 0, 0, 0 };
    static const uint16_t EXP_CT1[] = { 5, 4, 3, 2, 1, 3, 2, 1 };
    BMStatus_t status = BMStatus_SUCCESS;
    BMDispatcher_t disp;
    int test_ct = 0; // test counter for TestHandler0
    do {
        // Step 1: observe single-shot operation
        BMDispatcher_SET(&disp, DISP_COUNT, 0, &test_ct, TestHandler0);
        for (int i = 0; i < BMArray_SIZE(EXP_CT0); i++)
        {
            if (EXP_CT0[i] != disp.count)
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("(EXP_CT0[%d] != disp.count)", i);
            }
            if (BMStatus_SUCCESS != (status = BMDispatcher_Dispatch(&disp)))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDispatcher_Dispatch()");
            }
        }

        // Step 2: observe interval operation without errors
        test_ct = 0;
        BMDispatcher_SET(&disp, DISP_COUNT, DISP_IT, &test_ct, TestHandler0);
        for (int i = 0; i < BMArray_SIZE(EXP_CT1); i++)
        {
            if (EXP_CT1[i] != disp.count)
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("(EXP_CT1[%d] != disp.count)", i);
            }
            if (BMStatus_SUCCESS != (status = BMDispatcher_Dispatch(&disp)))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDispatcher_Dispatch()");
            }
        }

        // Step 3: observe interval operation with errors
        BMDispatcher_SET(&disp, DISP_COUNT, DISP_IT, &test_ct, TestHandler0);
        for (int i = 0; i < BMArray_SIZE(EXP_CT1); i++)
        {
            if (EXP_CT1[i] != disp.count)
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("(EXP_CT1[%d] != disp.count)", i);
            }
            status = BMDispatcher_Dispatch(&disp);
            if (i == (BMArray_SIZE(EXP_CT1)-1))
            { // an error occurs at the last count.
                if (!BMStatus_IS_ERR(status))
                {
                    status = BMStatus_FAILURE;
                    BMTest_ERRLOGBREAKEX
                    ("Fail to detect error in BMDispatcher_Dispatch()");
                }
                else
                {
                    BMStatus_RESET(&status);
                }
            }
            else
            { // no error occurs at counts except the last one.
                if (BMStatus_IS_ERR(status))
                {
                    BMTest_ERRLOGBREAKEX
                    ("Fail in BMDispatcher_Dispatch()");
                }
            }
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMDispatchersUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {

    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMTick_IntegratedUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {

    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}