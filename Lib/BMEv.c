#include "BMEv.h"
#include "BMDLNode.h"
#include <assert.h>

BMStatus_t BMEv_PutQ(BMEv_pt ev, BMDLNode_pt qptr)
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        BMDLNode_pt dlnode = BMDLNodePool_SGet();
        if (!dlnode)
        {
            status = BMStatus_NORESOURCE;
            break;
        }
        dlnode->payload = ev;
        ev->listeners++;
        BMDLNode_AddNext(qptr, dlnode);
    } while (0);
    return status;
}

BMEv_pt BMEv_GetQ(BMDLNode_pt qptr)
{
    BMEv_pt evptr = NULL;
    do {
        BMDLNode_pt dlnode = BMDLNode_GetPrev(qptr);
        if (!dlnode) break;
        evptr = (BMEv_pt)dlnode->payload;
        assert(BMDLNodePool_SReturn(dlnode) == BMStatus_SUCCESS);
    } while (0);
    return evptr;
}

BMEv_pt BMEvPool_Get(BMEvPool_pt pool, BMEvId_t id, void *param)
{
    BMEv_pt ev = NULL;
    BMPoolBase_LOCK((BMPoolBase_pt)pool);
    do {
        int16_t available = BMPoolBase_FindAvailable(&pool->base);
        if (available >= 0)
        {
            ev = pool->ev + available;
            ev->id = id;
            ev->listeners = 0;
            ev->param = param;
        }
    } while (0);
    BMPoolBase_UNLOCK((BMPoolBase_pt)pool);
    return ev;
}

BMStatus_t BMEvPool_Return(BMEvPool_pt pool, BMEv_pt ev)
{
    ptrdiff_t offs = ev - pool->ev;
    BMStatus_t status = BMStatus_INVALID;
    BMPoolBase_LOCK((BMPoolBase_pt)pool);
    do {
        // The event is still under listen.
        if (ev->listeners) break;
        status = BMPoolBase_Return(&pool->base, offs);
    } while (0);
    BMPoolBase_UNLOCK((BMPoolBase_pt)pool);
    return status;
}
