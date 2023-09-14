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

#ifndef _HSM_TESTS/* Guard against multiple inclusion */
#define _HSM_TESTS


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdint.h>
#include <stdbool.h>
#include "hsm_command.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

//Cmds
//void HsmCmdHashBlockSha256(bool int_mode);  
void HsmCmdHashBlockSha256(uint8_t * dataIn,  int numDataInBytes, 
                           uint8_t * dataOut, bool int_mode); 
void HsmCmdVsmInputDataUnencrypted(int vssSlotNum, bool int_mode); 
void HsmCmdVsmOutputDataUnencrypted(int vssSlotNum, bool int_mode); //Slot 1
void HsmCmdVsmDeleteSlot(int vssSlotNum, bool int_mode);            //Slot 1


//Commands
int HsmVsmSlotGetInfo(int vssSlotNum, bool int_mode); 

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _HSM_TESTS*/

/* *****************************************************************************
 End of File
 */
