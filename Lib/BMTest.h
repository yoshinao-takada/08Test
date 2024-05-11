#if !defined(BMTEST_H)
#define BMTEST_H
#include "BMDefs.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#if defined(BMTEST_MAIN_C)
pthread_spinlock_t BMTest_lock;
#else
extern pthread_spinlock_t BMTest_lock;
#endif

#define BMTest_START pthread_spin_init(&BMTest_lock, PTHREAD_PROCESS_PRIVATE)
#define BMTest_END  pthread_spin_destroy(&BMTest_lock)
#define BMTest_LOCK pthread_spin_lock(&BMTest_lock)
#define BMTest_UNLOCK pthread_spin_unlock(&BMTest_lock)

#define BMTest_FPRINTF(_fl, ...) \
    BMTest_LOCK; fprintf(_fl, __VA_ARGS__); BMTest_UNLOCK

#define BMTest_PRINTF(...) \
    BMTest_LOCK; printf(__VA_ARGS__); BMTest_UNLOCK

#define BMTest_ENDFUNC(_status) \
    BMTest_PRINTF("END FUNC status = 0x%04x(%d) @ %s,%s,%d\n", \
        _status, _status, __FILE__, __FUNCTION__, __LINE__)

#define BMTest_ERRLOG(_file,_fn,_ln,...) \
    BMTest_LOCK; fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, " @ %s,%s,%d\n", _file, _fn, _ln); \
    BMTest_UNLOCK

#define BMTest_ERRLOGBREAK(_file,_fn,_ln,...) \
    BMTest_ERRLOG(_file,_fn,_ln,__VA_ARGS__); break

#define BMTest_ERRLOGBREAKEX(...) \
    BMTest_ERRLOGBREAK(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#endif /* BMTEST_H */