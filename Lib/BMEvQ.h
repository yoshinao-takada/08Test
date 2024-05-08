#if !defined(BMEVQ_H)
#define BMEVQ_H
#include "BMDefs.h"
#include "BMEv.h"
#include "BMQBase.h"
typedef struct {
    BMQBase_t base;
    BMEv_pt *evptrs;
} BMEvQ_t, *BMEvQ_pt;
#pragma region BMEvQ_MACROS
#define BMEvQ_DECL(_varname, _count) \
    BMEv_pt _varname ## _evptrs[_count]; \
    BMEvQ_t _varname = { \
        BMQBase(_count), _varname ## _evptrs }

#define BMEvQ_SDECL(_varname, _count) \
    static BMEv_pt _varname ## _evptrs[_count]; \
    static BMEvQ_t _varname = { \
        BMQBase(_count), _varname ## _evptrs }

#define BMEvQ_INIT(_varptr) BMQBase_INIT((BMQBase_pt)_varptr)

#define BMEvQ_INIT(_varptr) BMQBase_DEINIT((BMQBase_pt)_varptr)

#define BMEvQ_INIT(_varptr) BMQBase_LOCK((BMQBase_pt)_varptr)

#define BMEvQ_INIT(_varptr) BMQBase_UNLOCK((BMQBase_pt)_varptr)
#pragma endregion BMEvQ_MACROS
#pragma region BMEvQ_METHODS
/*!
\brief get the first element in the queue.
\param _varptr [in,out] The event queue
\param evptrptr [out] pointer-pointer to the event message
\return 1: success, 0: no event message is available.
*/
uint16_t BMEvQ_Get(BMEvQ_pt _varptr, BMEv_pt* evptrptr);

/*!
\brief put an element into the queue.
\param _varptr [in,out] The event queue
\param evptr [in] pointer to the event message
\return SUCCESS: success,
\return NORESOURCE: queue is full and failed.
*/
BMStatus_t BMEvQ_Put(BMEvQ_pt _varptr, BMEv_pt evptr);
#pragma endregion BMEvQ_METHODS

typedef struct {
    BMPoolBase_t base;
    BMEvQ_pt *evqptrs;
} BMEvQPool_t, *BMEvQPool_pt;

#define BMEvQPool_DECL(_varname, _count, _qsize) \
    BMEv_pt _varname ## _evptrs[_count * _qsize]; \
    BMEvQ_t _varname ## _evqs[_count] = { \
        { BMQBase(_qsize), _varname ## _evptrs } \
    }; \
    uint16_t _varname ## _used[BMALIGN_TO16(_count) >> 4]; \
    BMEvQPool_t _varname = { \
        { _varname ## _used, 0, _count }, _varname ## _evqs }

#define BMEvQPool_SDECL(_varname, _count, _qsize) \
    static BMEv_pt _varname ## _evptrs[_count * _qsize]; \
    static BMEvQ_t _varname ## _evqs[_count] = { \
        { BMQBase(_qsize), _varname ## _evptrs } \
    }; \
    static uint16_t _varname ## _used[BMALIGN_TO16(_count) >> 4]; \
    static BMEvQPool_t _varname = { \
        { _varname ## _used, 0, _count }, _varname ## _evqs }

#define BMEvQPool_INIT(_varptr) BMPoolBase_INIT((BMPoolBase_pt)_varptr); \
    for (uint16_t _i = 0; _i < (_varptr)->base.count; _i++) \
    { \
        BMEvQ_INIT((_varptr)->evqptrs + _i); \
    }

#define BMEvQPool_DEINIT(_varptr) BMPoolBase_DEINIT((BMPoolBase_pt)_varptr); \
    for (uint16_t _i = 0; _i < (_varptr)->base.count; _i++) \
    { \
        BMEvQ_DEINIT((_varptr)->evqptrs + _i); \
    }


#define BMEvQPool_LOCK(_varptr) BMPoolBase_LOCK((BMPoolBase_pt)_varptr)

#define BMEvQPool_UNLOCK(_varptr) BMPoolBase_UNLOCK((BMPoolBase_pt)_varptr)

BMEvQ_pt BMEvQPool_Get(BMEvQPool_pt poolptr);

BMStatus_t BMEvQPool_Return(BMEvQPool_pt poolptr, BMEvQ_pt qptr);

#endif /* BMEVQ_H */