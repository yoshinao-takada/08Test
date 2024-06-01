#if !defined(BMDLLAYERDBG_H)
#define BMDLLAYERDBG_H
#include "BMDLLayer.h"
typedef BMStateResult_t 
    (*BMDLLayerState_f)(BMDLDecoder_pt decoder, uint8_t byte);

typedef enum {
    BMDLLayerDBG_WHMK = 0,
    BMDLLayerDBG_GFRL0,
    BMDLLayerDBG_GFRL1,
    BMDLLayerDBG_RDPL,
    BMDLLayerDBG_CCRC0,
    BMDLLayerDBG_CCRC1,
    BMDLLayerDBG_FRCMP,
} BMDLLayerDBG_StateID_t;

BMDLLayerState_f BMDLLayerDBG_State(BMDLLayerDBG_StateID_t stateIndex);
#endif /* BMDLLAYERDBG_H */