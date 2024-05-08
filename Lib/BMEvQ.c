#include "BMEvQ.h"

BMEvQ_pt BMEvQPool_Get(BMEvQPool_pt poolptr)
{
    uint16_t available = BMPoolBase_FindAvailable((BMPoolBase_pt)poolptr);
    return (available >= 0) ? (poolptr->evqptrs + available) : NULL;
}

BMStatus_t BMEvQPool_Return(BMEvQPool_pt poolptr, BMEvQ_pt qptr)
{
    ptrdiff_t offs = qptr - poolptr->evqptrs;
    return BMPoolBase_Return((BMPoolBase_pt)poolptr, offs);
}
