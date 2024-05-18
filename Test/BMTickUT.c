#include "BMTick.h"
#include "BMTest.h"
#define  POOL_SIZE  4
#define  EVQ_SIZE   16
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

// Core function testing BMDispatchher_t instance.
static BMStatus_t BMDispatcherUTCore(BMDispatcher_pt dispptr)
{
    static const int DISP_IT = 3, DISP_COUNT = 5;
    static const uint16_t EXP_CT0[] = { 5, 4, 3, 2, 1, 0, 0, 0, 0 };
    static const uint16_t EXP_CT1[] = { 5, 4, 3, 2, 1, 3, 2, 1 };
    BMStatus_t status = BMStatus_SUCCESS;
    int test_ct = 0; // test counter for TestHandler0
    do {
        // Step 1: observe single-shot operation
        BMDispatcher_SET(dispptr, DISP_COUNT, 0, &test_ct, TestHandler0);
        for (int i = 0; i < BMArray_SIZE(EXP_CT0); i++)
        {
            if (EXP_CT0[i] != dispptr->count)
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("(EXP_CT0[%d] != dispptr->count)", i);
            }
            if (BMStatus_SUCCESS != (status = BMDispatcher_Dispatch(dispptr)))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDispatcher_Dispatch()");
            }
        }

        // Step 2: observe interval operation without errors
        test_ct = 0;
        BMDispatcher_SET(dispptr, DISP_COUNT, DISP_IT, &test_ct, TestHandler0);
        for (int i = 0; i < BMArray_SIZE(EXP_CT1); i++)
        {
            if (EXP_CT1[i] != dispptr->count)
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("(EXP_CT1[%d] != dispptr->count)", i);
            }
            if (BMStatus_SUCCESS != (status = BMDispatcher_Dispatch(dispptr)))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDispatcher_Dispatch()");
            }
        }

        // Step 3: observe interval operation with errors
        BMDispatcher_SET(dispptr, DISP_COUNT, DISP_IT, &test_ct, TestHandler0);
        for (int i = 0; i < BMArray_SIZE(EXP_CT1); i++)
        {
            if (EXP_CT1[i] != dispptr->count)
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("(EXP_CT1[%d] != dispptr->count)", i);
            }
            status = BMDispatcher_Dispatch(dispptr);
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
BMStatus_t BMDispatcherUT()
{
    BMDispatcher_t disp;
    BMStatus_t status = BMDispatcherUTCore(&disp);
    return status;
}

#pragma region DISPATCH_OPERATION_TEST_FUNCTION_AND_DATA
static void GetDispatchers(BMDispatcher_pt *dispptrs, BMDispatchers_pt disppool)
{
    for (int i = 0; i < disppool->base.count; i++)
    {
        dispptrs[i] = BMDispatchers_Get(disppool);
    }
}

static void ReturnDispatchers(BMDispatcher_pt *dispptrs, BMDispatchers_pt disppool)
{
    for (int i = 0; i < disppool->base.count; i++)
    {
        BMDispatchers_Return(disppool, dispptrs[i]);
    }
}

static void* handler0(void* param)
{
    int* ipparam = (int*)param;
    (*ipparam)++;
    return param;
}

static int params[POOL_SIZE] = { 0, 0, 0, 0 };
static const int EXPECTED_PARAMS[POOL_SIZE] = { 27, 1, 13, 0 };

static const BMThread_f handlers[POOL_SIZE] =
    { handler0, handler0, handler0, handler0 };

static const uint16_t ctr_settings[POOL_SIZE][2] =
{
    { 2, 2 }, { 4, 0 }, { 4, 4 }, { 0, 0 }
};

static void SetDispatchers(BMDispatcher_pt *dispptrs)
{
    for (int i = 0; i < POOL_SIZE; i++)
    {
        BMDispatcher_SET(dispptrs[i],
            ctr_settings[i][0], ctr_settings[i][1],
            (void*)&params[i], handlers[i]);
    }
}

static void ClearDispatchers(BMDispatcher_pt *dispptrs)
{
    for (int i = 0; i < POOL_SIZE; i++)
    {
        BMDispatcher_CLEAR(dispptrs[i]);
    }
}
#pragma endregion DISPATCH_OPERATION_TEST_FUNCTION_AND_DATA

BMStatus_t BMDispatchersUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMEvQ_SDECL(evq, EVQ_SIZE);
    BMDispatchers_SDECL(disps, POOL_SIZE);
    BMEv_t ev = { BMEvId_TICK, 0, NULL };
    BMEvQ_INIT(&evq);
    BMDispatchers_INIT(&disps, &evq);
    BMDispatcher_pt dispptrs[POOL_SIZE];
    do {
        // Step 1: Get dispatchers from the pool
        #pragma region STEP1
        for (int i = 0; i < POOL_SIZE; i++)
        {
            if (NULL == (dispptrs[i] = BMDispatchers_Get(&disps)))
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("Fail in BMDispatchers_Get(), i = %d", i);
            }
            if (BMStatus_SUCCESS !=
                (status = BMDispatcherUTCore(dispptrs[i])))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDispatcherUTCore(%d)", i);
            }
        }
        if (status) break;
        for (int i = 0; i < POOL_SIZE; i++)
        {
            if (BMStatus_SUCCESS != 
                (status = BMDispatchers_Return(&disps, dispptrs[i])))
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("Fail in BMDispatchers_Return(), i = %d", i);
            }
        }
        #pragma endregion STEP1
        // Step 2: enqueue events and crunch them
        #pragma region STEP2
        GetDispatchers(dispptrs, &disps);
        SetDispatchers(dispptrs);
        while (BMStatus_SUCCESS == BMEvQ_Put(&evq, &ev)) ;
        if (ev.listeners != (EVQ_SIZE - 1))
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("(ev.listeners != (EVQ_SIZE - 1))");
        }
        while (evq.base.wridx != evq.base.rdidx)
        {
            if (BMStatus_SUCCESS != BMDispatchers_Crunch(&disps))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDispatchers_Crunch()");
            }
        }
        if (0 != ev.listeners)
        {
            status = BMStatus_INVALID;
            BMTest_ERRLOGBREAKEX("(0 != ev.listeners)");
        }
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                BMEvQ_Put(&evq, &ev);
            }
            while (evq.base.wridx != evq.base.rdidx)
            {
                if (BMStatus_SUCCESS != BMDispatchers_Crunch(&disps))
                {
                    BMTest_ERRLOGBREAKEX("Fail in BMDispatchers_Crunch()");
                }
            }
        }
        ClearDispatchers(dispptrs);
        ReturnDispatchers(dispptrs, &disps);
        for (int i = 0; i < POOL_SIZE; i++)
        {
            if (EXPECTED_PARAMS[i] != params[i])
            {
                status = BMStatus_INVALID;
                BMTest_ERRLOGBREAKEX(
                    "(EXPECTED_PARAMS[%d] != params[%d])", i, i);
            }
        }
        #pragma endregion STEP2
    } while (0);
    BMEvQ_DEINIT(&evq);
    BMDispatchers_DEINIT(&disps);
    BMTest_ENDFUNC(status);
    return status;
}

#pragma region INTEGRATED_TICK_TEST_DATA_AND_HANDLER
static int IntegratedCtrs[STATIC_DISPATCHER_COUNT] = { 0, 0, 0, 0 };
static BMDispatcher_pt IntegratedDispptrs[STATIC_DISPATCHER_COUNT];
static uint16_t IntegratedPrescales[STATIC_DISPATCHER_COUNT][2] =
{
    { 100, 150 }, { 150, 150 }, { 100, 300 }, { 250, 300 }
};
static void* IntegratedDispHandler(void* param) 
{
    int* ip = (int*)param;
    (*ip)++;
    BMTest_PRINTF("%s, %d\n", __FUNCTION__, *ip);
    return param;
}

static BMStatus_t SetIntegratedDispatchers()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        for (int i = 0; i < STATIC_DISPATCHER_COUNT; i++)
        {
            BMDispatcher_pt dispptr = BMDispatchers_SGet();
            if (NULL == dispptr)
            {
                BMTest_ERRLOGBREAKEX(
                    "Fail in BMDispatchers_SGet(), i = %d", i);
            }
            BMDispatcher_SET(dispptr,
                IntegratedPrescales[i][0], IntegratedPrescales[i][1],
                (void*)&IntegratedCtrs[i], IntegratedDispHandler);
        }
    } while (0);
    return status;
}

static int actctr = 0;
static BMActCtx_t actctx = { &actctr, NULL, BMStatus_SUCCESS };
static void act(BMActCtx_pt ctx)
{
    int* intptr_ctx = (int*)actctx.ctx;
    if ((*intptr_ctx)++ >= 10)
    {
        *intptr_ctx = 0;
        BMTest_PRINTF("%s\n", __FUNCTION__);
    }
    if (IntegratedCtrs[0] >= 15)
    {
        ctx->stat = BMActStatus_STOP;
        BMTest_PRINTF("%s terminates.\n", __FUNCTION__);
    }
}
#pragma endregion INTEGRATED_TICK_TEST_DATA_AND_HANDLER

BMStatus_t BMTick_IntegratedUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMEvQ_SDECL(evq, EVQ_SIZE);
    BMEvQ_INIT(&evq);
    BMEv_t ev = { BMEvId_TICK, 0, NULL };
    do {
        if (BMStatus_SUCCESS != 
            (status = BMTick_Init(10, &evq)))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMTick_Init()");
        }
        if (BMStatus_SUCCESS != 
            (status = SetIntegratedDispatchers(10, &evq)))
        {
            BMTest_ERRLOGBREAKEX("Fail in SetIntegratedDispatchers()");
        }
        if (BMStatus_SUCCESS !=
            (status = BMTick_Main(act, &actctx)))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMTick_Main()");
        }
        if (BMStatus_SUCCESS != 
            (status = BMTick_Deinit(10, &evq)))
        {
            BMTest_ERRLOGBREAKEX("Fail in BMTick_Deinit()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}