#include "BMRingBuf.h"
#include "BMTest.h"
#define RB_SIZE     8
#define POOL_SIZE_S 3
#define POOL_SIZE_L 27

static BMStatus_t BMRingBuf_TestRb();
static BMStatus_t BMRingBuf_TestPool();
static BMStatus_t BMRingBuf_TestPoolRb();

BMStatus_t BMRingBufUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        if (BMStatus_SUCCESS !=
            (status = BMRingBuf_TestRb()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMRingBuf_TestRb()");
        }
        if (BMStatus_SUCCESS !=
            (status = BMRingBuf_TestPool()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMRingBuf_TestPool()");
        }
        if (BMStatus_SUCCESS !=
            (status = BMRingBuf_TestPoolRb()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMRingBuf_TestPoolRb()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}


static BMStatus_t TestRbCore(BMRingBuf_pt rb)
{
    static const uint8_t TEST_BYTES[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    BMStatus_t status = BMStatus_SUCCESS;
    uint8_t buf[RB_SIZE];
    const uint8_t* bytes = TEST_BYTES;
    for (int k = 0; k < 4; k++)
    {
        uint16_t count = 0;
        if ((0 == k) && (rb->base.rdidx || rb->base.wridx))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("rb is not cleared.");
        }
        while (BMStatus_SUCCESS == BMRingBuf_Put(rb, bytes))
        {
            bytes++;
            count++;
        }
        if (count != (RB_SIZE - 1))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Fail in BMRingBuf_Put()");
        }
        uint8_t *bufptr = buf;
        for (uint16_t i = 0; i < count; i++)
        {
            status = BMRingBuf_Get(rb, bufptr++);
            if (BMStatus_SUCCESS != status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMRingBuf_Get()");
            }
        }
        if (status) break;
        if (strncmp(buf, TEST_BYTES + k * (RB_SIZE - 1), count))
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("IO bytes mismatch");
        }
    }
    BMTest_ENDFUNC(status);
    return status;
}

static BMStatus_t BMRingBuf_TestRb()
{
    BMRingBuf_SDECL(rb, RB_SIZE);
    BMStatus_t status = BMStatus_SUCCESS;
    BMRingBuf_INIT(&rb);
    do {
        if (BMStatus_SUCCESS !=
            (status = TestRbCore(&rb)))
        {
            BMTest_ERRLOGBREAKEX("Fail in TestRbCore()");
        }
    } while (0);
    BMRingBuf_DEINIT(&rb);
    BMTest_ENDFUNC(status);
    return status;
}

static BMStatus_t BMRingBuf_TestPool()
{
    BMRingBufPool_SDECL(pool, POOL_SIZE_L, RB_SIZE);
    BMStatus_t status = BMStatus_SUCCESS;
    BMRingBufPool_INIT(&pool);
    BMRingBuf_pt rbs[BMAlign_TO16(POOL_SIZE_L)];
    do {
        int i = 0;
        for (; i < BMArray_SIZE(rbs); i++)
        {
            rbs[i] = BMRingBufPool_Get(&pool);
            if (NULL == rbs[i])
            {
                break;
            }
        }
        if (POOL_SIZE_L != i)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("(POOL_SIZE_L != i)");
        }
        do {
            --i;
            status = BMRingBufPool_Return(&pool, rbs[i]);
            if (status)
            {
                BMTest_ERRLOGBREAKEX("Fail in BMRingBufPool_Return()");
            }
        } while (i);
        i++;
        status = BMRingBufPool_Return(&pool, rbs[i]);
        if (BMStatus_NOTFOUND != status)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Invalid ringbuffer pointer not detected.");
        }
        status = BMStatus_SUCCESS;
    } while (0);
    BMRingBufPool_DEINIT(&pool);
    BMTest_ENDFUNC(status);
    return status;
}

static BMStatus_t BMRingBuf_TestPoolRb()
{
    BMRingBufPool_SDECL(pool, POOL_SIZE_S, RB_SIZE);
    BMStatus_t status = BMStatus_SUCCESS;
    BMRingBufPool_INIT(&pool);
    do {
        BMRingBuf_pt rb = BMRingBufPool_Get(&pool);
        if (BMStatus_SUCCESS != (status = TestRbCore(rb)))
        {
            BMTest_ERRLOGBREAKEX("Fail in TestRbCore()");
        }
        if (BMStatus_SUCCESS != 
            (status = BMRingBufPool_Return(&pool, rb)))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMRingBufPool_Return()");
        }
        rb = BMRingBufPool_Get(&pool);
        if (BMStatus_SUCCESS != (status = TestRbCore(rb)))
        {
            BMTest_ERRLOGBREAKEX("Fail in TestRbCore()");
        }
        if (BMStatus_SUCCESS != 
            (status = BMRingBufPool_Return(&pool, rb)))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMRingBufPool_Return()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}
