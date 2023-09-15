/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    hsm_command.h

  @Summary
    Decodes ascii hsm commands from serial byte string sent from PC

  @Description
    Decodes ascii hsm commands from serial byte string sent from PC
 */
/* ************************************************************************** */

#ifndef _HSM_COMMAND_H /* Guard against multiple inclusion */
#define _HSM_COMMAND_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdint.h>
#include <stdbool.h>
#include "user.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define CRLF      SYS_MESSAGE("\r\n")
#define LINEBYTES      32   //PRINT Lines
#define BYTES_PER_WORD  4   //32 bit words
#define MINSLOTNUM      0    //Slot 0 is reserved
#define MAXSLOTNUM    255  //Slot 255 is reserved
#define BITS_PER_BYTE   8
#define BITS_PER_WORD  32

#define MAX_CMD_PARAM_WORDS    10
#define MAX_OUTPUT_DMA_LENGTH  1024  //Words
#define MAX_INPUT_DMA_LENGTH   1024  //Words
#define IOBUFFERSIZEWORDS      65
#define MAX_PRINT_BYTES        1024

#define ALIGN4 __attribute__((aligned(0x4)))

#define CMDSPECIALINDEXMASK    0xF0000000 
#define CMDSPECIALRESULTMASK   0x000000FF
#define HSMCMDERRMASK          0x00000F00
#define HSMCMDERRNUMMASK       0x000000FF
#define HSMGENERALERR          0x00000F00 //General HSM CMD Error
#define HSMBUSYERR             0x00000F01 //HSM is Busy prior to sending the command
#define HSMBUSYTIMEOUTERR      0x00000F02 //HSM Busy Timeout after sending command 
#define HSMNONOPERR            0x00000F03 //HSM Not in Op mode

#define HSM_STATUS_BUSY_VAL  ((HSM_REGS->HSM_STATUS & HSM_STATUS_BUSY_Msk) >> HSM_STATUS_BUSY_Pos)
#define HSM_STATUS_ECODE_VAL ((HSM_REGS->HSM_STATUS & HSM_STATUS_ECODE_Msk) >> HSM_STATUS_ECODE_Pos)
#define HSM_STATUS_SBS_VAL   ((HSM_REGS->HSM_STATUS & HSM_STATUS_SBS_Msk) >> HSM_STATUS_SBS_Pos)
#define HSM_STATUS_LCS_VAL   ((HSM_REGS->HSM_STATUS & HSM_STATUS_LCS_Msk) >> HSM_STATUS_LCS_Pos)
#define HSM_STATUS_PS_VAL    ((HSM_REGS->HSM_STATUS & HSM_STATUS_PS_Msk) >> HSM_STATUS_PS_Pos)

#define NBCYCLESPERTICK        1000
#define MBRXSTATUS_RXINT_MASK  0x00100000
#define MAILBOX_FIFO_ADDR      (HSM_REGS->HSM_MBFIFO)

//Command Request Types
#define CMD_HEADER_HASH_BLOCK_SHA256    0x00040005 //SHA256 HASH-No Auth/No VS
#define CMD_HEADER_BOOT_SELF_TEST       0x00200014 //BOOT ST-No Auth/No VS

#define HASH_MD5_RESULT_BYTES    16 
#define HASH_SHA1_RESULT_BYTES   20 
#define HASH_SHA224_RESULT_BYTES 28
#define HASH_SHA256_RESULT_BYTES 32 
#define HASH_SHA384_RESULT_BYTES 48
#define HASH_SHA512_RESULT_BYTES 64

#define CMD_REQ_TRUSTED_MSK    (1<<25) 
#define CMD_REQ_SLOT_MSK       (1<<24) 
#define CMD_REQ_AUTH_MSK       (1<<23) 
#define MB_HEADER_PROT_MSK     (1<<20)      

// Why isn't this in part.h
#define MBRXSTATUS_RXINT_MASK     0x00100000
#define MBRXHEADER_LEN_MASK       0x0000FFFF

//====================================================================== 
// Mailbox/Command
//====================================================================== 
typedef union 
{
    struct
    {
        unsigned short length;
        unsigned char : 5;
        unsigned char unprotected: 1;
        unsigned short:10;
    }s;
    uint32_t v;
} CmdMailboxHeader;

typedef enum _cmdCommandGroups
{
    CMD_INVALID = 0xFF,
    CMD_BOOT   = 0,
    CMD_SDBG   = 1,
    CMD_TMPR   = 2,
    CMD_VSM    = 3,
    CMD_KEYMGM = 4,
    CMD_HASH   = 5,
    CMD_AES    = 6,
    CMD_CHACHA = 7,
    CMD_TDES   = 8,
    CMD_DES    = 9,
    CMD_RSA    = 10,
    CMD_SIGN   = 11,
    CMD_X509   = 12,
    CMD_DH     = 13,
    CMD_DICE   = 14,
    CMD_MISC   = 0xF0,
} CmdCommandGroups;

typedef struct
{
    CmdCommandGroups cmdGroup     :8;
    unsigned char    cmdType      :8;
    unsigned char                 :8;
    unsigned char    authInc      :1;
    unsigned char    slotParamInc :1;
    unsigned char    honsec       :1;
    unsigned char                 :5;
} CmdCommandHeader;

// HSM ROM Test Command Header Types
typedef enum _HSM_TEST_CMD_REQ {
    
    CMD_NULL_INST                    = 0xFFFFFFFF,
    CMD_BOOT_GET_SBC_INST            = 0x00000000,
    CMD_BOOT_LOAD_FIRMWARE_INST      = 0x00000100,
    CMD_BOOT_SELF_TEST_INST          = 0x00000400,
    CMD_SD_GET_CHALLENGE_INST        = 0x00000001,
    CMD_SD_AUTHORIZATION_INST        = 0x00000101,
    CMD_TMPR_GET_NUM_TMPREVENTS_INST = 0x00000602,
    CMD_TMPR_GET_TMPREVENT_INST      = 0x00000702,
    CMD_MISC_GET_TS_INST             = 0x000000F0,

    //Implemented
    CMD_HASH_HASH_BLOCK_SHA256_INST  = 0x00040005,
    CMD_VSM_INPUT_DATA_INST          = 0x00000003,
    CMD_VSM_OUTPUT_DATA_INST         = 0x00000103,
    CMD_VSM_DELETE_SLOT_INST         = 0x00000403,
    CMD_VSM_SLOT_GET_INFO_INST       = 0x00000503,
} HsmCmdHeader;

//HSM Status (SR) Register Field Values
//Error Codes
typedef enum HSM_ECODE {
    //1111-1000 = Reserved.
    HSM_ECODE_AUTH_FAIL              = 0b0111, //HSM firmware authentication failure.
    HSM_ECODE_SAFE_MODE              = 0b0110, //Host safe mode command.
    HSM_ECODE_TAMPER_EVENT           = 0b0101, //Safe mode tamper event.
    HSM_ECODE_UNRECOV_FAULT          = 0b0100, //Unrecoverable fault.
    HSM_ECODE_UNCORRECT_LOC_MEM_ERR  = 0b0011, //Uncorrectable local memory bit error.
    HSM_ECODE_SEC_FLASH_ERR          = 0b0010, //Secure flash integrity error.
    HSM_ECODE_SELF_TEST_FAIL         = 0b0001, //Self test failure.
    HSM_ECODE_UNDETERM               = 0b0000, //Reset - secure boot state not determined.
} HsmStatusECODE;

typedef struct
{
    unsigned char signatureSlot;
    unsigned int  : 24;
    unsigned char challengeValue[16];
    unsigned char signature[96];
} CmdCommandAuthFooter;

// HSM MB Command Result Codes
typedef enum _HSM_CMD_RESULT_CODE {

    //NULL Code
    E_NULL           = 0xFFFFFFFF,

    //General Result Codes
    E_UNKNOWN        = 0x00000000,           // Unspecified Failure
    S_OK             = 0x00000001,           // Command was Successful
    NUMCMDRESULTCODES  = 2,

    //ERROR RESULT CODES
    E_GENERAL        = 0x80000000, //"General Unspecified Failure"
    E_CMDCANCEL      = 0x80000001, //"ommand was Canceled"
    E_NOTSUPPORTED   = 0x80000002, //"ommand not supported in current mode"
    E_INVPARAM       = 0x80000003, //"Invalid Parameter"
    E_INVINPUT       = 0x80000004, //"Invalid Input"
    E_INPUTDMA       = 0x80000005, //"MA Input Error"
    E_OUTPUTDMA      = 0x80000006, //"MA Output Error"
    E_SYSTEMMEM      = 0x80000007, //"rror Accessing System Memory"
    E_INPUTAUTH      = 0x80000008, //"rror authenticating Input"
    E_HSMFWAUTH      = 0x80000009, //"rror authenticating HSM Firmware"
    E_HOSTFWAUTH     = 0x8000000A, //"rror Authenticating HOST firmware"
    E_SAFEMODE       = 0x8000000B, //" critical error has occurred and the HSM is entering safe mode"
    E_SELFTEST       = 0x8000000C, //"The command requires that self test to be conducted, and the test has failed."
    E_TIMELOCKED     = 0x8000000D, //"The command requires that the time be unlocked before it can execute."
    E_TIMEUNLOCKED   = 0x8000000E, //"The command failed because time is currently unlocked."
    E_TMPRLOCKED     = 0x8000000F, //"The command requires the tamper response module to be unlocked"
    E_TMPRUNLOCKED   = 0x80000010, //"The command failed because the tamper response module is currently unlocked"
    E_VSINUSE        = 0x80000011, //"Variable Slot is in used."
    E_VSEMPTY        = 0x80000012, //"Variable Slot is empty"
    E_NOHSMFW        = 0x80000013, //"No HSM FW was provided Load Firmware"
    E_INVVS          = 0x80000014, //"Invalid Variable slot. The data of the variable slot has not been loaded."
    E_HASHFAILED     = 0x80000015, //"Secure Flash Page Hash Error"
    E_FWMDAUTH       = 0x80000016, //"irmware Metadata Authentication Failed"
    E_UNKNOWNSBC     = 0x80000017, //"The flash pages have not been read yet"
    E_CRYPTOHW       = 0x80000018, //"There was an error processing the request because the crypto hardware has thrown an error."
    E_OUTPUTNOTSET   = 0x80000019, //"The output DMA descriptor in the command needs to be set and wasn?t."
    E_INPUTNOTSET    = 0x8000001A, //"The input DMA descriptor in the command needs to be set and wasn?t."
    E_OUTPUTSET      = 0x8000001B, //"The output DMA descriptor in the command was set but this command does not have any output"
    E_INPUTSET       = 0x8000001C, //"The input DMA descriptor in the command request was set but this command does not have any inputs"
    E_AUTHBITSET     = 0x8000001D, //"The authentication fields included bit was set, but this command does not require authentication"
    E_AUTHBITNOTSET  = 0x8000001E, //"The authentication fields included bit was not set, but this command requires authentication"
    E_SLOTPARASET    = 0x8000001F, //"The slot parameters included bit is set, but this command does not use variable slots"
    E_SLOTPARANOTSET = 0x80000020, //"The slot parameters included bit was not set, but this command does use variable slots."
    E_INVFORMAT      = 0x80000021, //"The command request was formatted incorrectly"
    E_OUTPUTTOOSMALL = 0x80000022, //"The output area provided in the scatter gather descriptor was too small"
    E_OUTOFMEMORY    = 0x80000023, //"The HSM could not allocate internal memory to process the command"
    E_INVHASHTYPE    = 0x80000024, //"The hash type specified in the command is invalid"
    E_HOSTFLASHLCK   = 0x80000025, //"Only valid during secure boot. The host flash could not be locked or unlocked during the process."
    E_HOSTFLASHERASE = 0x80000026, //"n operation to erase a host flash page or panel has failed"
    E_HOSTFLASHREAD  = 0x80000027, //"n operation to read the host flash has failed"
    E_HOSTFLASHWRITE = 0x80000028, //"n operation to write the host flash has failed"
    E_HSMFLASHLCK    = 0x80000029, //"n operation to lock the HSM flash has failed, likely during the transition to Safe Mode"
    E_HSMFLASHERASE  = 0x8000002A, //"n operation to erase the HSM flash page has failed"
    E_HSMFLASHREAD   = 0x8000002B, //"n operation to read the HSM flash page has failed"
    E_HSMFLASHWRITE  = 0x8000002C, //"n oepration to write to the HSM flash page has failed"
    NUMCMDERRORRESULTCODES      = 0x2D, 
} CmdResultCodes;

//Secure Boot Status
typedef enum HSM_SBS{
    //111-110 //Reserved
    HSM_SBS_PASSED     = 0b101, //Passed - secure boot completed with all required software authentications passing.
    HSM_SBS_FAIL       = 0b100, //Failed - secure boot failed due to a required software authentication failing.
    HSM_SBS_ADD_AUTH   = 0b011, //Additional Authentication - host directed authentication in progress.
    HSM_SBS_FLASH_AUTH = 0b010, //Boot Flash Authentication - boot flash authentication in progress.
    HSM_SBS_DISABLED   = 0b001, //Disabled - secure boot disabled.
    HSM_SBS_UNDETERM   = 0b000, //Reset - secure boot state not determined.
} HsmStatusSBS;

//HSM Status (SR) Register Field Values
//LiveCycle Status Codes
typedef enum HSM_LCS{
    //1111-1000 //= Reserved111-101 = Reserved
    HSM_LCS_SECURED      = 0b100, //Secured - secured, secure flash holds data.
    HSM_LCS_OPEN         = 0b011, //Open - not secured, secure flash holds data.
    HSM_LCS_ERASED       = 0b010, //Erased - not secured, secure flash erased.
    HSM_LCS_NOT_SECURED  = 0b001, //IC Manufacturing - not secured, secure flash available for test.
    HSM_LCS_RESET        = 0b000, //Reset - lifecycle state not determined.
} HsmStatusLCS;

//Processor Status
typedef enum HSM_STATUS_PS {
    HSM_PS_RESET = 0,
    HSM_PS_BOOT,
    HSM_PS_OPERATIONAL,
    HSM_PS_SAFEMODE
} HsmStatusPS;

typedef union
{
#pragma pack(push, 1)
    struct {
        bool    busy  : 1;
        bool    lkup  : 1;
        uint8_t       : 2;
        uint8_t ps    : 3;
        uint8_t       : 1;
        uint8_t lcs   : 3;
        uint8_t       : 1;
        uint8_t sbs   : 3;
        uint8_t       : 1;
        uint8_t ecode : 4;
        uint16_t      : 12;
    } s;
#pragma pack(pop)
    uint32_t v;
} HsmStatusReg;

typedef enum {
    INDX_HOST_READY = 0,
    INDX_HSM_MB_READY,
    INDX_CMD_WAIT,
    INDX_CMD_RCVD,
    INDX_CMD_EXEC,
    INDX_CMD_COMPLETE
} INDXMSG;

// HSM ROM Test Command Request
typedef struct 
{
    uint32_t        mbHeader;
    HsmCmdHeader    cmdHeader;   //TODO:  Make this a union with fields
    uint32_t        cmdInputs[4];
    uint32_t        expMbHeader;
    CmdResultCodes  expResultCode;
    uint32_t        expStatus;
    uint32_t        expIntFlag;
    uint8_t *       expData;
    uint32_t        expNumDataBytes;
} HsmCmdReq;

// HSM ROM Test Command Response 
#define MAX_RSP_RESULT_WORDS 8

typedef struct {
    CmdMailboxHeader            mbHeader;
    HsmCmdHeader                cmdHeader;
    CmdResultCodes              resultCode;
    uint32_t                    numResultWords; 
    uint32_t                    resultData[MAX_RSP_RESULT_WORDS];
} HsmCmdResp;

// Test Data Struct
typedef struct {
  bool               invArgs;       // Invalid Command Arguments
  bool               invSlot;
  uint32_t           testFailCnt;   // Variable to keep track of test errs
  bool               rspChksPassed; // Variable to keep track of whether 
                                    //    command response completed 
  bool               hsmRxInt;      // Boolean variable to indicate when 
  CmdResultCodes     resultCode;    // HSM Command result code 
} RSP_DATA;

//======================================================================         
// Scatter/Gather DMA 
//======================================================================         
typedef struct
{
    void * addr;
} CmdSGAddressOfData;

typedef union
{
    struct {
    unsigned char stop : 1;
    unsigned char      : 1;
    unsigned int  addr : 30;
    } s;
    uint32_t v;
} CmdSGNextDescriptor;

typedef union
{
    struct {
        uint32_t length  : 28;
        uint8_t  cstAddr : 1;
        uint8_t  realign : 1;
        uint8_t  discard : 1;
        uint8_t  intEn   : 1;
    } s;
    uint32_t v;
} CmdSGFlagsLength;

typedef struct
{
    CmdSGAddressOfData  data;
    CmdSGNextDescriptor next;
    CmdSGFlagsLength    flagsLength;
} CmdSGDescriptor;


extern bool           busy;
extern HsmStatusECODE ecode;
extern HsmStatusSBS   sbs;
extern HsmStatusLCS   lcs; 
extern HsmStatusPS    ps;
extern RSP_DATA       gRspData;


// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

unsigned char GetResultChar(int toConv, int toShift);
void          Print08X(int word);
void          InitDmaForOutput(uint32_t *buffer, int bufSize);
void          InitDmaForInput(uint32_t * buffer, int bufSize);
void          InitDmaInputLength(int length);
void          InitDmaOutputLength(int length);
void          ConsolidateDmaForOutput(int* firstDescriptorAddress);
int           SendHsmCommandRequest(void);
void          GetHsmStatus(bool *busy, 
                  HsmStatusECODE * ecode, 
                  HsmStatusSBS *sbs, 
                  HsmStatusLCS *lcs, 
                  HsmStatusPS *ps);           
char *        CmdResultCodeStr(uint32_t cmdResultCode);
void          PrintSG(CmdSGDescriptor dmaData, bool printData);
void          HsmCmdRsp(void); 

void          HsmCmdRsp(void); 
void          HsmMbCmdDriver(HsmCmdReq * cmd_req, bool int_mode); 
void          HsmCmdRspChkr(RSP_DATA * rsp, bool printExpData); 
void          ClearRsp();


// Create a global command response variable so it can be accessed by the 
// interrupt handler when (int_mode == true)

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _HSM_COMMAND*/

/* *****************************************************************************
 End of File
 */
