/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip

  @File Name
    hsm_test_suite.c

  @Summary

  @Description
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "hsm_test_suite.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */


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
// HSM Test Suite
//******************************************************************************
int hsm_test_suite(int vsSlotNum, int aesSlotNum)
{
    VSMetaData vsMetaData; 
    uint32_t   slotSizeBytes;

    //HSM MB Command Test Suite
    LED0_On();
    LED1_Off();

#if 1    
    //HASH Test Suite
    TestHsmCmdHashBlockSha256(); 

    //VSM Raw 256 Bit Key Tests
    TestHsmCmdVsmInputDataUnencryptedRaw(vsSlotNum, VSS_KEY_256); 

    if (HsmCmdVsmGetSlotInfo(vsSlotNum, &vsMetaData, &slotSizeBytes) != E_VSEMPTY)
    {
        SYS_PRINT("VSInfo:  Slot %d %d Bytes (%d Words)\r\n", 
            vsSlotNum, (int) slotSizeBytes, (int) slotSizeBytes>>2); 
        TestHsmCmdVsmOutputDataUnencryptedRaw(vsSlotNum, VSS_KEY_256); 

        TestHsmCmdVsmDeleteSlot(vsSlotNum); 
    }
    else
    {
        SYS_PRINT("SLOT %d Empty\r\n", vsSlotNum);
    }
#endif //0

#if 1
    //VSM Raw - 512 Bit Key
    TestHsmCmdVsmInputDataUnencryptedRaw(vsSlotNum, VSS_KEY_512); 
    if (HsmCmdVsmGetSlotInfo(vsSlotNum, &vsMetaData, &slotSizeBytes) != E_VSEMPTY)
    {
        SYS_PRINT("VSInfo:  Slot %d %d Bytes (%d Words)\r\n", 
            vsSlotNum, (int) slotSizeBytes, (int) slotSizeBytes>>2); 
        TestHsmCmdVsmOutputDataUnencryptedRaw(vsSlotNum, VSS_KEY_512); 
        TestHsmCmdVsmDeleteSlot(vsSlotNum); 
    }
    else
    {
        SYS_PRINT("SLOT %d Empty\r\n", vsSlotNum);
    }
#endif //0

#if 1
    //AES
    if (aesSlotNum > MINSLOTNUM && aesSlotNum < MAXSLOTNUM)
    {
        //TODO: Other AES modes 
        //VSM AES KEY Tests
        TestHsmCmdVsmInputDataUnencryptedAes(aesSlotNum); 
        if (HsmCmdVsmGetSlotInfo(vsSlotNum, &vsMetaData, &slotSizeBytes) != E_VSEMPTY)
        {
            TestHsmCmdVsmOutputDataUnencryptedAes(aesSlotNum); 
            TestHsmCmdVsmDeleteSlot(aesSlotNum); 
            SYS_PRINT("VSS AES ECB Test- Using Key Slot %d\r\n",
                (int)aesSlotNum);
        }
    }
    else
    {
        SYS_PRINT("VSS AES ECB Test- Using Streaming Key (%d)\r\n",
            (int)aesSlotNum);
    }
    
    //AES Encryption on text data
    int noSlot = -1;
    TestHsmCmdAes128Ecb(noSlot); 

    //AES Encryption on Test Data G1 T1
    TestHsmCmdAesEcbG1T1(noSlot); 
#endif //0

    LED1_On();

    return 0;

} //End hsm_test_suite()


/* *****************************************************************************
 End of File
 */
