
/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#ifndef _HSM_COMMAND_GLOBALS /* Guard against multiple inclusion */
#define _HSM_COMMAND_GLOBALS

#include <stdint.h>
#include <stdbool.h>
#include "hsm_command.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Definitions and Structs                                           */
/* ************************************************************************** */
/* ************************************************************************** */

#define MAXSLOTDATABYTES   1024
#define MAXSLOTDATAWORDS   MAXSLOTDATABYTES/8 
#define MAX_KEY_LENGTH  CMD_AES_KEY_256
#define SLOTMETAWORDS      4


//====================================================================== 
// Mailbox/Command
//====================================================================== 

//******************************************************************************
//  Struct/Class
//******************************************************************************

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Externals/Function Prototypes                                               */
/* ************************************************************************** */
/* ************************************************************************** */
extern uint32_t               globalMbHeader; 
extern uint32_t               globalCmdHeader;
extern uint32_t               globalCmdInput;  
extern uint32_t               globalCmdOutput;
extern uint32_t               globalCmdParams[MAX_CMD_PARAM_WORDS];

extern uint32_t *             dataPtrOut;
extern uint32_t ALIGN4        dmaDataOut[MAX_OUTPUT_DMA_LENGTH];
extern uint32_t ALIGN4        keyHostStoragePublic[16];

//HSM Response
extern HsmCmdResp gHsmCmdResp;
extern RSP_DATA   gRspData;
extern HsmCmdReq  gHsmCmdReq;

//HSM Status
//extern bool           busy;  //HSM Busy
//extern HsmStatusECODE ecode; //Error Return Code
//extern HsmStatusSBS   sbs;   //Secure Boot Status
//extern HsmStatusLCS   lcs;   //Lifecycle Status 
//extern HsmStatusPS    ps;    //Processor Status

extern char messageBuffer[5][64];

extern char * gPrintStr;
extern char   gPrintBuffer[MAX_PRINT_BYTES];

//COM UART 
extern int volatile gRcvCnt;
extern int volatile gRcvCmdRdy;

extern uint8_t gRcvCmdStr[MAX_PRINT_BYTES];

//HSM Status Register Fields
extern const char * ecodeStr[];
extern const char * sbsStr[];
extern const char * lcsStr[];
extern const char * psStr[];
extern char * cmdResultStr[NUMCMDRESULTCODES];
extern char * cmdSpecialResultStr[NUMCMDERRORRESULTCODES];
extern char * cmdHsmErrorStr[];

extern CmdSGDescriptor ALIGN4 dmaDescriptorIn[3]; 
extern CmdSGDescriptor ALIGN4 dmaDescriptorOut[3];
extern uint32_t ALIGN4 dmaDataIn[MAX_INPUT_DMA_LENGTH];

extern uint32_t vssSlotData[MAXSLOTDATAWORDS+SLOTMETAWORDS];

// Function Prototypes
char * CmdResultCodeStr(uint32_t cmdResultCode);


/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _HSM_COMMAND_GLOBALS */

/* *****************************************************************************
 End of File
 */
