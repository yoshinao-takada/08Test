#if !defined(BMDLNODE_H)
#define BMDLNODE_H
#include "BMDefs.h"
#include "BMPoolBase.h"

#if !defined(BMDLNodePool_STATIC_SIZE)
#define BMDLNodePool_STATIC_SIZE    128
#endif

/*!
\brief infinite queue node
*/
typedef struct BMDLNode {
    struct BMDLNode *prev, *next;
    void* payload;
    pthread_spinlock_t lock;
} BMDLNode_t, *BMDLNode_pt;

#define BMDLNode_HAS_ANY(_anchorptr) ((_anchorptr)->prev != (_anchorptr))
#define BMDLNode_EMPTY(_anchorptr) ((_anchorptr)->prev == (_anchorptr))
/*!
\brief count nodes linked to the anchor.
*/
uint16_t BMDLNode_Count(BMDLNode_pt anchor);

/*!
\brief add a new node at anchor->next. It is same as to insert a new node
    at the beginning of the linked list.
*/
void BMDLNode_AddNext(BMDLNode_pt anchor, BMDLNode_pt newnode);

/*!
\brief add a new node at anchor->prev. It is same as to append a new node
    at the end point of the linked list.
*/
void BMDLNode_AddPrev(BMDLNode_pt anchor, BMDLNode_pt newnode);

/*!
\brief get the node at anchor->next and remove the node from the list.
*/
BMDLNode_pt BMDLNode_GetNext(BMDLNode_pt anchor);

/*!
\brief get the node at anchor->prev and remove the node from the list.
*/
BMDLNode_pt BMDLNode_GetPrev(BMDLNode_pt anchor);

#define BMDLNode_DECLANCHOR(_varname) \
    BMDLNode_t _varname = { &_varname, &_varname, NULL, 0 } \

#define BMDLNode_INITANCHOR(_varptr) \
    pthread_spin_init(&(_varptr)->lock, PTHREAD_PROCESS_PRIVATE)

#define BMDLNode_DEINITANCHOR(_varptr) \
    pthread_spin_destroy(&(_varptr)->lock)

/*!
\brief find the node with the matched condition specified by zeroifmatch.
    searching begins from anchor->next and the first matching node is reported.
    Other matching nodes are ignored even it they exist.
\param tofind [in] payload pointer to compare
\param zeroifmatch [in] comparator function which returns zero if the two
    payload pointers satisfy match conditions.
*/
BMDLNode_pt BMDLNode_Find(BMDLNode_pt anchor, const void* tofind,
    int (*zeroifmatch)(const void*, const void*));

typedef struct {
    BMPoolBase_t base;
    BMDLNode_pt nodes;
} BMDLNodePool_t, *BMDLNodePool_pt;

#define BMDLNodePool_DECL(_varname, _poolsize) \
    uint16_t _varname ## _used[BMAlign_BC2SWC(_poolsize)]; \
    BMDLNode_t _varname ## _nodes[_poolsize]; \
    BMDLNodePool_t _varname = { \
        { _varname ## _used, 0, _poolsize }, _varname ## _nodes }

#define BMDLNodePool_SDECL(_varname, _poolsize) \
    static uint16_t _varname ## _used[BMAlign_BC2SWC(_poolsize)]; \
    static BMDLNode_t _varname ## _nodes[_poolsize]; \
    static BMDLNodePool_t _varname = { \
        { _varname ## _used, 0, _poolsize }, _varname ## _nodes }

#define BMDLNodePool_INIT(_varptr) BMPoolBase_INIT(&(_varptr)->base)

#define BMDLNodePool_DEINIT(_varptr) BMPoolBase_DEINIT(&(_varptr)->base)

/*!
\brief get a node from the pool.
*/
BMDLNode_pt BMDLNodePool_Get(BMDLNodePool_pt poolptr);

/*!
\brief return the node to the pool.
    The function fails if nodeptr is not member of the pool.
*/
BMStatus_t BMDLNodePool_Return(
    BMDLNodePool_pt poolptr, BMDLNode_pt nodeptr);

/*!
\brief get a node from the static pool.
    The static pool size is defined by BMDLNodePool_STATIC_SIZE.
*/
BMDLNode_pt BMDLNodePool_SGet();

/*!
\brief return the node to the static pool.
*/
BMStatus_t BMDLNodePool_SReturn(BMDLNode_pt nodeptr);

void BMDLNodePool_SInit();

void BMDLNodePool_SDeinit();
#endif /* BMDLNODE_H */