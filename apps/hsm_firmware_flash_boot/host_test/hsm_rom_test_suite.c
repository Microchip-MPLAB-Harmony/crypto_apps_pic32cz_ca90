/*
* \brief hsm_rom_test_suite.c
*
* ----------------------------------------------------------------------------
*         Microchip Microcontroller Software Support
* ----------------------------------------------------------------------------
* Copyright (C) 2017, Microchip Technology Inc. and its subsidiaries (Microchip).
*
* All rights reserved.
*
* You are permitted to use the software and its derivatives with Microchip
* products. See the license agreement accompanying this software, if any,
* for additional info regarding your rights and obligations.
*
* THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
* OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
* PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED
* UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF
* WARRANTY, OR OTHER LEGAL OR EQUITABLE THEORY FOR ANY DIRECT OR INDIRECT
* DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES, OR OTHER SIMILAR COSTS. TO THE FULLEST
* EXTENT ALLOWED BY LAW, MICROCHIP AND ITS LICENSORS LIABILITY WILL NOT EXCEED
* THE AMOUNT OF FEES, IF ANY, THAT YOU PAID DIRECTLY TO MICROCHIP TO USE THIS
* SOFTWARE.  MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR
* ACCEPTANCE OF THESE TERMS.
* ----------------------------------------------------------------------------
*/

#include "hsm_rom_test_suite.h"
#include "hsm_rom_test_driver.h"
#include "string.h"
#include <xc.h>

uint32_t flag_systick = 0;

#define NBCYCLESPERTICK 1000
// Clock period in milliseconds
//#define CLKPER 0.000041667 // 24MHz
#define CLKPER 0.000006667 // 150MHz

#define BAUD_RATE 115200

#pragma FUSES_USERCFG1_FUCFG9_RAM_ECC_EN0 = SET
#pragma FUSES_USERCFG1_FUCFG9_ECC_RAM_INIT_ENB = SET
#pragma FUSES_USERCFG2_FUCFG9_RAM_ECC_EN0 = SET
#pragma FUSES_USERCFG2_FUCFG9_ECC_RAM_INIT_ENB = SET

//Sercom   *SercomArray[SERCOM_INST_NUM] = SERCOM_INSTS;

/////////////////////
// Systick Handler //
/////////////////////
void SysTick_Handler(void)
{
  flag_systick++ ;
}

//////////////////////////
// HSM Response Checker //
//////////////////////////
void hsm_rom_test_cmd_rsp_chkr(void) {
  
  uint32_t rd_data = 0;
  
  // Reset the response checker global variable
  testData.rsp_chks_passed = true;
  
  // Check the Mailbox Header
  if (hsm_rom_test_cmd_resp.mbHeader != testData.hsm_rom_test_cmd_req.expMbHeader) {

    if (hsm_rom_test_cmd_resp.resultCode != E_INVFORMAT && hsm_rom_test_cmd_resp.resultCode != E_SAFEMODE)
    {
#if 0
      printc("FAILURE: Mailbox Header Match Failed\n");
#ifdef __FPGA__
      printc("Expected: 0x%x\n", testData.hsm_rom_test_cmd_req.expMbHeader);
      printc("Actual:   0x%x\n", hsm_rom_test_cmd_resp.mbHeader);
#else
      printc("Expected: ");
      printx(testData.hsm_rom_test_cmd_req.expMbHeader);
      printc("\nActual:   ");
      printx(hsm_rom_test_cmd_resp.mbHeader);
      printc("\n");
#endif
#endif
      testData.testFail++;
      testData.rsp_chks_passed = false;
    }
  }
  
  // Check the Command Header 
  if (hsm_rom_test_cmd_resp.cmdHeader != testData.hsm_rom_test_cmd_req.cmdHeader) {
    if (hsm_rom_test_cmd_resp.resultCode != E_INVFORMAT && hsm_rom_test_cmd_resp.resultCode != E_SAFEMODE)
    {
#if 0
      printc("FAILURE: Command Header Match Failed\n");
#ifdef __FPGA__
      printc("Expected: 0x%x\n", testData.hsm_rom_test_cmd_req.cmdHeader);
      printc("Actual:   0x%x\n", hsm_rom_test_cmd_resp.cmdHeader);
#else
      printc("Expected: ");
      printx(testData.hsm_rom_test_cmd_req.cmdHeader);
      printc("\nActual:   ");
      printx(hsm_rom_test_cmd_resp.cmdHeader);
      printc("\n");
#endif
#endif
      testData.testFail++;
      testData.rsp_chks_passed = false;
    }
  }
  
  // Check the Result Code
  if (hsm_rom_test_cmd_resp.resultCode != testData.hsm_rom_test_cmd_req.expResultCode) {
#if 0
    printc("FAILURE: Result Code Match Failed\n");
#ifdef __FPGA__
    printc("Expected: 0x%x\n", testData.hsm_rom_test_cmd_req.expResultCode);
    printc("Actual:   0x%x\n", hsm_rom_test_cmd_resp.resultCode);
#else
    printc("Expected: ");
    printx(testData.hsm_rom_test_cmd_req.expResultCode);
    printc("\nActual:   ");
    printx(hsm_rom_test_cmd_resp.resultCode);
    printc("\n");
#endif
#endif
    testData.testFail++;
    testData.rsp_chks_passed = false;
  }
  
  // Check the HSM STATUS register
  rd_data = HSM_REGS->HSM_STATUS;
  if (rd_data != testData.hsm_rom_test_cmd_req.expStatus) {
    if (hsm_rom_test_cmd_resp.resultCode != S_OK)
    {
      testData.testFail++;
#if 0
      printc("FAILURE: STATUS read after command completion failed:\n");
#ifdef __FPGA__
      printc("Expected: 0x%x\n", testData.hsm_rom_test_cmd_req.expStatus);
      printc("Actual:   0x%x\n", rd_data);
#else
      printc("Expected: ");
      printx(testData.hsm_rom_test_cmd_req.expStatus);
      printc("\nActual:   ");
      printx(rd_data);
      printc("\n");
#endif
#endif
    }
  }
  
  // Check the HSM INTFLAG register
  rd_data = HSM_REGS->HSM_INTFLAG;
  if (rd_data != testData.hsm_rom_test_cmd_req.expIntFlag) {
    if (hsm_rom_test_cmd_resp.resultCode != E_INVFORMAT && hsm_rom_test_cmd_resp.resultCode != E_SAFEMODE)
    {
      testData.testFail++;
#if 0
      printc("FAILURE: INTFLAG read after command completion failed:\n");
#ifdef __FPGA__
      printc("Expected: 0x%x\n", testData.hsm_rom_test_cmd_req.expIntFlag);
      printc("Actual:   0x%x\n", rd_data);
#else
      printc("Expected: ");
      printx(testData.hsm_rom_test_cmd_req.expIntFlag);
      printc("\nActual:   ");
      printx(rd_data);
      printc("\n");
#endif
#endif
    }
  }
  
  // Clear the INTFLAG.ERROR if it was set
  if ((rd_data & 0x00000001) == 0x00000001) {
    HSM_REGS->HSM_INTFLAG |= 0x00000001;
  }
}

///////////////////////////
// HSM Interrupt Handler //
///////////////////////////
#ifdef __FPGA__
void HSM_RXINT_Handler(void) {
#else
  void INTREQ_217_Handler(void) {
#endif
    
    // Disable interrupt while processing
    HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(0);
    
    // Process the command response
    hsm_rom_test_cmd_rsp_prcsr();
    
    // Check the command response 
    hsm_rom_test_cmd_rsp_chkr();
    
    // Set the interrupt trigged flag so the test can check the output data
    testData.hsm_rx_int = true;
    
    // Re-enable the interrupt
    HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(1);
  }
  
  //////////////////////////////////////////////////////
  // Function to issue the CMD_BOOT_SELF_TEST command //
  //////////////////////////////////////////////////////
  void hsm_rom_test_bist(bool int_mode) {
    
    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = CMD_BOOT_SELF_TEST_MBHEADER;
    testData.hsm_rom_test_cmd_req.cmdHeader     = CMD_BOOT_SELF_TEST;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = CMD_BOOT_SELF_TEST_PARAMETER1;
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
  }
    
typedef struct
{
    void * addr;
} cmdSGAddressOfData;

typedef union
{
    struct {
    unsigned char stop : 1;
    unsigned char : 1;
    uint32_t addr : 30;
    }s;
    uint32_t v;
} cmdSGNextDescriptor;

typedef union
{
    struct {
    unsigned int length : 28;
    unsigned char cstAddr : 1;
    unsigned char realign : 1;
    unsigned char discard : 1;
    unsigned char IntEn: 1;
    }s;
    uint32_t v;
} cmdSGFlagsLength;

typedef struct
{
    cmdSGAddressOfData data;
    cmdSGNextDescriptor next;
    cmdSGFlagsLength length;
} cmdSGDescriptor;

//char dataBlock[] = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz01";
char dataBlock[] = "abcdefghijk";
    cmdSGDescriptor inpDesc[5] = {0};
cmdSGDescriptor outDesc[5] = {0};
  uint32_t HashInitBuffer[64] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
                                 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                                 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
                                 0x51, 0x52, 0x63, 0x54, 0x55, 0x56, 0x57, 0x58,
                                 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
                                 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
  };
  

void hsm_rom_test_hinit(bool int_mode) 
{
    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = 0x00f00014;
    testData.hsm_rom_test_cmd_req.cmdHeader     = 0x00040105;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = (uint32_t)(&(outDesc[0])); // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = 0x00000000;
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
    
    outDesc[0].data.addr = &HashInitBuffer;
    outDesc[0].next.s.stop = 1;
    outDesc[0].next.s.addr = 0;
    outDesc[0].length.s.length = 32;
    outDesc[0].length.s.cstAddr = 0;
    outDesc[0].length.s.discard = 0;
    outDesc[0].length.s.realign = 1;
    outDesc[0].length.s.IntEn = 0;
    
    SCB_CleanDCache_by_Addr((uint32_t *) inpDesc, sizeof(inpDesc));
    SCB_CleanDCache_by_Addr((uint32_t *) outDesc, sizeof(outDesc));
    SCB_CleanInvalidateDCache_by_Addr(HashInitBuffer, sizeof(HashInitBuffer));
    
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
  }
  



void hsm_rom_test_hblock(bool int_mode) {

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = 0x00f00018;
    testData.hsm_rom_test_cmd_req.cmdHeader     = 0x00040005;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = (uint32_t)(&(inpDesc[0])); // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = (uint32_t)(&(outDesc[0])); // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = strlen(dataBlock);
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
    
    outDesc[0].data.addr = &HashInitBuffer;
    outDesc[0].next.s.stop = 1;
    outDesc[0].next.s.addr = 0;
    outDesc[0].length.s.length = 32;
    outDesc[0].length.s.cstAddr = 0;
    outDesc[0].length.s.discard = 0;
    outDesc[0].length.s.realign = 1;
    outDesc[0].length.s.IntEn = 0;
    
    inpDesc[0].data.addr = &dataBlock;
    inpDesc[0].next.s.stop = 1;
    inpDesc[0].next.s.addr = 0;
    inpDesc[0].length.s.length = strlen(dataBlock);
    inpDesc[0].length.s.cstAddr = 0;
    inpDesc[0].length.s.discard = 0;
    inpDesc[0].length.s.realign = 1;
    inpDesc[0].length.s.IntEn = 0;  
    SCB_CleanDCache_by_Addr((uint32_t *)inpDesc, sizeof(inpDesc));
    SCB_CleanDCache_by_Addr((uint32_t *)outDesc, sizeof(outDesc));
    SCB_CleanInvalidateDCache_by_Addr(HashInitBuffer, sizeof(HashInitBuffer));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)dataBlock, sizeof(dataBlock));
    
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
  }
  ////////////////////////////////////////////////////////
  // Function to issue the CMD_SD_GET_CHALLENGE command //
  ////////////////////////////////////////////////////////

uint32_t keyBlockTest[] =
{
    0x0000002c, // Length
    0x00000000,  // Not Valid Before
    0xffffffff,  // Not Valid After
    0x4c702000,  // Key Metadata
    0x12345678,
    0x9ABCDEF0,
    0x12345678,
    0x9ABCDEF0,
    0x12345678,
    0x9ABCDEF0,
    0x12345678,
    0x9ABCDEF0,
};

void hsm_rom_test_inputdata(bool int_mode) {

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = 0x00f00014;
    testData.hsm_rom_test_cmd_req.cmdHeader     = 0x00000003;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = (uint32_t)(&(inpDesc[0])); // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = 0x011d0020;
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
    
    outDesc[0].data.addr = &HashInitBuffer;
    outDesc[0].next.s.stop = 1;
    outDesc[0].next.s.addr = 0;
    outDesc[0].length.s.length = 0x30;
    outDesc[0].length.s.cstAddr = 0;
    outDesc[0].length.s.discard = 0;
    outDesc[0].length.s.realign = 1;
    outDesc[0].length.s.IntEn = 0;
    
    inpDesc[0].data.addr = &keyBlockTest;
    inpDesc[0].next.s.stop = 1;
    inpDesc[0].next.s.addr = 0;
    inpDesc[0].length.s.length = sizeof(keyBlockTest);
    inpDesc[0].length.s.cstAddr = 0;
    inpDesc[0].length.s.discard = 0;
    inpDesc[0].length.s.realign = 1;
    inpDesc[0].length.s.IntEn = 0;  
    SCB_CleanDCache_by_Addr((uint32_t *)inpDesc, sizeof(inpDesc));
    SCB_CleanDCache_by_Addr((uint32_t *)outDesc, sizeof(outDesc));
    SCB_CleanInvalidateDCache_by_Addr(HashInitBuffer, sizeof(HashInitBuffer));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)dataBlock, sizeof(dataBlock));
    SCB_CleanInvalidateDCache_by_Addr(keyBlockTest, sizeof(keyBlockTest));
    
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
  }

void hsm_rom_test_outputdata(bool int_mode) {

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = 0x00f00014;
    testData.hsm_rom_test_cmd_req.cmdHeader     = 0x00000103;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = (uint32_t)(&(outDesc[0])); // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = 0x00000100;
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
    
    outDesc[0].data.addr = &HashInitBuffer;
    outDesc[0].next.s.stop = 1;
    outDesc[0].next.s.addr = 0;
    outDesc[0].length.s.length = 0x30;
    outDesc[0].length.s.cstAddr = 0;
    outDesc[0].length.s.discard = 0;
    outDesc[0].length.s.realign = 1;
    outDesc[0].length.s.IntEn = 0;
    
    inpDesc[0].data.addr = &keyBlockTest;
    inpDesc[0].next.s.stop = 1;
    inpDesc[0].next.s.addr = 0;
    inpDesc[0].length.s.length = sizeof(keyBlockTest);
    inpDesc[0].length.s.cstAddr = 0;
    inpDesc[0].length.s.discard = 0;
    inpDesc[0].length.s.realign = 1;
    inpDesc[0].length.s.IntEn = 0;  

    SCB_CleanDCache_by_Addr((uint32_t *)inpDesc, sizeof(inpDesc));
    SCB_CleanDCache_by_Addr((uint32_t *)outDesc, sizeof(outDesc));
    SCB_CleanInvalidateDCache_by_Addr(HashInitBuffer, sizeof(HashInitBuffer));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)dataBlock, sizeof(dataBlock));
    SCB_CleanInvalidateDCache_by_Addr(keyBlockTest, sizeof(keyBlockTest));

    
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
  }
void hsm_rom_test_getInfo(bool int_mode) {

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = 0x00f00014;
    testData.hsm_rom_test_cmd_req.cmdHeader     = 0x00000503;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = (uint32_t)(&(outDesc[0])); // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = 0x00000100;
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
    
    outDesc[0].data.addr = &HashInitBuffer;
    outDesc[0].next.s.stop = 1;
    outDesc[0].next.s.addr = 0;
    outDesc[0].length.s.length = 16;
    outDesc[0].length.s.cstAddr = 0;
    outDesc[0].length.s.discard = 0;
    outDesc[0].length.s.realign = 1;
    outDesc[0].length.s.IntEn = 0;
    
    inpDesc[0].data.addr = &keyBlockTest;
    inpDesc[0].next.s.stop = 1;
    inpDesc[0].next.s.addr = 0;
    inpDesc[0].length.s.length = sizeof(keyBlockTest);
    inpDesc[0].length.s.cstAddr = 0;
    inpDesc[0].length.s.discard = 0;
    inpDesc[0].length.s.realign = 1;
    inpDesc[0].length.s.IntEn = 0;  

    SCB_CleanDCache_by_Addr((uint32_t *)inpDesc, sizeof(inpDesc));
    SCB_CleanDCache_by_Addr((uint32_t *)outDesc, sizeof(outDesc));
    SCB_CleanInvalidateDCache_by_Addr(HashInitBuffer, sizeof(HashInitBuffer));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)dataBlock, sizeof(dataBlock));
    SCB_CleanInvalidateDCache_by_Addr(keyBlockTest, sizeof(keyBlockTest));
    
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
  }

void hsm_rom_test_loadFirmware(bool int_mode) {

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = 0x00f00018;
    testData.hsm_rom_test_cmd_req.cmdHeader     = 0x00000100;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = 0x0c7df800;
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
        
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
}



void hsm_rom_test_writeNvm(bool int_mode) {

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = 0x00f00010;
    testData.hsm_rom_test_cmd_req.cmdHeader     = 0x000006F0;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = 0x00000000;
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
    
    outDesc[0].data.addr = &HashInitBuffer;
    outDesc[0].next.s.stop = 1;
    outDesc[0].next.s.addr = 0;
    outDesc[0].length.s.length = 16;
    outDesc[0].length.s.cstAddr = 0;
    outDesc[0].length.s.discard = 0;
    outDesc[0].length.s.realign = 1;
    outDesc[0].length.s.IntEn = 0;
    
    inpDesc[0].data.addr = &keyBlockTest;
    inpDesc[0].next.s.stop = 1;
    inpDesc[0].next.s.addr = 0;
    inpDesc[0].length.s.length = sizeof(keyBlockTest);
    inpDesc[0].length.s.cstAddr = 0;
    inpDesc[0].length.s.discard = 0;
    inpDesc[0].length.s.realign = 1;
    inpDesc[0].length.s.IntEn = 0;  
    
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
  }

void hsm_rom_test_factoryReset(bool int_mode) {

    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = 0x00f00010;
    testData.hsm_rom_test_cmd_req.cmdHeader     = 0x000009F0;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = 0x00000000;
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000; // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SELF_TEST_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SELF_TEST_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_BOOT_SELF_TEST command\n");
    

    
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
#ifdef __FPGA__
      printc("Failed Test code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
#endif
      //printc("HSM ROM Test: CMD_BOOT_SELF_TEST test was unsuccessful.\n");
    }
    
    //printc("---------------------------------------------\r\n");
  }


#if 0
  void hsm_rom_test_get_challenge(bool int_mode) {
    
    // Local Variables
    uint8_t                         i;
    uint32_t                        chlngCmp = 0;   // Challenge comparison flag
    cmdSGDescriptor                 outDescr;       // DMA output descriptor for challenge value 
    cmdSDGetChallengeOutput1        chlngOutput;    // Variable for storing challenge value 
    
    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Create a dma descriptor for the challenge value output data
    outDescr.data   = (uint32_t) &chlngOutput;  // Pointer to challenge value output
    outDescr.next   = 0x00000001;               // Bit 0 - Stop (end of descriptor linked-list)
    outDescr.length = 0x20000010;               // Bit 29 - Realign; Length (16-bytes)
    
    // Initialize challenge variable to zeros
    for (i=0; i<sizeof(chlngOutput.challenge); i++) {
      chlngOutput.challenge[i] = 0x00;
    }
    
    // Send CMD_SD_GET_CHALLENGE command to load the HSM firmware
    testData.hsm_rom_test_cmd_req.mbHeader      = CMD_SD_GET_CHALLENGE_MBHEADER;
    testData.hsm_rom_test_cmd_req.cmdHeader     = CMD_SD_GET_CHALLENGE;
    testData.hsm_rom_test_cmd_req.cmdInputs[0]  = 0x00000000;            // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[1]  = (uint32_t) &outDescr;  // Pointer to output descriptor
    testData.hsm_rom_test_cmd_req.cmdInputs[2]  = 0x00000000;            // Unused
    testData.hsm_rom_test_cmd_req.cmdInputs[3]  = 0x00000000;            // Unused
    testData.hsm_rom_test_cmd_req.expMbHeader   = CMD_SD_GET_CHALLENGE_MBHEADER_RX;
    testData.hsm_rom_test_cmd_req.expResultCode = SD_GET_CHLNG_EXP_RSLT_CODE;
    testData.hsm_rom_test_cmd_req.expStatus     = SD_CHLNG_STATUS;
    testData.hsm_rom_test_cmd_req.expIntFlag    = SD_CHLNG_INTFLAG;
    //printc("HSM ROM Test: Sending CMD_SD_GET_CHALLENGE command\n");
    hsm_rom_test_driver(&testData.hsm_rom_test_cmd_req, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //printc("HSM ROM Test: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Assign the challenge output to the global variable for use by the auth command
    //strcpy(sd_auth_challenge, chlngOutput.challenge);
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SD_GET_CHLNG_EXP_RSLT_CODE == E_INVFORMAT) {
        //printc("HSM ROM Test: CMD_SD_GET_CHALLENGE terminated properly with E_INVFORMAT for negative test.\n");
        //printc("HSM ROM Test: CMD_SD_GET_CHALLENGE test was successful.\n");        
      } else {
        // Make sure the challenge output is not zero (32-bits is enough to compare)
        if (chlngOutput.challenge == 0x00000000) {
          //printc("FAILURE: Challenge output is zeros.\n");
          //printc("HSM ROM Test: CMD_SD_GET_CHALLENGE test was unsuccessful.\n");
          testData.testFail++;
        } else {
          // Make sure the challenge output does not match the previous value (32-bits is enough to compare)
          chlngCmp = memcmp(&chlngOutput.challenge, &last_challenge, 4);
#ifndef FIXED_CHLNG
          if (chlngCmp == 0) {
            //printc("FAILURE: Challenge output matches last value.\n");
            //printc("HSM ROM Test: CMD_SD_GET_CHALLENGE test was unsuccessful.\n");
            testData.testFail++;
          } else {
#else
          if (1) {
#endif
            //printc("Challenge Output: ");
#ifdef __FPGA__
            for (i=0; i<sizeof(chlngOutput.challenge); i++) {
              printc("%x ", chlngOutput.challenge[i]);
            }
#else
            //printx(chlngOutput.challenge);
            //printc("\n");
#endif
            //printc("\nHSM ROM Test: CMD_SD_GET_CHALLENGE test was successful.\n");
          }
        }
      }
    } else {
      //printc("HSM ROM Test: CMD_SD_GET_CHALLENGE test was unsuccessful.\n");
    }
    memcpy(&last_challenge, &chlngOutput, sizeof(chlngOutput));
    //printc("\n---------------------------------------------\n");
  }
    
#endif
  void port_set_pinmux(uint32_t pinmux) {
    uint32_t function = ( pinmux &0xFF);
    uint32_t port_nb  =((pinmux>>16)&0xFF) / 32;
    uint32_t pin      =((pinmux>>16)&0xFF) % 32;
    
    if (pin & 1) {
      PORT_REGS->GROUP[port_nb].PORT_PMUX[pin >> 1] |= PORT_PMUX_PMUXO(function); // Odd-numbered pin
    }
    else {
      PORT_REGS->GROUP[port_nb].PORT_PMUX[pin >> 1] |= PORT_PMUX_PMUXE(function); // Even-numbered pin
    }
    PORT_REGS->GROUP[port_nb].PORT_PINCFG[pin] |= PORT_PINCFG_PMUXEN(1); // Enable port mux
  }
  
  uint16_t compute_baud_frac(uint32_t fbaud, uint32_t fref, uint32_t Sperbit)
  {
    uint16_t u16_baudrate_int;
    uint16_t u16_baudrate_fp;    
    uint16_t u16_baudrate_reg;        
    
    //compute integral and fractionnal part of baudrate reg
    float fTmp;
    fTmp = fref;
    fTmp = fTmp / fbaud;
    fTmp = fTmp / Sperbit;
    u16_baudrate_int = (uint16_t)fTmp;
    fTmp -= u16_baudrate_int;
    fTmp = fTmp*8;
    u16_baudrate_fp = (uint16_t)fTmp;
    u16_baudrate_reg = u16_baudrate_int | u16_baudrate_fp<<13;
    return(u16_baudrate_reg);
  }
  
  uint16_t compute_baud(uint32_t fbaud, uint32_t fref, uint16_t SAMPR)
  {
    uint16_t BAUD = 0; // Register Value
    switch(SAMPR) {
    case 0:
      BAUD = (uint16_t)((float)65536.0*((float)1.0- ((float)16*((float)fbaud/(float)fref))));
      break;
    case 1:
      BAUD = compute_baud_frac(fbaud, fref, 16);
      break;
    case 2:
      BAUD = (uint16_t)((float)65536.0*((float)1.0- ((float)8*((float)fbaud/(float)fref))));
      break;
    case 3:
      BAUD = compute_baud_frac(fbaud, fref, 8);
      break;
    case 4:
      BAUD = (uint16_t)((float)65536.0*((float)1.0- ((float)3*((float)fbaud/(float)fref))));
      break;
    }
    return BAUD;
  }
  
  void __LOCAL_SERCOM_PinsSetup(uint32_t instanceNum)
  {
    switch (instanceNum)
    {
    case 0:
      port_set_pinmux(PINMUX_PC00D_SERCOM0_PAD0);         //PC00 - PAD0
      port_set_pinmux(PINMUX_PC01D_SERCOM0_PAD1);         //PC01 - PAD1
      port_set_pinmux(PINMUX_PC02D_SERCOM0_PAD2);         //PC02 - PAD2
      port_set_pinmux(PINMUX_PC03D_SERCOM0_PAD3);         //PC03 - PAD3
      break;
    case 1:
      port_set_pinmux(PINMUX_PC04D_SERCOM1_PAD0);         //PC04 - PAD0
      port_set_pinmux(PINMUX_PC05D_SERCOM1_PAD1);         //PC05 - PAD1
      port_set_pinmux(PINMUX_PC06D_SERCOM1_PAD2);         //PC06 - PAD2
      port_set_pinmux(PINMUX_PC07D_SERCOM1_PAD3);         //PC07 - PAD3
      break;
    case 2:
      port_set_pinmux(PINMUX_PC08D_SERCOM2_PAD0);         //PC08 - PAD0
      port_set_pinmux(PINMUX_PC09D_SERCOM2_PAD1);         //PC09 - PAD1
      port_set_pinmux(PINMUX_PC10D_SERCOM2_PAD2);         //PC10 - PAD2
      port_set_pinmux(PINMUX_PC11D_SERCOM2_PAD3);         //PC11 - PAD3
      break;
    case 3:
      port_set_pinmux(PINMUX_PC12D_SERCOM3_PAD0);         //PC12 - PAD0
      port_set_pinmux(PINMUX_PC13D_SERCOM3_PAD1);         //PC13 - PAD1
      port_set_pinmux(PINMUX_PC14D_SERCOM3_PAD2);         //PC14 - PAD2
      port_set_pinmux(PINMUX_PC15D_SERCOM3_PAD3);         //PC15 - PAD3
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      port_set_pinmux(PINMUX_PD14D_SERCOM7_PAD0);         //PD14 - PAD0
      port_set_pinmux(PINMUX_PD15D_SERCOM7_PAD1);         //PD15 - PAD1
      port_set_pinmux(PINMUX_PD16D_SERCOM7_PAD2);         //PD16 - PAD2
      port_set_pinmux(PINMUX_PD17D_SERCOM7_PAD3);         //PD17 - PAD3
      break;
    case 8:
      port_set_pinmux(PINMUX_PD24D_SERCOM8_PAD0);         //PD24 - PAD0
      port_set_pinmux(PINMUX_PD25D_SERCOM8_PAD1);         //PD25 - PAD1
      port_set_pinmux(PINMUX_PD26D_SERCOM8_PAD2);         //PD26 - PAD2
      port_set_pinmux(PINMUX_PD27D_SERCOM8_PAD3);         //PD27 - PAD3
      break;
    case 9:
      break;
    default:
      break;
    }
  }
  
#ifdef __FPGA__
  void __LOCAL_SERCOM_Initialize(uint32_t instanceNum)
  {
    // Setup pins for SERCOMs
    __LOCAL_SERCOM_PinsSetup(instanceNum);
    
//    if (SERCOM1_REGS->USART.SERCOM_CTRLA & SERCOM_USART_CTRLA_ENABLE_Msk == 0)
//    {
      // SERCOM(instanceNum) CTRLA settings
      // Disable SERCOM before configuration
      SERCOM1_REGS->USART.SERCOM_CTRLA |= SERCOM_USART_CTRLA_ENABLE(0);
      //Wait for synchronization
      while (SERCOM1_REGS->USART.SERCOM_SYNCBUSY & SERCOM_USART_SYNCBUSY_ENABLE_Msk);
      
      // Reset SERCOM before configuration
      SERCOM1_REGS->USART.SERCOM_CTRLA |= SERCOM_USART_CTRLA_SWRST_Msk;
      //Wait for synchronization
      while (SERCOM1_REGS->USART.SERCOM_SYNCBUSY & SERCOM_USART_SYNCBUSY_SWRST_Msk);
      
      SERCOM1_REGS->USART.SERCOM_CTRLA |= SERCOM_USART_CTRLA_MODE_INTCLK;
      
      // connect TX with RX >> Loop Back Mode
      // TODO: Check this
      SERCOM1_REGS->USART.SERCOM_CTRLA |= SERCOM_USART_CTRLA_TXPO(0) | SERCOM_USART_CTRLA_RXPO(1) | SERCOM_USART_CTRLA_DORD(1); 
      
      // SERCOM(instanceNum) CTRLB settings
      // Enable TX and RX
      SERCOM1_REGS->USART.SERCOM_CTRLB |= SERCOM_USART_CTRLB_TXEN_Msk| SERCOM_USART_CTRLB_RXEN_Msk;
      
      //Wait for synchronization
      while (SERCOM1_REGS->USART.SERCOM_SYNCBUSY & SERCOM_USART_SYNCBUSY_CTRLB_Msk);
      
      // Setup Baud Rate
      SERCOM1_REGS->USART.SERCOM_BAUD |= compute_baud(BAUD_RATE,FPGA_HZ/2,0);
      
      // Enable SERCOM before configuration
      SERCOM1_REGS->USART.SERCOM_CTRLA |= SERCOM_USART_CTRLA_ENABLE(1);
      //Wait for synchronization
      while (SERCOM1_REGS->USART.SERCOM_SYNCBUSY & SERCOM_USART_SYNCBUSY_ENABLE_Msk);
    //}
  }
  
  void print_char_through_sercom_uart (char c, uint32_t instanceNum)
  {
    SERCOM1_REGS->USART.SERCOM_DATA = c;
    //while ((SERCOM1_REGS->USART.SERCOM_INTFLAG & SERCOM_USART_INTFLAG_TXC_Msk) == 0);
  }
#endif  
  
  ///////////////////
  // Main Function //
  ///////////////////
  
  
  uint8_t secureFlash[4096*4] = {0};
  
  void sysinit()
{
    FCR_REGS->FCR_CTRLA 
            = FCR_CTRLA_FWS(9);
    
    /*XOSC*/
    OSCCTRL_REGS->OSCCTRL_XOSCCTRLA 
            = OSCCTRL_XOSCCTRLA_STARTUP(0xE) // 0.5sec start-up
            //| OSCCTRL_XOSCCTRLA_AGC_Msk  //THIS PREVENTED THE XOSCRDY FLAG FROM BEING SET.  TYPICALLY CAUSED THE XOSCFAIL TO OCCUR. 
            | OSCCTRL_XOSCCTRLA_XTALEN_Msk
            | OSCCTRL_XOSCCTRLA_ENABLE_Msk;
    while((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY_Msk) 
            != OSCCTRL_STATUS_XOSCRDY_Msk);
    
    /*XOSC32K*/
    OSC32KCTRL_REGS->OSC32KCTRL_XOSC32K 
            = OSC32KCTRL_XOSC32K_STARTUP(OSC32KCTRL_XOSC32K_STARTUP_32768CYCLES_Val)
            | OSC32KCTRL_XOSC32K_XTALEN_Msk
            | OSC32KCTRL_XOSC32K_ENABLE_Msk;
    while((OSC32KCTRL_REGS->OSC32KCTRL_STATUS & OSC32KCTRL_STATUS_XOSC32KRDY_Msk) 
            != OSC32KCTRL_STATUS_XOSC32KRDY_Msk);
    
    /*GCLK0*/
    GCLK_REGS->GCLK_GENCTRL[0] 
            = GCLK_GENCTRL_DIV(0) 
            | GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DFLL_Val) 
            | GCLK_GENCTRL_GENEN_Msk;
//            | GCLK_GENCTRL_OE_Msk;
    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL0_Msk) 
            == GCLK_SYNCBUSY_GENCTRL0_Msk);
    
    /*GCLK4*/
    GCLK_REGS->GCLK_GENCTRL[4] 
            = GCLK_GENCTRL_DIV(0) 
            | GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_XOSC32K_Val) 
            | GCLK_GENCTRL_GENEN_Msk
            | GCLK_GENCTRL_OE_Msk;
    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL4_Msk) 
            == GCLK_SYNCBUSY_GENCTRL4_Msk);    
    
    

}

  
  
  int main(void)
  {
      //volatile uint8_t stophere = 1;
      //while(stophere == 1);
      sysinit();
    // Variables to compute performance
    uint32_t  i;
    uint32_t  instanceNum = 1;    
    double    t_bist_delta = 0;
    double    t_get_challenge_delta = 0;  
    SCB_DisableDCache();

    
    
#ifdef __FPGA__
    /* Init stdio on SERCOM0 */
    /* Default speed is 115200 baud with freq at FPGA_HZ Mhz */
    //stdio_serial_init(BAUD_RATE);
    
    //__LOCAL_SERCOM_Initialize(instanceNum);    
    
#ifdef __ICCARM__    
    // Enable global IRQ for CPU
    cpu_irq_enable();
#endif  
#else
    // Enable PA11-PA03 pins as ouptputs for displaying HSM M0 printf messages
    //PORT->Group[0].DIR.reg = 0x00000FF8;
#endif
    
    // Enable HSM Clock
    SCB_DisableDCache();
    HSM_REGS->HSM_CTRLA  = HSM_CTRLA_ENABLE_Msk;
    
    NVIC_ClearPendingIRQ(HSM_RXINT_IRQn);
    NVIC_SetPriority(HSM_RXINT_IRQn, 0x1);
    NVIC_EnableIRQ(HSM_RXINT_IRQn);

    
//#ifdef __FPGA__
    // Configure SysTick
    if (SysTick_Config(NBCYCLESPERTICK)) {
      printf("Failed to Configure the Core Timer\r\n");
    }
    
    // Setup Systick Interrupt
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    NVIC_SetPriority(SysTick_IRQn, 0x1);
    NVIC_EnableIRQ(SysTick_IRQn);
    
    /* Define the test suite */
    //uint32_t version = CHAMELEON->VERSION.reg;
    
//    //Initialize all the necessary clock services for actual silicon
//    printc("##########################################################\r\n");
//    printc("<TEST HEADER>");
//    printc("\r\n[FPGA_VERSION]%02X_%04X[FPGA_VERSION]\r\n",(int)((version>>16)&0xFF),(int)(version&0xFFFF));
//#ifdef __GNUC__
//    printc("[COMPILED]GCC " __DATE__ " " __TIME__"[COMPILED]\r\n\r\n");
//#elif defined(__ICCARM__)
//    printc("[COMPILED]IAR " __DATE__ " " __TIME__"[COMPILED]\r\n\r\n");
//#endif
//    printc("</TEST HEADER>\r\n");
//    printc("##########################################################\r\n");
//    
//#endif
//    
//    printc("\n");
//    printc("############################################\r\n");
//    printc("##### Program executing from Host FLASH#####\r\n");
//    printc("##### Starting HSM FIRMWARE Test Suite #####\r\n");
//    printc("############################################\n");
//    printc("Test Seed Number: ");
//#ifdef __FPGA__
//    printc("%d\n", TEST_SEED);
//#endif
//#ifdef NEG_TEST_EN
//    printc("NOTE: Negative test.\n");
//#endif
    
    //memcpy(secureFlash, 0x0a00c000, 4*4096);
    
    while (1)
    {
        // Clear the testFail counter variable
        testData.testFail = 0;
    
        // Check to make sure Boot ROM is ready to accept commands
        //printc("HSM ROM Test: Waiting for HSM FIRMWARE to be ready\r\n");

        uint8_t hsmFirmware[1024];
        uint8_t * hsmFirmwareLoc = (uint8_t *) 0x0c7df800;
        memcpy(hsmFirmware, hsmFirmwareLoc, sizeof(hsmFirmware));
        
        memcpy(hsmFirmware, hsmFirmwareLoc, sizeof(hsmFirmware));
        
        hsm_rom_test_loadFirmware(false);
        
        while((HSM_REGS->HSM_STATUS & HSM_STATUS_PS_Msk) != HSM_STATUS_PS_OPERATIONAL);

    #ifdef __FPGA__
        //printc("Test Start Time (x1000 ticks): ");
        //printc("%d\n", flag_systick);
    #endif

        //printc("---------------------------------------------\r\n");

        //Run the test sequence
        t_bist_delta = flag_systick * NBCYCLESPERTICK;
        //hsm_rom_test_bist(false);
        hsm_rom_test_hinit(false);
        //hsm_rom_test_hblock(false);
        //hsm_rom_test_inputdata(false);
        //hsm_rom_test_outputdata(false);
        //hsm_rom_test_getInfo(false);
        //hsm_rom_test_writeNvm(false);
        //hsm_rom_test_factoryReset(false);
        t_bist_delta = ((flag_systick * NBCYCLESPERTICK) - t_bist_delta) * CLKPER;

        //t_get_challenge_delta = flag_systick * NBCYCLESPERTICK;
        //hsm_rom_test_get_challenge(false);
        //t_get_challenge_delta = ((flag_systick * NBCYCLESPERTICK) - t_get_challenge_delta) * CLKPER;

    #ifdef __FPGA__
        printc("Test End Time (x1000 ticks): ");
        printc("%d\r\n", flag_systick);
    #endif

    #ifdef __FPGA__
        printc("CMD_BOOT_SELF_TEST command time in milliseconds at 150MHz: %f\r\n", t_bist_delta);
        //printc("CMD_SD_GET_CHALLENGE command time in milliseconds at 150MHz: %f\n", t_get_challenge_delta);        
    #endif
        if (testData.testFail != 0) {
#if 0
              printc("############################################\n");

          printc("FAILURE: Test FAILED, number of failures: ");
    #ifdef __FPGA__
          printc("0x%x\n", testData.testFail);
    #else
          printx(testData.testFail);
          printc("\n");
    #endif
          printc("\n   Test Seed Number: ");
    #ifdef __FPGA__
          printc("%d\n", TEST_SEED);
          printc("   Test Loop Number: %d\n", TEST_LOOP);
    #else
          printc(TEST_SEED);
          printc("\n");
    #endif
              printc("############################################\n");
#endif
          // End of test, reset fail counter
          testData.testFail = 0;

    #ifndef __FPGA__
          return(0x0BAD);
    #endif
        } else {
#if 0
          printc("\n#########################################\n");
          printc("SUCCESS: HSM ROM Suite Test PASSED\n");
          printc("\n   Test Seed Number: ");
    #ifdef __FPGA__
          printc("%d\n", TEST_SEED);
          printc("   Test Loop Number: %d\n", TEST_LOOP);
    #else
          printc(TEST_SEED);
          printc("\n");
    #endif
          printc("#########################################\n");
#endif
          // End of test, reset fail counter
          testData.testFail = 0;

    #ifndef __FPGA__
          return(0x0ACE);
    #endif
        }

//        printc("##########################################################\r\n");
    }
  }
  
  
