#if !defined(BMEV_H)
#define BMEV_H
#include "BMDefs.h"
#include "BMPoolBase.h"
#include "BMDLNode.h"

typedef struct {
    BMEvId_t id;
    uint16_t listeners;
    void* param;
} BMEv_t, *BMEv_pt;

#define BMEv_INIT(_evptr) { \
    (_evptr)->id = 0; (_evptr)->listeners = 0; (_evptr)->param = NULL; }

/*!/
\brief put an event object into a queue.
*/
BMStatus_t BMEv_PutQ(BMEv_pt ev, BMDLNode_pt qptr);

/*!
\brief get an event object from a queue.
*/
BMEv_pt BMEv_GetQ(BMDLNode_pt qptr);

typedef struct {
    BMPoolBase_t base;
    BMEv_pt ev;
} BMEvPool_t, *BMEvPool_pt;

/*!
\brief declare a set of variables for a pool of BMEv_t.
*/
#define BMEvPool_DECL(_varname, _count) \
    BMEv_t _varname ## _ev[_count]; \
    uint16_t _varname ## _used[BMAlign_TO16(_count) >> 4]; \
    BMEvPool_t _varname = { \
        { _varname ## _used, 0, _count }, _varname ## _ev }

/*!
\brief declare a set of static variables for a pool of BMEv_t.
*/
#define BMEvPool_SDECL(_varname, _count) \
    static BMEv_t _varname ## _ev[_count]; \
    static uint16_t _varname ## _used[BMAlign_TO16(_count) >> 4]; \
    static BMEvPool_t _varname = { \
        { _varname ## _used, 0, _count }, _varname ## _ev }

#define BMEvPool_INIT(_poolptr) \
    BMPoolBase_INIT(&(_poolptr)->base); \
    for (uint16_t _i = 0; _i < (_poolptr)->base.count; _i++) \
    { \
        BMEv_INIT((_poolptr)->ev + _i); \
    }


#define BMEvPool_DEINIT(_poolptr) BMPoolBase_DEINIT(&(_poolptr)->base)

/*!
\brief Get an element from a pool.
*/
BMEv_pt BMEvPool_Get(BMEvPool_pt pool, BMEvId_t id, void* param);

/*!
\brief Return an element to a pool.
\return NOTFOUND: ev is not an element of a pool, 
\return INVALID: ev->listeners != 0
\return SUCCESS : success
*/
BMStatus_t BMEvPool_Return(BMEvPool_pt pool, BMEv_pt ev);
#endif /* BMEV_H */