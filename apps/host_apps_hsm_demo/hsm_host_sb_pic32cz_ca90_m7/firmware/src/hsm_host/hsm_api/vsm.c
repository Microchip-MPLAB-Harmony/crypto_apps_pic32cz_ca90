/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    vsm.c

  @Summary
    VSM MB API Commands

  @Description
    VSM MB API Commands
 */
/* ************************************************************************** */

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
#include "vsm.h"
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

char * vsmStorageTypeStr[] =
{
    "VM_STORAGE",
    "NVM_UNENCRYPTED",
    "NVM_FLASHKEYPROTECTED",
    "NVM_TAMPERKEYPROTECTED"
};

char * vsmSlotTypeStr[] =
{
    "VSS_RAW",
    "VSS_SYMMETRICALKEY",
    "VSS_ASYMMETRICALKEY ",
    "VSS_HASHVALUE",
    "VSS_HASHIV",
};

char * vsmSlotInfoTypeStr[] =
{
    "CMD_VSM_NOT_ENCRYPTED",
    "CMS_VSM_ENCRYPTED",
    "CMD_VSM_WRAPPED_ENCRYPTED"
};

char * VssSymmetricalKeyTypeStr[] =
{
    "VSS_SK_AES",
    "VSS_SK_CHACHA",
    "VSS_SK_TDES",
    "VSS_SK_DES",
};


/* ************************************************************************** */
/* ************************************************************************** */
// Section: API Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

//******************************************************************************
// CMD_VSM_INPUT_DATA - Unencrypted VSS Internal Slot Input Command--
//
// NOTE:  APL=0, No Auth, Unencrypted Slot, NVM_Unencrypted Storage Type 
//
//******************************************************************************
RSP_DATA * HsmCmdVsmInputDataUnencrypted(
               int                        vssSlotNum,
               uint32_t *                 vsmInputDataPtr, //including meta
               unsigned short             numSlotWords,
               CmdVSMSlotType             slotType,
               CmdVSMDataSpecificMetaData specMetaData)
{
    CmdVSMInputSlotInfoParameter1 vsmInputParam1;
    RSP_DATA *                    rsp = &gRspData;
    //VSHeader *                    vsHeaderPtr;

    // Reset the response checker global variable
    rsp->invArgs       = false;
    rsp->rspChksPassed = false;
    rsp->resultCode    = S_OK;
    rsp->testFailCnt   = 0;

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);

    //Send CMD_VSM_INPUT_DATA - Input Clear Data
    //--No TA/No Slot/No Auth
    gHsmCmdReq.mbHeader      = 0x00f00014; //5 Words
    gHsmCmdReq.cmdHeader     = CMD_VSM_INPUT_DATA_INST; //CMD_VSM_INPUT_DATA
    gHsmCmdReq.cmdInputs[0]  = (uint32_t)(&(dmaDescriptorIn[0]));
    gHsmCmdReq.cmdInputs[1]  = 0x00000000; //OUT: Unused

    vsmInputParam1.v = 0;
    vsmInputParam1.s.slotInfo                    = CMD_VSM_NOT_ENCRYPTED;
    vsmInputParam1.s.slotNumber                  = vssSlotNum;
    vsmInputParam1.s.slotType                    = slotType;  //Sym/Asym/hash/iv
    vsmInputParam1.s.vsStorageData.s.apl         = 0;  //Only storage that is unencrypt. 
    vsmInputParam1.s.vsStorageData.s.storageType = NVM_UNENCRYPTED; 
    //TODO: vsStorageData HSMonly/ext/valid

    gHsmCmdReq.cmdInputs[2]  = vsmInputParam1.v;
    //SYS_PRINT("XXCMD INP[2]: 0x%08lx, VS#%d\r\n",
    //           gHsmCmdReq.cmdInputs[2], vssSlotNum);
    
    gHsmCmdReq.cmdInputs[3]  = 0x00000000; //(unused)

    //Input SG for Key Block Data
    dmaDescriptorIn[0].data.addr              = vsmInputDataPtr;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    dmaDescriptorIn[0].flagsLength.s.length   = 
                (unsigned int) (numSlotWords+1)*BYTES_PER_WORD; 
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
    gHsmCmdReq.expMbHeader     = 0x0020000c;
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = 0x00000320;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0x00000000;
    gHsmCmdReq.expNumDataBytes = 0;
    
    SCB_CleanDCache_by_Addr((uint32_t *)           dmaDescriptorIn,  sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *)           dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dummy32,          sizeof(uint32_t));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) vsmInputDataPtr,  numSlotWords*sizeof(uint32_t));
    
    //SYS_PRINT("HSM: Sending CMD_VSM_INPUT_DATA Command\r\n");
    HsmMbCmdDriver(&gHsmCmdReq, false);

    HsmCmdRspChkr(rsp, true);

    return rsp;
    
} //End HsmCmdVsmInputDataUnencrypted()

//******************************************************************************
//CMD_VSM_OUTPUT_DATA Unencrypted Internal Slot Output Data Command--
//
//   MB: 0x00f00014 (20 bytes, 5 Words)
//  CMD: 0x00000003 (VS Management, Input Key Data to slot)
//  OUT: 0x00000000 (SG descriptor for VSS Data)
//   P1: KK|00|SS|0O 
//       KK(31:24) Key Slot       -- VS# wrapped key slot/data encrypt key slot 
//       00(23-16) reserved
//       SS(15:08) Storage Slot# -- VS# where data stored
//       0O(07-00) Output Type   -- UNEN|ENCR|WRAPPED
//
//******************************************************************************
RSP_DATA * HsmCmdVsmOutputDataUnencrypted(
                int        vssSlotNum, 
                uint32_t * dataOut,
                int      * numDataWords,
                uint32_t   maxDataBytes)
{
    RSP_DATA * rsp = &gRspData;
    //VSMetaData vsMetaData;
    CmdVSMOutputSlotParameter1 vssOutputParam1;
    CmdResultCodes  __attribute__((unused))  rcode;
    uint32_t        __attribute__((unused))  slotInfoBytes;

    //Reset the response 
    rsp->invArgs       = false;
    rsp->rspChksPassed = false;
    rsp->testFailCnt   = 0;
    rsp->resultCode    = S_OK;

    vssOutputParam1.v               = 0x00000000;
    //SYS_PRINT("***NOTE:  Using keySlotNumber param to set slotNum\r\n***");
    vssOutputParam1.s.slotNumber    = (uint8_t) vssSlotNum;
    vssOutputParam1.s.slotInfo      = CMD_VSM_NOT_ENCRYPTED;

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    //SYS_PRINT("HSM: CMD_VSM_OUTPUT_DATA Slot %d Command\r\n", vssSlotNum);

#undef USEVSINFO
#ifdef USEVSINFO 
    //Check Slot Info 
    //SYS_PRINT("VSM Get Slot Info Slot#%d\r\n",vssSlotNum);
    rcode = HsmCmdVsmGetSlotInfo(vssSlotNum, &vsMetaData, &slotInfoBytes); 
    if (rcode != S_OK)
    {
        //SYS_PRINT("HSM Key Slot# %d Empty or Invalid - Abort Output Data Command\r\n", 
        //           vssSlotNum);
        rsp->invArgs       = true;
        rsp->rspChksPassed = false;
        rsp->testFailCnt   = 0;
        rsp->resultCode    = rcode;
        return rsp;
    }

    // Make sure the HSM is not busy after the Slot Info Command
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
#endif //0
    
    //Send CMD_VSM_OUTPUT_DATA - Input Clear Data/Internal Slot/APL 0/No Auth
    //--No TA/No Slot/No Auth
    gHsmCmdReq.mbHeader     = 0x00f00014;          //5 Words/PROT bits
    gHsmCmdReq.cmdHeader    = CMD_VSM_OUTPUT_DATA_INST; //CMD_VSM_OUTPUT_DATA

    //IN/OUT
    gHsmCmdReq.cmdInputs[0] = 0x00000000;          //IN: (No Auth - Unused)
    gHsmCmdReq.cmdInputs[1] = (uint32_t)(&(dmaDescriptorOut[0])); //OUT

    //PARAM
    gHsmCmdReq.cmdInputs[2] = vssOutputParam1.v;   
    //SYS_PRINT("XXCMD INP[2]: 0x%08lx \r\n", gHsmCmdReq.cmdInputs[2]);
    gHsmCmdReq.cmdInputs[3] = 0x00000000;          //(unused)

    //Expected CMD Response 
    gHsmCmdReq.expMbHeader     = 0x00200010; //4 Words
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = 0x00000320;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0x00000000;
    gHsmCmdReq.expNumDataBytes = 0x00000000; 

    //Input SG (unused))
    dmaDescriptorIn[0].data.addr              = &dummy32;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    dmaDescriptorIn[0].flagsLength.s.length   = 0;
    dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorIn[0].flagsLength.s.discard  = 0;
    dmaDescriptorIn[0].flagsLength.s.realign  = 1;
    dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
    
    //Output SG for Key Block Result
    dmaDescriptorOut[0].data.addr              = dataOut;
    dmaDescriptorOut[0].next.s.stop            = 1;
    dmaDescriptorOut[0].next.s.addr            = 0;
    dmaDescriptorOut[0].flagsLength.s.length   = (*numDataWords)*BYTES_PER_WORD; 
    dmaDescriptorOut[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorOut[0].flagsLength.s.discard  = 0;
    dmaDescriptorOut[0].flagsLength.s.realign  = 1;
    dmaDescriptorOut[0].flagsLength.s.intEn    = 0;  
    
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorIn, sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dataOut, maxDataBytes);
    //SCB_CleanInvalidateDCache_by_Addr((uint32_t *) vsmInputDataRaw, 12*sizeof(uint32_t));
    
    //SYS_MESSAGE("VSM OUT SG (Data Prior to DMA Write):\r\n");
    //PrintSG(dmaDescriptorOut[0], true);
    HsmMbCmdDriver(&gHsmCmdReq, false);


    SYS_MESSAGE("****HSM Cmd Response Checks****\r\n");
    // Wait for an interrupt in interrupt mode
    //if (int_mode) 
    //{
          //SYS_PRINT("HSM ROM Cmd: Waiting for interrupt...\r\n");
          //while (!gRspData.hsm_rx_int);
          //gRspData.hsm_rx_int = false;
    //} 
    HsmCmdRspChkr(rsp, true);

    //if (gHsmCmdResp.resultCode == S_OK)
    //{
    //    vsMetaData.vsHeader             = (VSHeader) dataOut[0];
    //    vsMetaData.validAfter           = dataOut[1];
    //    vsMetaData.validBefore          = dataOut[2];
    //    vsMetaData.dataSpecificMetaData = dataOut[3];
    //}

    //TODO:  Other Key types, other than RAW
    //int slotSizeBytes = 0;

    //if (vsMetaData.vsHeader.s.vsSlotType == VSS_RAW)
    //{
    //    slotSizeBytes = vsMetaData.dataSpecificMetaData; 
    //}
    //else
    //{
    //    //TODO: 
    //    slotSizeBytes = 0;
    //}

    //Check the FiFo data count against the metadata derived count
    //if (gHsmCmdResp.numResultWords == 1)
    //{
    //    if (slotSizeBytes > 0 &&
    //        slotSizeBytes != gHsmCmdResp.resultData[0])
    //    {
            SYS_PRINT("DMA slot size (%d bytes)\r\n", 
                (int)gHsmCmdResp.resultData[0]);
    //      numDataWords = gHsmCmdResp.resultData[0]/BYTES_PER_WORD;
    //    }
        //else
        //{
        //    *numDataWords = gHsmCmdResp.resultData[0];
        //}
    //}
    //else
    //{
        //Not Valid Output
    //    *numDataWords = slotSizeBytes/BYTES_PER_WORD;
    //}

    return rsp;

} //End HsmCmdVsmOutputDataUnencrypted()


//******************************************************************************
//CMD_VSM_SLOT_GET_INFO Command
//
//   Return ErrCode:
//   -1 - General Error
//    0  - Slot Info Available
//    1  - VS is Empty
//
//******************************************************************************
CmdResultCodes HsmCmdVsmGetSlotInfo(int         vssSlotNum, 
                                    VSMetaData *vsMetaData, 
                                    uint32_t   *slotSizeBytes)
{
    static CmdVSMGetSlotInfoParameter1 cmdParam1;

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    //SYS_PRINT("VSM SLOT INFO (Slot %d):\r\n", vssSlotNum);

    //Send CMD_VSM_GET_SLOT_INFO - Input Clear Data/Internal Slot/APL 0/No Auth
    //--No TA/No Slot/No Auth
    gHsmCmdReq.mbHeader     = 0x00f00014;            //5 Words/PROT bits
    gHsmCmdReq.cmdHeader    = CMD_VSM_SLOT_GET_INFO_INST; //CMD_VSM_SLOT_GET_INFO
    gHsmCmdReq.cmdInputs[0] = 0x00000000;            //IN: (No Auth - Unused)
    gHsmCmdReq.cmdInputs[1] = (uint32_t)(&(dmaDescriptorOut[0])); //OUT

    //Parameters
    cmdParam1.s.slotNumber = vssSlotNum;  //Only field
    gHsmCmdReq.cmdInputs[2] = (uint32_t) cmdParam1.v; 
    gHsmCmdReq.cmdInputs[3] = 0x00000000;           //(unused)

    //Expected Response CMD
    gHsmCmdReq.expMbHeader     = 0x00200010; //4 Words(UNPROT))
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = 0x00000320;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0;
    gHsmCmdReq.expNumDataBytes = 0;

    //Input SG (unused))
    dmaDescriptorIn[0].data.addr              = &dummy32;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    dmaDescriptorIn[0].flagsLength.s.length   = 0;
    dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorIn[0].flagsLength.s.discard  = 0;
    dmaDescriptorIn[0].flagsLength.s.realign  = 1;
    dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
    
    //Output SG /VS Header/NV Before/NV After/VS Meta
    dmaDescriptorOut[0].data.addr              = vsmSlotInfoOut;
    dmaDescriptorOut[0].next.s.stop            = 1;
    dmaDescriptorOut[0].next.s.addr            = 0;
    dmaDescriptorOut[0].flagsLength.s.length   = 16; //4 Words
    dmaDescriptorOut[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorOut[0].flagsLength.s.discard  = 0;
    dmaDescriptorOut[0].flagsLength.s.realign  = 1;
    dmaDescriptorOut[0].flagsLength.s.intEn    = 0;  
    
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorIn, sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dummy32, 4);
    
    //Send HSM Command
    HsmMbCmdDriver(&gHsmCmdReq, false);

    // Check Response
    // --Mailbox Header
    if (gHsmCmdResp.mbHeader.v != gHsmCmdReq.expMbHeader) 
    {
        return gHsmCmdResp.resultCode;
    }
  
    //Check the Command Header 
    if (gHsmCmdResp.cmdHeader != gHsmCmdReq.cmdHeader) 
    {
        return gHsmCmdResp.resultCode;
    }

    if (gHsmCmdResp.resultCode == S_OK)
    {
        vsMetaData->vsHeader             = (VSHeader) vsmSlotInfoOut[0];
        vsMetaData->validBefore          = vsmSlotInfoOut[1];
        vsMetaData->validAfter           = vsmSlotInfoOut[2];
        vsMetaData->dataSpecificMetaData = vsmSlotInfoOut[3];
    }

    //TODO:  Other Key types, other than RAW
    if (vsMetaData->vsHeader.s.vsSlotType == VSS_RAW)
    {
        *slotSizeBytes = vsMetaData->dataSpecificMetaData; 
    }
    else
    {
        //This should be specific key sizes
        *slotSizeBytes = 0;
    }

    
    return gHsmCmdResp.resultCode;

} //End HsmCmdVsmGetSlotInfo()


//******************************************************************************
//CMD_VSM_SLOT_GET_INFO Command
//
//   Return ErrCode:
//   -1 - General Error
//    0  - Slot Info Available
//    1  - VS is Empty
//
//******************************************************************************

SLOTINFORETURN HsmCmdVsmPrintSlotInfo(int vssSlotNum) 
{
    static CmdVSMGetSlotInfoParameter1 cmdParam1;
    char * rcStr;

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    SYS_PRINT("CMD_VSM_GET_SLOT_INFO (VSS %d)\r\n", vssSlotNum);

    //Send CMD_VSM_GET_SLOT_INFO - Input Clear Data/Internal Slot/APL 0/No Auth
    //--No TA/No Slot/No Auth
    gHsmCmdReq.mbHeader     = 0x00f00014;            //5 Words/PROT bits
    gHsmCmdReq.cmdHeader    = CMD_VSM_SLOT_GET_INFO_INST; //CMD_VSM_SLOT_GET_INFO
    gHsmCmdReq.cmdInputs[0] = 0x00000000;            //IN: (No Auth - Unused)
    gHsmCmdReq.cmdInputs[1] = (uint32_t)(&(dmaDescriptorOut[0])); //OUT

    //Parameters
    cmdParam1.s.slotNumber = vssSlotNum;  //Only field
    gHsmCmdReq.cmdInputs[2] = (uint32_t) cmdParam1.v; 
    gHsmCmdReq.cmdInputs[3] = 0x00000000;           //(unused)

    //Expected Response CMD
    gHsmCmdReq.expMbHeader     = 0x00200010; //4 Words(UNPROT))
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = 0x00000320;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0;
    gHsmCmdReq.expNumDataBytes = 0;

    //Input SG (unused))
    dmaDescriptorIn[0].data.addr              = &dummy32;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    dmaDescriptorIn[0].flagsLength.s.length   = 0;
    dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorIn[0].flagsLength.s.discard  = 0;
    dmaDescriptorIn[0].flagsLength.s.realign  = 1;
    dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
    
    //Output SG /VS Header/NV Before/NV After/VS Meta
    dmaDescriptorOut[0].data.addr              = vsmSlotInfoOut;
    dmaDescriptorOut[0].next.s.stop            = 1;
    dmaDescriptorOut[0].next.s.addr            = 0;
    dmaDescriptorOut[0].flagsLength.s.length   = 16; //4 Words
    dmaDescriptorOut[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorOut[0].flagsLength.s.discard  = 0;
    dmaDescriptorOut[0].flagsLength.s.realign  = 1;
    dmaDescriptorOut[0].flagsLength.s.intEn    = 0;  
    
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorIn, sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dummy32, 4);
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) vsmSlotInfoOut, 16);
    
    //Send HSM Command
    HsmMbCmdDriver(&gHsmCmdReq, false);

    // Check Response
    // --Mailbox Header
    if (gHsmCmdResp.mbHeader.v != gHsmCmdReq.expMbHeader) 
    {
        if (gHsmCmdResp.resultCode != E_INVFORMAT && 
            gHsmCmdResp.resultCode != E_SAFEMODE)
        {
            SYS_MESSAGE("HSM CMD VSS Slot Info FAIL: Mailbox Header Match\r\n");
            SYS_PRINT("Expected: 0x%08lx\r\n", (uint32_t)gHsmCmdReq.expMbHeader);
            SYS_PRINT("Actual:   0x%08lx\r\n", (uint32_t)gHsmCmdResp.mbHeader.v);
            //return -1;
        }
    }
  
    //Check the Command Header 
    if (gHsmCmdResp.cmdHeader != gHsmCmdReq.cmdHeader) 
    {
        if (gHsmCmdResp.resultCode != E_INVFORMAT && 
            gHsmCmdResp.resultCode != E_SAFEMODE)
        {
             SYS_MESSAGE("HSM CMD VSS Slot Info FAIL: Command Header Match\r\n");
             SYS_PRINT("Expected: 0x%08lx\r\n", (uint32_t)gHsmCmdReq.cmdHeader);
             SYS_PRINT("Actual:   0x%08lx\r\n", (uint32_t)gHsmCmdResp.cmdHeader);
             return ERR_GENERAL;
        }
        else
        {
            SYS_MESSAGE("HSM FAIL: INV Command or SAFEMODE:\r\n");
            return ERR_GENERAL;
        }
    }
  
    // Check the Result Code
    VSMetaData vsMetaData;
    int        slotSizeBytes;

    rcStr = CmdResultCodeStr(gHsmCmdResp.resultCode);
    if (gHsmCmdResp.resultCode != S_OK)
    {
        if (gHsmCmdResp.resultCode == E_VSEMPTY)
        {
            SYS_PRINT("VS #%d is EMPTY\r\n", vssSlotNum);
            return VS_SLOT_EMPTY;
        }
        else if (gHsmCmdResp.resultCode == E_VSINUSE)
        {
            SYS_PRINT("VS #%d IN USE\r\n", vssSlotNum);
            return ERR_GENERAL;
        }
        else
        {
            SYS_PRINT("!!!VS #%d - %s!!!\r\n", vssSlotNum, rcStr);
            return ERR_GENERAL;
        }
    }
    else if (gHsmCmdResp.resultCode == S_OK)
    {
        vsMetaData.vsHeader             = (VSHeader) vsmSlotInfoOut[0];
        vsMetaData.validBefore          = vsmSlotInfoOut[1];
        vsMetaData.validAfter           = vsmSlotInfoOut[2];
        vsMetaData.dataSpecificMetaData = vsmSlotInfoOut[3];
    }

    //TODO:  Other Key types, other than RAW
    if (vsMetaData.vsHeader.s.vsSlotType == VSS_RAW)
    {
        slotSizeBytes = vsMetaData.dataSpecificMetaData; 
    }
    else
    {
        //Not really.
        slotSizeBytes = vsMetaData.dataSpecificMetaData;
    }
    
    SYS_PRINT("VSS Slot# %d - slotSize %d bytes\r\n",
                   vssSlotNum, (uint16_t)slotSizeBytes);
  
    SYS_PRINT("VS #%d Info:\r\n",vssSlotNum); CRLF;
    VsHeaderInfo(&vsMetaData.vsHeader); CRLF;
    SYS_PRINT("  VSS Before: 0x%08x\r\n",vsMetaData.validBefore);
    SYS_PRINT("  VSS  After: 0x%08x\r\n",vsMetaData.validAfter);
    SYS_PRINT("  VSS   Meta: 0x%08x\r\n",vsMetaData.dataSpecificMetaData);
    //PrintSG(dmaDescriptorOut[0], true);

    return VS_SLOT_INFO_AVAIL;

} //End HsmCmdVsmPrintSlotInfo()



//******************************************************************************
//CMD_VSM_DELETE_SLOT Command--Delete the slot data
//   returns: ResultCode
//******************************************************************************
RSP_DATA * HsmCmdVsmDeleteSlot(int vssSlotNum) 
{
    RSP_DATA *                        rsp = &gRspData;
    static CmdVSMDeleteSlotParameter1 cmdParam1;

    SYS_PRINT("HSM: Sending CMD_VSM_DELETE_SLOT Command (VSS %d)\r\n",
            vssSlotNum);

    // Reset the response checker global variable
    rsp->invArgs       = false;
    rsp->rspChksPassed = false;
    rsp->resultCode    = S_OK;
    rsp->testFailCnt   = 0;


    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);

    //Send CMD_VSM_DELETE_SLOT - Input Clear Data/Internal Slot/APL 0/No Auth
    //--No TA/No Slot/No Auth
    gHsmCmdReq.mbHeader     = 0x00f00014;          //5 Words/PROT bits
    gHsmCmdReq.cmdHeader    = CMD_VSM_DELETE_SLOT_INST; //CMD_VSM_DELETE_SLOT
    gHsmCmdReq.cmdInputs[0] = 0x00000000; //IN: (No Auth - Unused)
    gHsmCmdReq.cmdInputs[1] = 0x00000000; //OUT

    //gHsmCmdReq.cmdInputs[2] = CMD_VSM_SLOT_NUMBER_FIELD & 
    //                         vssSlotNum<<CMD_VSM_SLOT_NUMBER_SHIFT;
    cmdParam1.v            = 0x0F000000;
    cmdParam1.s.slotNumber = vssSlotNum;
    SYS_PRINT("--CMD INP[2]: 0x%08lx\r\n",cmdParam1.v);
    gHsmCmdReq.cmdInputs[2] = cmdParam1.v;
    SYS_PRINT("--CMD INP[2]: 0x%08lx\r\n",gHsmCmdReq.cmdInputs[2]);

    gHsmCmdReq.cmdInputs[3] = 0x00000000;          //(unused)

    //Expected Response CMD
    gHsmCmdReq.expMbHeader     = 0x0020000c; //4 Words(UNPROT))
    gHsmCmdReq.expResultCode   = S_OK;       //E_VSEMPTY;
    gHsmCmdReq.expStatus       = 0x00000320;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0;
    gHsmCmdReq.expNumDataBytes = 0;


    //Input SG (unused))
    dmaDescriptorIn[0].data.addr              = &dummy32;
    dmaDescriptorIn[0].next.s.stop            = 1;
    dmaDescriptorIn[0].next.s.addr            = 0;
    dmaDescriptorIn[0].flagsLength.s.length   = 0;
    dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorIn[0].flagsLength.s.discard  = 0;
    dmaDescriptorIn[0].flagsLength.s.realign  = 1;
    dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
    
    //Output SG
    dmaDescriptorOut[0].data.addr              = &dummy32;
    dmaDescriptorOut[0].next.s.stop            = 1;
    dmaDescriptorOut[0].next.s.addr            = 0;
    dmaDescriptorOut[0].flagsLength.s.length   = 0*sizeof(uint32_t);
    dmaDescriptorOut[0].flagsLength.s.cstAddr  = 0;
    dmaDescriptorOut[0].flagsLength.s.discard  = 0;
    dmaDescriptorOut[0].flagsLength.s.realign  = 1;
    dmaDescriptorOut[0].flagsLength.s.intEn    = 0;  
    
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorIn, sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorOut, sizeof(dmaDescriptorOut));
    
    HsmMbCmdDriver(&gHsmCmdReq, false);
    SYS_PRINT("HSM: RC 0x%08lx %s\r\n", (uint32_t) gHsmCmdResp.resultCode,
        CmdResultCodeStr(gHsmCmdResp.resultCode));

    HsmCmdRspChkr(rsp, true);

    return rsp;
} //End HsmCmdVsmDeleteSlot() 

//******************************************************************************
//******************************************************************************
//  VSM Utility Functions
//******************************************************************************
//******************************************************************************

//******************************************************************************
// VSM Input Data Info
// --Parses/Prints the VSM Info from the Input Data Command input/param words
//******************************************************************************
void VsmInputDataInfo(uint32_t * vsmInputData, 
                      CmdVSMInputSlotInfoParameter1 param)
{
    int                                   i;
    uint32_t                              vsDataLenWord;
    int        __attribute__((unused))    validBefore;
    int        __attribute__((unused))    validAfter;
    int                                   dataSpecificMetaData;
    VSHeader *   __attribute__((unused))  vsHeaderPtr;
    int        __attribute__((unused))    vsIndex;
    int        __attribute__((unused))    vsEncrKeySlotNum;
    int                                   vsSlotNum;
    VSSlotType                            vsSlotType;  //Type of Data
    char*                                 vsSlotTypeStr;
    VSStorageType                         vsStorageType; //Where stored
    char * vsStorageTypeStr;
    uint32_t *                            vsData;
    CmdVSMInputSlotInfoInput1             slotDataInput;
    VSStorageData __attribute__((unused)) vsStorageInfo;

    int __attribute__((unused)) vsExt;
    int __attribute__((unused)) vsValid;
    int __attribute__((unused)) vsHsmOnly;
    
    slotDataInput.wrappedKey             = 0x00000000;  //0
    slotDataInput.slotDataLength         = vsmInputData[0];  //0 or 1

    slotDataInput.vsMetaData.validBefore          = vsmInputData[1];
    slotDataInput.vsMetaData.validAfter           = vsmInputData[2];
    slotDataInput.vsMetaData.dataSpecificMetaData = vsmInputData[3];

    validBefore          = slotDataInput.vsMetaData.validBefore;
    validAfter           = slotDataInput.vsMetaData.validAfter;
    dataSpecificMetaData = slotDataInput.vsMetaData.dataSpecificMetaData;
    

    //VS Data
    slotDataInput.vsDataPtr      = &vsmInputData[4];
    slotDataInput.aesGcmTag      = 0x00000000;  //After Data
    slotDataInput.auth           = 0x00000000;  //End

    //VSM Command Input Data Parameter1
    vsSlotType        = param.s.slotInfo;
    vsSlotNum         = param.s.slotNumber;
    vsSlotTypeStr     = vsmSlotTypeStr[vsSlotType];
    vsStorageType     = param.s.vsStorageData.s.storageType;
    vsStorageTypeStr  = vsmStorageTypeStr[vsStorageType];

    vsData = slotDataInput.vsDataPtr; //Start of the data

    SYS_PRINT("--VSM Slot Type:    %s \r\n", vsSlotTypeStr); 
    SYS_PRINT("--VSM Storage Type: %s\r\n", vsStorageTypeStr);

    //Raw metadata is just the length in bytes
    //TODO: Data for other key types
    if (vsSlotType == VSS_RAW) 
    {
        vsDataLenWord = dataSpecificMetaData/BYTES_PER_WORD;
    }
    else return; 

    //Input Data
    SYS_PRINT("VSM Input Data Command INPUT Data Slot #%d (%d bytes):\r\n", 
               vsSlotNum, slotDataInput.slotDataLength);
    SYS_PRINT("--VSM Input Slot Data @ 0x%08lx (%d words)\r\n", 
        (uint32_t) vsData, (int)vsDataLenWord);

    //SYS_MESSAGE("    ");
    for (i=0; i<vsDataLenWord; i++) 
    {
        ((i%(4)==0)&&(i!=0))?SYS_MESSAGE("\r\n    "):dummy32;
        SYS_PRINT("0x%08lx ",vsData[i]);
    }
    SYS_MESSAGE("\r\n");

} //End VsmInputDataInfo()

//******************************************************************************
// VSM Output Data Info
// --parses the VSM Info Output Data Command DMA Write
//******************************************************************************
void VsmOutputDataInfo(uint32_t *                 vsmOutputData, 
                       uint32_t                   maxDataWords,
                       CmdVSMOutputSlotParameter1 param)
{
    CmdVSMOutputSlotOutput1               vsmSlotDataOutput;
    int                                   i;
    uint32_t                              vsDataLenWord;
    VSHeader                              vsHeader;
    uint32_t   __attribute__((unused))    validBefore;
    uint32_t   __attribute__((unused))    validAfter;
    CmdVSMDataSpecificMetaData            dataSpec;
    VSHeader * __attribute__((unused))    vsHeaderPtr;
    int        __attribute__((unused))    vsIndex;
    int        __attribute__((unused))    vsEncrKeySlotNum;
    CmdVSMSlotInfoType                    vsSlotInfoType; //Type of encryption
    char*                                 vsSlotInfoTypeStr;
    //char *                                vsStorageTypeStr;
    int                                   numDataWords;
    char*                                 aesTypeStr;

    uint32_t *                            vsData;
    VSStorageData __attribute__((unused)) vsStorageInfo;

    int __attribute__((unused)) vsExt;
    int __attribute__((unused)) vsValid;
    int __attribute__((unused)) vsHsmOnly;
    int __attribute__((unused)) vsApl;
    
    //TODO:  Encrypted/Wrapped 
    vsmSlotDataOutput.wrappedKey                = 0x00000000; //Not Implemented
    vsmSlotDataOutput.vsMetaData.vsHeader       = (VSHeader) vsmOutputData[0]; 
    vsmSlotDataOutput.vsMetaData.validBefore    = vsmOutputData[1];
    vsmSlotDataOutput.vsMetaData.validAfter     = vsmOutputData[2];
    vsmSlotDataOutput.vsMetaData.
                        dataSpecificMetaData  = vsmOutputData[3];

    vsHeader             = vsmSlotDataOutput.vsMetaData.vsHeader;
    validBefore          = vsmSlotDataOutput.vsMetaData.validBefore;
    validAfter           = vsmSlotDataOutput.vsMetaData.validAfter;
    dataSpec.v         = vsmSlotDataOutput.vsMetaData.dataSpecificMetaData;

    //VS Data
    vsmSlotDataOutput.vsDataPtr      = &vsmOutputData[4];
    vsmSlotDataOutput.aesGcmTag      = 0x00000000;  //Not Implemented

    //VSM Command Output Data Parameter1
    vsSlotInfoType        = param.s.slotInfo;
    vsSlotInfoTypeStr     = vsmSlotInfoTypeStr[vsSlotInfoType];

    SYS_PRINT("VSM Output Data Command Param1:   Slot #%d\r\n", param.s.slotNumber);
    SYS_PRINT("--CMD param:          Slot #%d\r\n", param.s.slotNumber);
    SYS_PRINT("--CMD param: Encr Key Slot #%d\r\n", param.s.keySlotNumber);
    SYS_PRINT("--CMD param: %s\r\n",vsSlotInfoTypeStr);

    vsData = vsmSlotDataOutput.vsDataPtr; //Start of the data
    
    VsHeaderInfo(&vsHeader);

    //Interpret the dataSpecificMetatdata
    if (vsHeader.s.vsSlotType == VSS_RAW) 
    {
        //RAW: Data Length
        vsDataLenWord = dataSpec.rawMeta.s.length/BYTES_PER_WORD;

        SYS_PRINT("--VSM Output Slot Data @ 0x%08lx (RAW: %ld words)\r\n", 
            (uint32_t)vsData, vsDataLenWord);
    }
    else if (vsHeader.s.vsSlotType == VSS_SYMMETRICALKEY) 
    {
        VssSkAesKey        aesMeta;

        if (dataSpec.symKeyMeta.keyType == VSS_SK_AES)
        {
            aesMeta       = dataSpec.aesSkMeta;
            vsDataLenWord = aesKeyLength[aesMeta.s.keySize]/BYTES_PER_WORD;
            aesTypeStr    = VssSymmetricalKeyTypeStr[aesMeta.s.keyType]; 

            SYS_PRINT("--VSM Output Slot Data @ 0x%08lx (%s: %ld words)\r\n", 
                (uint32_t) vsData, aesTypeStr, vsDataLenWord);
        }
    }
    else
    {
        //TODO:  Other slot Metadata types
        return;
    }

    numDataWords = min(vsDataLenWord, maxDataWords);
    SYS_MESSAGE("    ");
    for (i=0; i<numDataWords; i++) 
    {
        ((i%(4)==0)&&(i!=0))?SYS_MESSAGE("\r\n    "):dummy32;
        SYS_PRINT("0x%08lx ",vsData[i]);
    }
    if (numDataWords < vsDataLenWord) SYS_MESSAGE("...\r\n");
    else CRLF;

} //End VsmOutputDataInfo()



//******************************************************************************
// VS Header Info
// --parses the VS Header and prints the data.
//******************************************************************************
void VsHeaderInfo( VSHeader * header)
{
    int           vsSlotIndex;
    int           vsSlotNum;
    VSSlotType    vsSlotType;
    char *        slotTypeStr;
    VSStorageData vsStorageInfo;
    VSStorageType vsStorageType;
    char *        storageInfoTypeStr;
    int           ext;
    int           hsmOnly;
    int           apl;
    int           valid;

    vsSlotIndex    = header->s.vsIndex;
    vsSlotNum      = header->s.vsSlotNum; //Internal usage only

    vsSlotType     = header->s.vsSlotType;
    slotTypeStr    = vsmSlotTypeStr[vsSlotType];

    vsStorageInfo      = header->s.vsStorageInfo;
    ext                = vsStorageInfo.s.externalStorage;
    valid              = vsStorageInfo.s.valid;
    hsmOnly            = vsStorageInfo.s.hsmOnly;
    apl                = vsStorageInfo.s.apl;
    vsStorageType      = vsStorageInfo.s.storageType;
    storageInfoTypeStr = vsmStorageTypeStr[vsStorageType];

    SYS_PRINT("VS Header (0x%08lx) Slot #%d \r\n", header->v, vsSlotNum);
    SYS_PRINT("--    VS Slot Index %2d\r\n", vsSlotIndex);
    SYS_PRINT("--        Type: %s\r\n", slotTypeStr); 
    SYS_PRINT("--Storage Type: %s\r\n", storageInfoTypeStr);
    SYS_PRINT("--E=%d V=%d H=%d APL=%d\r\n", ext, valid, hsmOnly, apl); 
}

/* *****************************************************************************
 End of File
 */
