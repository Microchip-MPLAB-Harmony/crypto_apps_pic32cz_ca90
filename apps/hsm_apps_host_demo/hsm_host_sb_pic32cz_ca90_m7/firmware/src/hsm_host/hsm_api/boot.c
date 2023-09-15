/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    boot.c

  @Summary
    Secure Boot HSM from the M7 Host API Commands

  @Description
    Secure Boot HSM from the M7 Host API Commands
 */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "pic32cz8110ca90208.h"
#include "core_cm7.h"
#include "user.h"
#include "system/system_module.h"
#include "boot.h"
#include "hash.h"
#include "app.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif // min


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

extern uint32_t dummy32;

/* ************************************************************************** */
/* ************************************************************************** */
// Section: API Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

#define SELF_TEST_STATUS 0x00000410

//******************************************************************************
//CMD_BOOT_SELF_TEST
//--This command uses the command header defined in the boot commands section 
//  with no changes.
//******************************************************************************
RSP_DATA * HsmCmdBootSelfTest()
               
{
    RSP_DATA *                    rsp = &gRspData;

    // Reset the response checker global variable
    rsp->invArgs       = false;
    rsp->rspChksPassed = false;
    rsp->resultCode    = S_OK;
    rsp->testFailCnt   = 0;

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);

    gHsmCmdReq.mbHeader      = 0x00f00018; //5 Words
    gHsmCmdReq.cmdHeader     = CMD_BOOT_SELF_TEST; 
    gHsmCmdReq.cmdInputs[0]  = 0x00000000; // IN: Unused
    gHsmCmdReq.cmdInputs[1]  = 0x00000000; //OUT: Unused
    gHsmCmdReq.cmdInputs[2]  = 0x0c7df800; // P1: Location of the Boot Image
    gHsmCmdReq.cmdInputs[3]  = 0x00000000; // P2: unused

    //Input SG for Key Block Data
    dmaDescriptorIn[0].data.addr              = &dummy32;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    dmaDescriptorIn[0].flagsLength.s.length   = 1; 
    dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorIn[0].flagsLength.s.discard  = 0;
    dmaDescriptorIn[0].flagsLength.s.realign  = 1;
    dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
    //SYS_MESSAGE("VSM INPUT DATA SG:\r\n");
    //PrintSG(dmaDescriptorIn[0], true);
    
    //Output (Not Used)
    dmaDescriptorOut[0].data.addr             = &dummy32;
    dmaDescriptorOut[0].next.s.stop           = 1;
    dmaDescriptorOut[0].next.s.addr           = 0;
    dmaDescriptorOut[0].flagsLength.s.length  = 1;
    dmaDescriptorOut[0].flagsLength.s.cstAddr = 0;
    dmaDescriptorOut[0].flagsLength.s.discard = 0;
    dmaDescriptorOut[0].flagsLength.s.realign = 1;
    dmaDescriptorOut[0].flagsLength.s.intEn   = 0;

    //Expected Response 
    gHsmCmdReq.expMbHeader     = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = SELF_TEST_STATUS;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0x00000000;
    gHsmCmdReq.expNumDataBytes = 0;
    
    //SCB_CleanDCache_by_Addr((uint32_t *)           dmaDescriptorIn,  sizeof(dmaDescriptorIn));
    //SCB_CleanDCache_by_Addr((uint32_t *)           dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dummy32,          sizeof(uint32_t));
    //SCB_CleanInvalidateDCache_by_Addr((uint32_t *) vsmInputDataPtr,  numSlotWords*sizeof(uint32_t));
    
    //SYS_PRINT("HSM: Sending CMD_VSM_INPUT_DATA Command\r\n");
    HsmMbCmdDriver(&gHsmCmdReq, false);

    HsmCmdRspChkr(rsp, true);
    //if (rsp.resultCode != E_INVFORMAT && rsp.resultCode != E_SAFEMODE)

    return rsp;
    
} //End HsmCmdBootSelfTest()
  

//******************************************************************************
// Send CMD_BOOT_SELF_TEST command to load the HSM firmware
//--This command uses the command header defined in the boot commands section 
//  with no changes.
//******************************************************************************
RSP_DATA * HsmCmdBootLoadFirmware()
               
{
    RSP_DATA *                    rsp = &gRspData;

    // Reset the response checker global variable
    rsp->invArgs       = false;
    rsp->rspChksPassed = false;
    rsp->resultCode    = S_OK;
    rsp->testFailCnt   = 0;

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);

    gHsmCmdReq.mbHeader      = 0x00f00014; //  MB: 5 Words
    gHsmCmdReq.cmdHeader     = 0x00000100; // CMD: Boot
    gHsmCmdReq.cmdInputs[0]  = 0x00000000; //  IN: Unused
    gHsmCmdReq.cmdInputs[1]  = 0x00000000; // OUT: Unused
    gHsmCmdReq.cmdInputs[2]  = 0x0c7df800; //  P1: Location of the Boot Image
    gHsmCmdReq.cmdInputs[3]  = 0x00000000; //  P2: unused

    //Input SG for Key Block Data
    dmaDescriptorIn[0].data.addr              = &dummy32;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    dmaDescriptorIn[0].flagsLength.s.length   = 1; 
    dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorIn[0].flagsLength.s.discard  = 0;
    dmaDescriptorIn[0].flagsLength.s.realign  = 1;
    dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
    //SYS_MESSAGE("VSM INPUT DATA SG:\r\n");
    //PrintSG(dmaDescriptorIn[0], true);
    
    //Output (Not Used)
    dmaDescriptorOut[0].data.addr             = &dummy32;
    dmaDescriptorOut[0].next.s.stop           = 1;
    dmaDescriptorOut[0].next.s.addr           = 0;
    dmaDescriptorOut[0].flagsLength.s.length  = 1;
    dmaDescriptorOut[0].flagsLength.s.cstAddr = 0;
    dmaDescriptorOut[0].flagsLength.s.discard = 0;
    dmaDescriptorOut[0].flagsLength.s.realign = 1;
    dmaDescriptorOut[0].flagsLength.s.intEn   = 0;

    //Expected Response 
    gHsmCmdReq.expMbHeader     = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = SELF_TEST_STATUS;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0x00000000;
    gHsmCmdReq.expNumDataBytes = 0;
    
    //SCB_CleanDCache_by_Addr((uint32_t *)           dmaDescriptorIn,  sizeof(dmaDescriptorIn));
    //SCB_CleanDCache_by_Addr((uint32_t *)           dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dummy32,          sizeof(uint32_t));
    //SCB_CleanInvalidateDCache_by_Addr((uint32_t *) vsmInputDataPtr,  numSlotWords*sizeof(uint32_t));
    
    //SYS_PRINT("HSM: Sending CMD_VSM_INPUT_DATA Command\r\n");
    HsmMbCmdDriver(&gHsmCmdReq, false);

    HsmCmdRspChkr(rsp, true);
    //if (rsp.resultCode != E_INVFORMAT && rsp.resultCode != E_SAFEMODE)

    return rsp;
    
} //End HsmCmdBootSelfTest()


#define BOOT_HASH_INIT_WORDS 2
uint8_t bootHashInitBuffer[64] = 
{   
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x51, 0x52, 0x63, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
};

int32_t expBootHashResult[BOOT_HASH_INIT_WORDS] = {0x6A09E667, 0xBB67AE85};

//******************************************************************************
//Firmware Validation Test using SHA256 Hash
//******************************************************************************
RSP_DATA * HsmCmdBootTestHashInit()
{
    RSP_DATA * rsp = &gRspData;
    //static int numDataBytes = 0;

    rsp = &gRspData;

    rsp->invArgs = false;

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);

    //TODO: Pad input data to 32bit boundary
    
    // Send HASH BLOCK command request to HSM MB 
    // -- External Data
    // -- External Result
    gHsmCmdReq.mbHeader        = 0x00f00014; //MB:
    gHsmCmdReq.cmdHeader       = 0x00040105; //CMD: 
    gHsmCmdReq.cmdInputs[0]    = 0x00000000; //IN: Unused 
    gHsmCmdReq.cmdInputs[1]    = (uint32_t)(&(dmaDescriptorOut[0])); //OUT 
    gHsmCmdReq.cmdInputs[2]    = 0x00000000; //P1
    gHsmCmdReq.cmdInputs[3]    = 0x00000000; //P2

    gHsmCmdReq.expMbHeader     = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = 0x00000420;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0x00000000;
    gHsmCmdReq.expNumDataBytes = 0;
    //gHsmCmdReq.expData         = expHashBlockResult;
    //gHsmCmdReq.expNumDataBytes = BOOT_HASH_INIT_WORDS*BYTES_PER_WORD;

    dmaDescriptorIn[0].data.addr              = &dummy32;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    dmaDescriptorIn[0].flagsLength.s.length   = 1;
    dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorIn[0].flagsLength.s.discard  = 0;
    dmaDescriptorIn[0].flagsLength.s.realign  = 1;
    dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
    
    dmaDescriptorOut[0].data.addr             = (uint32_t *) bootHashInitBuffer;
    dmaDescriptorOut[0].next.s.stop           = 1;
    dmaDescriptorOut[0].next.s.addr           = 0;
    dmaDescriptorOut[0].flagsLength.s.length  = HASH_SHA256_RESULT_BYTES; //SHA26 Hash Size
    dmaDescriptorOut[0].flagsLength.s.cstAddr = 0;
    dmaDescriptorOut[0].flagsLength.s.discard = 0;
    dmaDescriptorOut[0].flagsLength.s.realign = 1;
    dmaDescriptorOut[0].flagsLength.s.intEn   = 0;

    //SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorIn,  sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr(bootHashInitBuffer, sizeof(bootHashInitBuffer));
    //SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dataIn, numDataInBytes);

    HsmMbCmdDriver(&gHsmCmdReq, false);
    
    //Check the command response 
    HsmCmdRspChkr(rsp, true);

    if (rsp->rspChksPassed)
    { 
        SYS_PRINT("BOOT SHA256:\r\n");
        PrintSG(dmaDescriptorOut[0], true);
    }

    return rsp;
    
} //End HsmCmdBootTestHashInit()

/* *****************************************************************************
 End of File
 */

