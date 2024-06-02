#if !defined(BMCOMMBASE_H)
#define BMCOMMBASE_H
#include "BMDefs.h"

#pragma region BAUDRATE_UTILITY
/*!
\brief convert baudrate descriptor to baudrate (bit/sec)
*/
int BMBaudDesc_ToBaudrate(int bauddesc);

/*!
\brief convert baudrate to baudrate descriptor.
*/
int BMBaudDesc_FromBaudrate(int baudrate);

/*!
\brief Get seconds required to transfer a byte.
*/
BMStatus_t
BMBaudDesc_ToSecPerByte(int fd, double* secPerByte);
#pragma endregion BAUDRATE_UTILITY
#pragma region SERIALPORT_BASIC_CONTROL
// BMComm_t manage a file descriptor and its byte time perild.
/*!
\brief serialport configuration struct.
*/
typedef struct {
    uint8_t devname[32]; // device name like "/dev/ttyUSB0"
    int bauddesc; // baudrate descriptor defined in termios.h
} BMCommConf_t, *BMCommConf_pt;
typedef const BMCommConf_t *BMCommConf_cpt;

typedef struct {
    int fd; // file descriptor
    double secPerByte; // 1 byte transfer time in seconds
} BMComm_t, *BMComm_pt;
typedef const BMComm_t *BMComm_cpt;

/*!
\brief open a serialport
\param conf [in] devicename and baudrate descriptor
\param pfd [out] file descriptor
\return result status
*/
BMStatus_t BMComm_Open(BMCommConf_cpt conf, BMComm_pt comm);

void BMComm_Close(BMComm_pt comm);
#pragma endregion SERIALPORT_BASIC_CONTROL
#endif /* BMCOMMBASE_H */