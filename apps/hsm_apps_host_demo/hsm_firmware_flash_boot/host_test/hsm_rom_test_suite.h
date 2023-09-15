/*
 * \brief hsm_rom_test_suite.h
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

#ifdef __FPGA__
#ifdef __XC32__
  #include <xc.h>
#else
  #include "compiler.h"
  #include "core_cm7.h"
  #include "parts.h"
#endif
  #include "stdio_serial.h"
  #include "string.h"
  #define printc printf
#else
  #include "stdio.h"
  //#include "cmsis/part.h"
  //#include "printk.h"
  //#include "tb_sync.h"
  //#include "osc.h" // osc_fsel_8M()
#endif

#include "hsm_rom_test_ctrl.h"
#include "hsm_rom_test_driver.h"

#define DSU_DAL_CPU1_MASK			0x0000000C
#define DSU_DAL_CPU1_SHFT			2

#ifdef CMD_SZ_MISMATCH
#ifdef CMD_SZ_MISMATCH_MORE
  #define CMD_BOOT_GET_SBC_MBHEADER             0x00200015
  #define CMD_TMPR_GET_NUMEVENTS_MBHEADER       0x00200019
  #define CMD_TMPR_GET_EVENT_MBHEADER           0x00200021
  #define CMD_MISC_GET_TS_MBHEADER              0x00200015
#ifdef TWO_FWMD_INPUTS
    #define CMD_BOOT_LOAD_FW_MBHEADER           0x00200023
#else
    #define CMD_BOOT_LOAD_FW_MBHEADER           0x0020001F
#endif
  #define CMD_SD_GET_CHALLENGE_MBHEADER         0x00200017
  #define CMD_SD_AUTHORIZATION_MBHEADER         0x00200019
  #define CMD_BOOT_SELF_TEST_MBHEADER           0x0020001E
#else
  #define CMD_BOOT_GET_SBC_MBHEADER             0x00200007
  #define CMD_TMPR_GET_NUMEVENTS_MBHEADER       0x00200005
  #define CMD_TMPR_GET_EVENT_MBHEADER           0x00200011
  #define CMD_MISC_GET_TS_MBHEADER              0x00200005
#ifdef TWO_FWMD_INPUTS
    #define CMD_BOOT_LOAD_FW_MBHEADER           0x00200013
#else
    #define CMD_BOOT_LOAD_FW_MBHEADER           0x00200004
#endif
  #define CMD_SD_GET_CHALLENGE_MBHEADER         0x0020000A
  #define CMD_SD_AUTHORIZATION_MBHEADER         0x00200012
  #define CMD_BOOT_SELF_TEST_MBHEADER           0x00200008
#endif
#else
  #define CMD_BOOT_GET_SBC_MBHEADER             0x00200010
  #define CMD_TMPR_GET_NUMEVENTS_MBHEADER       0x00200010
  #define CMD_TMPR_GET_EVENT_MBHEADER           0x00200014
  #define CMD_MISC_GET_TS_MBHEADER              0x00200010
#ifdef TWO_FWMD_INPUTS
    #define CMD_BOOT_LOAD_FW_MBHEADER           0x00200018
#else
    #define CMD_BOOT_LOAD_FW_MBHEADER           0x00200014
#endif
  #define CMD_SD_GET_CHALLENGE_MBHEADER         0x00200010
  #define CMD_SD_AUTHORIZATION_MBHEADER         0x00200014
  #define CMD_BOOT_SELF_TEST_MBHEADER           0x00200014
#endif

#define CMD_TMPR_GET_EVENT_MBHEADER_RX          0x0020000C
#define CMD_MISC_GET_TS_MBHEADER_RX             0x0020000C
#define CMD_BOOT_LOAD_FW_MBHEADER_RX            0x0020000C
#define CMD_SD_GET_CHALLENGE_MBHEADER_RX        0x0020000C
#define CMD_SD_AUTHORIZATION_MBHEADER_RX        0x0020000C
#define CMD_BOOT_SELF_TEST_MBHEADER_RX          0x0020000C

#define CMD_RESULT_SBC_DISABLED                 0xAA
#define CMD_RESULT_SBC_SIMULTANEOUS             0x01
#define CMD_RESULT_SBC_SEQUENTIAL               0x02

#define HSM_STATUS_PS_RESET                     0x00
#define HSM_STATUS_PS_BOOT                      0x10
#define HSM_STATUS_PS_OPERATIONAL               0x20
#define HSM_STATUS_PS_SAFEMODE                  0x30
#define HSM_STATUS_LCS_RESET                    0x0
#define HSM_STATUS_LCS_MANUFACTURING            0x100
#define HSM_STATUS_LCS_ERASED                   0x200
#define HSM_STATUS_LCS_OPEN                     0x300
#define HSM_STATUS_LCS_SECURED                  0x400
#define HSM_STATUS_SBS_RESET                    0x0
#define HSM_STATUS_SBS_DISABLED                 0x1000
#if 0
// DMA Descriptor
typedef struct {
    uint32_t                data;
    uint32_t                next;
    uint32_t                length;
} cmdSGDescriptor;
#endif
// Time-Stamp Output Data
typedef struct {
    unsigned long long      seconds;
    unsigned short          : 16;
    unsigned short          fractional:15;
    unsigned char           :1;
} cmdMiscGetTimestampOutput1;

// Debug Challenge Output Data
typedef struct {
    unsigned char           challenge[16];
} cmdSDGetChallengeOutput1;

// Tamper Event Output Data
typedef struct {
    unsigned short          reserved;
    unsigned short          fractional;
    unsigned long           timestampLS;
    unsigned int            timestampMS;
    char                    tamperLog[20];
} tamperLog;

// Secure Debug Authorization Input Data Structure
typedef struct {
    uint32_t                dalLevels;
    unsigned char           signature[96];
} cmdSDAuthDataInput1_0;

// Secure Debug Authorization Input Data Structure
typedef struct {
    uint32_t                dalLevels;
    unsigned char           uniqueId[16];
    unsigned char           signature[96];
} cmdSDAuthDataInput1_1;

// Secure Debug Authorization Input Data Structure
typedef struct {
    uint32_t                dalLevels;
    unsigned char           challenge[16];
    unsigned char           signature[96];
} cmdSDAuthDataInput1_2;

// Secure Debug Authorization Input Data Structure
typedef struct {
    uint32_t                dalLevels;
    unsigned char           uniqueId[16];
    unsigned char           challenge[16];
    unsigned char           signature[96];
} cmdSDAuthDataInput1_3;

// Test Data Struct
typedef struct {
  uint32_t                  testFail;                 // Variable to keep track of test errors
  bool                      rsp_chks_passed;          // Variable to keep track of whether command response completed 
  hsmRomTestCmdReq          hsm_rom_test_cmd_req;     // Command Request for sending to HSM ROM Test Driver
  bool                      hsm_rx_int;               // Boolean variable to indicate when the interrupt has triggered
} TEST_DATA;

// Create a global variable for the challenge value from get challenge, so it can be used by the authorization command
unsigned char sd_auth_challenge[16];

// Create a global Test Data variable so it can be accessed the interrupt handler when (int_mode == true)
TEST_DATA testData;

// Create a global command response variable so it can be accessed by the interrupt handler when (int_mode == true)
extern hsmRomTestCmdResp    hsm_rom_test_cmd_resp;

// Create a global variable to store the last challenge value
cmdSDGetChallengeOutput1    last_challenge;

