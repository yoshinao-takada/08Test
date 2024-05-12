#include "BMEvQ.h"
#include "BMTest.h"
#define QSIZE       4
#define EVPOOL_SIZE 32
#define EVPQOOL_SIZE 16

BMEvPool_SDECL(evpool, EVPOOL_SIZE);
BMEvQPool_SDECL(evqpool, EVPQOOL_SIZE, QSIZE);

BMStatus_t BMEvQ_PutAndGet(BMEvQ_pt evqptr, BMEvId_t id0)
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        int count = 0;
        // Step 1: fill the event queue
        for (; count < (evqptr->base.count - 1); count++) 
        {
            BMEv_pt evptr = 
                BMEvPool_Get(&evpool, id0 + (BMEvId_t)count, NULL);
            if (!evptr)
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("Fail in BMEvPool_Get()");
            }
            status = BMEvQ_Put(evqptr, evptr);
            if (status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMEvQ_Put()");
            }
        }
        if (status) break;
        if (count != (QSIZE - 1))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("count != (QSIZE - 1)");
        }

        // Step 2: retrieve the events from the quque upto empty.
        for (int count2 = 0; count2 < count; count2++)
        {
            BMEv_pt evptr = NULL;
            status = BMEvQ_Get(evqptr, &evptr);
            if (status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMEvQ_Get()");
            }
            if (evptr->id != (id0 + count2))
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("evptr->id != (id0 + count2)");
            }
            evptr->listeners--;
            status = BMEvPool_Return(&evpool, evptr);
            if (status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMEvPool_Return()");
            }
        }
        if (status) break;

        // Step 3: Check if the event queue is empty.
        if (evqptr->base.rdidx != evqptr->base.wridx)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("(evqptr->base.rdidx != evqptr->base.wridx)");
        }
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
            status = BMEvQ_PutAndGet(evqs[i], (BMEvId_t)(i * 16));
        }
        if (status) break;
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