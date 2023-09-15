/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    vsm.c

  @Summary
     VSM Tests

  @Description
     VSM Tests
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "pic32cz8110ca90208.h"
#include "core_cm7.h"
#include "user.h"
#include "system/system_module.h"
#include "vsm_test.h"


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
#define AESKEYLENBITS   128
#define AESKEYLENBYTES  (AESKEYLENBITS/8)
#define AES_KEY_LENGTH  CMD_AES_KEY_128
static uint8_t ALIGN4 aesKey128[AESKEYLENBYTES+VSS_META_WORDS*4] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Meta
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Meta
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, //KEY
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, //KEY
};

// ACVP-AES-ECB AFT tgId 1 tcId 2: encrypt keyLen=128
//AES Encryption Key
#define AESG1T2KEYLENBITS   128
#define AESG1T2KEYLENBYTES  (AESKEYLENBITS/8)
#define AESG1T2_KEY_LENGTH  CMD_AES_KEY_128
static uint8_t __attribute__((unused)) keyAesG1T2[AESG1T2KEYLENBYTES+SLOTMETAWORDS*4] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Meta
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Meta
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //KEY 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //KEY
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

//******************************************************************************
//******************************************************************************
bool TestHsmCmdVsmInputDataUnencryptedAes(int vssSlotNum) 
{
    //static VSMetaData vsMeta;  
    uint32_t * vsmInputDataPtr;
    //static CmdVSMInputSlotInfoParameter1 vsmInputParam1;
    RSP_DATA * rsp;
    bool       ret_val    = false;
    int        retStatus  = 0;
    CmdVSMDataSpecificMetaData specMetaData;
    VSInputMetaData vsInputMeta;

    SYS_PRINT("\r\n---------------------------------------------\r\n");
    SYS_PRINT("**CMD_VSM_INPUT_DATA TEST (AES ECB KEY %d bytes, Slot %d)**\r\n", 
        AESKEYLENBYTES, vssSlotNum);

    vsmInputDataPtr = (uint32_t *)&aesKey128[0];

    //Print the Input Data
    //VsmInputDataInfo(vsmInputDataPtr, vsmInputParam1);

    //Specific RAW Meta Data
    specMetaData.aesSkMeta.v = 0;
    specMetaData.aesSkMeta.s.keyType = VSS_SK_AES;
    specMetaData.aesSkMeta.s.aesType = VSS_SK_AES_ECB;
    specMetaData.aesSkMeta.s.keySize = AES_KEY_LENGTH; 

    //VSS Meta Data Words
    vsInputMeta.inputLength = AESKEYLENBYTES + VSS_META_WORDS*BYTES_PER_WORD;
    vsInputMeta.validBefore = 0x00000000;
    vsInputMeta.validAfter  = 0xFFFFFFFF;
    vsInputMeta.dataSpecificMetaData = specMetaData.v; 

    //Writer the VS Input Metadata (four words) prior to key data
    vsmInputDataPtr[0] = (uint32_t) vsInputMeta.inputLength; 
    vsmInputDataPtr[1] = (uint32_t) vsInputMeta.validBefore; 
    vsmInputDataPtr[2] = (uint32_t) vsInputMeta.validAfter; 
    vsmInputDataPtr[3] = (uint32_t) vsInputMeta.dataSpecificMetaData; 

    //Write the Slot with Raw Data Words
    //NOTE:  Alway assume slot meta words included with the data
    rsp = HsmCmdVsmInputDataUnencrypted(vssSlotNum, 
                                        (uint32_t *) aesKey128, 
                                        AESKEYLENBYTES+SLOTMETAWORDS*BYTES_PER_WORD, 
                                        CMD_VSS_SYMMETRICALKEY, 
                                        specMetaData);  //AesECBKey

    // Check the results of the command
    if (rsp->rspChksPassed == true) 
    {
        SYS_PRINT("VSM Pass: CMD_VSM_INPUT_DATA terminated properly.\r\n");
    } 
    else 
    {
        SYS_PRINT("VSM FAIL: !!!CMD_VSM_INPUT_DATA FAILED!!! r\n");
        ret_val = true; //FAIL
    }

    //Check Input Slot Info 
    SYS_MESSAGE("\r\n");
    retStatus = HsmCmdVsmPrintSlotInfo(vssSlotNum);
    if (retStatus != 0)
    {
        if (retStatus == 1)
        {
            SYS_PRINT("VSM FAIL: !!!Slot# %d Empty!!!\r\n",vssSlotNum);
            ret_val = true; //FAIL
        }
        else
        {
            SYS_MESSAGE("VSM FAIL: !!!General Error!!!\r\n");
            ret_val = true; //FAIL
        }
    }
    else
    {
        SYS_PRINT("VSM Pass: Slot# %d Written\r\n",vssSlotNum);
    }

    SYS_PRINT("HSM TEST: MD_VSM_INPUT_DATA Slot %d Complete\r\n", vssSlotNum);
    return ret_val;
    
} //End TestHsmCmdVsmInputDataUnencryptedRaw()

//******************************************************************************
//CMD_VSM_OUTPUT_DATA Unencrypted Internal Slot Output Data Command--
//  --Assumes that it will read the same data as the 
//  TestHsmCmdVsmInputDataUnencryptedRaw() 
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
bool TestHsmCmdVsmOutputDataUnencryptedAes(int vssSlotNum) 
{
    static RSP_DATA * rsp;
    bool              ret_val      = false;
    int               dataLenBytes = 0;
    int i;
    VSMetaData        vsMetaData;
    CmdResultCodes    rcode;
    uint32_t          slotSizeBytes;
    VSHeader          expVsHeader;
    int               expKeyLengthBits;
    int               expKeyLengthBytes;
    int               expSlotLengthWords;
    bool              outError = false;
    int               outIdx;

    expKeyLengthBits   = 128;
    expKeyLengthBytes  = expKeyLengthBits/BITS_PER_BYTE;
    expSlotLengthWords = (expKeyLengthBits/BITS_PER_WORD) + VSS_META_WORDS;

    expVsHeader.v = 0;
    expVsHeader.s.vsIndex         = 0;
    expVsHeader.s.vsSlotNum       = vssSlotNum;
    expVsHeader.s.vsSlotType      = VSS_SYMMETRICALKEY;
    expVsHeader.s.vsStorageInfo.v = 0;   //APL/EXT/VAL/
    expVsHeader.s.vsStorageInfo.s.apl             = 0;  
    expVsHeader.s.vsStorageInfo.s.externalStorage = 0; 
    expVsHeader.s.vsStorageInfo.s.hsmOnly         = 0;
    expVsHeader.s.vsStorageInfo.s.storageType     = VM_STORAGE;
    expVsHeader.s.vsStorageInfo.s.valid           = 0;

    SYS_MESSAGE("\r\n---------------------------------------------\r\n");
    SYS_PRINT("**HSM CMD_VSM_OUTPUT_DATA TEST (AES ECB KEY SLOT %d (exp %d key bits) **\r\n", 
              vssSlotNum, expKeyLengthBits);

    //Check Slot is NOT Empty
    rcode = HsmCmdVsmGetSlotInfo(vssSlotNum, &vsMetaData, 
                                 &slotSizeBytes);
    if (rcode == E_VSEMPTY)
    {
        SYS_PRINT(
            "VSM FAIL: !!!CMD_VSM_OUTPUT_DATA TEST ABORT - SLOT #%d EMPTY!!!\r\n",
                vssSlotNum);
        ret_val = true; //FAIL
    }
    else
    {
        uint8_t * dmaDataOutBytePtr = (uint8_t *) dmaDataOut;

        //Initialize dmaDataOut
        for (int i; i<MAX_OUTPUT_DMA_LENGTH*BYTES_PER_WORD; i++)
        {
            dmaDataOutBytePtr[i] = i%0xFF;
        }

        SYS_PRINT("-->Send CMD_VSM_OUTPUT_DATA for AES ECB Key in Slot %d\r\n",
                  vssSlotNum);
        rsp = HsmCmdVsmOutputDataUnencrypted( vssSlotNum, 
                                              dmaDataOut,
                                              &dataLenBytes, 
                                              48); 
            
        // Check the results of the command
        if (rsp->rspChksPassed == true) 
        {
            SYS_PRINT("VSM Pass: CMD_VSM_OUTPUT_DATA terminated properly.\r\n");
            //Check the Output Length
            if (dataLenBytes/BYTES_PER_WORD == 
                (expKeyLengthBytes + VSS_META_WORDS*BYTES_PER_WORD))
            {
                SYS_PRINT("VSM Pass:  Slot #%d Output Data (%d Bytes):\r\n",
                               vssSlotNum, dataLenBytes);
                ret_val= false;
            }
            else
            {
                SYS_PRINT(
                    "VSM FAIL: Output Data (%d words!= Input Data(%d Bytes)\r\n",
                    dataLenBytes/BYTES_PER_WORD, 
                    (AESKEYLENBYTES/BYTES_PER_WORD + VSS_META_WORDS));
                
                //Metadata
                SYS_MESSAGE("\n**NOTE**: VSM Output VS Header is incorrect\r\n");
                SYS_MESSAGE("--Instead it only gives the slot number (shown below)**\r\n");
                SYS_PRINT("---VS Header Info:  0x%08lx IS NOT VALID\r\n",dmaDataOut[0]);
                VsHeaderInfo((VSHeader *)&dmaDataOut[0]);
                ret_val = true;
            }
        } 
        else
        {
            SYS_PRINT("VSM FAIL: !!!CMD_VSM_OUTPUT_DATA FAILED (Bad Response)!!!\r\n");
            ret_val= true;
        }

        //Test the Output
        if (ret_val == false)
        {
            SYS_PRINT("VSM Pass: VSM_CMD_OUTPUT_DATA Command Return Value\r\n");
        }
        else
        {
            SYS_PRINT("VSM FAIL: VSM_CMD_OUTPUT_DATA Command Return Value\r\n");
        }

        //Check Data
        if (dataLenBytes > 0)
        {
            SYS_PRINT("...Checking Data (%d Words\r\n)",
                dataLenBytes/BYTES_PER_WORD);


            if (expVsHeader.v != dmaDataOut[0])
            {
                SYS_PRINT("VSS Header FAIL:  Slot %d Header Error\r\n", 
                    vssSlotNum);
                SYS_PRINT("  RECEIVED:");
                VsHeaderInfo((VSHeader *)&dmaDataOut[0]);

                SYS_PRINT("  EXPECTED:");
                VsHeaderInfo((VSHeader *)&expVsHeader);
                outError=true;
                outIdx = 0;
                SYS_PRINT("VSM FAIL:  VS Header Error\r\n");
            }
            else
            {
                SYS_PRINT("VS %d Header", vssSlotNum);
                VsHeaderInfo((VSHeader *)&dmaDataOut[0]);
            }

            for (i=1; i<dataLenBytes/BYTES_PER_WORD; i++) 
            {
                if (aesKey128[i] != dmaDataOut[i]) 
                {
                    SYS_PRINT("VSM FAIL:  Data Error @ %d\r\n", i);
                    outError=true;
                    outIdx = i;
                    break;
                }
            }
            CRLF;

            //Print the Actual Slot Output 
            SYS_PRINT("Actual: VSM Aes Output Data(%d Words):", 
                dataLenBytes/BYTES_PER_WORD);
            SYS_MESSAGE("** NOTE:  Initial word should be VS Metadata\r\n");
            SYS_PRINT("         -->Not the Slot Number %d\r\n",vssSlotNum);
            for (i=0; i<dataLenBytes/BYTES_PER_WORD; i++) 
            {
                //Print and Test
                if (i%4==0 || i==0) {CRLF; SYS_PRINT("%04x: ",i);} else {SYS_MESSAGE(" ");}
                SYS_PRINT("%08lx ", dmaDataOut[i]);
            }
            CRLF;

            if (outError == true)
            {
                SYS_PRINT("Data Error @ index = %d\r\n", outIdx);
                //VSM_CMD_INPUT_DATA Data
                SYS_PRINT("Expected: VSM AES Output Data(%d Words):\r\n", 
                   expSlotLengthWords);
                for (i=0; i<expSlotLengthWords; i++) 
                {
                    uint32_t * inP = (uint32_t *)aesKey128;
                    //Print and Test
                    if (i%4==0 || i==0) {CRLF; SYS_PRINT("%04x: ",i);} else {SYS_MESSAGE(" ");}
                    if (i==0)
                    {
                        SYS_PRINT("%08lx ", expVsHeader.v);
                    }
                    else
                    {
                        SYS_PRINT("%08lx ", inP[i]);
                    }
                }
                CRLF;
            }
            else
            {
                SYS_MESSAGE("VSM Pass: CMD_VSM_OUTPUT_DATA DATA AES KEY OUT MATCH\r\n");

            }
        }

    } //End Slot !Empty Block


    SYS_PRINT("HSM TEST: CMD_VSM_OUTPUT AES KEY Complete\r\n");
    return ret_val; 

} //End TestHsmCmdVsmOutputDataUnencryptedAes())



/* *****************************************************************************
 End of File
 */
