# Data Link Layer Format
## Overview
Data Link Layer detects frame boundaries.
We consider a binary frame format descirbed below.
```
[BMK][LEN][PL][CRC]
```
where</br>
`[BMK]` : beginning mark, two bytes, 0xaa, 0x55</br>
`[LEN]` : payload length, little endian uint16_t</br>
`[PL]` : payload, any byte sequence. Its length is equal to `[LEN]`.</br>
`[CRC]` : CCTIT16 CRC code calculated with seed 0xc000.

## State machine parsing the frame
The state machine reads a byte sequence byte-by-byte.
The state machine check the state and input on each byte.
