#if !defined(BMDEFS_H)
#define BMDEFS_H
#include <stdint.h>

#define BMARRAY_SIZE(_a)    sizeof(_a)/sizeof(_a[0])

#define BMALIGN_ARCH        8   /* x64, AArch64 */
// #define BMALIGN_ARCH     4   /* ARM Cortex-M, -R, x86, AArch32 */
// #define BMALIGN_ARCH     2   /* MSP430, RL78 */
#define BMALIGN_MASK        (BMALIGN_ARCH - 1)
#define BMALIGN_DOWN(_n)    (_n & ~BMALIGN_MASK)
#define BMALIGN_UP(_n)      (BMALIGN_DOWN(_n) + BMALIGN_ARCH)
#define BMALIGN_TO_ARCH(_n) \
    ((_n & BMALIGN_MASK) ? BMALIGN_UP(_n) : BMALIGN_DOWN(_n))

// Boundary alignment by 16.
#define BMALIGN_MASK16      (15)
#define BMALIGN_DOWN16(_n)  (_n & ~BMALIGN_MASK16)
#define BMALIGN_UP16(_n)    (BMALIGN_DOWN16(_n) + 16)
#define BMALIGN_TO16(_n) \
    ((_n & BMALIGN_MASK16) ? BMALIGN_UP16(_n) : BMALIGN_DOWN16(_n))

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