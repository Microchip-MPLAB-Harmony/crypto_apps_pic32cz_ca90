/*
 * \brief hsm_test_driver.h
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
#ifndef _DRV_HSM_H
#define _DRV_HSM_H

#ifdef __FPGA__
    #define printc printf
    #include "stdbool.h"
#ifdef __XC32__
    #include <xc.h>
    #include "stdio.h" 
#else
    #include "compiler.h"
#endif
#else
//    #include "cmsis/part.h"
    #include "stdbool.h"
//    #include "printk.h"
    #include "stddef.h"
#endif

#include <stdint.h>

// Why isn't this in part.h
#define MBRXSTATUS_RXINT_MASK     0x00100000
#define MBRXHEADER_LEN_MASK       0x0000FFFF

// HSM ROM Test Command Result Codes
typedef enum _HSM_ROM_TEST_CMD_RESULT_CODE {
    E_NULL                      = 0xFFFFFFFF,
    E_UNKNOWN                   = 0x00000000,           // Unspecified Failure
    S_OK                        = 0x00000001,           // Commamd was Successful
    E_GENERAL                   = 0x80000000,           // Geneneal Unspecified Failure
    E_CMDCANCEL                 = 0x80000001,           // Command was Cancelled
    E_NOTSUPPORTED              = 0x80000002,           // Command not Supported in Current Mode
    E_INVPARAM                  = 0x80000003,           // Invalid Parameter
    E_INVINPUT                  = 0x80000004,           // Invalid Input
    E_INPUTDMA                  = 0x80000005,           // DMA Input Error
    E_OUTPUTDMA                 = 0x80000006,           // DMA Output Error
    E_SYSTEMMEM                 = 0x80000007,           // Error Accessing System Memory
    E_INPUTAUTH                 = 0x80000008,           // Error Authenticating Input
    E_HSMFWAUTH                 = 0x80000009,           // Error Authenticating HSM Firmware
    E_HOSTFWAUTH                = 0x8000000A,           // Error Authenticating Host Firmware
    E_SAFEMODE                  = 0x8000000B,           // Acritical Error has Occurred and HSM is Entering Safe Mode
    E_SELFTEST                  = 0x8000000C,           // Error Occurred during Self-test
    E_TIMELOCKED                = 0x8000000D,           //
    E_TIMEUNLOCKED              = 0x8000000E,
    E_TMPRLOCKED                = 0x8000000F,
    E_TMPRUNLOCKED              = 0x80000010,
    E_VSINUSE                   = 0x80000011,           // Variable Slot in USE Error
    E_VSEMPTY                   = 0x80000012,           // Variable Slot Empty Error
    E_NOHSMFW                   = 0x80000013,           // No HSM FW was provided
    E_INVVS                     = 0x80000014,           // Invalid Variable Slot
    E_HASHFAILED                = 0x80000015,           // Secure Flash Page Hash Error
    E_FWMDAUTH                  = 0x80000016,           // Firmware Metadata Authentication Failed
    E_UNKNOWNSBC                = 0x80000017,           // If SBC is not yet read
    E_CRYPTOHW                  = 0x80000018,
    E_OUTPUTNOTSET              = 0x80000019,
    E_INPUTNOTSET               = 0x8000001A,
    E_OUTPUTSET                 = 0x8000001B,
    E_INPUTSET                  = 0x8000001C,
    E_AUTHBITSET                = 0x8000001D,
    E_AUTHBITNOTSET             = 0x8000001E,
    E_SLOTPARASET               = 0x8000001F,
    E_SLOTPARANOTSET            = 0x80000020,
    E_INVFORMAT                 = 0x80000021,
    E_OUTPUTTOOSMALL            = 0x80000022
} cmdResultCodes;

// HSM ROM Test Command Header Types
typedef enum _HSM_ROM_TEST_CMD_HDR {
    CMD_NULL                    = 0xFFFFFFFF,
    CMD_BOOT_GET_SBC            = 0x00000000,
    CMD_BOOT_LOAD_FIRMWARE      = 0x00000100,
    CMD_BOOT_SELF_TEST          = 0x00000400,
    CMD_SD_GET_CHALLENGE        = 0x00000001,
    CMD_SD_AUTHORIZATION        = 0x00000101,
    CMD_TMPR_GET_NUM_TMPREVENTS = 0x00000602,
    CMD_TMPR_GET_TMPREVENT      = 0x00000702,
    CMD_MISC_GET_TS             = 0x000000F0
} hsmRomTestCmdHeader;

// HSM ROM Test Command Request
typedef struct {
    uint32_t                    mbHeader;
    hsmRomTestCmdHeader         cmdHeader;
    uint32_t                    cmdInputs[4];
    uint32_t                    expMbHeader;
    cmdResultCodes              expResultCode;
    uint32_t                    expStatus;
    uint32_t                    expIntFlag;
} hsmRomTestCmdReq;

// HSM ROM Test Command Response 
typedef struct {
    uint32_t                    mbHeader;
    hsmRomTestCmdHeader         cmdHeader;
    cmdResultCodes              resultCode;
    uint32_t                    resultData[1];
} hsmRomTestCmdResp;

// Function Prototypes
void hsm_rom_test_cmd_rsp_prcsr(void);
void hsm_rom_test_driver(hsmRomTestCmdReq * cmd_req, bool int_mode);

#endif
