#include "BMEvQ.h"
#include "BMTest.h"
#define QSIZE       4
#define EVPOOL_SIZE 32
#define EVPQOOL_SIZE 16

BMEvPool_SDECL(evpool, EVPOOL_SIZE);
BMEvQPool_SDECL(evqpool, EVPQOOL_SIZE, QSIZE);

BMStatus_t BMEvQ_PutAndGet(BMEvQ_pt evqptr)
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {

    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMEvQ_PutAndGetEx()
{
    BMEvQ_DECL(evq, QSIZE);
    BMEvQ_INIT(&evq);
    BMStatus_t status = BMStatus_SUCCESS;
    BMEvQ_pt evqs[3] = { NULL, NULL, NULL };
    do {
        for (int i = 0; i < BMArray_SIZE(evqs); i++)
        {
            status = BMStatus_FAILURE;
            evqs[i] = BMEvQPool_Get(&evqpool);
            if (!evqs[i])
            {
                BMTest_ERRLOGBREAKEX("Fail in BMEvQPool_Get(), i = %d", i);
            }
            if (evqs[i]->base.count != QSIZE)
            {
                BMTest_ERRLOGBREAKEX("(evqs[%d]->base.count != QSIZE)", i);
            }
            if ((evqs[i]->base.wridx) || (evqs[i]->base.rdidx))
            {
                BMTest_ERRLOGBREAKEX(
                    "(evqs[%d]->base.wridx) || (evqs[%d]->base.rdidx)", i, i);
            }
            status = BMStatus_SUCCESS;
        }
        if (status) break;
        status = BMEvQ_PutAndGet(evqs[1]);
        if (status)
        {
            BMTest_ERRLOGBREAKEX("Fail in BMEvQ_PutAndGet()");
        }
    } while (0);
    BMEvQ_DEINIT(&evq);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMEvQUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMEvPool_INIT(&evpool);
    BMEvQPool_INIT(&evqpool);
    do {
        status = BMEvQ_PutAndGetEx();
        if (BMStatus_SUCCESS != status)
        {
            BMTest_ERRLOGBREAKEX("Fail in BMEvQ_PutAndGet()");
        }
    } while (0);
    BMEvPool_DEINIT(&evqpool);
    BMTest_ENDFUNC(status);
    return status;
}