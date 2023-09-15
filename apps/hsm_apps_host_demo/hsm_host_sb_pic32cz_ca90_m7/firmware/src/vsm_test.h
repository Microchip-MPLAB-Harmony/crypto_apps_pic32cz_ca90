/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    vsm_tests.h

  @Summary
    VSM Tests

  @Description
    VSM Tests
    
 */
/* ************************************************************************** */

#ifndef _VSM_TESTS_H/* Guard against multiple inclusion */
#define _VSM_TESTS_H


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

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

//TEST Cmds
bool TestHsmCmdVsmInputDataUnencryptedAes(int vssSlotNum); 
bool TestHsmCmdVsmOutputDataUnencryptedAes(int vssSlotNum); 

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
