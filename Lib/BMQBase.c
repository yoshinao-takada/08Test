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
