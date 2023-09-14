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

#ifndef _VSM_H    /* Guard against multiple inclusion */
#define _VSM_H

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdint.h>
#include <stdbool.h>
#include "hsm_host\hsm_command.h"
#include "hsm_host\hsm_command_globals.h"
//#include "hash.h"
#include "aes.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

#define VSS_META_WORDS  4  
#define VSS_META_BYTES  VSS_META_WORDS*BYTES_PER_WORD 

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

//====================================================================== 
//  VS Definition
//====================================================================== 
typedef enum _SLOTINFORETURN
{
    ERR_GENERAL        = -1,
    VS_SLOT_INFO_AVAIL = 0,
    VS_SLOT_EMPTY       = 1,
} SLOTINFORETURN;

typedef enum vssStorageType
{
    VM_STORAGE             = 0,
    NVM_UNENCRYPTED        = 1,
    NVM_FLASHKEYPROTECTED  = 2,
    NVM_TAMPERKEYPROTECTED = 3
} VSStorageType;

typedef enum vssSlotType
{
    VSS_RAW             = 0,
    VSS_SYMMETRICALKEY  = 1,
    VSS_ASYMMETRICALKEY = 2,
    VSS_HASHVALUE       = 3,
    VSS_HASHIV          = 4
} VSSlotType;

typedef union 
{
    struct
    {
        unsigned int hsmOnly         : 1;
        unsigned int                 : 1;
        unsigned int apl             : 2;
        VSStorageType storageType    : 2;
        unsigned int valid           : 1;
        unsigned int externalStorage : 1;
    }s;
    uint8_t v;
} VSStorageData;

typedef union
{
    struct
    {
        uint32_t      vsIndex         : 8;   
        uint32_t      vsSlotNum       : 8;
        VSStorageData vsStorageInfo      ;      
        VSSlotType    vsSlotType      : 3;
        uint32_t                      : 5;
    }s;
    uint32_t v;
} VSHeader;

typedef struct
{
    VSHeader     vsHeader;
    unsigned int validAfter;
    unsigned int validBefore;
    unsigned int dataSpecificMetaData;
} VSMetaData;  //VS Output Metadata


//Data Specific Meta Data 
//------------------------------------------------------------------------------
//RAW Data (Other types in their specific .h files)
typedef union 
{
    struct
    {
        unsigned short length;
        unsigned short : 16;
    } s;
    uint32_t v;
} VssRawType;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//VSS Symmetrical Key
typedef enum _VssSymmetricalKeyType
{
    VSS_SK_AES    = 0,
    VSS_SK_CHACHA = 1,
    VSS_SK_TDES   = 2,
    VSS_SK_DES    = 3
} VssSymmetricalKeyType;

//SYMMETRICAL KEY
typedef struct
{
    VssSymmetricalKeyType keyType : 3;
    unsigned int                  :29;
} VssSymmetricalKey;

//AES Key Types
typedef enum _VssSkAesType
{
    VSS_SK_AES_ECB = 0,
    VSS_SK_AES_CBC = 1,
    VSS_SK_AES_CTR = 2,
    VSS_SK_AES_CFB = 3,
    VSS_SK_AES_OFB = 4,
    VSS_SK_AES_XTS = 5,
    VSS_SK_AES_GCM = 6,
    VSS_SK_AES_CMAC = 7,
    VSS_SK_AES_CCM = 8
} VssSkAesType;

//AES Key Size
typedef enum _VssSkAesKeySize
{
    VSS_SK_AES_128 = 0,
    VSS_SK_AES_192 = 1,
    VSS_SK_AES_256 = 2
} VssSkAesKeySize;

//VS Data Specific Metadata AES Key
typedef struct
{
    struct {
        VssSymmetricalKeyType keyType : 3;
        unsigned char                 : 1;
        VssSkAesKeySize       keySize : 2;
        unsigned char                 : 2;
        VssSkAesType          aesType : 4;
        unsigned int                  : 20;
    } s;
    uint32_t v;
} VssSkAesKey;

typedef union
{
    unsigned char aes128bitKey[16];
    unsigned char aes192bitKey[24];
    unsigned char aes256bitKey[32];
} VssSkAesKeyData;

typedef enum _VssHashSigType
{
    VSS_HT_SHA1          = 2,
    VSS_HT_SHA2_224      = 3,
    VSS_HT_SHA2_256      = 4,
    VSS_HT_SHA2_384      = 5,
    VSS_HT_SHA2_512      = 6,
    VSS_HT_SHA1_HMAC     = 12,
    VSS_HT_SHA2_224_HMAC = 13,
    VSS_HT_SHA2_256_HMAC = 14,
    VSS_HT_SHA2_384_HMAC = 15,
    VSS_HT_SHA2_512_HMAC = 16,
    VSS_HT_VSS_SIGN      = 21
} VssHashSigType;

typedef enum _VssHashDataSize
{
    VSS_HT_SHA1_DATA_SIZE    = 20,
    VSS_HT_SHA_224_DATA_SIZE = 28,
    VSS_HT_SHA_256_DATA_SIZE = 32,
    VSS_HT_SHA_384_DATA_SIZE = 48,
    VSS_HT_SHA_512_DATA_SIZE = 64
} VssHashDataSize;

typedef union
{
    unsigned char sha1Hash[VSS_HT_SHA1_DATA_SIZE];
    unsigned char sha224Hash[VSS_HT_SHA_224_DATA_SIZE];
    unsigned char sha256Hash[VSS_HT_SHA_256_DATA_SIZE];
    unsigned char sha384Hash[VSS_HT_SHA_384_DATA_SIZE];
    unsigned char sha512Hash[VSS_HT_SHA_512_DATA_SIZE];
} VssHashDataUnion;


//VS Specific Hash Meta Data
typedef struct
{
    struct {
        VssHashSigType   hashType : 5;
        unsigned char             : 3;
        unsigned char    keySlot     ;
        unsigned int              :16;
    } s;
    uint32_t v;
} VssHashData;

//------------------------------------------------------------------------------
//Parse all VSM Command Data Specific Metadata Words 
typedef union _CmdVSMDataSpecificMetaData 
{
    //TODO: Asymmetric Keys
    VssSymmetricalKey symKeyMeta;
    VssHashData       hashMeta;
    VssRawType        rawMeta; 
    VssSkAesKey       aesSkMeta;
    uint32_t          v;
} CmdVSMDataSpecificMetaData;


 
//====================================================================== 
//VSM Commands
//====================================================================== 

typedef enum _cmdVSMCommandTypes
{
    CMD_VSM_INPUT_DATA = 0,
    CMD_VSM_OUTPUT_DATA = 1,
    CMD_VSM_BULK_INPUT_DATA = 2,
    CMD_VSM_BULK_OUTPUT_DATA = 3,
    CMD_VSM_DELETE_SLOT = 4,
    CMD_VSM_GET_SLOT_INFO = 5,
    CMD_VSM_SAVE_EXTERNAL_DATA = 6,
    CMD_VSM_LOAD_EXTERNAL_DATA = 7
} CmdVSMCommandTypes;

//NOTE cmdGroup = CMD_VSM; 
typedef struct
{
    CmdCommandGroups   cmdGroup      : 8;
    CmdVSMCommandTypes cmdType       : 8;
    unsigned char                    : 8;
    unsigned char      authInc       : 1;
    unsigned char      slotParamInc  : 1;
    unsigned char                    : 6;
} cmdVSMCommandHeader;


//====================================================================== 
//VSM Input Data Command
//====================================================================== 

typedef enum _cmdVSMSlotType
{
    CMD_VSS_RAW = 0,
    CMD_VSS_SYMMETRICALKEY = 1,
    CMD_VSS_ASYMMERTRICALKEY = 2,
    CMD_VSS_HASHVALUE = 3
} CmdVSMSlotType;

typedef enum _cmdVSMSlotInfoType
{
    CMD_VSM_NOT_ENCRYPTED = 0,
    CMS_VSM_ENCRYPTED = 1,
    CMD_VSM_WRAPPED_ENCRYPTED = 2,
} CmdVSMSlotInfoType;

typedef union 
{
    struct 
    { 
        CmdVSMSlotInfoType slotInfo          : 2;
        unsigned int                         : 2;
        CmdVSMSlotType     slotType          : 4;
        unsigned int       slotNumber        : 8;
        VSStorageData      vsStorageData;
        unsigned int       encrKeySlotNumber : 8;
    } s;
    uint32_t v;
} CmdVSMInputSlotInfoParameter1;

typedef struct _aes256wrappedKey 
    { unsigned char byte[48]; } aes256wrappedKey;

typedef struct _aesGcmTag 
    { unsigned char byte[16]; } aesGcmTag;

typedef uint32_t *  VSDataPtr;

//VSM Command Input Data MetaData
//--Parameter 1 replaces the VSHeader
typedef struct
{
    unsigned int inputLength;
    VSHeader     vsHeader;
    unsigned int validAfter;
    unsigned int validBefore;
    unsigned int dataSpecificMetaData;
} VSInputMetaData;


//NOTE:  This holds the data but does not inform the memory layout.
typedef struct
{
    aes256wrappedKey *     wrappedKey;
    unsigned int           slotDataLength;
    VSInputMetaData        vsMetaData;
    VSDataPtr              vsDataPtr;   //Pointer
    aesGcmTag *            aesGcmTag;
    CmdCommandAuthFooter * auth;
} CmdVSMInputSlotInfoInput1;

typedef struct
{
    CmdMailboxHeader mbHeader;
    CmdCommandHeader cmdHeader;
    CmdSGDescriptor* inputs;
    CmdSGDescriptor* outputs;
    CmdVSMInputSlotInfoParameter1 param1;
} CmdVSMInputDataRequest;

typedef struct
{
    CmdMailboxHeader mbHeader;
    CmdCommandHeader cmdHeader;
    CmdResultCodes resultCode;
} CmdVSMInputDataResponse;

//====================================================================== 
// VSM Data Output Command
//====================================================================== 

typedef union 
{
#pragma pack(push, 1)
    struct {
        CmdVSMSlotInfoType slotInfo      : 2;
        unsigned char                    : 6;
        unsigned char      slotNumber;      
        unsigned char                    : 8;
        unsigned char      keySlotNumber; //For encrypted data
    } s;
#pragma pack(pop)
    uint32_t v;
} CmdVSMOutputSlotParameter1;

typedef struct
{
    aes256wrappedKey * wrappedKey;
    VSMetaData         vsMetaData;
    VSDataPtr          vsDataPtr;   //Pointer
    aesGcmTag *        aesGcmTag;
} CmdVSMOutputSlotOutput1;

typedef struct
{
    unsigned short outputSize;
    unsigned short :16;
} CmdVSMOutputSlotResults;

typedef struct
{
    CmdMailboxHeader           mbHeader;
    CmdCommandHeader           cmdHeader;
    CmdSGDescriptor*           inputs;
    CmdSGDescriptor*           outputs;
    CmdVSMOutputSlotParameter1 param1;
} CmdVSMOutputDataRequest;

typedef struct
{
    CmdMailboxHeader        mbHeader;
    CmdCommandHeader        cmdHeader;
    CmdResultCodes          resultCode;
    CmdVSMOutputSlotResults size;
} CmdVSMOutputDataResponse;

//====================================================================== 
//VSM Get Slot Info Command
//====================================================================== 
typedef union 
{
    struct
    {
        unsigned char             :8;
        unsigned char slotNumber;
        unsigned short            : 16;
    } s;
    uint32_t v;
} CmdVSMGetSlotInfoParameter1;

typedef struct
{
    VSHeader     vsHeader;
    unsigned int keyMetaData;
} CmdVSMGetSlotInfoOutput;

//====================================================================== 
// VSMDeleteSlot Command
//====================================================================== 
typedef union 
{
    struct
    {
        unsigned char             :8;
        unsigned char  slotNumber;
        unsigned short            : 16;
    }s;
    uint32_t v;
} CmdVSMDeleteSlotParameter1;

typedef struct
{
    CmdMailboxHeader           mbHeader;
    CmdCommandHeader           cmdHeader;
    CmdSGDescriptor*           inputs;
    CmdSGDescriptor*           outputs;
    CmdVSMDeleteSlotParameter1 param1;
} cmdVSMDeleteSlotRequest;

typedef struct
{
    CmdMailboxHeader mbHeader;
    CmdCommandHeader cmdHeader;
    CmdResultCodes   resultCode;
} cmdVSMDeleteSlotResponse;

extern uint32_t ALIGN4        vsmInputDataRaw[];
extern uint32_t ALIGN4        vsmOutputDataRaw[];
extern uint32_t ALIGN4        vsmSlotInfoOut[4];


// *****************************************************************************
// *****************************************************************************
// Section: API Functions
// *****************************************************************************
// *****************************************************************************
CmdResultCodes HsmCmdVsmGetSlotInfo(int         vssSlotNum, 
                                    VSMetaData *vsMetaData, 
                                    uint32_t   *slotSizeBytes);

SLOTINFORETURN HsmCmdVsmPrintSlotInfo(int vssSlotNum);

RSP_DATA *  HsmCmdVsmDeleteSlot(int vssSlotNum); 

RSP_DATA *  HsmCmdVsmInputDataUnencrypted(
                int                        vssSlotNum,
                uint32_t *                 vsmInputDataPtr,
                unsigned short             numDataWords,
                CmdVSMSlotType             slotType,
                CmdVSMDataSpecificMetaData specMetaData);

RSP_DATA * HsmCmdVsmOutputDataUnencrypted(
                int        vssSlotNum, 
                uint32_t * dataOut,
                int      * dataLenBytes,
                uint32_t   maxDataBytes);

void        VsHeaderInfo(VSHeader * header);

void        VsmOutputDataInfo(
                uint32_t * vsmOutputData, 
                uint32_t maxDataWords,
                CmdVSMOutputSlotParameter1 param);

void        VsmInputDataInfo(
                uint32_t * vsmInputData, 
                CmdVSMInputSlotInfoParameter1 param);

// *****************************************************************************
// *****************************************************************************
// Section: globals/Functions
// *****************************************************************************
// *****************************************************************************

//extern uint32_t dummy32;

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _VSM_H */

/* *****************************************************************************
 End of File
 */
