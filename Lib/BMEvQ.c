#include "BMEvQ.h"
BMStatus_t BMEvQ_Get(BMEvQ_pt _varptr, BMEv_pt* evptrptr)
{
    BMStatus_t status = BMStatus_NOTFOUND;
    BMQBase_LOCK(&_varptr->base);
    do {
        if (_varptr->base.rdidx == _varptr->base.wridx) break;
        uint16_t nextrdidx = BMQBase_NextRdidx(&_varptr->base);
        *evptrptr = _varptr->evptrs[_varptr->base.rdidx];
        _varptr->base.rdidx = nextrdidx;
        status = BMStatus_SUCCESS;
    } while (0);
    BMQBase_UNLOCK((BMQBase_pt)_varptr);
    return status;
}

BMStatus_t BMEvQ_Put(BMEvQ_pt _varptr, BMEv_pt evptr)
{
    BMStatus_t status = BMStatus_NORESOURCE;
    BMQBase_LOCK(&_varptr->base);
    do {
        uint16_t nextwridx = BMQBase_NextWridx(&_varptr->base);
        if (nextwridx == _varptr->base.rdidx) break;
        _varptr->evptrs[_varptr->base.wridx] = evptr;
        evptr->listeners++;
        _varptr->base.wridx = nextwridx;
        status = BMStatus_SUCCESS;
    } while (0);
    BMQBase_UNLOCK(&_varptr->base);
    return status;
}

BMEvQ_pt BMEvQPool_Get(BMEvQPool_pt poolptr)
{
    BMEvQ_pt evqptr = NULL;
    BMEvQPool_LOCK(poolptr);
    {
        int16_t available = BMPoolBase_FindAvailable((BMPoolBase_pt)poolptr);
        if (available >= 0)
        {
            evqptr = poolptr->evqs + available;
            BMEvQ_INIT(evqptr);
        }
    }
    BMEvQPool_UNLOCK(poolptr);
    return evqptr;
}

BMStatus_t BMEvQPool_Return(BMEvQPool_pt poolptr, BMEvQ_pt qptr)
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMEvQPool_LOCK(&poolptr->base);
    do {
        ptrdiff_t offs = qptr - (poolptr)->evqs;
        status = BMPoolBase_Return((BMPoolBase_pt)poolptr, offs);
        if (status) break;
        // clear the queue
        qptr->base.wridx = qptr->base.rdidx = 0;
        BMEvQ_DEINIT(qptr);
    } while (0);
    BMEvQPool_UNLOCK(&poolptr->base);
    return status;
}
