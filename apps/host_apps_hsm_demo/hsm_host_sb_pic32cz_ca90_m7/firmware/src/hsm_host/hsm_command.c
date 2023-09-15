#include <stdint.h>
#include <string.h>
#include <math.h>

#include "pic32cz8110ca90208.h"
#include "core_cm7.h"
//#include "app.h"
#include "hsm_command.h"
#include "hsm_command_globals.h"
#include "vsm.h"
#include "hash.h"

//******************************************************************************
//******************************************************************************
// Definitions/Macros
//******************************************************************************
//******************************************************************************

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif // min

//******************************************************************************
//******************************************************************************
// Globals/Statics
//******************************************************************************
//******************************************************************************

bool trust                                  = false;
bool slot                                   = false;
bool auth                                   = false;
bool prot                                   = false;

int ctr                                     = 0;
int numParamWords                           = 0;
int numDataWords                            = 0;
int dataLengthBytes                         = 0;

int retVal                                  = 0;

uint32_t dummy32;
char tempStr[9]                             = "00000000";

uint32_t mailBoxHeaderRx                    = 0;
uint32_t cmdHeaderResponseRx                = 0;
uint32_t cmdResultRx                        = 0;
char *   cmdRCStr                           = 0;

uint32_t  __attribute__((unused)) hsmStatus = 0;
int  hsmCmdLength                           = 0;

int  responseDelay                          = 100000;
int  temp;


//******************************************************************************
//******************************************************************************
// Functions
//******************************************************************************
//******************************************************************************


//******************************************************************************
// int SendHsmCommandRequest(void)
// 
// Send HSM Command Words through CSM Mailbox

//     HSM HOST Registers:
//     HSM_CTRLA       : 0x00  (R/W  32) CONTROL REGISTER A 
//         7:0   — RUNSTDBY — — — PRIV ENABLE —
//     HSM_CTRLB       : 0x04  (R/W  32) CONTROL REGISTER B 
//         7:0   — — — — — — — CANCEL
//     HSM_INTENCLR    : 0x08  (R/W  32) INTERRUPT ENABLE CLEAR REGISTER 
//         7:0   — — — — — — TAMPER ERROR
//     HSM_INTENSET    : 0x0C  (R/W  32) INTERRUPT ENABLE SET 
//         7:0   — — — — — — TAMPER ERROR
//     HSM_INTFLAG     : 0x10  (R/W  32) INTERRUPT FLAG 
//         7:0   — — — — — — TAMPER ERROR
//     HSM_STATUS      : 0x14  (R/W  32) STATUS 
//         23:16 —  — — —     ECODE(4)
//         15:8  —  SBS(3) —    LCS(3)
//         7:0   —  PS(3)  — — —  BUSY
//     HSM_MBFIFO[16]  : 0x100 (R/W  32) MAILBOX FIFO 
//         31-0 FIFO(32)
//     HSM_MBTXSTATUS  : 0x140 (R/W  32) MAILBOX TRANSMIT STATUS 
//         23:16 TXERR — TXFULL TXINT TXTYPE(4)
//         15:0  TXREMAINING(8)
//     HSM_MBRXSTATUS  : 0x144 (R/W  32) MAILBOX RECEIVE STATUS 
//         23:16 RXERR RXHEAD RXEMPTY RXINT RXTYPE(4) 
//         15:0  RXREMAINING(8)
//     HSM_MBTXPROT    : 0x148 (R/W  32) MAILBOX TX PROTECTION 
//         31:24 USER(8)
//         23:16 NSEC PRIV UNPROT - - - - -
//     HSM_MBRXPROT    : 0x14C (R/W  32) MAILBOX RX PROTECTION 
//         31:24 USER(8)
//         23:16 NSEC PRIV UNPROT - - - - -
//     HSM_MBTXHEAD    : 0x150 (R/W  32) MAILBOX TRANSMIT HEADER 
//         31-0  TXHEAD(32)
//     HSM_MBRXHEAD    : 0x154 (R/W  32) MAILBOX RECEIVE HEADER 
//         31-0  RXHEAD(32)
//     HSM_MBCONFIG    : 0x158 (R/W  32) MAILBOX CONFIGURATION 
//         7:0   — — — — — — RXINT TXINT
//
//******************************************************************************
int SendHsmCommandRequest()
{
    //HSM MB Status Reg:
    uint32_t mbrxstatus;
    uint32_t mbtxstatus;

    mbrxstatus = HSM_REGS->HSM_MBRXSTATUS;
    SYS_PRINT("T0 MBRXSTATUS = 0x%08lx\r\n", mbrxstatus);
    mbtxstatus = HSM_REGS->HSM_MBTXSTATUS;
    SYS_PRINT("T0 MBTXSTATUS = 0x%08lx\r\n", mbtxstatus);

    //Disable RX Interrupt
    HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(0);

    hsmStatus = HSM_REGS->HSM_STATUS; 
    GetHsmStatus(&busy, &ecode, &sbs, &lcs, &ps);
    SYS_PRINT("T0 HSM Status: 0x%08lx\r\n", hsmStatus);
    SYS_PRINT( "    %s  ECODE: %s\r\n    SBS: %s  LCS: %s  PS: %s\r\n",
        busy?"BUSY":"NOT busy",ecodeStr[ecode], sbsStr[sbs], lcsStr[sbs], psStr[ps]);

    if (busy)
    {
        cmdRCStr = CmdResultCodeStr(HSMBUSYERR);
        return HSMBUSYERR; 
    }

    //Boot the HSM
    //while(((hsmStatus & HSM_STATUS_PS_BOOT) == HSM_STATUS_PS_BOOT))
    //{
        //Print08X(hsmStatus);
    //    hsmStatus = HSM_REGS->HSM_STATUS;
        //;//wait while hsm boots
    //}
    //Print08X(hsmStatus);
    
    //Check the Status
    mbrxstatus = HSM_REGS->HSM_MBRXSTATUS;
    SYS_PRINT("T1 MBRXSTATUS = 0x%08lx\r\n", mbrxstatus);
    mbtxstatus = HSM_REGS->HSM_MBTXSTATUS;
    SYS_PRINT("T1 MBTXSTATUS = 0x%08lx\r\n", mbtxstatus);

    //Clear the Data CACHE
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorIn, sizeof(dmaDescriptorIn));
    SCB_CleanDCache_by_Addr((uint32_t *) dmaDescriptorOut, sizeof(dmaDescriptorOut));
    SCB_CleanInvalidateDCache_by_Addr(dmaDataIn, sizeof(dmaDataIn));
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dmaDataOut, sizeof(dmaDataOut));

    //Send the HSM Command Words
    HSM_REGS->HSM_MBTXHEAD = globalMbHeader; 
    hsmCmdLength = globalMbHeader & 0xff;
    SYS_PRINT("  TX  MB: 0x%08lx\r\n", globalMbHeader);

    HSM_REGS->HSM_MBFIFO[0] = globalCmdHeader;
    SYS_PRINT("  TX CMD: 0x%08lx\r\n", globalCmdHeader);

    HSM_REGS->HSM_MBFIFO[0] = globalCmdInput;
    SYS_PRINT("  TX  IN: 0x%08lx\r\n", globalCmdInput);

    HSM_REGS->HSM_MBFIFO[0] = globalCmdOutput;
    SYS_PRINT("  TX OUT: 0x%08lx\r\n", globalCmdOutput);

    //Parameter words after the first 4 words 
    ctr=0;
    while(hsmCmdLength > 0x10)//support Parameter Words
    {
        HSM_REGS->HSM_MBFIFO[0] = globalCmdParams[ctr];
        SYS_PRINT("  TX PARAM%d: 0x%08lx\r\n", ctr, globalCmdParams[ctr]);
        hsmCmdLength -= 4;
        ctr++;
    }

    hsmStatus = HSM_REGS->HSM_STATUS; 
    SYS_PRINT("TC CMD HSM Status: 0x%08lx\r\n",hsmStatus);
    GetHsmStatus(&busy, &ecode, &sbs, &lcs, &ps);
    SYS_PRINT( "    %s  ECODE: %s\r\n    SBS: %s  LCS: %s  PS: %s\r\n",
        busy?"BUSY":"NOT busy", 
        ecodeStr[ecode], sbsStr[sbs], lcsStr[sbs], psStr[ps]);

    if (ps != 2)
    {
        cmdRCStr = CmdResultCodeStr(HSMNONOPERR);
        return HSMNONOPERR; 
    }
    SYS_PRINT("HSM Operational\r\n");

    //Wait to Complete Tx
    while(responseDelay)   // the busy polling isn't working so replace with this delay
    {
        if (!(HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk))
        {
            SYS_MESSAGE("HSM Send Complete(not busy)\r\n");
            break;
        }
        responseDelay--;
    }

    hsmStatus = HSM_REGS->HSM_STATUS; 
    SYS_PRINT("TF CMD HSM Status: 0x%08lx\r\n",hsmStatus);
    GetHsmStatus(&busy, &ecode, &sbs, &lcs, &ps);
    SYS_PRINT( "    %s  ECODE: %s\r\n    SBS: %s  LCS: %s  PS: %s\r\n",
        busy?"BUSY":"NOT busy", 
        ecodeStr[ecode], sbsStr[sbs], lcsStr[sbs], psStr[ps]);
    
    // Poll RXINT in non-interrupt mode (in interrupt mode this 'while' acts as an 'if')
    //while((mbrxstatus & MBRXSTATUS_RXINT_MASK) != MBRXSTATUS_RXINT_MASK) 
    //{
    //    mbrxstatus = HSM_REGS->HSM_MBRXSTATUS; 
    //} 

    mbrxstatus = HSM_REGS->HSM_MBRXSTATUS;
    SYS_PRINT("TF MBRXSTATUS = 0x%08lx\r\n",mbrxstatus);
    mbrxstatus = HSM_REGS->HSM_MBRXSTATUS;
    SYS_PRINT("TF MBTXSTATUS = 0x%08lx\r\n",mbtxstatus);
    
    //Read the Response
    mailBoxHeaderRx = HSM_REGS->HSM_MBRXHEAD;
    SYS_PRINT("  Rx  MB: 0x%08lx\r\n", mailBoxHeaderRx);

    cmdHeaderResponseRx = HSM_REGS->HSM_MBFIFO[0];
    SYS_PRINT("  Rx CMD: 0x%08lx\r\n", cmdHeaderResponseRx);

    cmdResultRx = HSM_REGS->HSM_MBFIFO[0];
    cmdRCStr = CmdResultCodeStr(cmdResultRx);
    SYS_PRINT("  Rx  RC: 0x%08lx %s\r\n", cmdResultRx, cmdRCStr);

#if 0
    //Get VSS Info (after the result code)
    //--Variable Slot
    if (cmdHeaderResponseRx == 0x00000503)
    {
        vssInfoRx = HSM_REGS->HSM_MBFIFO[0];
        SYS_PRINT("Rx VSS: 0x%08lx\r\n", vssInfoRx);
    }
#endif //0
    
    //RESULTS
    if (cmdHeaderResponseRx == CMD_HEADER_HASH_BLOCK_SHA256)
    {
        SYS_PRINT("HASH COMMAND 0x%08lx Result:\r\n", cmdHeaderResponseRx);
        for (ctr=0; ctr<HASH_SHA256_RESULT_BYTES/4; ctr+=4)
        {
            SYS_PRINT("    %08lx ",    dmaDataOut[ctr]);
            SYS_PRINT    ("%08lx ",    dmaDataOut[ctr+1]);
            SYS_PRINT("    %08lx ",    dmaDataOut[ctr+2]);
            SYS_PRINT("    %08lx\r\n", dmaDataOut[ctr+3]);
        }
        //TODO:  Send string to python for result validation
    }

    return cmdResultRx; 

} //End SendHsmCommandRequest()



//******************************************************************************
//Init DMA Input SG Descriptor
//******************************************************************************
void InitDmaForInput(uint32_t * buffer, int bufSize)
{
    //Initialize a single block DMA transfer descriptor
    dmaDescriptorIn[0].data.addr             = buffer;
    dmaDescriptorIn[0].next.s.stop           = 1;
    dmaDescriptorIn[0].next.s.addr           = 0x00000000;
    dmaDescriptorIn[0].flagsLength.s.length  = bufSize;         
    dmaDescriptorIn[0].flagsLength.s.cstAddr = 0;     //Constant Address (FIFO)
    dmaDescriptorIn[0].flagsLength.s.realign = 0;     //Realign to 32 bits
    dmaDescriptorIn[0].flagsLength.s.discard = 0;     //Do not Write:w
}


//******************************************************************************
//Init DMA Output SG Descriptor
//******************************************************************************
void InitDmaForOutput(uint32_t *buffer, int bufSize)
{
    //Initialize a single block DMA transfer descriptor
    dmaDescriptorOut[0].data.addr             = buffer;
    dmaDescriptorOut[0].next.s.stop           = 1;
    dmaDescriptorOut[0].next.s.addr           = 0x00000000;
    dmaDescriptorOut[0].flagsLength.s.length  = bufSize;         
    dmaDescriptorOut[0].flagsLength.s.cstAddr = 0;     //Constant Address (FIFO)
    dmaDescriptorOut[0].flagsLength.s.realign = 0;     //Realign to 32 bits
    dmaDescriptorOut[0].flagsLength.s.discard = 0;     //Do not Write:w
}


#if 0
//******************************************************************************
//Consolidated DMA Output
//******************************************************************************
void ConsolidateDmaForOutput(int* firstDescriptorAddress)
{
    int  outWordCounter    = 1; //counts the word destinations/strOffset to word array
    int  totalLength       = 0;
    int  currentLength     = 0;
    int* descriptorAddress = firstDescriptorAddress;
    int* dataBlockBase     = 0;
    int  lastStopBit       = 0;
    int  i                 = 0;
    //SYS_PRINT("****In ConsDmaForOutput first descriptor addr:...\r\n");
    //Print08X((unsigned int)firstDescriptorAddress);
    //SYS_PRINT("****In ConsDmaForOutput first descriptor val:...\r\n");
    //int descriptorAddressTgt = *firstDescriptorAddress;
    //Print08X((unsigned int)descriptorAddressTgt);
    
    while(lastStopBit == 0)//until STOP bit
    {
        lastStopBit = descriptorAddress[1] & 0x00000001;
        //process a descriptor whose stop bit is one, then stop looping
        currentLength = descriptorAddress[2] & 0x0fffffff;
        totalLength += currentLength;

        //Dereference first entry in the descriptor
        dataBlockBase = (int*)((int *)(descriptorAddress[0]));

        //DMA output is an memory not cache so ignore what's in cache for those addresses
        SCB_InvalidateDCache_by_Addr ((uint32_t *)dataBlockBase, currentLength);

        //DataBlockBase is assigned 0 on first pass
        for(i = 0; i < currentLength; i += 4)
        {
            //Over 4 to get word index
            globalOutputToSerial[outWordCounter] = dataBlockBase[i >> 2];

            //SYS_PRINT("****Stored to globalOutputToSerial:...\r\n");
            //Print08X(dataBlockBase[i >> 2]);
            outWordCounter++;
        }
        descriptorAddress = (int *)(descriptorAddress[1] & 0xfffffffe);
    } 

    globalOutputToSerial[0] = totalLength;

} //End ConsolidateDmaForOutput;
#endif //0


//******************************************************************************
// ResultCode Strings and HSM Error Strings
//******************************************************************************
char * CmdResultCodeStr(uint32_t cmdResultCode)
{
    int strIndex = 0;
    static char * strPtr;

    strPtr =  cmdResultStr[0];

        //0x0000 00XX
    if (cmdResultCode < NUMCMDRESULTCODES)  
    {
        strPtr = cmdResultStr[cmdResultCode];
        return strPtr; 
    }
    else if ((cmdResultCode&CMDSPECIALINDEXMASK) > 0)
    {  
        //0x8000 00XX
        strIndex = cmdResultCode & CMDSPECIALRESULTMASK; 
        if (strIndex < NUMCMDERRORRESULTCODES)
        {   
            strPtr = cmdSpecialResultStr[strIndex];
            return strPtr; 
        }
        else return cmdSpecialResultStr[NUMCMDERRORRESULTCODES]; //Invalid
    }
#if 0
    else if ((cmdResultCode & HSMCMDERRMASK) > 0)
    {
        strIndex = cmdResultCode & HSMCMDERRNUMMASK;
        if (strIndex < NUMHSMERRCODES)
        {
            return cmdHsmErrorStr[strIndex];
        }
        else return cmdHsmErrStr[NUMHSMERRCODES];
    }
#endif //0
    else return cmdSpecialResultStr[NUMCMDERRORRESULTCODES]; //Invalid
} //End CmdResultCodes()


//******************************************************************************
// GetHsmStatus()
// --Decode the HSM Status Register 
//******************************************************************************
void GetHsmStatus(bool *busy, 
                  HsmStatusECODE * ecode, 
                  HsmStatusSBS *sbs, 
                  HsmStatusLCS *lcs, 
                  HsmStatusPS *ps )            
{
    *busy = (bool) ((HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk) >> HSM_STATUS_BUSY_Pos);
    *ecode = HSM_STATUS_ECODE_VAL;  //Error Code
    *sbs   = HSM_STATUS_SBS_VAL;    //Secure Boot Status
    *lcs   = HSM_STATUS_LCS_VAL;    //Lifecycle Status
    *ps    = HSM_STATUS_PS_VAL;     //Processor Status
}


//******************************************************************************
// HSM ROM Test Driver Function
// --Send the HSM Command Request to HSM Mailbox (SendHsmCommandRequest())
//******************************************************************************
void HsmMbCmdDriver(HsmCmdReq * cmdReq, bool int_mode) 
{
    uint8_t                 i;
    uint16_t                cmd_size;

    // Enable HSM Mailbox RX interrupt in int_mode
    if (int_mode) 
    {
        HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(1);
    } 
    else 
    {
        HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(0);
    }

    // Extract the command length
        cmd_size = (uint16_t) ((cmdReq->mbHeader & MBRXHEADER_LEN_MASK) / 4);

    // Write the Mailbox Header
    HSM_REGS->HSM_MBTXHEAD  = (uint32_t) cmdReq->mbHeader;
    //SYS_PRINT("CMD MB: 0x%08lx\r\n",cmdReq->mbHeader);

    // Write the Command Header
    HSM_REGS->HSM_MBFIFO[0] = (uint32_t) cmdReq->cmdHeader;
    //SYS_PRINT("CMD RQ: 0x%08x\r\n",cmdReq->cmdHeader);

    // Write the rest of the command inputs
    for (i = 0; i < cmd_size - 2; i++) 
    {
        CmdSGDescriptor *dataDesc;

        HSM_REGS->HSM_MBFIFO[0] = (uint32_t) cmdReq->cmdInputs[i];
        if (i<2) //IN and OUT
        {
            //if (i==0) SYS_PRINT("CMD IN: 0x%08lx\r\n",cmdReq->cmdInputs[i]);
            //else      SYS_PRINT("CMD OUT: 0x%08lx\r\n",cmdReq->cmdInputs[i]);

            dataDesc = (CmdSGDescriptor *)cmdReq->cmdInputs[i];
            if (dataDesc != 0x00000000)
            {
                if (dataDesc->flagsLength.s.length < 65 && i!=1)
                {
                    //PrintSG(*dataDesc, true);
                }
                else
                {
                    //PrintSG(*dataDesc, false);
                }
            }
        }
        else
        {
            //SYS_PRINT("CMD  P%d: 0x%08lx\r\n",i-2, cmdReq->cmdInputs[i]);
        }
    } //End cmdInput print Loop
    
    // Check if interrupt mode is enabled 
    //if (int_mode) 
    //{ //    // Create a null structure to return in interrupt mode
    //    gHsmCmdResp.mbHeader.v      = 0;
    //    gHsmCmdResp.cmdHeader     = 0;
    //    gHsmCmdResp.resultCode    = 0;
    //    gHsmCmdResp.resultData[0] = 0;
    //} 
    //else 
    //{
        // Process the command response 
        HsmCmdRsp();
    //}

} //End hsmMbDriver()


//******************************************************************************
// Function to process the command response in gHsmCmdResp
// --read back the response from the HSM MB FIFO 
// --The numResultWords is the remaining words receive after the 1st 3,
//   i.e. the number of cmd specific resultWords received. 
//******************************************************************************
void HsmCmdRsp() 
{

    uint32_t                mbrxstatus;
    uint16_t                cmdSizeWds;
    uint8_t                 i;

    // Check for response recieved by reading RXINT
    //printc("HSM ROM Test Driver: Waiting for the command response (RXINT)...\n");
    mbrxstatus = HSM_REGS->HSM_MBRXSTATUS;

    //Poll RXINT in non-interrupt mode 
    //  (in interrupt mode this 'while' acts as an 'if')
    while ((mbrxstatus & MBRXSTATUS_RXINT_MASK) != MBRXSTATUS_RXINT_MASK) 
        { mbrxstatus = HSM_REGS->HSM_MBRXSTATUS; }
    
    // Check Mailbox Header
    gHsmCmdResp.mbHeader.v = HSM_REGS->HSM_MBRXHEAD;
    
    // Extract the command length
    cmdSizeWds = (uint16_t) ((gHsmCmdResp.mbHeader.v & MBRXHEADER_LEN_MASK) / 4);
    //SYS_PRINT("RSP  MB: 0x%08lx (%d Words)\r\n",gHsmCmdResp.mbHeader.v, cmdSizeWds);
    
    // Check Mailbox Command Header
    gHsmCmdResp.cmdHeader = HSM_REGS->HSM_MBFIFO[0];
    //SYS_PRINT("RSP CMD: 0x%08lx\r\n",(uint32_t) gHsmCmdResp.cmdHeader);
    
    // Check Response Code
    gHsmCmdResp.resultCode = HSM_REGS->HSM_MBFIFO[0];
    cmdRCStr = CmdResultCodeStr(gHsmCmdResp.resultCode);
    //SYS_PRINT("RSP  RC: 0x%08lx %s\r\n", (uint32_t) gHsmCmdResp.resultCode,
    //    CmdResultCodeStr(gHsmCmdResp.resultCode));
    
    // Read the Command Result Data       
    cmdSizeWds = min(cmdSizeWds,MAX_RSP_RESULT_WORDS);
    gHsmCmdResp.numResultWords = cmdSizeWds - 3;
    for (i = 0; i < cmdSizeWds - 3; i++) 
    { 
        gHsmCmdResp.resultData[i] = HSM_REGS->HSM_MBFIFO[0]; 
        //SYS_PRINT("RSP  W%d: 0x%08lx\r\n", i, gHsmCmdResp.resultData[i]);
    }
} //End HsmCmdRsp() 


//******************************************************************************
// Check the expected HSM Command Response against the Command
// --Check:
//     1. MB Header
//     2. CMD Header
//     3. Result Code
//     4. HSM Status
//     5. Output Data (if valid)
//     6. Interrupt Flag
//
// --Globals:  rspData, gHsmCmdReq, gHsmCmdResp
//
// --Returns:
//******************************************************************************
void HsmCmdRspChkr(RSP_DATA * rsp, bool printExpData) 
{
    uint32_t          rdData = 0;
    static char *     rcStr; 

    rsp->resultCode = gHsmCmdResp.resultCode;

    // Check the Mailbox Header
    if (gHsmCmdResp.mbHeader.v != gHsmCmdReq.expMbHeader) 
    {
            SYS_MESSAGE("HSM FAIL: Mailbox Header Match\r\n");
            SYS_PRINT("MBExpected: 0x%08lx\r\n", (uint32_t)gHsmCmdReq.expMbHeader);
            SYS_PRINT("MBActual:   0x%08lx\r\n", (uint32_t)gHsmCmdResp.mbHeader.v);
            rsp->testFailCnt++;
            rsp->rspChksPassed = false;
    }
    else
    {
        SYS_MESSAGE("HSM Pass: MB Header Match OK:\r\n");
    }
  
    //Check the Command Header 
    if (gHsmCmdResp.cmdHeader != gHsmCmdReq.cmdHeader) 
    {
        if (gHsmCmdResp.resultCode != E_INVFORMAT && 
            gHsmCmdResp.resultCode != E_SAFEMODE)
        {
            SYS_MESSAGE("HSM FAIL: Command Header Match\r\n");
            SYS_PRINT("CMDExpected: 0x%08lx\r\n", (uint32_t)gHsmCmdReq.cmdHeader);
            SYS_PRINT("CMDActual:   0x%08lx\r\n", (uint32_t)gHsmCmdResp.cmdHeader);
        }
    }
    {
        SYS_MESSAGE("HSM Pass: CMD Header Match OK:\r\n");
    }
  
    // Check the Result Code
    rcStr = CmdResultCodeStr((gHsmCmdResp.resultCode));
    if (gHsmCmdResp.resultCode != gHsmCmdReq.expResultCode) 
    {
        SYS_PRINT("HSM FAIL: RC ""%s""\r\n",rcStr);
        rsp->testFailCnt++;
        rsp->rspChksPassed = false;
    }
    else
    {
        SYS_PRINT("HSM Pass: RC OK(""%s""):\r\n",rcStr);
    }
  
    // Check the HSM STATUS register
    rdData = HSM_REGS->HSM_STATUS;
    GetHsmStatus(&busy,&ecode,&sbs,&lcs,&ps);
    if (rdData != gHsmCmdReq.expStatus) 
    {
        HsmStatusReg r; 

        if (sbs != HSM_SBS_DISABLED &&
            sbs != HSM_SBS_UNDETERM)
        {

            rsp->testFailCnt++;
            SYS_MESSAGE("HSM STATUS FAIL \r\n");
            //SYS_PRINT("    Actual: busy=%d ecode=%d sbs=%d lcs=%d ps=%d\r\n",
            //          busy,ecode,sbs,lcs,ps);
            SYS_PRINT( "     Actual- %s ECODE:%s SBS:%s LCS:%s PS:%s\r\n",
                busy?"BUSY":"NOT busy", 
                ecodeStr[ecode], sbsStr[sbs], lcsStr[lcs], psStr[ps]);
               
            r.v = gHsmCmdReq.expStatus;
            //SYS_PRINT("  Expected: busy=%d ecode=%d sbs=%d lcs=%d ps=%d\r\n",
            //           r.s.busy,r.s.ecode,r.s.sbs,r.s.lcs,ps);
            SYS_PRINT( "   Expected- %s ECODE:%s SBS:%s LCS:%s PS:%s\r\n",
                r.s.busy?"BUSY":"NOT busy", 
                ecodeStr[r.s.ecode], sbsStr[r.s.sbs], lcsStr[r.s.lcs], psStr[r.s.ps]);
        }
        else
        {
            //SYS_PRINT( "HSM STATUS - %s ECODE:%s SBS:%s LCS:%s PS:%s\r\n",
            //    busy?"BUSY":"NOT busy", 
            //    ecodeStr[ecode], sbsStr[sbs], lcsStr[lcs], psStr[ps]);
            SYS_MESSAGE("HSM Pass: Reg. STATUS OK:\r\n");
        }
    }
    else
    {
        //SYS_PRINT( "HSM STATUS - %s ECODE:%s SBS:%s LCS:%s PS:%s\r\n",
        //    busy?"BUSY":"NOT busy", 
        //    ecodeStr[ecode], sbsStr[sbs], lcsStr[lcs], psStr[ps]);
        SYS_MESSAGE("HSM Pass: Reg. STATUS OK:\r\n");
    }

    //Check the HSM INTFLAG register
    rdData = HSM_REGS->HSM_INTFLAG;
    if (rdData != gHsmCmdReq.expIntFlag) 
    {
      if (gHsmCmdResp.resultCode != E_INVFORMAT && 
          gHsmCmdResp.resultCode != E_SAFEMODE)
      {
          rsp->testFailCnt++;
          SYS_MESSAGE("HSM FAIL: INTFLAG read after command completion failed:\r\n");
          SYS_PRINT("Expected: 0x%08lx\r\n", gHsmCmdReq.expIntFlag);
          SYS_PRINT("Actual:   0x%08lx\r\n", rdData);
      }
    }
  
    // Clear the INTFLAG.ERROR if it was set
    if ((rdData & 0x00000001) == 0x00000001) 
    {
        HSM_REGS->HSM_INTFLAG |= 0x00000001;
    }

    if (rsp->testFailCnt == 0) 
    {
        rsp->rspChksPassed = true; 
    }
    else 
    {
        rsp->rspChksPassed = false;
    }

} //End HsmCmdRspChkr(void) 


//******************************************************************************
//PrintSG()
//******************************************************************************
void PrintSG(CmdSGDescriptor dmaSG, bool printData)
{
    int i;
    SYS_PRINT("    SG ADDR: 0x%08lx (%d bytes)\r\n", 
        (uint32_t) dmaSG.data.addr, dmaSG.flagsLength.s.length);

    if (printData)
    {
        SYS_MESSAGE("    ");
        for (i=0; i<dmaSG.flagsLength.s.length/4; i++) 
        {
            ((i%(4)==0)&&(i!=0))?SYS_MESSAGE("\r\n    "):dummy32;
            SYS_PRINT("0x%08lx ",((uint32_t *)dmaSG.data.addr)[i]);
        }
        SYS_MESSAGE("\r\n");
    }
}

#if USE_HSM_INT==TRUE
//******************************************************************************
// HSM Interrupt Handler
//******************************************************************************
void INTREQ_217_Handler(void) 
{

    //Disable interrupt while processing
    HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(0);
    
    //Process the command response
    //HsmCmdRsp();
    
    //Check the command response 
    //HsmCmdRspChkr(rsp, true);
    
    //Set the interrupt triggered flag so the test can check the output data
    gRspData.hsmRxInt = true;
    
    //Re-enable the interrupt
    HSM_REGS->HSM_MBCONFIG = HSM_MBCONFIG_RXINT(1);
}
#endif //0


#if 0
//******************************************************************************
//WRITE NVM Boot Command Cmd
//******************************************************************************
void HsmWriteNvm(bool int_mode) 
{
    // Make sure the HSM is not busy
    while (HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk);
    
    // Send CMD_BOOT_SELF_TEST command to load the HSM firmware
    testData.gHsmCmdReq.mbHeader      = 0x00f00010;
    testData.gHsmCmdReq.cmdHeader     = 0x000006F0;
    testData.gHsmCmdReq.cmdInputs[0]  = 0x00000000; // Unused
    testData.gHsmCmdReq.cmdInputs[1]  = 0x00000000; // Unused
    testData.gHsmCmdReq.cmdInputs[2]  = 0x00000000;
    testData.gHsmCmdReq.cmdInputs[3]  = 0x00000000; // Unused
    testData.gHsmCmdReq.expMbHeader   = CMD_BOOT_SELF_TEST_MBHEADER_RX;
    testData.gHsmCmdReq.expResultCode = SELF_TEST_EXP_RSLT_CODE;
    testData.gHsmCmdReq.expStatus     = SELF_TEST_STATUS;
    testData.gHsmCmdReq.expIntFlag    = SELF_TEST_INTFLAG;
    //SYS_PRINT("HSM ROM Cmd: Sending CMD_BOOT_SELF_TEST command\r\n");
    
    dmaDescriptorOut[0].data.addr = &HashInitBuffer;
    dmaDescriptorOut[0].next.s.stop = 1;
    dmaDescriptorOut[0].next.s.addr = 0;
    dmaDescriptorOut[0].length.s.length = 16;
    dmaDescriptorOut[0].length.s.cstAddr = 0;
    dmaDescriptorOut[0].length.s.discard = 0;
    dmaDescriptorOut[0].length.s.realign = 1;
    dmaDescriptorOut[0].length.s.IntEn = 0;
    
    dmaDescriptorIn[0].data.addr = &vsmInputDataRaw;
    dmaDescriptorIn[0].next.s.stop = 1;
    dmaDescriptorIn[0].next.s.addr = 0;
    dmaDescriptorIn[0].length.s.length = sizeof(vsmInputDataRaw);
    dmaDescriptorIn[0].length.s.cstAddr = 0;
    dmaDescriptorIn[0].length.s.discard = 0;
    dmaDescriptorIn[0].length.s.realign = 1;
    dmaDescriptorIn[0].length.s.IntEn = 0;  
    
    HsmMbCmdDriver(&testData.gHsmCmdReq, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //SYS_PRINT("HSM ROM Cmd: Waiting for interrupt...\n");
      while (!testData.hsm_rx_int);
      testData.hsm_rx_int = false;
    } else {
      // Check the command response 
      hsm_rom_test_cmd_rsp_chkr();
    }
    
    // Check the results of the command
    if (testData.rsp_chks_passed) {
      if (SELF_TEST_EXP_RSLT_CODE == E_INVPARAM) {
        //SYS_PRINT("HSM ROM Cmd: CMD_BOOT_SELF_TEST terminated properly with E_INVPARAM due to invalid parameter.\n");
      } else      if (SELF_TEST_EXP_RSLT_CODE == E_INVFORMAT) {
        //SYS_PRINT("HSM ROM Cmd: CMD_BOOT_SELF_TEST terminated properly with E_INVFORMAT for negative test.\n");
      }
      //SYS_PRINT("HSM ROM Cmd: CMD_BOOT_SELF_TEST test was successful.\r\n");
    } else {
      //SYS_PRINT("Failed Cmd code:   0x%x\n", hsm_rom_test_cmd_resp.resultData[0]);
    }
    
    //SYS_PRINT("---------------------------------------------\r\n");
}
#endif //0


//******************************************************************************
//Authentication Commands
//******************************************************************************
#if 0
void hsm_rom_test_get_challenge(bool int_mode) 
{
    
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
    testData.gHsmCmdReq.mbHeader      = CMD_SD_GET_CHALLENGE_MBHEADER;
    testData.gHsmCmdReq.cmdHeader     = CMD_SD_GET_CHALLENGE_INST;
    testData.gHsmCmdReq.cmdInputs[0]  = 0x00000000;            // Unused
    testData.gHsmCmdReq.cmdInputs[1]  = (uint32_t) &outDescr;  // Pointer to output descriptor
    testData.gHsmCmdReq.cmdInputs[2]  = 0x00000000;            // Unused
    testData.gHsmCmdReq.cmdInputs[3]  = 0x00000000;            // Unused
    testData.gHsmCmdReq.expMbHeader   = CMD_SD_GET_CHALLENGE_MBHEADER_RX;
    testData.gHsmCmdReq.expResultCode = SD_GET_CHLNG_EXP_RSLT_CODE;
    testData.gHsmCmdReq.expStatus     = SD_CHLNG_STATUS;
    testData.gHsmCmdReq.expIntFlag    = SD_CHLNG_INTFLAG;
    //SYS_PRINT("HSM ROM Cmd: Sending CMD_SD_GET_CHALLENGE command\r\n");
    HsmMbCmdDriver(&testData.gHsmCmdReq, int_mode);
    
    // Wait for an interrupt in interrupt mode
    if (int_mode) {
      //SYS_PRINT("HSM ROM Cmd: Waiting for interrupt...\n");
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
        //SYS_PRINT("HSM ROM Cmd: CMD_SD_GET_CHALLENGE terminated properly with E_INVFORMAT for negative test.\n");
        //SYS_PRINT("HSM ROM Cmd: CMD_SD_GET_CHALLENGE test was successful.\n");        
      } else {
        // Make sure the challenge output is not zero (32-bits is enough to compare)
        if (chlngOutput.challenge == 0x00000000) {
          //SYS_PRINT("FAILURE: Challenge output is zeros.\n");
          //SYS_PRINT("HSM ROM Cmd: CMD_SD_GET_CHALLENGE test was unsuccessful.\n");
          testData.testFail++;
        } else {
          // Make sure the challenge output does not match the previous value (32-bits is enough to compare)
          chlngCmp = memcmp(&chlngOutput.challenge, &last_challenge, 4);
#ifndef FIXED_CHLNG
          if (chlngCmp == 0) {
            //SYS_PRINT("FAILURE: Challenge output matches last value.\n");
            //SYS_PRINT("HSM ROM Cmd: CMD_SD_GET_CHALLENGE test was unsuccessful.\n");
            testData.testFail++;
          } else {
#else
          if (1) {
#endif
            //SYS_PRINT("Challenge Output: ");
#ifdef __FPGA__
            for (i=0; i<sizeof(chlngOutput.challenge); i++) {
              SYS_PRINT("%x ", chlngOutput.challenge[i]);
            }
#else
            //Printx(chlngOutput.challenge);
            //SYS_PRINT("\n");
#endif
            //SYS_PRINT("\nHSM ROM Cmd: CMD_SD_GET_CHALLENGE test was successful.\n");
          }
        }
      }
    } else {
      //SYS_PRINT("HSM ROM Cmd: CMD_SD_GET_CHALLENGE test was unsuccessful.\n");
    }
    memcpy(&last_challenge, &chlngOutput, sizeof(chlngOutput));
    //SYS_PRINT("\n---------------------------------------------\n");

} //End hsm_rom_test_get_challenge() 
#endif //0

//******************************************************************************
//
//******************************************************************************
void ClearRsp()
{
    static RSP_DATA *      rsp    = &gRspData;

    // Reset the response checker global variable
    rsp->invArgs       = false;
    rsp->invSlot       = false;
    rsp->rspChksPassed = false;
    rsp->testFailCnt   = 0;
    rsp->hsmRxInt      = 0;
    rsp->resultCode    = E_NULL;
}

/* *****************************************************************************
 End of File
 */
