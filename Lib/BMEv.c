#include "BMEv.h"

BMEv_pt BMEvPool_Get(BMEvPool_pt pool, BMEvId_t id, void* param)
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
