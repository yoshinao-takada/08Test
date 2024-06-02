#if !defined(BMFSM_H)
#define BMFSM_H
#include "BMEv.h"
#define MAX_FSM_COUNT   8
#define MAX_DOWNSTREAM_QUEUES   4

typedef enum {
    BMStateResult_IGNORE,
    BMStateResult_ACCEPT,
    BMStateResult_TRANSIT,
    BMStateResult_ERROR
} BMStateResult_t;

typedef struct BMFSM {
    // state
    BMStateResult_t (*state)(struct BMFSM* fsm, BMEv_pt ev);

    // input event queue
    BMDLNode_t iq;

    // input event queues of downstream objects
    BMDLNode_t oq;

    // context can hold a variable of any type.
    void* ctx;
} BMFSM_t, *BMFSM_pt;

#define BMFSM_DECL(_varname) \
    BMFSM_t _varname = { \
        NULL, \
        { &(_varname.iq), &(_varname.iq), NULL }, \
        { &(_varname.oq), &(_varname.oq), NULL }, \
        NULL }

#define BMFSM_SDECL(_varname) \
    static BMFSM_t _varname = { \
        NULL, \
        { &(_varname.iq), &(_varname.iq), NULL }, \
        { &(_varname.oq), &(_varname.oq), NULL }, \
        NULL }

typedef BMFSM_t BMSched_t[MAX_FSM_COUNT];
#endif /* BMFSM_H */