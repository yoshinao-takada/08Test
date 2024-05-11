#include "BMPoolBase.h"
#include "BMTest.h"
#define POOL_SIZE 99

// Test1. Find upto empty
BMStatus_t BMPoolBase_FindUptoEmpty()
{
    BMPoolBase_DECL(pool, POOL_SIZE);
    BMPoolBase_INIT(&pool);
    BMStatus_t status = BMStatus_SUCCESS;
    int16_t indices[128];
    int16_t* index = indices;
    const int16_t* end = indices + BMArray_SIZE(indices);
    BMArray_FILL(index, end, -1);    
    do {
        // Step 1: Test _FindAvailable
        for (index = indices; index != end; index++)
        {
            *index = BMPoolBase_FindAvailable(&pool);
            if (*index < 0) break;
        }
        if ((index - indices) != POOL_SIZE)
        {
            BMTest_ERRLOGBREAKEX("((index - indices) != POOL_SIZE)");
        }

        // Step 2: Confirm the acquired indices by _FindAvailable.
        for (int i = 0; i < POOL_SIZE; i++)
        {
            if (indices[i] != i)
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("indices[%d] != %d", i, i);
            }
        }
    } while (0);
    BMPoolBase_DEINIT(&pool);
    BMTest_ENDFUNC(status);
    return status;
}

// Test2. Find upto empty and return invalid pointers
BMStatus_t BMPoolBase_FindAndReturn()
{
    BMPoolBase_DECL(pool, POOL_SIZE);
    BMPoolBase_INIT(&pool);
    int16_t indices[128];
    int16_t* index  =indices;
    const int16_t* const end = indices + BMArray_SIZE(indices);
    BMArray_FILL(index, end, -1);
    BMStatus_t status = BMStatus_FAILURE;
    do {
        // Step 1: Apply _FindAvailable to all the elements.
        for (index = indices; index != end; index++)
        {
            *index = BMPoolBase_FindAvailable(&pool);
            if (*index < 0) break;
        }

        // Step 2: Return the indices acquired in step 1.
        for (index = indices; *index != -1; index++)
        {
            if (BMStatus_SUCCESS !=
                (status = BMPoolBase_Return(&pool, *index)))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMPoolBase_Return()");
            }
        }
        if ((index - indices) != POOL_SIZE)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("((index - indices) != POOL_SIZE)");
        }

        // Step 3: Confirm failure by double _Return().
        index = indices;
        status = BMPoolBase_Return(&pool, *index);
        if (status == BMStatus_SUCCESS)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("Double _Return() was accepted.");
        }

        // Step 4: check if poll_used[] are all zeros.
        for (int i = 0; i < BMArray_SIZE(pool_used); i++)
        {
            if (pool_used[i])
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX(
                    "(pool_used[%d]) = %u", i, pool_used[i]);
            }
        }
        status = BMStatus_SUCCESS;
    } while (0);
    BMPoolBase_DEINIT(&pool);
    BMTest_ENDFUNC(status);
    return status;
}

BMStatus_t BMPoolBaseUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        if (BMStatus_SUCCESS != (status = BMPoolBase_FindUptoEmpty()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMPoolBase_FindUptoEmpty()");
        }
        if (BMStatus_SUCCESS != (status = BMPoolBase_FindAndReturn()))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMPoolBase_FindAndReturn()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}