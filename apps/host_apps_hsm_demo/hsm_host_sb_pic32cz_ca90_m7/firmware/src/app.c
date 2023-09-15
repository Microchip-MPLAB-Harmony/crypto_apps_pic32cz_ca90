/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xc.h>

#include "app.h"
#include "hsm_test_suite.h"

#include "boot.h"

char * sepStr = 
    "\r\n---------------------------------------------------------\r\n";


#define NBCYCLESPERTICK 1000
// Clock period in milliseconds
//#define CLKPER 0.000041667 // 24MHz
#define CLKPER 0.000006667 // 150MHz

#define BAUD_RATE 115200

#if 0
#pragma FUSES_USERCFG1_FUCFG9_RAM_ECC_EN0 = SET
#pragma FUSES_USERCFG1_FUCFG9_ECC_RAM_INIT_ENB = SET
#pragma FUSES_USERCFG2_FUCFG9_RAM_ECC_EN0 = SET
#pragma FUSES_USERCFG2_FUCFG9_ECC_RAM_INIT_ENB = SET
#endif //0


//#include "app_button.h"
//void _APP_LED_Tasks(void);

//#ifndef min
//#define min(a, b) (((a) < (b)) ? (a) : (b))
//#endif // min

// *****************************************************************************;

// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
APP_DATA appData;
uint32_t flag_systick = 0;

#define HID_REPORT_PACKET_SIZE_BYTES 64

//Kit Protocol Buffers
/* Receive data buffer */
uint8_t receiveDataBuffer[HID_REPORT_PACKET_SIZE_BYTES] CACHE_ALIGN;

/* Transmit data buffer */
uint8_t transmitDataBuffer[HID_REPORT_PACKET_SIZE_BYTES] CACHE_ALIGN;

size_t usb_report_length = 0;
size_t current_response_location = 0;

int errNum = 0; 

static bool bootFailed = false;


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

void APP_TimerCallback(uintptr_t context);

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************


//******************************************************************************
//******************************************************************************
void APP_Initialize()
{
    char __attribute__((unused)) *initMessage = 
    "\r\nApplication created " __DATE__ " " __TIME__ " initialized!\r\n";
    //SYS_CONSOLE_Write(SYS_CONSOLE_INDEX_0, STDOUT_FILENO, initMessage, 
    //        strlen(initMessage));
    //SYS_CONSOLE_Write(SYS_CONSOLE_INDEX_0, initMessage, 
    //        strlen(initMessage));
    SYS_MESSAGE(initMessage); 

    //Disable Data Cache
    //SYS_MESSAGE("Disable Cache...");
    //SCB_DisableDCache();
    //SYS_MESSAGE("Cache Disabled\r\n");
#ifdef SECURE_BOOT
    SYS_MESSAGE("BOOT from FLASH)\r\n");
#else
    SYS_MESSAGE("DUAL SESSION DEBUG BOOT\r\n");
#endif //SECURE_BOOT

    SYS_MESSAGE("HSM TEST SUITE Commands\r\n");

#ifdef __ICCARM__
    //Enable global IRQ for CPU
    //cpu_irq_enable();

    // Enable PA11-PA03 pins as ouptputs for displaying HSM M0 printf messages
    //PORT->Group[0].DIR.reg = 0x00000FF8;
#endif //__ICCARM__

    //HSM Enable
    //--HSM CTRLA Reg:
    //    RUNSTDBY[6]: HSM main clock remains enabled in standby mode.
    //    PRIV(2)    : Host Interface registers only accessible in privileged mode.
    //    ENABLE(1)   : HSM main clock Enable bit (1)
    HSM_REGS->HSM_CTRLA = HSM_CTRLA_ENABLE_Msk;
    SYS_MESSAGE("HSM Enabled\r\n");
    
    //RX Interrupts
    NVIC_ClearPendingIRQ(HSM_RXINT_IRQn);
    NVIC_SetPriority(HSM_RXINT_IRQn, 0x1);
    NVIC_EnableIRQ(HSM_RXINT_IRQn);
    
    /* Place the App state machine in its initial state. */
    appData.currentState = APP_STATE_INIT;

    appData.appInitialized = true;

} //End APP_Initialize()


//*****************************************************************************
//  Function:
//    void APP_Tasks ( void )
//
//  Description:
//      1) Initialize UART Driver
//      2) Send message[0]
//      3) Receive Command String/Send Received command/Print message[21]]
//      4) Execute HSM Command String/Print message[32]
//
//  Remarks:
//    See prototype in app.h.
//
//*****************************************************************************
void APP_Tasks ( void )
{
    //_APP_Button_Tasks();
    //_APP_LED_Tasks();

    /* Check the application's current state. */
    switch ( appData.currentState )
    {
        //====================================================================== 
        // Application's initial state.
        //====================================================================== 
        case APP_STATE_INIT:
        {
                appData.currentState = APP_STATE_INIT_HSM;
        }
        break;

        //====================================================================== 
        //====================================================================== 
        case APP_STATE_INIT_HSM:
        {
            //Initial Status
            int __attribute__((unused)) hsmStatus = HSM_REGS->HSM_STATUS; 
            GetHsmStatus(&busy,&ecode,&sbs,&lcs,&ps);
            SYS_PRINT("Initial HSM Status: 0x%08x\r\n",hsmStatus);
            SYS_PRINT("    %s  ECODE: %s\r\n    SBS: %s  LCS: %s  PS: %s\r\n",
                      busy?"BUSY":"NOT busy", 
                      ecodeStr[ecode], sbsStr[sbs], lcsStr[sbs], psStr[ps]);


#if defined(SECURE_BOOT)
            uint8_t   hsmFirmware[1024];
            uint32_t * fwP = (uint32_t *)hsmFirmware;

            //HSM Firmware image is loaded via .hex project to flash
            uint8_t * hsmFirmwareLoc     = (uint8_t *) HSM_FIRMWARE_INIT_ADDR; //0x0c7df800;

            // Check to make sure Boot ROM is ready to accept commands
            SYS_MESSAGE("\r\nHSM FLASH BOOT: Waiting for HSM FIRMWARE to be ready\r\n");
            memcpy(hsmFirmware, hsmFirmwareLoc, sizeof(hsmFirmware));
            SYS_PRINT("HSM Firmware Metadata:  0x%08lx %08lx %08lx %08lx...\r\n",
                *fwP, *(fwP+1), *(fwP+2), *(fwP+3));
            if (*fwP == 0xFFFFFFFF) 
            {
                SYS_PRINT("HSM Firmware Not Loaded - run host_firmware_boot\r\n");
            }

            hsmFirmwareLoc = (uint8_t *) HSM_FIRMWARE_ADDR; //0x0c7e0000;
            memcpy(hsmFirmware, hsmFirmwareLoc, sizeof(hsmFirmware));
            SYS_PRINT("         HSM Firmware:  0x%08lx %08lx %08lx %08lx...\r\n",
                *fwP, *(fwP+1), *(fwP+2), *(fwP+3));
            
            SYS_MESSAGE ("HSM Load Firmware\r\n");
            HsmCmdBootLoadFirmware();
#endif //SECURE_BOOT

            SYS_MESSAGE("--Waiting for HSM to become OPERATIONAL\r\n...\r\n");

            appData.currentState = APP_STATE_IDLE;   //Wait for HSM Status
            LED0_Off();
            LED1_Off();

        } //End case APP_STATE_INIT_HSM
        break;
        
        //====================================================================== 
        //Timer Initialization State
        // NOTE:  NOT USED
        //====================================================================== 
        case APP_STATE_TIMER_INIT:
        {
#if 1
            //TODO:  NOT WORKING!!!
            appData.tmrHandle = SYS_TIME_CallbackRegisterMS(APP_TimerCallback, 
                    (uintptr_t)0, 1/*ms*/, SYS_TIME_PERIODIC);

            if ( SYS_TIME_HANDLE_INVALID != appData.tmrHandle )
            {
                //appData.currentState = APP_STATE_TRANSMIT_MESSAGE;
                //gRcvCmdRdy           = false;
                //gRcvCnt              = 0;
                //appData.nextState    = APP_STATE_RECEIVE_CMD;
                appData.currentState = APP_STATE_IDLE;
                SYS_MESSAGE("Timer Initialized\r\n");
            }
            else
            {
                errNum = 2;
                appData.currentState = APP_STATE_ERROR;
            }
#endif //0
            appData.currentState = APP_STATE_IDLE;
        }
        break;

        //====================================================================== 
        // Wait for HSM Ready State
        // --Secure Boot Process or wait for debug load of HSM
        //====================================================================== 
        case APP_STATE_IDLE:
        {
            //Wait for HSM Ready and Operational Mode
            //while((HSM_REGS->HSM_STATUS & HSM_STATUS_PS_Msk) != HSM_STATUS_PS_OPERATIONAL);
            int __attribute__((unused)) hsmStatus = HSM_REGS->HSM_STATUS; 
            GetHsmStatus(&busy,&ecode,&sbs,&lcs,&ps);
            if (busy == false && ps == HSM_PS_OPERATIONAL)
            {
                hsmStatus = HSM_REGS->HSM_STATUS; 
                SYS_PRINT("HSM Status: 0x%08x\r\n",hsmStatus);
                SYS_PRINT( "    %s  ECODE: %s\r\n    SBS: %s  LCS: %s  PS: %s\r\n",
                    busy?"BUSY":"NOT busy", 
                    ecodeStr[ecode], sbsStr[sbs], lcsStr[sbs], psStr[ps]);

#ifdef SECURE_BOOT
                //HSM Init (CMD_BOOT_SELF_TEST)
                SYS_MESSAGE(sepStr);
                SYS_MESSAGE ("HSM (CMD_BOOT_SELF_TEST)...\r\n");

                //Validate the flash image 
                RSP_DATA * rsp = 0;
                bootFailed = false;
                rsp = HsmCmdBootTestHashInit();
                if (rsp->resultCode != S_OK)
                {
                    char * str;
                    str = CmdResultCodeStr(rsp->resultCode);
                    SYS_PRINT("HSM ROM Test FAIL: CMD_BOOT_SELF_TEST - %s\r\n",
                              str);
                    bootFailed = true;
                }
                SYS_MESSAGE("HSM ROM Test: CMD_BOOT_SELF_TEST Successful.\r\n");
#else
                bootFailed = false;
#endif //SECURE_BOOT

                appData.currentState = APP_STATE_HSM_COMMAND;
            }
            else
            {
                appData.currentState = APP_STATE_IDLE;
            }

        } //End case APP_STATE_IDLE:
        break;

        //====================================================================== 
        // Execute the HSM Command String
        // --Get HSM command response
        // --Signal that the command has completed
        //====================================================================== 
        case APP_STATE_HSM_COMMAND:
        {
            int __attribute__((unused)) retVal = 0;
            int __attribute__((unused)) numDataBytes = 0;


            SYS_MESSAGE("\r\nRunning HSM MB Command Test Suite\r\n");
#define VSSLOT  15 //NOTE: Slot Number to use. 0 and 255 are reserved
#define AESSLOT -1
            int __attribute__((unused)) hsmStatus = HSM_REGS->HSM_STATUS; 
            GetHsmStatus(&busy,&ecode,&sbs,&lcs,&ps);
            SYS_PRINT("PreTEST HSM Status: 0x%08x\r\n",hsmStatus);
            SYS_PRINT( "    %s  ECODE: %s\r\n    SBS: %s  LCS: %s  PS: %s\r\n",
                busy?"BUSY":"NOT busy", 
                ecodeStr[ecode], sbsStr[sbs], lcsStr[lcs], psStr[ps]);
            SYS_MESSAGE("\r\n");

            hsm_test_suite(VSSLOT, AESSLOT);

            SYS_MESSAGE("\r\n");
            hsmStatus = HSM_REGS->HSM_STATUS; 
            GetHsmStatus(&busy,&ecode,&sbs,&lcs,&ps);
            SYS_PRINT("PostTEST HSM Status: 0x%08x\r\n",hsmStatus);
            SYS_PRINT( "    %s  ECODE: %s\r\n    SBS: %s  LCS: %s  PS: %s\r\n",
                busy?"BUSY":"NOT busy", 
                ecodeStr[ecode], sbsStr[sbs], lcsStr[sbs], psStr[ps]);

            SYS_MESSAGE("\r\n**** COMPLETED HSM MB TEST Suite ****");
            appData.currentState    = APP_STATE_STOP;
        }
        break;

        //====================================================================== 
        // End HSM Command Testing
        //====================================================================== 
        case APP_STATE_STOP:
        {
            //TODO:  Button to run the test again.
            LED0_On();
            LED1_On();
            appData.currentState = APP_STATE_STOP;
        }
        break;

        //====================================================================== 
        // APP Error
        //====================================================================== 
        case APP_STATE_ERROR:
        {
            LED0_On();
            LED1_On();
            appData.currentState = APP_STATE_ERROR;
        }
        break;

        //====================================================================== 
        //====================================================================== 
        default:
            break;
    } //

} //end APP_Tasks()


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

#if 1
// ****************************************************************************
// APP_TimerCallback()
// --Callback for the periodic timer interrupt
//******************************************************************************
void APP_TimerCallback( uintptr_t context)
{
    //Button Delay
    //if (button.buttonDelay > 0)
    //{      
    //    button.buttonDelay--;
    //}

    asm("nop");

    //LED Blink Delay
    //if (appData.blinkDelay > 0)
    //{
    //    appData.blinkDelay--;
    //}
} 
#endif //0

/*******************************************************************************
 End of File
 */
