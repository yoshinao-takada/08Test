#include "BMEv.h"
#include "BMTest.h"
#define POOL_SIZE   33
#define EVBUF_SIZE  48
static BMStatus_t BMEvPool_DECLUT()
{
    BMStatus_t status = BMStatus_FAILURE;
    do {
        BMEvPool_DECL(pool, POOL_SIZE);
        size_t poolsize = BMArray_SIZE(pool_ev);
        if ((poolsize != POOL_SIZE) ||
            (pool.ev != pool_ev))
        {
            BMTest_ERRLOGBREAKEX("Fail in "
                "(poolsize != POOL_SIZE) || "
                "(pool.ev != pool_ev)");
        }

        BMEvPool_INIT(&pool);
        int result = pthread_spin_trylock(&(pool.base.lock));
        if (result)
        {
            BMTest_ERRLOGBREAKEX("Fail in pthread_spin_trylock() "
                "after BMEvPool_INIT(&pool)");
        }
        result = pthread_spin_unlock(&(pool.base.lock));
        if (result)
        {
            BMTest_ERRLOGBREAKEX("Fail in pthread_spin_unlock() "
                "after BMEvPool_INIT(&pool) and trylock");
        }
        BMEvPool_DEINIT(&pool);
        status = BMStatus_SUCCESS;
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

BMEvPool_SDECL(static_pool, POOL_SIZE);

static BMStatus_t BMEvPool_SDECLUT()
{
    BMStatus_t status = BMStatus_FAILURE;
    do {
        size_t poolsize = BMArray_SIZE(static_pool_ev);
        if ((poolsize != POOL_SIZE) ||
            (static_pool.ev != static_pool_ev))
        {
            BMTest_ERRLOGBREAKEX("Fail in "
                "(poolsize != POOL_SIZE) || "
                "(pool.ev != pool_ev)");
        }

        BMEvPool_INIT(&static_pool);
        int result = pthread_spin_trylock(&(static_pool.base.lock));
        if (result)
        {
            BMTest_ERRLOGBREAKEX("Fail in pthread_spin_trylock() "
                "after BMEvPool_INIT(&static_pool)");
        }
        result = pthread_spin_unlock(&(static_pool.base.lock));
        if (result)
        {
            BMTest_ERRLOGBREAKEX("Fail in pthread_spin_unlock() "
                "after BMEvPool_INIT(&static_pool) and trylock");
        }
        BMEvPool_DEINIT(&static_pool);
        status = BMStatus_SUCCESS;
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMEvPool_GetAndReturn()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMEvPool_DECL(evpool, POOL_SIZE);
    BMEvPool_INIT(&evpool);
    BMEv_pt evpbuf[EVBUF_SIZE];
    BMEv_pt* begin = evpbuf;
    BMEv_pt* const end = evpbuf + EVBUF_SIZE;
    BMArray_FILL(begin, end, NULL);
    int offs = 0;
    do {
        BMEvId_t id = 0x7fff;
        // Step 1: Get event ptrs
        for (begin = evpbuf; begin != end; begin++,offs++)
        {
            *begin = BMEvPool_Get(&evpool, id--, begin);
            if (*begin == NULL) 
            {
                break;
            }
        }
        if ((begin - evpbuf) != POOL_SIZE)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("((begin - evpool_ev) != POOL_SIZE)");
        }

        // Step 2: Confirm the events have been correctly initialized.
        for (begin = evpbuf, offs = 0; *begin; begin++, offs++)
        {
            status = BMStatus_FAILURE;
            if (*begin != (evpool_ev + offs))
            {
                BMTest_ERRLOGBREAKEX("(*begin != (evpool_ev + offs))");
            }
            if ((*begin)->id != (0x7fff - offs))
            {
                BMTest_ERRLOGBREAKEX("(*begin)->id != (0x7fff - offs)");
            }
            if ((*begin)->listeners != 0)
            {
                BMTest_ERRLOGBREAKEX("(*begin)->listeners != 0");
            }
            if ((*begin)->param != begin)
            {
                BMTest_ERRLOGBREAKEX("(*begin)->param != begin");
            }
            status = BMStatus_SUCCESS;
        }
        if (status) break;

        // Step 3: All the events are returned.
        for (begin = evpbuf; *begin != NULL; begin++)
        {
            status = BMEvPool_Return(&evpool, *begin);
            if (status != BMStatus_SUCCESS)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMEvPool_Return()");
            }
        }
        if (status) break;

        // Step 4: An event is gotten, is listened to, and return to result failure.
        begin = evpbuf;
        *begin = BMEvPool_Get(&evpool, 0, NULL);
        (*begin)->listeners++;
        if (BMStatus_SUCCESS == BMEvPool_Return(&evpool, *begin))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("BMEvPool_Return accepted unreleased event.");
        }
        (*begin)->listeners--;
        if (BMStatus_SUCCESS != BMEvPool_Return(&evpool, *begin))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("BMEvPool_Return failed.");
        }
    } while (0);
    BMEvPool_DEINIT(&evpool);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMEvUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        if (BMStatus_SUCCESS != (status = BMEvPool_DECLUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMEvPool_DECLUT()");
        }
        if (BMStatus_SUCCESS != (status = BMEvPool_SDECLUT()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMEvPool_SDECLUT()");
        }
        if (BMStatus_SUCCESS != (status = BMEvPool_GetAndReturn()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMEvPool_GetAndReturn()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}