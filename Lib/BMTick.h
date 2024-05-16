#if !defined(BMTICK_H)
#define BMTICK_H
#include "BMPoolBase.h"
#include "BMEvQ.h"

#define STATIC_DISPATCHER_COUNT     4

typedef struct {
    uint16_t count, interval;
    void* param;
    void* result;
    void* (*handler)(void*);
} BMDispatcher_t, *BMDispatcher_pt;

/*!
\brief clear a dispatcher.
*/
#define BMDispatcher_CLEAR(_dispptr) { \
    (_dispptr)->count = (_dispptr)->interval = 0; \
    (_dispptr)->param = (_dispptr)->result = NULL; \
    (_dispptr)->handler = NULL; \
}

/*!
\brief set parameters of a dispatcher.
*/
#define BMDispatcher_SET(_dispptr, _ct, _it, _par, _hdlr) { \
    (_dispptr)->count = _ct; \
    (_dispptr)->interval = _it; \
    (_dispptr)->param = _par; \
    (_dispptr)->handler = _hdlr; \
}

BMStatus_t BMDispatcher_Dispatch(BMDispatcher_pt dispatcher);

typedef struct {
    BMPoolBase_t base;
    BMDispatcher_pt dispatchers;
    BMEvQ_pt evq;
} BMDispatchers_t, *BMDispatchers_pt;

#define BMDispatchers_DECL(_varname, _poolsize) \
    uint16_t _varname ## _used[BMAlign_TO16(_poolsize) >> 4]; \
    BMDispatcher_t _varname ## _dispatchers[_poolsize]; \
    BMDispatchers_t _varname = { \
        { _varname ## _used, 0, _poolsize }, \
        _varname ## _dispatchers, NULL };

#define BMDispatchers_SDECL(_varname, _poolsize) \
    static uint16_t _varname ## _used[BMAlign_TO16(_poolsize) >> 4]; \
    static BMDispatcher_t _varname ## _dispatchers[_poolsize]; \
    static BMDispatchers_t _varname = { \
        { _varname ## _used, 0, _poolsize }, \
        _varname ## _dispatchers, NULL };

#define BMDispatchers_INIT(_varptr, _evqptr) \
BMPoolBase_INIT(&(_varptr)->base); \
{ \
    int _used_size = (BMAlign_TO16((_varptr)->base.count) >> 4); \
    for (int _i = 0; _i < _used_size; _i++) \
    { \
        (_varptr)->base.used[_i] = 0; \
    } \
} \
{ \
    BMDispatcher_pt _begin = (_varptr)->dispatchers; \
    BMDispatcher_pt const _end = _begin + (_varptr)->base.count; \
    for (; _begin != _end; _begin++) \
    { \
        _begin->count = _begin->interval = 0; \
        _begin->param = _begin->result = NULL; \
        _begin->handler = NULL; \
    } \
    (_varptr)->evq = _evqptr; \
}

#define BMDispatchers_DEINIT(_varptr) BMPoolBase_DEINIT(&(_varptr)->base)

/*!
\brief crunch queued events
*/
BMStatus_t BMDispatchers_Crunch(BMDispatchers_pt disps);

/*!
\brief crunch queued events in the static dispatchers.
*/
BMStatus_t BMDispatchers_SCrunch();

/*!
\brief Get a dispatcher from a pool pointed by dispsptr.
*/
BMDispatcher_pt BMDispatchers_Get(BMDispatchers_pt dispsptr);

/*!
\brief Return a dispatcher to a pool.
\param dispsptr [in,out] pointer to the pool
\param dispptr [in] pointer to the dispatcher.
\return BMStatus_SUCCESS : success
*/
BMStatus_t BMDispatchers_Return
(BMDispatchers_pt dispsptr, BMDispatcher_pt dispptr);

/*!
\brief setup interval timer and SIGALRM
\param itmillisec [in] systick timer interval in milliseconds
\param evqptr [in] event queue pointer
\param evpoolptr [in] event pool pointer
*/
BMStatus_t BMTick_Init(uint16_t itmillisec, BMEvQ_pt evqptr);

/*!
\brief deinitialize timer and SIGALRM
*/
BMStatus_t BMTick_Deinit();

double BMTick_GetInterval();

/*!
\brief Main loop crunching timer tick events and
    invoke act(actctx) for each iteration.
\param act [in] a function run once in each iteration
\param actctx [in,out] an argument of act
*/
BMStatus_t BMTick_Main(BMAct_f act, BMActCtx_pt actctx);
#endif /* BMTICK_H */