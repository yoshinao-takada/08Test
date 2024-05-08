# Overview
## Construction
Table 1: Key components
Module | Description
-------|------------------
BMDefs | Common definitions
BMQBase | Base of queue/ring-buffer structures
BMPoolBase | Base of pool structures
BMEv | Event message class
BMEvQ | Event queue and its pool
BMBuffer | Linear byte buffer and its pool
BMRingBuffer | Ring byte buffer and its pool
BMErr | Error codes, logging and error handlers
BMComm | Comm port, read thread, read thread argument, etc.
BMCRC | CRC error detection code endec
BMTick | Systemwide interval timer and dispatchers

## `BMDefs`
Table 2: Macros and functions
Macros | Description
-------|------------------------
BMARRAY_SIZE | retrieve a size of an array
BMALIGN_ARCH | 2: 16 bit, 4: 32 bit, 8: 64 bit architectures
BMALIGN_MASK | 1: 16 bit, 3: 32 bit, 7: 64 bit architectures
BMALIGN_DOWN | floor of alignment boundary
BMALIGN_UP | ceiling of alignment boundary

## `BMQBase`
Table 3.1: Struct
Struct | Description
-------|-----------------
BMQBase_t | Base of queue/ring-buffer structures
uint16_t .wridx | write index
uint16_t .rdidx | read index
uint16_t .count | element count in the queue(see note)
</br>
note: Actual capacity is equal to (.count - 1).

Table 3.2: Macros and functions
Macros/funcs | Descriptions
-------|-----------------
BMQBase(_count) | Initializer of BMQBase_t
BMQBase_LOCK(_qbase) | exclusive lock
BMQBase_UNLOCK(_qbase) | exclusive unlock
uint16_t BMQBase_NextWrIdx(_qbaseptr) | next value of wridx
uint16_t BMQBase_NextRdIdx(_qbaseptr) | next value of rdidx

## `BMPoolBase'
Table 4.1: Struct
Struct | Description
-------|-------------
BMPoolBase_t | Base of pool structures
uint16_t .count | elements in the pool
uint16_t .used[] | flags indicating used elements

Table 4.2: Macros and functions
Macros/funcs | Descriptions
-------------|-----------------
BMPoolBase_INIT(_pbptr) | Initialize spin lock
BMPoolBase_DEINIT(_pbptr) | Deinitialize spin lock
BMPoolBase_LOCK(_pbptr) | exclusive lock
BMPoolBase_UNLOCK(_pbptr) | exclusive unlock
BMPoolBase_FindAvailable(pbptr) | find the 1st available element
BMPoolBase_Reset(pbptr, offs) | reset used flag specified by offs.

## `BMEv`
Event message carrier and its pool.</br>
Table 5.1: Struct
Struct | Descriptions
-------|-------------
BMEv_t | Event message carrier
BMEvId_t .id | event id
uint16_t .listeners | event listener counter; i.e. a kind of reference counter.
void* .param | using .param, an event can carry any information.
BMEvPool_t | Event message pool
BMPoolBase_t .base | base class
BMEv_pt .ev | pointer to the first element of an array.
</br>

Table 5.2: Macros and functions
Macros/funcs | Descriptions
-------------|-----------------
