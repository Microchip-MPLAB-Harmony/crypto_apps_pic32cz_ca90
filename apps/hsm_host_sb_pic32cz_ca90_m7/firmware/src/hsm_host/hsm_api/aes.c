/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology         

  @File Name
    filename aesc

  @Summary
    AES ECB Encrypt/Decrypt

  @Description
    AES ECB Encrypt/Decrypt
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "pic32cz8110ca90208.h"
#include "system/system_module.h"
#include "core_cm7.h"
#include "user.h"
#include "aes.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif // min

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

char * aesCmdModeStr[] =
{
    "CMD_AES_ECB",
    "CMD_AES_CBC",
    "CMD_AES_CTR",
    "CMD_AES_CFB",
    "CMD_AES_OFB",
    "CMD_AES_XTS",
};

char * aesCmdTypeStr[] =
{
    "CMD_AES_ENCRYPT",
    "CMD_AES_DECRYPT",
    "CMD_AES_GCM_ENCRYPT",
    "CMD_AES_GCM_DECRYPT",
    "CMD_AES_CMAC",
    "CMD_AES_CCM_ENCRYPT",
    "CMD_AES_CCM_DECRYPT",
};

char * aesKeySizeStr[] =
{
    "CMD_AES_KEY_128",
    "CMD_AES_KEY_192",
    "CMD_AES_KEY_256",
};

int aesKeyLength[3] =
{
    128,
    192,
    256,
};

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

//Get key size indexed by CmdAesKeySize
uint32_t key_size[] = {128, 192, 256}; 

//******************************************************************************
//******************************************************************************
void PrintAesCmd(CmdAesEcbCommandHeader * aesEcbCmdHeader)
{
    int auth     = aesEcbCmdHeader->s.authInc; 
    int cmdGroup = aesEcbCmdHeader->s.cmdGroup; 
    int cmdType  = aesEcbCmdHeader->s.cmdType; 
    int keySize  = aesEcbCmdHeader->s.keySize; 
    int slotInc  = aesEcbCmdHeader->s.slotParamInc; 
    SYS_PRINT("Cmd %02x:%02X key=%d slot=%d auth=%d\r\n",
        cmdGroup,cmdType,keySize, slotInc, auth);
}


//******************************************************************************
//  AES ECB Encrypt/Decrypt Mode: CMD_AES_ECB (Electronic Code Book)
//  NULL key --> use key give by vsSlotNum
//******************************************************************************
RSP_DATA * HsmCmdAesEcbEncryptDecrypt( 
    int              vsSlotNum, //Encryption/Decryption Key
    bool             encrypt,   //not decrypt
    uint32_t *       key,
    CmdAesKeySize    keySize,
    uint32_t *       aesInputDataPtr,
    uint32_t *       aesOutputDataPtr,
    uint32_t         numDataWords)
{
    uint32_t __attribute__((unused))   keySizeBit;
    VSMetaData             vsMetaData;
    CmdAesEcbCommandHeader aesEcbCmdHeader;
    CmdAesEcbParameter1    param1;
    CmdAesEcbParameter2    param2;
    static RSP_DATA *      rsp    = &gRspData;
    uint32_t               slotInfoBytes;

    ClearRsp();

    if (keySize == CMD_AES_KEY_128)      keySizeBit = 128;
    else if (keySize == CMD_AES_KEY_192) keySizeBit = 192;
    else if (keySize == CMD_AES_KEY_256) keySizeBit = 256;
    else
    {
        rsp->invArgs = true;
        return rsp;
    }

    //AES ECB Encrypt/Decrypt Header
    aesEcbCmdHeader.v  = 0;
    aesEcbCmdHeader.s.cmdGroup = CMD_AES;
    aesEcbCmdHeader.s.aesMode  = CMD_AES_ECB;  
    aesEcbCmdHeader.s.keySize  = keySize;

    //TODO: Implement Cmd Auth
    aesEcbCmdHeader.s.authInc  = 0;
    aesEcbCmdHeader.s.aesMode  = CMD_AES_ECB;
    if (encrypt) aesEcbCmdHeader.s.cmdType = CMD_AES_ENCRYPT;
    else         aesEcbCmdHeader.s.cmdType = CMD_AES_DECRYPT;
    //PrintAesCmd(&aesEcbCmdHeader);

    //Either a slot # is provided or a key pointer is provided.
    if (vsSlotNum>MINSLOTNUM && vsSlotNum<MAXSLOTNUM && key == NULL)
    {
        aesEcbCmdHeader.s.slotParamInc  = 1;
    }
    else if (key != NULL)
    {
        aesEcbCmdHeader.s.slotParamInc  = 0;
    }
    else
    {
        rsp->invArgs = true;
        rsp->invSlot = true;
        return rsp;
    }

    //TODO: APL > 0 (AUTH Included)
    if (aesEcbCmdHeader.s.slotParamInc == 1)
    {
        int result;
        result = HsmCmdVsmGetSlotInfo(vsSlotNum, &vsMetaData, &slotInfoBytes); 
        if ((result != 0) ||
            (vsMetaData.vsHeader.s.vsSlotNum != vsSlotNum) ||
            (vsMetaData.vsHeader.s.vsSlotType != VSS_SYMMETRICALKEY) ||
            (vsMetaData.vsHeader.s.vsStorageInfo.s.apl != 0) ||
            ((vsMetaData.vsHeader.s.vsStorageInfo.s.storageType != NVM_UNENCRYPTED) &&
             (vsMetaData.vsHeader.s.vsStorageInfo.s.storageType != VM_STORAGE))) 
        {
            //TODO:  Check the slot size
            rsp->invSlot = true; //Invalid Slot
            return rsp;
        }
    };
    //PrintAesCmd(&aesEcbCmdHeader);

    //AES ECB Encrypt/Decrypt Parameter1
    param1.dataSize = numDataWords*BYTES_PER_WORD;

    param2.v              = 0;
    param2.s.resetCtxToIv = 1;   //Reset Context to IV stored with Key
    param2.s.slotIndex    = vsSlotNum;
    param2.s.useCtx       = 0;   //Use Context stored input (NA for ECB)


    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    //Send AES ECB Encrypt/Decrypt Command 
    //--No TA/No Slot/No Auth
    gHsmCmdReq.mbHeader      = 0x00200018; //5 Words (No Auth)
    gHsmCmdReq.cmdHeader     = aesEcbCmdHeader.v;
    gHsmCmdReq.cmdInputs[0]  = (uint32_t)(&(dmaDescriptorIn[0]));
    gHsmCmdReq.cmdInputs[1]  = (uint32_t)(&(dmaDescriptorOut[0]));

    gHsmCmdReq.cmdInputs[2]  = param1.dataSize;
    gHsmCmdReq.cmdInputs[3]  = param2.v; 

    if (encrypt == true)
    {
        if (aesEcbCmdHeader.s.slotParamInc == 1)
        {
            //Input SG (slotParamInc -> msg,  else key)
            dmaDescriptorIn[0].data.addr              = key;
            dmaDescriptorIn[0].next.s.stop            = 0;
            dmaDescriptorIn[0].next.s.addr            = (uint32_t) &dmaDescriptorIn[1];
            dmaDescriptorIn[0].flagsLength.s.length   = (unsigned int) keySizeBit/8; 
            dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
            dmaDescriptorIn[0].flagsLength.s.discard  = 0;
            dmaDescriptorIn[0].flagsLength.s.realign  = 1;
            dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
            //SYS_MESSAGE("VSM INPUT DATA SG:\r\n");
            //PrintSG(dmaDescriptorIn[0], true);

            //Input SG Msg
            dmaDescriptorIn[1].data.addr              = aesInputDataPtr;
            dmaDescriptorIn[1].next.s.stop            = 1;
            dmaDescriptorIn[1].next.s.addr            = 0;
            dmaDescriptorIn[1].flagsLength.s.length   = 
                        (unsigned int) numDataWords*BYTES_PER_WORD; 
            dmaDescriptorIn[1].flagsLength.s.cstAddr  = 0;
            dmaDescriptorIn[1].flagsLength.s.discard  = 0;
            dmaDescriptorIn[1].flagsLength.s.realign  = 1;
            dmaDescriptorIn[1].flagsLength.s.intEn    = 0;  
            //SYS_MESSAGE("VSM INPUT DATA SG:\r\n");
            //PrintSG(dmaDescriptorIn[0], true);
        }
        else
        {
            //Input SG Msg
            dmaDescriptorIn[0].data.addr              = aesInputDataPtr;
            dmaDescriptorIn[0].next.s.stop            = 1;
            dmaDescriptorIn[0].next.s.addr            = 0;
            dmaDescriptorIn[0].flagsLength.s.length   = 
                        (unsigned int) numDataWords*BYTES_PER_WORD; 
            dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
            dmaDescriptorIn[0].flagsLength.s.discard  = 0;
            dmaDescriptorIn[0].flagsLength.s.realign  = 1;
            dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
            //SYS_MESSAGE("VSM INPUT DATA SG:\r\n");
            //PrintSG(dmaDescriptorIn[0], true);
        }

        //Output (Encrypted Msg)
        dmaDescriptorOut[0].data.addr             = aesOutputDataPtr;
        dmaDescriptorOut[0].next.s.stop           = 1;
        dmaDescriptorOut[0].next.s.addr           = 0;
        dmaDescriptorOut[0].flagsLength.s.length  = 
                    (unsigned int) numDataWords*BYTES_PER_WORD;
        dmaDescriptorOut[0].flagsLength.s.cstAddr = 0;
        dmaDescriptorOut[0].flagsLength.s.discard = 0;
        dmaDescriptorOut[0].flagsLength.s.realign = 1;
        dmaDescriptorOut[0].flagsLength.s.intEn   = 0;
    }
    else
    {
        //DeCrypt
        if (aesEcbCmdHeader.s.slotParamInc == 1)
        {
            //Input SG (slotParamInc -> msg,  else key)
            dmaDescriptorIn[0].data.addr              = key;
            dmaDescriptorIn[0].next.s.stop            = 0;
            dmaDescriptorIn[0].next.s.addr            = 
                        (uint32_t)&dmaDescriptorIn[1];
            dmaDescriptorIn[0].flagsLength.s.length   = 
                        (unsigned int) keySizeBit/8; 
            dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
            dmaDescriptorIn[0].flagsLength.s.discard  = 0;
            dmaDescriptorIn[0].flagsLength.s.realign  = 1;
            dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
            //SYS_MESSAGE("VSM INPUT DATA SG:\r\n");
            //PrintSG(dmaDescriptorIn[0], true);

            //Input SG Msg
            dmaDescriptorIn[1].data.addr              = aesInputDataPtr;
            dmaDescriptorIn[1].next.s.stop            = 1;
            dmaDescriptorIn[1].next.s.addr            = 0;
            dmaDescriptorIn[1].flagsLength.s.length   = 
                        (unsigned int) numDataWords*BYTES_PER_WORD;
            dmaDescriptorIn[1].flagsLength.s.cstAddr  = 0;
            dmaDescriptorIn[1].flagsLength.s.discard  = 0;
            dmaDescriptorIn[1].flagsLength.s.realign  = 1;
            dmaDescriptorIn[1].flagsLength.s.intEn    = 0;  
            //SYS_MESSAGE("VSM INPUT DATA SG:\r\n");
            //PrintSG(dmaDescriptorIn[0], true);
        }
        else
        {
            //Input SG Msg
            dmaDescriptorIn[0].data.addr              = aesInputDataPtr;
            dmaDescriptorIn[0].next.s.stop            = 1;
            dmaDescriptorIn[0].next.s.addr            = 0;
            dmaDescriptorIn[0].flagsLength.s.length   = 
                        (unsigned int) numDataWords*BYTES_PER_WORD;
            dmaDescriptorIn[0].flagsLength.s.cstAddr  = 0;
            dmaDescriptorIn[0].flagsLength.s.discard  = 0;
            dmaDescriptorIn[0].flagsLength.s.realign  = 1;
            dmaDescriptorIn[0].flagsLength.s.intEn    = 0;  
            //SYS_MESSAGE("VSM INPUT DATA SG:\r\n");
            //PrintSG(dmaDescriptorIn[0], true);
        }

        //Output (Encrypted Msg)
        dmaDescriptorOut[0].data.addr             = aesOutputDataPtr;
        dmaDescriptorOut[0].next.s.stop           = 1;
        dmaDescriptorOut[0].next.s.addr           = 0;
        dmaDescriptorOut[0].flagsLength.s.length  = 
                        (unsigned int) numDataWords*BYTES_PER_WORD;
        dmaDescriptorOut[0].flagsLength.s.cstAddr = 0;
        dmaDescriptorOut[0].flagsLength.s.discard = 0;
        dmaDescriptorOut[0].flagsLength.s.realign = 1;
        dmaDescriptorOut[0].flagsLength.s.intEn   = 0;

    }

    //Expected Response 
    gHsmCmdReq.expMbHeader     = 0x0020000c;
    gHsmCmdReq.expResultCode   = S_OK;
    gHsmCmdReq.expStatus       = 0x00000320;
    gHsmCmdReq.expIntFlag      = 0x00000000;
    gHsmCmdReq.expData         = 0x00000000;  
    gHsmCmdReq.expNumDataBytes = 
                    (unsigned int) numDataWords*BYTES_PER_WORD;
    
    SCB_CleanDCache_by_Addr((uint32_t *)           dmaDescriptorIn,  sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *)           dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) aesInputDataPtr,     numDataWords*sizeof(uint32_t));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) aesOutputDataPtr,    numDataWords*sizeof(uint32_t));
    
    //Send HSM Command to HSM MB
    //PrintAesCmd((CmdAesEcbCommandHeader *)(&gHsmCmdReq.cmdHeader));
    HsmMbCmdDriver(&gHsmCmdReq, false);

    //Check the response/result
    HsmCmdRspChkr(rsp, true);

    return rsp;
    
} //End HsmCmdVsmInputDataUnencrypted()
  

/* *****************************************************************************
 End of File
 */
