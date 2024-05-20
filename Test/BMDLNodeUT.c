#include "BMDLNode.h"
#include "BMTest.h"

static BMStatus_t AddNextGetPrev();
static BMStatus_t AddPrevGetNext();
static BMStatus_t Find();
static BMStatus_t PoolSGetSReturn();

static BMDLNode_t TestNodes[] =
{
    { NULL, NULL, (void*)0 },
    { NULL, NULL, (void*)1 },
    { NULL, NULL, (void*)2 },
    { NULL, NULL, (void*)3 },
    { NULL, NULL, (void*)4 },
};

BMStatus_t BMDLNodeUT()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        if (BMStatus_SUCCESS != (status = AddNextGetPrev()))
        {
            BMTest_ERRLOGBREAKEX("Fail in AddNextGetPrev()");
        }
        if (BMStatus_SUCCESS != (status = AddPrevGetNext()))
        {
            BMTest_ERRLOGBREAKEX("Fail in AddPrevGetNext()");
        }
        if (BMStatus_SUCCESS != (status = Find()))
        {
            BMTest_ERRLOGBREAKEX("Fail in Find()");
        }
        if (BMStatus_SUCCESS != (status = PoolSGetSReturn()))
        {
            BMTest_ERRLOGBREAKEX("Fail in PooSGetSReturn()");
        }
    } while (0);
    BMTest_ENDFUNC(status);
    return status;
}

static BMStatus_t AddNextGetPrev()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMDLNode_DECLANCHOR(anchor);
    pthread_spin_init(&anchor.lock, PTHREAD_PROCESS_PRIVATE);
    do {
        for (int i = 0; i < BMArray_SIZE(TestNodes); i++)
        {
            BMDLNode_AddNext(&anchor, &TestNodes[i]);
        }
        if (BMArray_SIZE(TestNodes) != BMDLNode_Count(&anchor))
        {
            BMTest_ERRLOGBREAKEX(
                "(BMArraySize(TestNodes) != BMDLNode_Count(&anchor))");
        }
        for (int i = 0; i < BMArray_SIZE(TestNodes); i++)
        {
            BMDLNode_pt nodeptr = BMDLNode_GetPrev(&anchor);
            if (nodeptr != (TestNodes + i))
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("(nodeptr != (TestNodes + %d))", i);
            }
        }
        if (status) break;
        if (0 != BMDLNode_Count(&anchor))
        {
            BMTest_ERRLOGBREAKEX("(0 != BMDLNode_Count(&anchor))");
        }
    } while (0);
    pthread_spin_destroy(&anchor.lock);
    BMTest_ENDFUNC(status);
    return status;
}

static BMStatus_t AddPrevGetNext()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMDLNode_DECLANCHOR(anchor);
    pthread_spin_init(&anchor.lock, PTHREAD_PROCESS_PRIVATE);
    do {
        for (int i = 0; i < BMArray_SIZE(TestNodes); i++)
        {
            BMDLNode_AddPrev(&anchor, &TestNodes[i]);
        }
        if (BMArray_SIZE(TestNodes) != BMDLNode_Count(&anchor))
        {
            BMTest_ERRLOGBREAKEX(
                "(BMArraySize(TestNodes) != BMDLNode_Count(&anchor))");
        }
        for (int i = 0; i < BMArray_SIZE(TestNodes); i++)
        {
            BMDLNode_pt nodeptr = BMDLNode_GetNext(&anchor);
            if (nodeptr != (TestNodes + i))
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("(nodeptr != (TestNodes + %d))", i);
            }
        }
        if (status) break;
        if (0 != BMDLNode_Count(&anchor))
        {
            BMTest_ERRLOGBREAKEX("(0 != BMDLNode_Count(&anchor))");
        }
    } while (0);
    pthread_spin_destroy(&anchor.lock);
    BMTest_ENDFUNC(status);
    return status;
}

static int zeromatch(const void* pva, const void* pvb)
{
    return ((int)(uint64_t)pva - (int)(uint64_t)pvb);
}

static BMStatus_t Find()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMDLNode_DECLANCHOR(anchor);
    pthread_spin_init(&anchor.lock, PTHREAD_PROCESS_PRIVATE);
    do {
        for (int i = 0; i < BMArray_SIZE(TestNodes); i++)
        {
            BMDLNode_AddNext(&anchor, &TestNodes[i]);
        }
        BMDLNode_pt found = BMDLNode_Find(
            &anchor, (const void*)1, zeromatch);
        if (found && (found->payload != TestNodes[1].payload))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("(found->payload != TestNodes[1].payload)");
        }
        found = BMDLNode_Find(
            &anchor, (const void*)5, zeromatch);
        if (found != NULL)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("(found != NULL)");
        }
    } while (0);
    pthread_spin_destroy(&anchor.lock);
    BMTest_ENDFUNC(status);
    return status;
}

static BMStatus_t PoolSGetSReturn()
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMDLNode_pt nodes[BMDLNodePool_STATIC_SIZE];
    BMDLNode_pt exnode = NULL;
    BMDLNode_DECLANCHOR(anchor);
    BMDLNode_INITANCHOR(&anchor);
    BMDLNodePool_SInit();
    do {
        for (int i = 0; i < BMArray_SIZE(nodes); i++)
        {
            nodes[i] = BMDLNodePool_SGet();
            if (!nodes[i])
            {
                status = BMStatus_FAILURE;
                BMTest_ERRLOGBREAKEX("Fail in BMDLNodePool_SGet()");
            }
        }
        if (status) break;
        exnode = BMDLNodePool_SGet();
        if (exnode != NULL)
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX(
                "BMDLNodePool_SGet() did not detect empty.");
        }
        for (int i = 0; i < BMArray_SIZE(nodes); i++)
        {
            BMDLNode_AddNext(&anchor, nodes[i]);
        }
        if (BMArray_SIZE(nodes) != BMDLNode_Count(&anchor))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX(
                "(BMArray_SIZE(nodes) != BMDLNode_Count(&anchor))");
        }
        for (int i = 0; i < BMArray_SIZE(nodes); i++)
        {
            status = (nodes[i] == BMDLNode_GetPrev(&anchor)) ?
                BMStatus_SUCCESS : BMStatus_FAILURE;
            if (status)
            {
                BMTest_ERRLOGBREAKEX("(nodes[i] == BMDLNode_GetPrev(&anchor))");
            }
        }
        if (status) break;
        if (0 != BMDLNode_Count(&anchor))
        {
            status = BMStatus_FAILURE;
            BMTest_ERRLOGBREAKEX("(0 != BMDLNode_Count(&anchor))");
        }
        for (int i = 0; i < BMArray_SIZE(nodes); i++)
        {
            if (BMStatus_SUCCESS != (status = BMDLNodePool_SReturn(nodes[i])))
            {
                BMTest_ERRLOGBREAKEX("Fail in BMDLNodePool_SReturn()");
            }
        }
        if (status) break;
        if (BMStatus_SUCCESS == BMDLNodePool_SReturn(nodes[0]))
        {
            status = EXIT_FAILURE;
            BMTest_ERRLOGBREAKEX(
                "BMDLNodePool_SReturn() did not detect duplication.");
        }
    } while (0);
    BMDLNode_DEINITANCHOR(&anchor);
    BMDLNodePool_SDeinit();
    BMTest_ENDFUNC(status);
    return status;
}
