/*
 * \brief hsm_test_driver.c
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

#include "hsm_rom_test_driver.h"
#include <xc.h>

// Create a global command response variable so it can be used by the interrupt handler when (int_mode == true)
hsmRomTestCmdResp           hsm_rom_test_cmd_resp;

// Function to process the command response
void hsm_rom_test_cmd_rsp_prcsr(void) {

    uint32_t                mbrxstatus;
    uint16_t                cmd_size;
    uint8_t                 i;

    // Check for response recieved by reading RXINT
    //printc("HSM ROM Test Driver: Waiting for the command response (RXINT)...\n");
    mbrxstatus = HSM_REGS->HSM_MBRXSTATUS;

    // Pole RXINT in non-interrupt mode (in interrupt mode this 'while' acts as an 'if')
    while((mbrxstatus & MBRXSTATUS_RXINT_MASK) != MBRXSTATUS_RXINT_MASK) {
        mbrxstatus = HSM_REGS->HSM_MBRXSTATUS; 
    }
    
    // Check Mailbox Header
    hsm_rom_test_cmd_resp.mbHeader = HSM_REGS->HSM_MBRXHEAD;
    
    // Extract the command length
    cmd_size = (uint16_t) ((hsm_rom_test_cmd_resp.mbHeader & MBRXHEADER_LEN_MASK) / 4);
    
    // Check Mailbox Command Header
    hsm_rom_test_cmd_resp.cmdHeader = HSM_REGS->HSM_MBFIFO[0];
    
    // Check Response Code
    hsm_rom_test_cmd_resp.resultCode = HSM_REGS->HSM_MBFIFO[0];
    
    // Read the Command Result Data       
    for (i = 0; i < cmd_size - 3; i++) {
        hsm_rom_test_cmd_resp.resultData[i] = HSM_REGS->HSM_MBFIFO[0];
    }
}

// HSM ROM Test Driver Function
void hsm_rom_test_driver(hsmRomTestCmdReq * cmd_req, bool int_mode) {

    uint8_t                 i;
    uint16_t                cmd_size;

    // Enable HSM Mailbox RX interrupt in int_mode
    if (int_mode) {
        HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(1);
    } else {
        HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(0);
    }

    // Extract the command length
    cmd_size = (uint16_t) ((cmd_req->mbHeader & MBRXHEADER_LEN_MASK) / 4);

    // Write the Mailbox Header
    HSM_REGS->HSM_MBTXHEAD  = (uint32_t) cmd_req->mbHeader;

    // Write the Command Header
    HSM_REGS->HSM_MBFIFO[0] = (uint32_t) cmd_req->cmdHeader;

    // Write the rest of the command inputs
    for (i = 0; i < cmd_size - 2; i++) {
        HSM_REGS->HSM_MBFIFO[0] = (uint32_t) cmd_req->cmdInputs[i];
    }
    
    // Check if interrupt mode is enabled 
    if (int_mode) {
        // Create a null structure to return in interrupt mode
        hsm_rom_test_cmd_resp.mbHeader      = 0;
        hsm_rom_test_cmd_resp.cmdHeader     = 0;
        hsm_rom_test_cmd_resp.resultCode    = 0;
        hsm_rom_test_cmd_resp.resultData[0] = 0;
    } else {
        // Process the command response 
        hsm_rom_test_cmd_rsp_prcsr();
    }
}