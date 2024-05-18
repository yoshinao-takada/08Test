#if !defined(BMQBASE_H)
#define BMQBASE_H
#include <stdint.h>
#include <pthread.h>
/*!
\struct BMQBase
\brief Base of queue and ring-buffer structs.
*/
typedef struct {
    pthread_spinlock_t lock;
    uint16_t wridx, rdidx, count;
} BMQBase_t, *BMQBase_pt;

/*!
\brief default initializer
*/
#define BMQBase(_count) { 0, 0, 0, _count }

#define BMQBase_INIT(_qbaseptr) \
    pthread_spin_init(&(_qbaseptr)->lock, PTHREAD_PROCESS_PRIVATE)

#define BMQBase_DEINIT(_qbaseptr) pthread_spin_destroy(&(_qbaseptr)->lock)

#define BMQBase_LOCK(_qbaseptr) pthread_spin_lock(&(_qbaseptr)->lock)

#define BMQBase_UNLOCK(_qbaseptr) pthread_spin_unlock(&(_qbaseptr)->lock)

/*!
\brief get the next value of wridx.
*/
uint16_t BMQBase_NextWridx(BMQBase_pt qbaseptr);

/*!
\brief get the next value of rdidx.
*/
uint16_t BMQBase_NextRdidx(BMQBase_pt qbaseptr);

/*!
\brief get a byte count in the queue.
*/
uint16_t BMQBase_Available(BMQBase_pt qbaseptr);

/*!
\brief get a byte count vacant in the queue.
*/
uint16_t BMQBase_Vacant(BMQBase_pt qbaseptr);
#endif /* BMQBASE_H */