/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    aes_test.c

  @Summary

  @Description
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdlib.h>
#include "app.h"
#include "aes_test.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

//------------------------------------
// AES 128 ECB Test Vectors (From HSM AES Encrypt Decrypt Test.ipynb 4/21/23) 
//
// AES encryption key (16 byte):
//     b'abcdefghijklmnop'
//     b'6162636465666768696a6b6c6d6e6f70'
// 
// msg(L=32):                   b'TechTutorialsX!!TechTutorialsX!!'
//                              b'546563685475746f7269616c73582121546563685475746f7269616c73582121'
//
// encryptedMsg(L=32):          b'175aa7ea1dec81b68d6f1cd30316e7e6175aa7ea1dec81b68d6f1cd30316e7e6'
//
// decryptedMsg(L=32):          b'546563685475746f7269616c73582121546563685475746f7269616c73582121'


//AES 128 Key (16 byte))
static uint8_t ALIGN4 keyAes128[16] =
{
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 
    0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 
};

//AES 128 Message
static char * ALIGN4 msgAes128Str = "TechTutorialsX!!TechTutorialsX!!";
uint8_t msgAes128[] =
{
    0x54, 0x65, 0x63, 0x68, 0x54, 0x75, 0x74, 0x6f, 
    0x72, 0x69, 0x61, 0x6c, 0x73, 0x58, 0x21, 0x21, 
    0x54, 0x65, 0x63, 0x68, 0x54, 0x75, 0x74, 0x6f, 
    0x72, 0x69, 0x61, 0x6c, 0x73, 0x58, 0x21, 0x21, 
};

//AES 128 Encrypted Message
static uint8_t ALIGN4 expEncrAes128Msg[] =
{
    0x17, 0x5a, 0xa7, 0xea, 0x1d, 0xec, 0x81, 0xb6, 
    0x8d, 0x6f, 0x1c, 0xd3, 0x03, 0x16, 0xe7, 0xe6, 
    0x17, 0x5a, 0xa7, 0xea, 0x1d, 0xec, 0x81, 0xb6, 
    0x8d, 0x6f, 0x1c, 0xd3, 0x03, 0x16, 0xe7, 0xe6, 
};

//------------------------------------
// AES G1T2 Test ACVP-AES-ECB AFT tgId 1 tcId 2: 
//encrypt keyLen=128
//AES Encryption Key
#define KEYG1T2LENBITS 128
#define KEYG1T2LENBYTES  (KEYG1T2LENBITS/8)
#define MSGG1T2LENBYTES 16
#define AESG1T2_KEY_LENGTH  CMD_AES_KEY_128

//AES G1T2 Test 
uint8_t keyAesG1T2[KEYG1T2LENBYTES] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

//AES Message
uint8_t msgAesG1T2[MSGG1T2LENBYTES] =
{
    0xf3, 0x44, 0x81, 0xec, 0x3c, 0xc6, 0x27, 0xba, 
    0xcd, 0x5d, 0xc3, 0xfb, 0x08, 0xf2, 0x73, 0xe6, 
};

//AES Encrypted Message
uint8_t expEncrAesEcbG1T2Msg[MSGG1T2LENBYTES] =
{
    0x03, 0x36, 0x76, 0x3e, 0x96, 0x6d, 0x92, 0x59, 
    0x5a, 0x56, 0x7c, 0xc9, 0xce, 0x53, 0x7f, 0x5e, 
};

static uint8_t ALIGN4 encrAesMsg[sizeof(msgAes128)] = { 0x00 };

//static uint8_t ALIGN4 decrAes128Msg[sizeof(msgAes128)] = { 0x00 };


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Functions                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

//******************************************************************************
// AEC ECB Encrypt/Decrypt Command Test G1T1
//--TODO: Interrupt Mode
//  vsSlotNum < 0 --> streamed key
//******************************************************************************
bool TestHsmCmdAesEcbG1T1(int8_t vsSlotNum) 
{
    RSP_DATA *                 rsp;
    int                        i       = 0;
    bool                       ret_val = false;
    int                        msgSize = 0;
    int                        keySize = 0;
    //int                        numKeyWords = 0;
    bool                       useSlot = true;
    uint8_t                    slotNum;
    int                        numDataWords = 0;
    VSMetaData                 vsMetaData;
    CmdVSMDataSpecificMetaData specMetaData;
    CmdResultCodes             rcode;
    uint32_t                   slotSizeBytes;

    SYS_PRINT("\r\n---------------------------------------------\r\n");
    SYS_PRINT("\r\n**HSM AES128 ECB Encryption/Decryption TEST G1 T2 (Slot %d)**\r\n",
        vsSlotNum);

    keySize = sizeof(keyAesG1T2);
    //TODO:  Check KeySize to KeyLen

    //Print the Input Byte String
    SYS_PRINT("Key (%d Bytes):\r\n", keySize);
    SYS_MESSAGE("b\"/0x");
    for (i=0; i<keySize; i++) 
    {
        if (i%LINEBYTES==0 && i!=0) CRLF;
        SYS_PRINT("%02x/",keyAesG1T2[i]);
    }
    CRLF;

    msgSize = sizeof(msgAesG1T2);

    //Print the Input Byte String
    SYS_PRINT("Msg (%d Bytes): \r\n", sizeof(msgAesG1T2));
    SYS_MESSAGE("b\"/0x");
    for (i=0; i<msgSize; i++) 
    {
        if (i%LINEBYTES==0 && i!=0) CRLF;
        SYS_PRINT("%02x/",msgAesG1T2[i]);
    }
    CRLF;

    if (vsSlotNum > MINSLOTNUM && vsSlotNum < MAXSLOTNUM)
    {
        useSlot = true;

        //Check for the empty slot.
        rcode = HsmCmdVsmGetSlotInfo(vsSlotNum, &vsMetaData, &slotSizeBytes);
        //TODO:  Check the size of the slot key

        if (rcode != S_OK)
        {
            //Delete slot data
            HsmCmdVsmDeleteSlot(vsSlotNum); 
        }

        specMetaData.v = 0;
        specMetaData.aesSkMeta.s.aesType = CMD_AES_ECB; 
        specMetaData.aesSkMeta.s.keySize = AESG1T2_KEY_LENGTH; 
        specMetaData.aesSkMeta.s.keyType = VSS_SK_AES;

        //Load Symmetrical Key into Slot (APL 0)
        //TODO:  APL >0
        SYS_PRINT("HSM: Load AES 256 Key Slot %d\r\n", vsSlotNum);
        rsp = HsmCmdVsmInputDataUnencrypted(vsSlotNum, 
                                            (uint32_t *)&keyAesG1T2[0], 
                                            keySize, 
                                            CMD_VSS_SYMMETRICALKEY,
                                            specMetaData);
        
        if (rsp->rspChksPassed == true) 
        {

        }
        else
        {
            SYS_PRINT("AES FAIL:  Unable to Load Slot %d with Key - Abort\r\n", vsSlotNum);
            rsp->invSlot = true;
            return rsp;
        }
    }
    else
    {
        useSlot = false;
        slotNum = 0;
    }

    //Encrypt Msg
    //numKeyWords  = keySize/4;
    numDataWords = sizeof(msgAesG1T2)/4;
    SYS_PRINT("HSM: Sending CMD_AES_ENCRYPT (G1T2) command (msgLength=%d words\r\n",
               numDataWords);
    slotNum = useSlot?((uint8_t)vsSlotNum):0;
    rsp = HsmCmdAesEcbEncryptDecrypt(slotNum,
                                     true,   //encrypt
                                     useSlot?NULL:(uint32_t *) &keyAesG1T2[0],
                                     AESG1T2_KEY_LENGTH,
                                     (uint32_t *)msgAesG1T2,
                                     (uint32_t *)encrAesMsg, 
                                     numDataWords);

    // Check the results of the command
    if (rsp->rspChksPassed == true) 
    {
        SYS_PRINT("AESG1T2 Pass: CMD_VSM_INPUT_DATA terminated properly.\r\n");
    } 
    else 
    {
        SYS_PRINT("AESG1T2 FAIL: !!!CMD_VSM_AES_ECB ERROR!!! \r\n");
        rsp->invArgs?SYS_MESSAGE("-->Invalid Arguments\r\n"):0;
        rsp->invSlot?SYS_MESSAGE("-->Invalid Slot\r\n"):0;
        if (rsp->resultCode != S_OK) 
        {
            SYS_PRINT("RC: %s\r\n", CmdResultCodeStr(rsp->resultCode));
        }
        ret_val = true; //FAIL
    }

    //Test the Hash Output
    if (ret_val == false)
    {
        bool outErr = false;

        //Print the Output Hash Byte String
        SYS_PRINT("AESG1T2:  Encrypted MSG (%d Words)\r\n", numDataWords);
        SYS_MESSAGE("b\"/0x");
        for (i=0; i<numDataWords*4; i++) 
        {
            if (i%LINEBYTES==0 && i!=0) CRLF;
            SYS_PRINT("%02x/",encrAesMsg[i]);
            if (encrAesMsg[i] != expEncrAesEcbG1T2Msg[i]) outErr=true;
        }
        CRLF;

        if (outErr == true)
        {
            SYS_MESSAGE("AES G1T2 FAIL: !!!CMD_AES Encryption ERROR !!!\r\n");
            SYS_PRINT("Expected: Encrypted MSG (%d Words)\r\n", numDataWords);
            SYS_MESSAGE("b\"/0x");
            for (i=0; i<numDataWords*4; i++) 
            {
                if (i%LINEBYTES==0 && i!=0) CRLF;
                SYS_PRINT("%02x/",expEncrAesEcbG1T2Msg[i]);
            }
            CRLF;
        }
        else
        {
            SYS_MESSAGE("AESG1T2 Pass: CMD_AES256 Encrypt VALID\r\n");
        }
    }
    
    SYS_MESSAGE("HSM: CMD_AES_Encrypt G1T2 Complete\r\n");

    return ret_val;
    
} //End HsmCmdHashBlockSha256() 

//******************************************************************************
// AEC ECB Encrypt/Decrypt Command Test
//--TODO: Interrupt Mode
//  vsSlotNum < 0 --> streamed key
//******************************************************************************
bool TestHsmCmdAes128Ecb(int8_t vsSlotNum) 
{
    RSP_DATA *                 rsp;
    int                        i       = 0;
    bool                       ret_val = false;
    int                        msgSize = 0;
    int                        keySize = 0;
    //int                        numKeyWords = 0;
    bool                       useSlot = true;
    uint8_t                    slotNum;
    int                        numDataWords = 0;
    VSMetaData                 vsMetaData;
    CmdVSMDataSpecificMetaData specMetaData;
    CmdResultCodes             rcode;
    uint32_t                   slotSizeBytes;

    SYS_PRINT("\r\n---------------------------------------------\r\n");
    SYS_PRINT("\r\n**HSM AES256 ECB Encryption/Decryption TEST (Slot %d)**\r\n",
        vsSlotNum);

    keySize = sizeof(keyAes128);

    //Print the Input Byte String
    SYS_PRINT("Key (%d Bytes):\r\n", keySize);
    SYS_MESSAGE("b\"/0x");
    for (i=0; i<keySize; i++) 
    {
        if (i%LINEBYTES==0 && i!=0) CRLF;
        SYS_PRINT("%02x/",keyAes128[i]);
    }
    CRLF;

    msgSize = sizeof(msgAes128);

    //Print the Input Byte String
    SYS_PRINT("Msg (%d Bytes): \r\n", sizeof(msgAes128));
    SYS_PRINT("%s\r\n",msgAes128Str);
    SYS_MESSAGE("b\"/0x");
    for (i=0; i<msgSize; i++) 
    {
        if (i%LINEBYTES==0 && i!=0) CRLF;
        SYS_PRINT("%02x/",msgAes128[i]);
    }
    CRLF;

    if (vsSlotNum > MINSLOTNUM && vsSlotNum < MAXSLOTNUM)
    {
        useSlot = true;

        SYS_PRINT("Using Slot %d for AES Key\r\n",vsSlotNum);

        //Check for the empty slot.
        rcode = HsmCmdVsmGetSlotInfo(vsSlotNum, &vsMetaData, &slotSizeBytes);
        if (rcode != S_OK)
        {
            //Delete slot data
            HsmCmdVsmDeleteSlot(vsSlotNum); 
        }

        //Symmetric Key AES ECB
        specMetaData.v = 0;
        specMetaData.aesSkMeta.s.aesType = CMD_AES_ECB; 
        specMetaData.aesSkMeta.s.keySize = CMD_AES_KEY_256; 
        specMetaData.aesSkMeta.s.keyType = VSS_SK_AES;

        //Load Symmetrical Key into Slot (APL 0)
        SYS_PRINT("HSM: Load AES 256 Key Slot %d\r\n", vsSlotNum);
        rsp = HsmCmdVsmInputDataUnencrypted(vsSlotNum, 
                                            (uint32_t *)&keyAes128[0], 
                                            keySize, 
                                            CMD_VSS_SYMMETRICALKEY,
                                            specMetaData);
        
        if (rsp->rspChksPassed == true) 
        {
            SYS_PRINT("AES ECB Key loaded Slot %d\r\n",vsSlotNum);
        }
        else
        {
            SYS_PRINT("AES FAIL:  Unable to Load Slot %d with Key - Abort\r\n", vsSlotNum);
            rsp->invSlot = true;
            return rsp;
        }
    }
    else
    {
        //Stream the Key with the Data
        useSlot = false;
        slotNum = 0;
    }

    //Encrypt Msg
    //numKeyWords  = keySize/4;
    numDataWords = sizeof(msgAes128)/4;
    SYS_PRINT("HSM: Sending CMD_AES_ENCRYPT command (msg L=%d words\r\n",
               numDataWords);
    slotNum = useSlot?((uint8_t)vsSlotNum):0;
    rsp = HsmCmdAesEcbEncryptDecrypt(slotNum,
                                     true,   //encrypt
                                     useSlot?NULL:(uint32_t *) &keyAes128[0],
                                     CMD_AES_KEY_256,
                                     (uint32_t *)msgAes128,
                                     (uint32_t *)encrAesMsg, 
                                     numDataWords);

    // Check the results of the command
    if (rsp->rspChksPassed == true) 
    {
        SYS_PRINT("AES256 Pass: CMD_VSM_INPUT_DATA terminated properly.\r\n");
    } 
    else 
    {
        SYS_PRINT("AES256 FAIL: !!!CMD_VSM_AES_ECB ERROR!!! \r\n");
        rsp->invArgs?SYS_MESSAGE("-->Invalid Arguments\r\n"):0;
        rsp->invSlot?SYS_MESSAGE("-->Invalid Slot\r\n"):0;
        if (rsp->resultCode != S_OK) 
        {
            SYS_PRINT("RC: %s\r\n", CmdResultCodeStr(rsp->resultCode));
        }
        ret_val = true; //FAIL
    }

    //Test the encrypted Output
    if (ret_val == false)
    {
        bool outErr = false;

        //Print the Output Hash Byte String
        SYS_PRINT("AES256:  Encrypted MSG (%d Words)\r\n", numDataWords);
        SYS_MESSAGE("b\"/0x");
        for (i=0; i<numDataWords*4; i++) 
        {
            if (i%LINEBYTES==0 && i!=0) CRLF;
            SYS_PRINT("%02x/",encrAesMsg[i]);
            if (encrAesMsg[i] != expEncrAes128Msg[i]) outErr=true;
        }
        CRLF;

        if (outErr == true)
        {
            SYS_MESSAGE("AES 256 FAIL: !!!CMD_AES Encryption ERROR !!!\r\n");
            SYS_PRINT("Expected: Encrypted MSG (%d Words)\r\n", numDataWords);
            SYS_MESSAGE("b\"/0x");
            for (i=0; i<numDataWords*4; i++) 
            {
                if (i%LINEBYTES==0 && i!=0) CRLF;
                SYS_PRINT("%02x/",expEncrAes128Msg[i]);
            }
            CRLF;
        }
        else
        {
            SYS_MESSAGE("AES256 Pass: CMD_AES256 Encrypt VALID\r\n");
        }
    }
    
    SYS_MESSAGE("HSM: CMD_AES_256 Encrypt/Decrypt Complete\r\n");

    return ret_val;
    
} //End HsmCmdHashBlockSha256() 

/* *****************************************************************************
 End of File
 */
