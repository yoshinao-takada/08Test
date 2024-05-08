#include "BMPoolBase.h"

uint16_t BMPoolBase_FindAvailable(BMPoolBase_pt pbptr)
{
    uint16_t available = -1;
    for (uint16_t bitidx = 0; bitidx < pbptr->count; bitidx++)
    {
        uint16_t shortword = (bitidx >> 4);
        uint16_t bitidx_in_shortword = bitidx - (shortword << 4);
        uint16_t bitmask = (1 << bitidx_in_shortword);
        if ((bitmask & pbptr->used[shortword]) == 0)
        {
            available = bitidx;
            pbptr->used[shortword] |= bitmask;
        }
    }
    return available;
}


BMStatus_t BMPoolBase_Return(BMPoolBase_pt pbptr, ptrdiff_t offs)
{
    BMStatus_t status = BMStatus_NOTFOUND;
    do {
        // check range
        if (offs < 0 || offs >= pbptr->count) break;

        uint16_t shortword = (offs >> 4);
        uint16_t bitidx_in_shortword = offs - (shortword << 4);
        uint16_t bitmask = (1 << bitidx_in_shortword);
        if ((bitmask & pbptr->used[shortword]) == 0)
        {
            break;
        }
        pbptr->used[shortword] &= ~bitmask; // reset bit
        status = BMStatus_SUCCESS;
    } while (0);
    return status;
}
