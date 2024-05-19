#include "BMDLNode.h"

void BMDLNode_AddNext(BMDLNode_pt anchor, BMDLNode_pt newnode)
{
    BMDLNode_pt curnext = anchor->next;
    curnext->prev = newnode;
    newnode->next = curnext;
    anchor->next = newnode;
    newnode->prev = anchor;
}

void BMDLNode_AddPrev(BMDLNode_pt anchor, BMDLNode_pt newnode)
{
    BMDLNode_pt curprev = anchor->prev;
    curprev->next = newnode;
    newnode->prev = curprev;
    anchor->prev = newnode;
    newnode->next = anchor;
}

BMDLNode_pt BMDLNode_GetNext(BMDLNode_pt anchor)
{
    BMDLNode_pt curnext = anchor->next;
    anchor->next = curnext->next;
    curnext->next->prev = anchor;
    curnext->next = curnext->prev = curnext;
    return curnext;
}

BMDLNode_pt BMDLNode_GetPrev(BMDLNode_pt anchor)
{
    BMDLNode_pt curprev = anchor->prev;
    anchor->prev = curprev->prev;
    curprev->prev->next = anchor;
    curprev->next = curprev->prev = curprev;
    return curprev;
}

BMDLNode_pt BMDLNode_Find(
    BMDLNode_pt anchor, const void *tofind, 
    int (*zeroifmatch)(const void *, const void *))
{
    BMDLNode_pt iter = anchor->next;
    BMDLNode_pt end = anchor;
    for (; iter != end; iter = iter->next)
    {
        if (0 == zeroifmatch(iter->payload, tofind))
        {
            break;
        }
    }
    return iter;
}

BMDLNode_pt BMDLNodePool_Get(BMDLNodePool_pt poolptr)
{
    BMDLNode_pt node = NULL;
    BMPoolBase_LOCK(&poolptr->base);
    do {
        int16_t idx = BMPoolBase_FindAvailable(&poolptr->base);
        if (idx >= 0)
        {
            node = poolptr->nodes + idx;
            node->next = node->prev = node;
            node->payload = NULL;
        }
    } while (0);
    BMPoolBase_UNLOCK(&poolptr->base);
    return node;
}

BMStatus_t BMDLNodePool_Return(BMDLNodePool_pt poolptr, BMDLNode_pt nodeptr)
{
    ptrdiff_t offs = nodeptr - poolptr->nodes;
    BMPoolBase_LOCK(&poolptr->base);
    BMStatus_t status = BMPoolBase_Return(&poolptr->base, offs);
    BMPoolBase_UNLOCK(&poolptr->base);
    return status;
}

BMDLNodePool_SDECL(nodepool, BMDLNodePool_STATIC_SIZE);

BMDLNode_pt BMDLNodePool_SGet()
{
    return BMDLNodePool_Get(&nodepool);
}

BMStatus_t BMDLNodePool_SReturn(BMDLNode_pt nodeptr)
{
    return BMDLNodePool_Return(&nodepool, nodeptr);
}

void BMDLNodePool_SInit()
{
    BMDLNodePool_INIT(&nodepool);
}

void BMDLNodePool_SDeinit()
{
    BMDLNodePool_DEINIT(&nodepool);
}
