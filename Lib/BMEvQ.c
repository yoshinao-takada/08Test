#include "BMEvQ.h"

BMEvQ_pt BMEvQPool_Get(BMEvQPool_pt poolptr)
{
    int16_t available = BMPoolBase_FindAvailable((BMPoolBase_pt)poolptr);
    BMEvQ_pt evqptr = (available >= 0) ? (poolptr->evqs + available) : NULL;
    return evqptr;
}

BMStatus_t BMEvQPool_Return(BMEvQPool_pt poolptr, BMEvQ_pt qptr)
{
    ptrdiff_t offs = qptr - (poolptr)->evqs;
    BMEvQ_UNLOCK(&qptr);
    qptr->base.wridx = qptr->base.rdidx = 0;
    return BMPoolBase_Return((BMPoolBase_pt)poolptr, offs);
}
