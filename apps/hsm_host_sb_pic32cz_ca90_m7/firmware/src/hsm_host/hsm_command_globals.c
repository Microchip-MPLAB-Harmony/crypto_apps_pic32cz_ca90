#include "hsm_command_globals.h"

//******************************************************************************
//******************************************************************************
// Globals 
//******************************************************************************
//******************************************************************************

//Create a global command response variable 
HsmCmdReq   gHsmCmdReq;
HsmCmdResp  gHsmCmdResp;
RSP_DATA    gRspData;

//HSM Status Register
bool           busy;  //HSM Busy
HsmStatusECODE ecode; //Error Return Code
HsmStatusSBS   sbs;   //Secure Boot Status
HsmStatusLCS   lcs;   //Lifecycle Status 
HsmStatusPS    ps;    //Processor Status
HsmStatusReg   hsmStatusReg; //Processor Register

//HSM Status (SR) Register Field Values
//Error Codes
const char * ecodeStr[] = {
    //1111-1000",
    "UNDETERM",
    "SELF_TEST_FAIL",
    "SEC_FLASH_ERR",
    "UNCORRECT_LOC_MEM_ERR",
    "UNRECOV_FAULT",
    "TAMPER_EVENT",
    "SAFE_MODE",
    "AUTH_FAIL",
};

//Secure Boot Status
const char * sbsStr[] = {
    //111-110 //Reserved
    "UNDETERM",
    "DISABLED",
    "FLASH_AUTH",
    "ADD_AUTH",
    "FAIL",
    "PASSED",
} ;

//HSM Status (SR) Register Field Values
//LiveCycle Status Codes
const char * lcsStr[] = {
    //1111-1000 //
    "RESET",
    "NOT_SECURED",
    "ERASED",
    "OPEN",
    "SECURED",
} ;

const char * psStr[] = {  //Processing State bits
    "Reset",
    "BOOT", 
    "OPERATIONAL:", // - executing loaded operational firmware.
    "SAFE MODE",    // - limited functionality mode due to an error, sensitive data erased.
    "Reserved"};

#if 0
//--Error Codes
const char * ecodeStr[] = {
    "No error.", 
    "Host safe mode command.",
    "Safe mode tamper event.",
    "Unrecoverable fault.", 
    "Uncorrectable local memory bit error.",
    "Secure flash integrity error.",
    "Self test failure.",
    "Boot Authentication Failure." };

//HSM Status Register Fields
//--Secure Boot Status Codes
const char * sbsStr[] = { //Secure Boot State bits"
    "Reset", // - secure boot state not determined.",  //0
    "Disabled", // - secure boot disabled.",
    "Boot Flash Authentication", // - boot flash authentication in progress.",
    "Additional Authentication", // - host directed authentication in progress.",
    "Failed",   // - secure boot failed due to a required software authentication failing.",
    "Passed"};  // - secure boot completed with all required software authentications passing." };

const char * lcsStr[] = {//Lifecycle State bits:
    "Reset",             //- lifecycle state not determined.",
    "IC Manufacturing",  //- not secured, secure flash available for test.",
    "Erased",            // - not secured, secure flash erased.",
    "Open",              // - not secured, secure flash holds data.",
    "Secured",           // - secured, secure flash holds data.",
    "Reserved" };

const char * psStr[] = {  //"Processing State bits"
    "Reset",
    "Boot", // - executing from boot ROM, ready to load operational firmware.",
    "Operational", // - executing loaded operational firmware.",
    "Safe Mode", // - limited functionality mode due to an error, sensitive data erased.",
    "Reserved"};
#endif //0

//CMD Result Code
char * cmdResultStr[NUMCMDRESULTCODES] = 
    {"Unspecified failure All", "Command was successful"};
char * cmdResultHexStr[NUMCMDRESULTCODES] = 
    {"000000FF", "00000000"};

char * cmdSpecialResultStr[NUMCMDERRORRESULTCODES] =
{
/*E_GENERAL        = 0x80000000*/ "General Unspecified Failure",\
/*E_CMDCANCEL      = 0x80000001*/ "Command was Canceled",\
/*E_NOTSUPPORTED   = 0x80000002*/ "Command not supported in current mode",\
/*E_INVPARAM       = 0x80000003*/ "Invalid Parameter",\
/*E_INVINPUT       = 0x80000004*/ "Invalid Input",\
/*E_INPUTDMA       = 0x80000005*/ "MA Input Error",\
/*E_OUTPUTDMA      = 0x80000006*/ "MA Output Error",\
/*E_SYSTEMMEM      = 0x80000007*/ "Error Accessing System Memory",\
/*E_INPUTAUTH      = 0x80000008*/ "Error authenticating Input",\
/*E_HSMFWAUTH      = 0x80000009*/ "Error authenticating HSM Firmware",\
/*E_HOSTFWAUTH     = 0x8000000A*/ "Error Authenticating HOST firmware",\
/*E_SAFEMODE       = 0x8000000B*/ "Critical error has occurred and the HSM is entering safe mode",\
/*E_SELFTEST       = 0x8000000C*/ "The command requires that self test to be conducted, and the test has failed.",\
/*E_TIMELOCKED     = 0x8000000D*/ "The command requires that the time be unlocked before it can execute.",\
/*E_TIMEUNLOCKED   = 0x8000000E*/ "The command failed because time is currently unlocked.",\
/*E_TMPRLOCKED     = 0x8000000F*/ "The command requires the tamper response module to be unlocked",\
/*E_TMPRUNLOCKED   = 0x80000010*/ "The command failed because the tamper response module is currently unlocked",\
/*E_VSINUSE        = 0x80000011*/ "Variable Slot is in used.",\
/*E_VSEMPTY        = 0x80000012*/ "Variable Slot is empty",\
/*E_NOHSMFW        = 0x80000013*/ "No HSM FW was provided Load Firmware",\
/*E_INVVS          = 0x80000014*/ "Invalid Variable slot. The data of the variable slot has not been loaded.",\
/*E_HASHFAILED     = 0x80000015*/ "Secure Flash Page Hash Error",\
/*E_FWMDAUTH       = 0x80000016*/ "Firmware Metadata Authentication Failed",\
/*E_UNKNOWNSBC     = 0x80000017*/ "The flash pages have not been read yet",\
/*E_CRYPTOHW       = 0x80000018*/ "There was an error processing the request because the crypto hardware has thrown an error.",\
/*E_OUTPUTNOTSET   = 0x80000019*/ "The output DMA descriptor in the command needs to be set and wasn?t.",\
/*E_INPUTNOTSET    = 0x8000001A*/ "The input DMA descriptor in the command needs to be set and wasn?t.",\
/*E_OUTPUTSET      = 0x8000001B*/ "The output DMA descriptor in the command was set but this command does not have any output",\
/*E_INPUTSET       = 0x8000001C*/ "The input DMA descriptor in the command request was set but this command does not have any inputs",\
/*E_AUTHBITSET     = 0x8000001D*/ "The authentication fields included bit was set, but this command does not require authentication",\
/*E_AUTHBITNOTSET  = 0x8000001E*/ "The authentication fields included bit was not set, but this command requires authentication",\
/*E_SLOTPARASET    = 0x8000001F*/ "The slot parameters included bit is set, but this command does not use variable slots",\
/*E_SLOTPARANOTSET = 0x80000020*/ "The slot parameters included bit was not set, but this command does use variable slots.",\
/*E_INVFORMAT      = 0x80000021*/ "The command request was formatted incorrectly",\
/*E_OUTPUTTOOSMALL = 0x80000022*/ "The output area provided in the scatter gather descriptor was too small",\
/*E_OUTOFMEMORY    = 0x80000023*/ "The HSM could not allocate internal memory to process the command",\
/*E_INVHASHTYPE    = 0x80000024*/ "The hash type specified in the command is invalid",\
/*E_HOSTFLASHLCK   = 0x80000025*/ "Only valid during secure boot. The host flash could not be locked or unlocked during the process.",\
/*E_HOSTFLASHERASE = 0x80000026*/ "An operation to erase a host flash page or panel has failed",\
/*E_HOSTFLASHREAD  = 0x80000027*/ "An operation to read the host flash has failed",\
/*E_HOSTFLASHWRITE = 0x80000028*/ "An operation to write the host flash has failed",\
/*E_HSMFLASHLCK    = 0x80000029*/ "An operation to lock the HSM flash has failed, likely during the transition to Safe Mode",\
/*E_HSMFLASHERASE  = 0x8000002A*/ "An operation to erase the HSM flash page has failed",\
/*E_HSMFLASHREAD   = 0x8000002B*/ "An operation to read the HSM flash page has failed",\
/*E_HSMFLASHWRITE  = 0x8000002C*/ "An oepration to write to the HSM flash page has failed",\
};


char * cmdHsmErrorStr[] =
{
    "HSM ERROR:  General Error",
    "HSM ERROR:  HSM Busy Prior to Sending Command",
    "HSM ERROR:  HSM Busy - TIMEOUT",
    "HSM ERROR:  HSM - Not in Operational Mode", 
    "UNKNOWN HSM ERROR",
};

#define CMDREQGRPMASK   0x000000FF
#define CMDREQTYPEMASK  0x0000FF00
const char * cmdGrpStr[] =
{
    "CMD_BOOT",   //Boot Commands 0x00
    "CMD_SDBG",   //Secure Debug Commands 0x01
    "CMD_TMPR",   //Tamper Response Commands 0x02
    "CMD_VSM",    //Variable Slot Management Commands 0x03
    "CMD_KEYMGM", //Key Management Commands 0x04
    "CMD_HASH",   //Hash Commands 0x05
    "CMD_AES",    //AES Commands 0x06
    "CMD_CHACHA", //ChaCha Commands 0x07
    "CMD_TDES",   //Triple DES Commands 0x08
    "CMD_DES",    //DES Commands 0x09
    "CMD_RSA",    //RSA Commands 0x0A
    "CMD_SIGN",   //Signature Commands 0x0B
    "CMD_X509",   //X.509 Certificate Commands 0x0C
    "CMD_MISC",   //Miscellaneous Commands 0xF0
};

const char * bootCmdStr[] =
{
    "CMD_BOOT_GET_SBC",          //Get Secure Boot 0x00
    "CMD_BOOT_LOAD_FIRMWARE",    //Load Firmware X 0x01
    "CMD_BOOT_PROCESS_FIRMWARE", //Process Firmware X 0x02
    "CMD_BOOT_FINALIZE_SB",      //Finalize Secure 0x03
    "CMD_BOOT_SELF_TEST",        //Self-Test 0x04
};

const char * hashCmdStr[] =
{
    "CMD_HASH_HASH_BLOCK",    //Hash Block X * 0x00
    "CMD_HASH_HASH_INIT",     //Initialize Hash X * 0x01
    "CMD_HASH_HASH_UPDATE",   //Update Hash X * 0x02
    "CMD_HASH_HASH_FINALIZE", //Finalize Hash X * 0x03
    "CMD_HASH_HMAC",          //Conduct HMAC X * 0x04
    "CMD_HASH_VALIDATE",      //Validate Hash 0x05
};

const char * kmCmdStr[] =
{
    "CMD_KM_CREATE_KEY",        //Create Key X * X 0x00
    "CMD_KM_DERIVE_KEY_HKDF",   //Derive Key X * X 0x01
    "CMD_KM_DERIVE_KEY_PBKDF2", //Derive Key X * X 0x02
    "CMD_KM_DERIVE_KEY_KDF2",   //Derive Key X * X 0x03
    "CMD_KM_READ_PRIVATE_KEY",  //Read Private Key X X 0x04
    "CMD_KM_READ_PUBLIC_KEY",   //Read Public Key X X 0x05
    "CMD_KEY_CONVERT_KEY",      //Convert Key X * X 0x06
};

//HSM Command Words
uint32_t globalMbHeader  = 0x00000000; //HSM MB Header
uint32_t globalCmdHeader = 0x00000000; //HSM CMD Request Header 
uint32_t globalCmdInput  = 0x00000000; //HSM CMD Inputs
uint32_t globalCmdOutput = 0x00000000; //HSM CMD Outputs
uint32_t globalCmdParams[MAX_CMD_PARAM_WORDS]; //Optional HSM CMD Parameters

//CMD Output DMA Results
uint32_t * dataPtrOut = 0;

//DMA Data Out
uint32_t ALIGN4 dmaDataOut[MAX_OUTPUT_DMA_LENGTH] = {0};


//Host Key Storage (64 bytes)
uint32_t ALIGN4 keyHostStoragePublic[16] =
{
    0x02345678,
    0x12345678,
    0x22345678,
    0x32345678,
    0x42345678,
    0x52345678,
    0x62345678,
    0x72345678,
    0x82345678,
    0x92345678,
    0xA2345678,
    0xB2345678,
    0xC2345678,
    0xD2345678,
    0xE2345678,
    0xF2345678,
};

//Host Key Storage (64 bytes)
uint32_t ALIGN4 keyHostStoragePrivate[16] =
{
    0x00ABCDEF,
    0x01ABCDEF,
    0x02ABCDEF,
    0x03ABCDEF,
    0x04ABCDEF,
    0x05ABCDEF,
    0x06ABCDEF,
    0x07BCDEF8,
    0x08ABCDEF,
    0x09ABCDEF,
    0x0AABCDEF,
    0x0BABCDEF,
    0x0CABCDEF,
    0x0DABCDEF,
    0x0EABCDEF,
    0x0FBCDEF8,
};


//OUTPUT SG DMA Descriptor 
CmdSGDescriptor ALIGN4 dmaDescriptorOut[3] =
{
    {
     .data.addr  = (uint32_t *) &dmaDataOut,
     .next.s.stop = 1,
     .next.s.addr            = 0x00000000, 
     .flagsLength.s.length   = MAX_OUTPUT_DMA_LENGTH, //Length o data block
     .flagsLength.s.cstAddr  = 0,                     //Constant Address (FIFO)
     .flagsLength.s.realign  = 1,                     //Realign to 32 bits
     .flagsLength.s.discard  = 0,                     //Do not Write
    },
    {
    .data.addr               = 0x00000000,
    .next.s.stop             = 1,
    .next.s.addr             = 0x00000000, 
    .flagsLength.s.length    = 0,     //Length o data block
    .flagsLength.s.cstAddr   = 0,     //Constant Address (FIFO)
    .flagsLength.s.realign   = 0,     //Realign to 32 bits
    .flagsLength.s.discard   = 0      //Do not Write
    },
    {
    .data.addr               = 0x00000000,
    .next.s.stop             = 1,
    .next.s.addr             = 0x00000000,
    .flagsLength.s.length    = 0,     //Length o data block
    .flagsLength.s.cstAddr   = 0,     //Constant Address (FIFO)
    .flagsLength.s.realign   = 0,     //Realign to 32 bits
    .flagsLength.s.discard   = 0      //Do not Write
    },

};


CmdSGDescriptor inpDesc[5] = {0};
CmdSGDescriptor outDesc[5] = {0};

//DMA Data In
uint32_t * dataPtrIn = 0;

//INPUT DMA Data
//--seeing BAADF00D means a length was wrong or this was not assigned
uint32_t ALIGN4 dmaDataIn[MAX_INPUT_DMA_LENGTH] = 
{
0xDAADB001, 0xDAADB002, 0xDAADB003, 0xDAADB004,
0xDAADB005, 0xDAADB006, 0xDAADB007, 0xDAADB008,
0xDAADB009, 0xDAADB010, 0xDAADB011, 0xDAADB012,
0xDAADB013, 0xDAADB014, 0xDAADB015, 0xDAADB016,
0xDAADB017, 0xDAADB018, 0xDAADB019, 0xDAADB020,
0xDAADB021, 0xDAADB022, 0xDAADB023, 0xDAADB024,
0xDAADB025, 0xDAADB026, 0xDAADB027, 0xDAADB028,
0xDAADB029, 0xDAADB030, 0xDAADB031, 0xDAADB032,
};

//INPUT SG DMA Descriptor 
CmdSGDescriptor ALIGN4 dmaDescriptorIn[3] = 
{
    {
    .data.addr              = (uint32_t *) dmaDataIn,
    .next.s.stop            = 1,
    .next.s.addr            = 0x00000000,
    .flagsLength.s.length   = MAX_INPUT_DMA_LENGTH, //Length o data block
    .flagsLength.s.cstAddr  = 0,                    //Constant Address (FIFO)
    .flagsLength.s.realign  = 1,                    //Realign to 32 bits
    .flagsLength.s.discard  = 0                     //Do not Write:w
    },
    {
    .data.addr              = (uint32_t *) 0,
    .next.s.stop            = 1,
    .next.s.addr            = 0x00000000, 
    .flagsLength.s.length   = 0,     //Length o data block
    .flagsLength.s.cstAddr  = 0,     //Constant Address (FIFO)
    .flagsLength.s.realign  = 0,     //Realign to 32 bits
    .flagsLength.s.discard  = 0      //Do not Write:w
    },
    {
    .data.addr              = (uint32_t *) 0,
    .next.s.stop            = 1,
    .next.s.addr            = 0x00000000,
    .flagsLength.s.length   = 0,     //Length o data block
    .flagsLength.s.cstAddr  = 0,     //Constant Address (FIFO)
    .flagsLength.s.realign  = 0,     //Realign to 32 bits
    .flagsLength.s.discard  = 0      //Do not Write:w
    },

};

char messageBuffer[5][64] = {
   "HOST Ready\r\n",                //0
   "HSM  MB Ready\r\n",            //1
   "HSM Command Wait\r\n",          //2
   "HSM Command Received\r\n",      //3
   "HSM Execute HSM Command\r\n"    //4
   "HSM Execution Complete\r\n"     //5
};


//AES Input Data Command KEY Slot Data
uint32_t vssSlotData[MAXSLOTDATAWORDS+SLOTMETAWORDS];

char * gPrintStr;
char   gPrintBuffer[MAX_PRINT_BYTES];

uint8_t gRcvCmdStr[MAX_PRINT_BYTES] = { 0 };

//COM UART 
int volatile gRcvCnt;
int volatile gRcvCmdRdy;

// Function Prototypes
char * CmdResultCodeStr(uint32_t cmdResultCode);
