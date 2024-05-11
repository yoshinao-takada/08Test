#if !defined(BMDEFS_H)
#define BMDEFS_H
#include <stdint.h>

#define BMArray_SIZE(_a)    sizeof(_a)/sizeof(_a[0])

/*!
\brief fill array with a value.
\param _beginptr [in,out] _beginptr is modified in the macro. The final
    value is equal to _endptr. Threfore _beginptr must be modifiable
    with a real body.
*/
#define BMArray_FILL(_beginptr, _endptr, _val) \
    for (; _beginptr != (_endptr); _beginptr++) \
    { \
        *_beginptr = _val; \
    }

#define BMAlign_ARCH        8   /* x64, AArch64 */
// #define BMAlign_ARCH     4   /* ARM Cortex-M, -R, x86, AArch32 */
// #define BMAlign_ARCH     2   /* MSP430, RL78 */
#define BMAlign_MASK        (BMAlign_ARCH - 1)
#define BMAlign_DOWN(_n)    (_n & ~BMAlign_MASK)
#define BMAlign_UP(_n)      (BMAlign_DOWN(_n) + BMAlign_ARCH)
#define BMAlign_TO_ARCH(_n) \
    ((_n & BMAlign_MASK) ? BMAlign_UP(_n) : BMAlign_DOWN(_n))

// Boundary alignment by 16.
#define BMAlign_MASK16      (15)
#define BMAlign_DOWN16(_n)  (_n & ~BMAlign_MASK16)
#define BMAlign_UP16(_n)    (BMAlign_DOWN16(_n) + 16)
#define BMAlign_TO16(_n) \
    ((_n & BMAlign_MASK16) ? BMAlign_UP16(_n) : BMAlign_DOWN16(_n))

typedef uint16_t BMStatus_t;
typedef uint16_t BMEvId_t;

#define BMStatus_SUCCESS        0
#define BMStatus_ERRBEGIN       0x4000
#define BMStatus_FAILURE        (BMStatus_ERRBEGIN + 1)
#define BMStatus_INVALID        (BMStatus_ERRBEGIN + 2)
#define BMStatus_NORESOURCE     (BMStatus_ERRBEGIN + 3)
#define BMStatus_NOCONVERGENCE  (BMStatus_ERRBEGIN + 4)
#define BMStatus_SINGULAR       (BMStatus_ERRBEGIN + 5)
#define BMStatus_NOTFOUND       (BMStatus_ERRBEGIN + 6)

#define BMEvId_TICK             0
#define BMEvId_READBYTES        1
#define BMEvId_BUFEMPTY         2
#endif /* BMDEFS_H */