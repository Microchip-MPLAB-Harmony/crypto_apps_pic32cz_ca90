#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pic32cz8110ca90208.h"
#include "system/system_module.h"
#include "core_cm7.h"
#include "user.h"
#include "hash.h"

//******************************************************************************
//HASH BLOCK Command Cmd - 1
//--TODO: Interrupt Mode
//******************************************************************************
RSP_DATA * HsmCmdHashBlockSha256(
                uint8_t * dataIn,  
                int       numDataInBytes, 
                uint8_t * dataOut)
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
    gHsmCmdReq.mbHeader        = 0x00f00018;
    gHsmCmdReq.cmdHeader       = CMD_HASH_HASH_BLOCK_SHA256_INST,
    gHsmCmdReq.cmdInputs[0]    = (uint32_t)(&(dmaDescriptorIn[0]));  // Unused
    gHsmCmdReq.cmdInputs[1]    = (uint32_t)(&(dmaDescriptorOut[0])); // Unused
    gHsmCmdReq.cmdInputs[2]    = numDataInBytes;
    gHsmCmdReq.cmdInputs[3]    = 0x00000000; // Unused

    gHsmCmdReq.expMbHeader     = 0x00200010;
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = 0x00000320;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = expHashBlockResult;
    gHsmCmdReq.expNumDataBytes = HASH_SHA256_RESULT_BYTES;

    dmaDescriptorIn[0].data.addr              = dataIn;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    //dmaDescriptorIn[0].flagsLength.s.length   = numDataBytes;
    dmaDescriptorIn[0].flagsLength.s.length   = numDataInBytes;
    dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorIn[0].flagsLength.s.discard  = 0;
    dmaDescriptorIn[0].flagsLength.s.realign  = 1;
    dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
    
    //dmaDescriptorOut[0].data.addr             = &hashInitBuffer;
    dmaDescriptorOut[0].data.addr             = dataOut;
    dmaDescriptorOut[0].next.s.stop           = 1;
    dmaDescriptorOut[0].next.s.addr           = 0;
    dmaDescriptorOut[0].flagsLength.s.length  = HASH_SHA256_RESULT_BYTES; //SHA26 Hash Size
    dmaDescriptorOut[0].flagsLength.s.cstAddr = 0;
    dmaDescriptorOut[0].flagsLength.s.discard = 0;
    dmaDescriptorOut[0].flagsLength.s.realign = 1;
    dmaDescriptorOut[0].flagsLength.s.intEn   = 0;

    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorIn,  sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr(dataOut, sizeof(dataOut));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dataIn, numDataInBytes);

    HsmMbCmdDriver(&gHsmCmdReq, false);
    
    //Check the command response 
    HsmCmdRspChkr(rsp, true);

    return rsp;
    
} //End HsmCmdHashBlockSha256() 