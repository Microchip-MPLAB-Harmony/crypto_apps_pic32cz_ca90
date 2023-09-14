//hsm_tests.c

#include <stdint.h>
#include <string.h>
#include <math.h>

#include "pic32cz8110ca90208.h"
#include "core_cm7.h"
#include "user.h"
#include "system/system_module.h"
#include "hsm_test.h"

//Create a global Test Data variable so it can be accessed by the interrupt 
//handler when (int_mode == true)
TEST_DATA testData = {0};

uint8_t ALIGN4 hashBuffer[64] = 
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


//char hashMsgBlock[] = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz01";
char hashMsgBlock[] = "abcdefghijk";


//Result Generated from HSM HashBlockTest_1_Result.ipynb 
//--for 'abcdefghijk', i.e. hashMsgBlock
uint8_t ALIGN4 expHashBlockResult[SHA256_NUMBYTES] =
{
0xca, 0x2f, 0x20, 0x69, 0xea, 0x0c, 0x6e, 0x46, 
0x58, 0x22, 0x2e, 0x06, 0xf8, 0xdd, 0x63, 0x96, 
0x59, 0xcb, 0xb5, 0xe6, 0x7c, 0xbb, 0xba, 0x67, 
0x34, 0xbc, 0x33, 0x4a, 0x37, 0x99, 0xbc, 0x68, 
};

//VSM_INPUT_DATA Test  
//   Wrapped AES-GSM Key - 48 Bytes (If Used)
//   Length of VS Data - 4 bytes
//   VS Definition:
//      W0: Key Length - 4 bytes
//      W1: VS Header: VS Type|VS Storage Info| VS# | VSindx
//                  0T VV SS II
//            T: 0b0ttt -> RAW/SYM/ASYM/HASH/HASHIV
//            VV: (VS Storage Info) 
//                EXT|VALID|ss|HSMonly|0|apl
//                 ss -> RAM|UNENCRYPT|FKey|TKey
//            SS: VS#
//            II: VSindx
//      W2: !ValidBefore - 4 bytes
//      W3: !ValidAfter - 4 bytes
//      W4: Data Specific Metadata  - 4 bytes     
//          --RAW Data Key (VSS Key Input)
//
//  AES-GSM Tag (If Encrypted)
//  AUTH (If Authentication, i.e challenge/response required)

//
//

#define VSM_INPUT_RAW_DATA_256_WORDS 8 
uint32_t ALIGN4 vsmInputData256Raw[VSM_INPUT_RAW_DATA_256_WORDS + VSS_META_WORDS +1] =
{
    0x00000030,  // Input Length (44))
    0x00000F00,  // VS Header - RAW Slot #15
    0x00000000,  // Not Valid Before
    0xFFFFFFFF,  // Not Valid After
    0x00000020,  // RAW Metadata:  just the length
    0x11111111,  //W1 8*4 Bytes Data...
    0x22222222,  //W2
    0x33333333,  //W3
    0x44444444,  //W4
    0x55555555,  //W5
    0x66666666,  //W6
    0x77777777,  //W7
    0x88888888,  //W8 
};

#define VSM_INPUT_RAW_DATA_512_WORDS 16 
uint32_t ALIGN4 vsmInputData512Raw[VSM_INPUT_RAW_DATA_512_WORDS + VSS_META_WORDS +1] =
{
    0x00000050,  // Input Length (44))
    0x00000F00,  // VS Header - RAW Slot #15
    0x00000000,  // Not Valid Before
    0xFFFFFFFF,  // Not Valid After
    0x00000050,  // RAW Metadata:  just the length
    0x11111111,  //W1 8*4 Bytes Data...
    0x22222222,  //W2
    0x33333333,  //W3
    0x44444444,  //W4
    0x55555555,  //W5
    0x66666666,  //W6
    0x77777777,  //W7
    0x88888888,  //W8 
    0x99999999,  //W9
    0xAAAAAAAA,  //W10
    0xBBBBBBBB,  //W11
    0xCCCCCCCC,  //W12
    0xDDDDDDDD,  //W13
    0xEEEEEEEE,  //W14
    0xFFFFFFFF,  //W15
    0xABCDEFAB,  //W16 
};

//NOT USED
#define VSM_OUTPUT_DATA_WORDS 32
uint32_t ALIGN4 vsmOutputDataRaw[VSM_OUTPUT_DATA_WORDS + VSS_META_WORDS ] =
                 { 0 };

//TODO:  struct 
uint32_t ALIGN4 vsmSlotInfoOut[VSS_META_WORDS];


//******************************************************************************
//******************************************************************************
// Definitions/Macros
//******************************************************************************
//******************************************************************************

//******************************************************************************
//******************************************************************************
// Globals/Statics
//******************************************************************************
//******************************************************************************

extern uint32_t dummy32;

//******************************************************************************
//HASH BLOCK Command Cmd - 1
//--TODO: Interrupt Mode
//******************************************************************************
bool TestHsmCmdHashBlockSha256()
{
    int        numDataInBytes;
    RSP_DATA * rsp;
    int        i       = 0;
    bool       ret_val = false;

    SYS_PRINT("\r\n---------------------------------------------\r\n");
    SYS_MESSAGE("**HSM HASH BLOCK SHA256 TEST**\r\n");

    //TODO: Pad to 32 byte boundary
    numDataInBytes = strlen(hashMsgBlock); 

    //Print the Input Byte String
    SYS_PRINT("SHA256:  Msg of %d Bytes:\r\n",numDataInBytes);
    SYS_MESSAGE("b\"/0x");
    for (i=0; i<numDataInBytes; i++) 
    {
        if (i%LINEBYTES==0 && i!=0) CRLF;
        SYS_PRINT("%02x/",hashMsgBlock[i]);
    }
    CRLF;

    SYS_MESSAGE("HSM: Sending CMD_HASH_BLOCK command\r\n");
    rsp = HsmCmdHashBlockSha256(
              (uint8_t *)hashMsgBlock, numDataInBytes, hashBuffer);
    
    // Check the results of the command
    if (rsp->rspChksPassed == true) 
    {
        SYS_PRINT("SHA256 Pass: CMD_VSM_INPUT_DATA terminated properly.\r\n");
    } 
    else 
    {
        SYS_PRINT("SHA256 FAIL: !!!CMD_VSM_INPUT_DATA CMD ERROR!!! r\n");
        ret_val = true; //FAIL
    }

    //Test the Output
    if (ret_val == false)
    {
        bool outErr = false;

        //Print the Output Hash Byte String
        SYS_PRINT("  SHA256:  (%d Bytes)\r\n", SHA256_NUMBYTES);
        SYS_MESSAGE("b\"/0x");
        for (i=0; i<SHA256_NUMBYTES; i++) 
        {
            if (i%LINEBYTES==0 && i!=0) CRLF;
            SYS_PRINT("%02x/",hashBuffer[i]);
            if (hashBuffer[i] != expHashBlockResult[i]) outErr=true;
        }
        CRLF;


        if (outErr == true)
        {
            SYS_MESSAGE("SHA256 FAIL: !!!CMD_HASH_BLOCK DATA OUT ERROR !!!\r\n");
            SYS_PRINT("Expected: SHA256(%d Bytes)\r\n", SHA256_NUMBYTES);
            SYS_MESSAGE("b\"/0x");
            for (i=0; i<SHA256_NUMBYTES; i++) 
            {
                if (i%LINEBYTES==0 && i!=0) CRLF;
                SYS_PRINT("%02x/",expHashBlockResult[i]);
            }
            CRLF;
        }
        else
        {
            SYS_MESSAGE("SHA256 Pass: CMD_HASH_BLOCK DATA OUT VALID\r\n");
        }
    }
    
    SYS_MESSAGE("HSM: CMD_HASH_BLOCK SHA256 Complete\r\n");

    return ret_val;
    
} //End HsmCmdHashBlockSha256() 


//******************************************************************************
//******************************************************************************
bool TestHsmCmdVsmInputDataUnencryptedRaw(int vssSlotNum, VssKeySize keySize)
{
    //static VSMetaData vsMeta;  
    uint32_t * vsmInputDataPtr;
    //VSHeader   vsHeader;
    //VSInputMetaData vsInputMeta;
    //static CmdVSMInputSlotInfoParameter1 vsmInputParam1;
    RSP_DATA * rsp;
    bool       ret_val    = false;
    int        retStatus  = 0;
    CmdVSMDataSpecificMetaData specMetaData;
    int        keyLengthBits;
    int        slotLengthBytes;

    if      (keySize == VSS_KEY_256) 
    {
        keyLengthBits = 256;
        vsmInputDataPtr = &vsmInputData256Raw[0];
    }
    else if (keySize == VSS_KEY_512) 
    {
        keyLengthBits = 512;
        vsmInputDataPtr = &vsmInputData512Raw[0];
    }
    else
    {
        SYS_PRINT("--VSM INPUT DATA Test FAIL: Key size %d not supported\r\n",
                  keySize);
        return true;
    }
    slotLengthBytes = keyLengthBits/BITS_PER_BYTE + VSS_META_BYTES;

    SYS_PRINT("\r\n---------------------------------------------\r\n");
    SYS_PRINT("**CMD_VSM_INPUT_DATA TEST (Slot %d KeyLength %d (%d bytes)**\r\n", 
               vssSlotNum, keyLengthBits, keyLengthBits/BITS_PER_BYTE);

    //Print the Input Data
    //VsmInputDataInfo(vsmInputDataPtr, vsmInputParam1);

    //Specific RAW Meta Data
    //specMetaData.rawMeta.v = 0;
    //specMetaData.rawMeta.s.length = slotLengthBytes;

    //VSS Meta Data Words
    //vsHeader.v           = 0;
    //vsHeader.s.vsIndex   = vssSlotNum;
    //vsHeader.s.vsSlotNum = vssSlotNum;

    //vsInputMeta.inputLength          = slotLengthBytes;
    //vsInputMeta.vsHeader             = vsHeader;
    //vsInputMeta.validBefore          = 0x00000000;
    //vsInputMeta.validAfter           = 0xFFFFFFFF;
    //vsInputMeta.dataSpecificMetaData = specMetaData.v; 

    //Write the VS Input Metadata (four words) prior to key data
    //vsmInputDataPtr[0] = (uint32_t) vsInputMeta.inputLength;
    //vsmInputDataPtr[1] = (uint32_t) vsInputMeta.vsHeader.v; 
    //vsmInputDataPtr[2] = (uint32_t) vsInputMeta.validBefore; 
    //vsmInputDataPtr[3] = (uint32_t) vsInputMeta.validAfter; 
    //vsmInputDataPtr[4] = (uint32_t) vsInputMeta.dataSpecificMetaData; 

    //Write the Slot with Raw Data Words
    rsp = HsmCmdVsmInputDataUnencrypted(vssSlotNum, 
                                        vsmInputDataPtr, 
                                        slotLengthBytes,
                                        CMD_VSS_RAW, 
                                        specMetaData);

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

        SYS_PRINT("VSM PASSED: Slot# %d Written with %d bytes\r\n",
            slotLengthBytes, vssSlotNum);
    }

    SYS_PRINT("HSM TEST: MD_VSM_INPUT_DATA Slot %d Complete - %s\r\n", 
        vssSlotNum, ret_val==0?"PASSED":"FAILED");
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
bool TestHsmCmdVsmOutputDataUnencryptedRaw(int vssSlotNum, VssKeySize keySize) 
{
    int               i;
    static RSP_DATA * rsp;
    bool              ret_val            = false;
    int               dataLenWords       = 0;
    uint32_t        * vsmInputDataPtr    = 0;
    int               expKeyLengthBits;
    int               expSlotLengthWords;
    VSHeader          expVsHeader;
    VSMetaData  __attribute__((unused)) vsMetaData;
    uint32_t    __attribute__((unused)) slotSizeBytes;


    if      (keySize == VSS_KEY_256) 
    {
        expKeyLengthBits = 256;
        vsmInputDataPtr = &vsmInputData256Raw[1];
        slotSizeBytes = 256/BITS_PER_BYTE + VSS_META_BYTES;
    }
    else if (keySize == VSS_KEY_512) 
    {
        expKeyLengthBits = 512;
        vsmInputDataPtr = &vsmInputData512Raw[1];
        slotSizeBytes = 512/BITS_PER_BYTE + VSS_META_BYTES;
    }
    else
    {
        SYS_MESSAGE("\r\n---------------------------------------------\r\n");
        SYS_PRINT("**TEST VSM OUTPUT DATA Test FAIL: Key size %d not supported\r\n",
                  keySize);
        return true; //FAIL
    }
    expSlotLengthWords = (expKeyLengthBits/BITS_PER_WORD) + VSS_META_WORDS;

    expVsHeader.v = 0;
    expVsHeader.s.vsIndex         = 0;
    expVsHeader.s.vsSlotNum       = vssSlotNum;
    expVsHeader.s.vsSlotType      = VSS_RAW;
    expVsHeader.s.vsStorageInfo.v = 0;   //APL/EXT/VAL/
    expVsHeader.s.vsStorageInfo.s.apl             = 0;  
    expVsHeader.s.vsStorageInfo.s.externalStorage = 0; 
    expVsHeader.s.vsStorageInfo.s.hsmOnly         = 0;
    expVsHeader.s.vsStorageInfo.s.storageType     = VM_STORAGE;
    expVsHeader.s.vsStorageInfo.s.valid           = 0;

    SYS_MESSAGE("\r\n---------------------------------------------\r\n");
    SYS_PRINT("**TEST HSM CMD_VSM_OUTPUT_DATA - SLOT %d Exp KeyLength %d (%d words)**\r\n", 
               vssSlotNum, expKeyLengthBits, expSlotLengthWords);

#undef VSINFOTEST
#if VSINFOTEST 
    //Check Slot is NOT Empty
    if (HsmCmdVsmGetSlotInfo(vssSlotNum, &vsMetaData, &slotSizeBytes) 
        == E_VSEMPTY)
    {
        SYS_PRINT(
            "VSM FAIL: !!!CMD_VSM_OUTPUT_DATA TEST ABORT - SLOT #%d EMPTY!!!\r\n",
                vssSlotNum);
        ret_val = true; //FAIL
    }
    else
    {
#endif //0
        //Run Test
        bool outError = false;
        bool outIdx   = 0;
        uint8_t * dmaDataOutBytePtr = (uint8_t *) dmaDataOut;

        //Initialize dmaDataOut
        for (int i; i<MAX_OUTPUT_DMA_LENGTH*BYTES_PER_WORD; i++)
        {
            dmaDataOutBytePtr[i] = i%0xFF;
        }

        //SYS_PRINT("--SLOT #%d Not Empty\r\n",vssSlotNum);
        //SYS_MESSAGE("-->Send CMD_VSM_OUTPUT_DATA\r\n");
        dataLenWords = slotSizeBytes/BYTES_PER_WORD;
        rsp = HsmCmdVsmOutputDataUnencrypted( vssSlotNum, 
                                              dmaDataOut,
                                              &dataLenWords, 
                                              48); 
            
        // Check the results of the command
        if (rsp->rspChksPassed == true) 
        {
            SYS_PRINT("VSM Pass: CMD_VSM_OUTPUT_DATA terminated properly.\r\n");
            //Check the Output Length
            if (keySize == VSS_KEY_256 && 
                dataLenWords == (VSM_INPUT_RAW_DATA_256_WORDS + VSS_META_WORDS))
            {
                SYS_PRINT("VSM Pass:  Slot #%d Output Data (%d Words) OK\r\n",
                               vssSlotNum, dataLenWords);
            }
            else if (keySize == VSS_KEY_512 && 
                     dataLenWords == (VSM_INPUT_RAW_DATA_512_WORDS + VSS_META_WORDS))
            {
                SYS_PRINT("VSM Pass:  Slot #%d Output Data (%d Words) OK\r\n",
                               vssSlotNum, dataLenWords);
            }
            else
            {
                SYS_PRINT(
                    "VSM FAIL: Data Length %d incorrect (key=%d)\r\n",
                    dataLenWords, keySize);
                ret_val = true;
            }
        } 
        else
        {
            SYS_PRINT("VSM FAIL: !!!CMD_VSM_OUTPUT_DATA FAILED (Bad Response)!!!\r\n");
            ret_val= true;
        }

        //Check Data
        if (dataLenWords > 0)
        {
            SYS_PRINT("\r\n...Checking Data (%d Words\r\n)", dataLenWords);

            if (expVsHeader.v != dmaDataOut[0])
            {
                SYS_PRINT("VSS Header FAIL:  Slot %d Header Error\r\n", 
                    vssSlotNum);
                SYS_PRINT("  RECEIVED:\r\n");
                VsHeaderInfo((VSHeader *)&dmaDataOut[0]);

                SYS_PRINT("  EXPECTED:\r\n");
                VsHeaderInfo((VSHeader *)&expVsHeader);
                outError=true;
                outIdx = 0;
            }
            else
            {
                SYS_PRINT("VS %d Header", vssSlotNum);
                VsHeaderInfo((VSHeader *)&dmaDataOut[0]);
            }

            //Compare remaining Data
            //NOTE:  Assumes using the raw input data input, skipping the
            //       initial input word.
            for (i=1; i<dataLenWords; i++) 
            {
                if (vsmInputDataPtr[i] != dmaDataOut[i]) 
                {
                    outError=true;
                    outIdx = i;
                    break;
                }
            }
            CRLF;

            //Print the Actual Slot Output 
            SYS_PRINT("\r\nActual: VSM Output Data(%d Words):", 
                       dataLenWords);
            for (i=0; i<dataLenWords; i++) 
            {
                //Print and Test
                if (i%4==0 || i==0) {CRLF; SYS_PRINT("%04x: ",i);} else {SYS_MESSAGE(" ");}
                SYS_PRINT("%08lx ", dmaDataOut[i]);
            }
            CRLF;

            if (outError == true)
            {
                SYS_PRINT("VSM FAIL:  Data Error @ i=%d\r\n", outIdx);
                ret_val = true;

                //VSM_CMD_INPUT_DATA Data
                SYS_PRINT("\r\nExpected: VSM Input Data(%d Words):", expSlotLengthWords); 
                for (i=0; i<expSlotLengthWords; i++) 
                {
                    //Print and Test
                    if (i%4==0 || i==0) {CRLF; SYS_PRINT("%04x: ",i);} else {SYS_MESSAGE(" ");}
                    if (i==0)
                    {
                        SYS_PRINT("%08lx ", expVsHeader.v);
                    }
                    else
                    {
                        SYS_PRINT("%08lx ", vsmInputDataPtr[i]);
                    }
                }
                CRLF;
            }
            else
            {
                SYS_MESSAGE("VSM Pass: CMD_VSM_OUTPUT_DATA DATA OUT MATCH\r\n");

            }
        }
#ifdef VSINFOTEST 
    } //End Slot !Empty Block
#endif //0


    SYS_PRINT("HSM TEST: CMD_VSM_OUTPUT_DATA Slot %d Complete - %s\r\n", 
        vssSlotNum, ret_val==0?"PASSED":"FAILED");
    return ret_val; 

} //End HsmCmdVsmOutputDataUnencrypted()


//******************************************************************************
//CMD_VSM_DELETE_SLOT Command--Delete the slot data
//   return: true --> Test Failed
//******************************************************************************
bool TestHsmCmdVsmDeleteSlot(int vssSlotNum) 
{
    RSP_DATA      *rsp;
    bool           ret_val = false;
    CmdResultCodes rcode;
    VSMetaData     vsMetaData;
    uint32_t       slotSizeBytes;

    SYS_PRINT("\r\n---------------------------------------------\r\n");
    SYS_PRINT("**TEST HSM VSM DELETE SLOT DATA - Slot %d**\r\n", 
        vssSlotNum);

    //Check Slot is NOT Empty
    rcode = HsmCmdVsmGetSlotInfo(vssSlotNum, &vsMetaData, &slotSizeBytes);
    if (rcode != S_OK)
    {
        SYS_PRINT("HSM: !!!CMD_VSM_DELETE_SLOT_INST TEST ABORT - SLOT #%d %s!!!\r\n",
                vssSlotNum, CmdResultCodeStr(rcode));
        ret_val = true; //FAIL
    }
    else
    {
        SYS_PRINT("--SLOT #%d NOT EMPTY!!!\r\n", vssSlotNum);
    }

    if (ret_val == false)
    {
        //Send the Command
        rsp = HsmCmdVsmDeleteSlot(vssSlotNum); 

        // Check the results of the command
        if (rsp->rspChksPassed == true) 
        {
            SYS_PRINT("HSM Cmd: CMD_VSM_DELETE_SLOT_INST terminated properly.\r\n");
        } 
        else 
        {
            SYS_PRINT("!!!CMD_VSM_DELETE_SLOT_INST FAILED!!!\r\n");
            SYS_PRINT("--""RC: %s""\r\n",CmdResultCodeStr(rsp->resultCode));
            ret_val = true; //FAIL
        }

        //Double Check Slot is Now Empty
        SYS_PRINT("CHECK SLOT %d Empty STATUS:\r\n", vssSlotNum);
        rcode = HsmCmdVsmGetSlotInfo(vssSlotNum, &vsMetaData, &slotSizeBytes); 
        if (rcode == E_VSEMPTY)
        {
            SYS_PRINT("--HSM PASS: Slot #%d EMPTY after delete\r\n",
                vssSlotNum);
            ret_val = false;
        }
        else
        {
            SYS_PRINT("--VSM FAIL: !!!Slot #%d %s!!!\r\n",
                          vssSlotNum, CmdResultCodeStr(rcode));
            //HsmCmdVsmPrintSlotInfo(vssSlotNum);
            ret_val = true; //FAIL
        }
    }

    SYS_PRINT("HSM TEST: CMD_VSM_DELETE_SLOT Slot %d Complete - %s\r\n", 
        vssSlotNum, ret_val==0?"PASSED":"FAILED");

    return ret_val;
} //End TestHsmCmdVsmDeleteSlot()


/* *****************************************************************************
 End of File
 */
