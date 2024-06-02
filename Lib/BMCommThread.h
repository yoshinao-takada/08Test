#if !defined(BMCOMMTHREAD_H)
#define BMCOMMTHREAD_H
#include "BMCommBase.h"
#include "BMRingBuf.h"
#include "BMEv.h"
#include <pthread.h>

typedef struct {
    BMComm_t base;
    void* thParam;
    BMThread_f thFunc;
    pthread_t thread;
    BMEv_t ev; // event raise to upper layer
    BMDLNode_pt evq; // event queue of upperr layer
} BMCommThread_t, *BMCommThread_pt;
#endif /* BMCOMMTHREAD_H */