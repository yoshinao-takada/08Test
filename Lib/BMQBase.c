#include "BMQBase.h"

uint16_t BMQBase_NextWridx(BMQBase_pt qbaseptr)
{
    uint16_t nextval = qbaseptr->wridx + 1;
    if (nextval >= qbaseptr->count)
    {
        nextval -= qbaseptr->count;
    }
    return nextval;
}

uint16_t BMQBase_NextRdidx(BMQBase_pt qbaseptr)
{
    uint16_t nextval = qbaseptr->rdidx + 1;
    if (nextval >= qbaseptr->count)
    {
        nextval -= qbaseptr->count;
    }
    return nextval;
}

uint16_t BMQBase_Available(BMQBase_pt qbaseptr)
{
    uint16_t available = (qbaseptr->wridx >= qbaseptr->rdidx) ?
        (qbaseptr->wridx - qbaseptr->rdidx) :
        (qbaseptr->wridx + qbaseptr->count - qbaseptr->rdidx);
    return available;
}

uint16_t BMQBase_Vacant(BMQBase_pt qbaseptr)
{
    uint16_t vacant = (qbaseptr->rdidx > qbaseptr->wridx) ?
        (qbaseptr->rdidx - 1 - qbaseptr->wridx) :
        (qbaseptr->rdidx + qbaseptr->count - 1 - qbaseptr->wridx);
    return vacant;
}
