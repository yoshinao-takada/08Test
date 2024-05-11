#if !defined(BMPOOLBASE_H)
#define BMPOOLBASE_H
#include "BMDefs.h"
#include <stdint.h>
#include <pthread.h>
#include <stddef.h>
typedef struct {
    uint16_t* used;
    pthread_spinlock_t lock;
    uint16_t count;
} BMPoolBase_t, *BMPoolBase_pt;

#define BMPoolBase_DECL(_varname, _count) \
    uint16_t _varname ## _used[BMAlign_TO16(_count) >> 4]; \
    BMPoolBase_t _varname = { _varname ## _used, 0, _count }

#define BMPoolBase_SDECL(_varname, _count) \
    static uint16_t _varname ## _used[BMAlign_TO16(_count) >> 4]; \
    static BMPoolBase_t _varname = { _varname ## _used, 0, _count }

#define BMPoolBase_INIT(_pbptr) \
    pthread_spin_init(&(_pbptr)->lock, PTHREAD_PROCESS_PRIVATE); \
    uint16_t _size_of_used = BMAlign_TO16((_pbptr)->count) >> 4; \
    for (uint16_t _i = 0; _i < _size_of_used; _i++) \
    { \
        (_pbptr)->used[_i] = 0; \
    }

#define BMPoolBase_DEINIT(_pbptr) \
    pthread_spin_destroy(&(_pbptr)->lock)

#define BMPoolBase_LOCK(_pbptr) \
    pthread_spin_lock(&(_pbptr)->lock)

#define BMPoolBase_UNLOCK(_pbptr) \
    pthread_spin_unlock(&(_pbptr)->lock)

/*!
\brief return index number of the first available element.
*/
int16_t BMPoolBase_FindAvailable(BMPoolBase_pt pbptr);

BMStatus_t BMPoolBase_Return(BMPoolBase_pt pbptr, ptrdiff_t offs);
#endif /* BMPOOLBASE_H */