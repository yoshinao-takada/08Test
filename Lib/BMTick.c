#include "BMTick.h"
#include "BMEv.h"
#include "BMDLNode.h"
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#pragma region TIME_CONVERSION_METHODS
static void TimeVal_FromMillisec(struct timeval* t, uint16_t millisec)
{
    t->tv_sec = millisec / 1000;
    t->tv_usec = 1000 * (millisec - t->tv_sec * 1000);
}

static void ITimerVal_FromMillisec(struct itimerval* t, uint16_t millisec)
{
    TimeVal_FromMillisec(&t->it_interval, millisec);
    TimeVal_FromMillisec(&t->it_value, millisec);
}
#pragma endregion TIME_CONVERSION_METHODS

BMDispatchers_SDECL(dispatchers, STATIC_DISPATCHER_COUNT);
static BMEv_t evtick = { BMEvId_TICK, 0, NULL };

BMStatus_t BMDispatcher_Dispatch(BMDispatcher_pt dispatcher)
{
    BMStatus_t status = BMStatus_SUCCESS;
    if (dispatcher->count && dispatcher->handler)
    {
        if (--(dispatcher->count) == 0)
        {
            dispatcher->count = dispatcher->interval;
            dispatcher->result = dispatcher->handler(dispatcher->param);
            if (dispatcher->result != dispatcher->param)
            {
                status = BMStatus_FAILURE;
            }
        }        
    }
    return status;
}

BMStatus_t BMDispatchers_Crunch(BMDispatchers_pt disps)
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMPoolBase_LOCK(&disps->base);
    do {
        BMEv_pt evptr = NULL;
        // Get a queued event. Exit if the queue is empty.
        if (!(evptr = BMEv_GetQ(disps->evq))) break;

        // Process the queued event.
        BMDispatcher_pt iter = disps->dispatchers;
        BMDispatcher_pt const end = iter + disps->base.count;
        for (; iter != end; iter++)
        {
            status |= BMDispatcher_Dispatch(iter);
        }
        --(evptr->listeners);
    } while (0);
    BMPoolBase_UNLOCK(&disps->base);
    return status;
}

BMDispatcher_pt BMDispatchers_Get(BMDispatchers_pt dispsptr)
{
    BMDispatcher_pt dispptr = NULL;
    BMPoolBase_LOCK(&dispsptr->base);
    do {
        uint16_t idx = BMPoolBase_FindAvailable(&dispsptr->base);
        if (idx < 0) break;
        dispptr = dispsptr->dispatchers + idx;
    } while (0);
    BMPoolBase_UNLOCK(&dispsptr->base);
    return dispptr;
}

BMStatus_t BMDispatchers_Return
(BMDispatchers_pt dispsptr, BMDispatcher_pt dispptr)
{
    BMStatus_t status = BMStatus_SUCCESS;
    BMPoolBase_LOCK(&dispsptr->base);
    do {
        ptrdiff_t offs = dispptr - dispsptr->dispatchers;
        if (offs < 0 || dispsptr->base.count <= offs)
        { // out of range
            status = BMStatus_INVALID;
            break;
        }
        if (status = BMPoolBase_Return(&dispsptr->base, offs))
        {
            break;
        }
        BMDispatcher_CLEAR(dispptr);
    } while (0);
    BMPoolBase_UNLOCK(&dispsptr->base);
    return status;
}

BMStatus_t BMDispatchers_SCrunch()
{
    return BMDispatchers_Crunch(&dispatchers);
}

BMDispatcher_pt BMDispatchers_SGet()
{
    return BMDispatchers_Get(&dispatchers);
}

BMStatus_t BMDispatchers_SReturn(BMDispatcher_pt dispptr)
{
    return BMDispatchers_Return(&dispatchers, dispptr);
}

void SIGALRMHandler(int sig)
{
    if (BMEv_PutQ(&evtick, dispatchers.evq))
    { // evq is full and fail to put.
        assert(0);
    }
}

static struct itimerval it_new, it_old;
static struct sigaction sa_new, sa_old;

BMStatus_t BMTick_Init(uint16_t itmillisec, BMDLNode_pt evqptr)
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {        
        sa_new.sa_flags = 0;
        sa_new.sa_handler = SIGALRMHandler;
        BMDispatchers_INIT(&dispatchers, evqptr);
        if (sigaction(SIGALRM, &sa_new, &sa_old))
        {
            status = BMStatus_FAILURE;
            break;
        }

        ITimerVal_FromMillisec(&it_new, itmillisec);
        if (setitimer(ITIMER_REAL, &it_new, &it_old))
        {
            status = BMStatus_FAILURE;
            break;
        }
    } while (0);
    return status;
}

BMStatus_t BMTick_Deinit()
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        if (setitimer(ITIMER_REAL, &it_old, &it_new))
        {
            status = BMStatus_FAILURE;
            break;
        }
        if (sigaction(SIGALRM, &sa_old, &sa_new))
        {
            status = BMStatus_FAILURE;
            break;
        }
    } while (0);
    BMDispatchers_DEINIT(&dispatchers);
    return status;
}

double BMTick_GetInterval()
{
    return 1.0e-6 * it_new.it_interval.tv_usec + it_new.it_interval.tv_sec;
}

BMStatus_t BMTick_Main(BMAct_f act, BMActCtx_pt actctx)
{
    BMStatus_t status = BMStatus_SUCCESS;
    do {
        BMDispatchers_SCrunch();
        act(actctx);
        pause();
    } while (actctx->stat == BMActStatus_ACTIVE);
    status = (actctx->stat == BMActStatus_ERRSTOP) ?
        BMStatus_FAILURE : BMStatus_SUCCESS;
    return status;
}