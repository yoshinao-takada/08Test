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

BMStatus_t BMDispatcherUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {

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