/* ************************************************************************** */
/** Descriptive File Name

  @Company
     Microchip Technology 

  @File Name
    vsm.h

  @Summary
    HSM VSM Commands

  @Description
    HSM VSM Commands
 */
/* ************************************************************************** */

#ifndef _BOOT_H    /* Guard against multiple inclusion */
#define _BOOT_H

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdint.h>
#include <stdbool.h>
#include "hsm_host\hsm_command.h"
#include "hsm_host\hsm_command_globals.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

#define CMD_BOOT_SELF_TEST_MBHEADER_RX  0x0020000C
//#define CMD_BOOT_SELF_TEST_MBHEADER_RX  0x0030000C
#define SELF_TEST_STATUS                0x00000410

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef enum _CmdBootCommandTypes
{
    CMD_BOOT_GET_SBC = 0,
    CMD_BOOT_LOAD_FIRMWARE = 1,
    CMD_BOOT_PROCESS_FIRMWARE = 2,
    CMD_BOOT_FINALIZE_SB = 3,
    CMD_BOOT_SELF_TEST = 4
} CmdBootCommandTypes;

typedef union 
{
    struct 
    {
        CmdCommandGroups cmdGroup   :8;
        CmdBootCommandTypes cmdType :8;
        unsigned char               :8;
        unsigned char authInc       :1;
        unsigned char slotParamInc  :1;
        unsigned char               :6;
    }s;
    uint32_t w;
} CmdBootCommandHeader;

//====================================================================== 
//FW Meta Data
//====================================================================== 

typedef enum _FwMdAlgoUsed
{
    FW_MD_AUTH_TYPE_NONE = 0x0,
    FM_MD_SHA2_256_HMAC = 0x9,
    FM_MD_VSS_SIGN = 0xC
} FwMdAlgoUsed;

typedef enum _MdHashUsed
{
    MD_SIGN_SHA1 = 0x2,
    MD_SIGN_SHA224 = 0x3,
    MD_SIGN_SHA256 = 0x4,
    MD_SIGN_SHA384 = 0x5,
    MD_SIGN_SHA512 = 0x6,
} MdHashUsed;

typedef struct _FwMdSigMd
{
    unsigned char               :4;
    FwMdAlgoUsed  algoUsed      :4;
    unsigned char keySlotIndex; ;
    MdHashUsed    hashAlgo      :3;
    unsigned short              :12; //????
} FwMdSigMd;

#if 0
typedef struct _ImageMetaData
{
    void * srcAddress;
    void * dstAddress;
    unsigned int size;
    imgMdVersion version;
    imgDescType desc;
    imgMdEncMd encryptionMd;
    unsigned char wrappedKey[40];
    imgMdSigMd signatureMd;
    unsigned char Signature[96];
} ImgMetadata;

typedef struct
{
    unsigned int  length;
    FwMdSigMd     signatureMetadata;
    unsigned int  numImages;
    ImgMetadata   image1;
    unsigned char signature[96];
} FwMetadata;

typedef struct
{
    unsigned int  length;
    FwMdSigMd     signatureMetadata;
    unsigned int  numImages;
    ImgMetadata   image1;
    ImgMetadata   image2;
    unsigned char signature[96];
} FwMetadata2;

typedef struct
{
    unsigned int  length;
    FwMdSigMd     signatureMetadata;
    unsigned int  numImages;
    ImgMetadata   image1;
    ImgMetadata   image2;
    ImgMetadata   image3;
    unsigned char signature[96];
} FwMetadata3;

typedef struct
{
    unsigned int  length;
    FwMdSigMd     signatureMetadata;
    unsigned int  numImages;
    ImgMetadata   image1;
    ImgMetadata   image2;
    ImgMetadata   image3;
    ImgMetadata   image4;
    unsigned char signature[96];
} FwMetadata4;
#endif //0



//====================================================================== 
//BOOT - Self Test Command
//====================================================================== 
typedef enum _CmdBootSelfTestType
{
    CMD_BOOT_ST_BRT  = 0,
    CMD_BOOT_ST_PRT  = 1,
    CMD_BOOT_ST_TRNG = 2,
    CMD_BOOT_ST_AES  = 3,
    CMD_BOOT_ST_PK   = 4,
    CMD_BOOT_ST_ALL  = 5,
} CmdBootSelfTestType;

typedef enum _CmdBootSelfTestFailedTestType
{
    CMD_BOOT_STF_BRT  = 0,
    CMD_BOOT_STF_PRT  = 1,
    CMD_BOOT_STF_TRNG = 2,
    CMD_BOOT_STF_AES  = 3,
    CMD_BOOT_STF_PK   = 4,
} CmdBootSelfTestFailedTestType;

typedef struct
{
    struct
    {
        CmdBootSelfTestType selfTest : 3;
        unsigned int                 : 29;
    } s;
    uint32_t w;
} CmdBootSelfTestParameter1;

typedef struct
{
    struct
    {
        CmdBootSelfTestFailedTestType selfTest : 3;
        unsigned int                           : 29;
    } s;
    uint32_t w;
} CmdBootSelfTestResults;

typedef struct
{
    CmdMailboxHeader mbHeader;
    CmdBootCommandHeader cmdHeader;
    CmdResultCodes resultCode;
    CmdBootSelfTestResults results;
} CmdBootSelfTestResponse;

typedef struct
{
    CmdBootCommandHeader cmdHeader;
    CmdSGDescriptor* inputs;
    CmdMailboxHeader mbHeader;
    CmdSGDescriptor* outputs;
    CmdBootSelfTestParameter1* param1;
} CmdBootSelfTestRequest;

//====================================================================== 
//BOOT - Process Firmware Command
//====================================================================== 
typedef struct
{
    CmdMailboxHeader mbHeader;
    CmdBootCommandHeader cmdHeader;
    CmdSGDescriptor* inputs;
    CmdSGDescriptor* outputs;
    uint32_t * param1;           //FwMetadata * param1;
    uint32_t * param2;           //FwMetadata * param2;
} CmdBootProcessFirmwareRequest;

typedef struct
{
    CmdMailboxHeader mbHeader;
    CmdBootCommandHeader cmdHeader;
    CmdResultCodes resultCode;
} CmdBootProcessFirmwareResponse;
  
//====================================================================== 
//BOOT - Load Firmware Command
//====================================================================== 
//--This command is used by the HOST ROM when secure boot is enabled to load 
//the HSM firmware. When secure boot is disabled, this command is used by 
//the HOST firmware to load the HSM firmware.  When secure boot is enabled 
//this command is also used to start the HOST firmware validation. The HSM 
//ROM will copy the inputs into shared memory with the HSM Firmware. When 
//the HSM /firmware is operational it will continue to process this command 
//and validate the HOST firmware.

typedef struct
{
    CmdMailboxHeader     mbHeader;
    CmdBootCommandHeader cmdHeader;
    CmdSGDescriptor*     inputs;
    CmdSGDescriptor*     outputs;
    uint32_t *           param1; //FwMetadata *         param1;
    uint32_t *           param2; //FwMetadata *         param2;
} CmdBootLoadFirmwareRequest;

typedef struct
{
    CmdMailboxHeader     mbHeader;
    CmdBootCommandHeader cmdHeader;
    CmdResultCodes       resultCode;
} CmdBootLoadFirmwareResponse;


// *****************************************************************************
// *****************************************************************************
// Section: API Functions
// *****************************************************************************
// *****************************************************************************
RSP_DATA * HsmCmdBootSelfTest(void);
RSP_DATA * HsmCmdBootLoadFirmware(void); 
RSP_DATA * HsmCmdBootTestHashInit(void);

// *****************************************************************************
// *****************************************************************************
// Section: globals/Functions
// *****************************************************************************
// *****************************************************************************

extern uint8_t bootHashInitBuffer[64];

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _BOOT_H */

/* *****************************************************************************
 End of File
 */