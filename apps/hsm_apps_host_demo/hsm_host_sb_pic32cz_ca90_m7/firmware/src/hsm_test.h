/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    hsm_tests.h

  @Summary
    Decodes ascii hsm commands from serial byte string sent from PC

  @Description
    Decodes ascii hsm commands from serial byte string sent from PC
 */
/* ************************************************************************** */

#ifndef _HSM_TESTS_H/* Guard against multiple inclusion */
#define _HSM_TESTS_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdint.h>
#include <stdbool.h>
#include "hsm_command.h"
#include "hsm_command_globals.h"
#include "vsm.h"
#include "hash.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

// Test Data Struct
typedef struct {
  uint32_t           testFailCnt;      // Variable to keep track of test errs
  bool               rspChksPassed; // Variable to keep track of whether 
                                    //    command response completed 
  HsmCmdReq          hsmCmdReq;     // Command Request for sending to 
                                    //     HSM ROM Test Driver
  bool               hsmRxInt;      // Boolean variable to indicate when 
  CmdResultCodes     resultCode;    // 
} TEST_DATA;

//Key Size
typedef enum _VssKeySize
{
    VSS_KEY_128 = 0,
    VSS_KEY_192 = 1,
    VSS_KEY_256 = 2,
    VSS_KEY_512 = 3
} VssKeySize;

//TEST Cmds
bool TestHsmCmdHashBlockSha256(void);
bool TestHsmCmdVsmInputDataUnencryptedRaw(int vssSlotNum, VssKeySize keySize); 
bool TestHsmCmdVsmOutputDataUnencryptedRaw(int vssSlotNum, VssKeySize keySize);
bool TestHsmCmdVsmDeleteSlot(int vssSlotNum);            //Slot 1

//Commands
//int HsmVsmSlotGetInfo(int vssSlotNum, bool int_mode); 

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _HSM_TESTS*/

/* *****************************************************************************
 End of File
 */
