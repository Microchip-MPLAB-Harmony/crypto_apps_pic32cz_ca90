/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    hash.h

  @Summary

  @Description
 */
/* ************************************************************************** */

#include <stdint.h>
#include <stdbool.h>
#include "hsm_host/hsm_command.h"
#include "hsm_host/hsm_command_globals.h"
#include "vsm.h"

#ifndef _HASH_H
#define _HASH_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_NUMBYTES        (256/8)

typedef enum _CmdHashCommandTypes
{
    CMD_HASH_HASH_BLOCK    = 0,
    CMD_HASH_HASH_INIT     = 1,
    CMD_HASH_HASH_UPDATE   = 2,
    CMD_HASH_HASH_FINALIZE = 3,
    CMD_HASH_HMAC          = 4,
    CMD_HASH_VALIDATE      = 5,
} CmdHashCommandTypes;

typedef union 
{
    struct {
        CmdCommandGroups    cmdGroup     :8;
        CmdHashCommandTypes cmdType      :8;
        unsigned char                    :8;
        unsigned char       authInc      :1;
        unsigned char       slotParamInc :1;
        unsigned char                    :6;
    }s;
    uint32_t w;
} CmdHashCommandHeader;

typedef enum _CmdHashTypes
{
    CMD_HASH_MD5    = 1,
    CMD_HASH_SHA1   = 2,
    CMD_HASH_SHA224 = 3,
    CMD_HASH_SHA256 = 4,
    CMD_HASH_SHA384 = 5,
    CMD_HASH_SHA512 = 6,
} CmdHashTypes;

typedef struct
{
    CmdCommandGroups cmdGroup  :8;
    CmdHashCommandTypes cmdType:8;
    CmdHashTypes hashType      :4;
    unsigned char              :4;
    unsigned char authInc      :1;
    unsigned char slotParamInc :1;
    unsigned char              :6;
} CmdHashHashBlockHeader;

typedef struct
{
    unsigned int inputLength;
} CmdHashBlockHashParameter1;

typedef  union
{
    
    struct {
        unsigned char             :8;
        unsigned char slotIndex     ;
        VSStorageData storageData   ;
        unsigned short            :8;
    }s;
    uint32_t w;
} CmdHashBlockHashParameter2;

typedef enum _CmdHashValueSizes
{
    CMD_HASH_MD5_SIZE    = 16,
    CMD_HASH_SHA1_SIZE   = 20,
    CMD_HASH_SHA224_SIZE = 28,
    CMD_HASH_SHA256_SIZE = 32,
    CMD_HASH_SHA384_SIZE = 48,
    CMD_HASH_SHA512_SIZE = 64
}CmdHashValueSizes;

typedef union
{
    struct {
        CmdHashValueSizes hashSize : 8;
        unsigned int               : 24;
    }s;
    uint32_t w;
} CmdHashBlockHashResults;

typedef struct
{
    CmdMailboxHeader           mbHeader;
    CmdHashHashBlockHeader     cmdHeader;
    CmdSGDescriptor*           inputs;
    CmdSGDescriptor*           outputs;
    CmdHashBlockHashParameter1 param1;
    CmdHashBlockHashParameter2 param2;
} CmdHashBlockHashRequest;

typedef struct
{
    CmdMailboxHeader mbHeader;
    CmdHashCommandHeader cmdHeader;
    CmdResultCodes resultCode;
    CmdHashBlockHashResults size;
} CmdHashBlockHashResponse;

extern uint8_t  ALIGN4   hashInitBuffer[64];
extern uint8_t  ALIGN4   expHashBlockResult[SHA256_NUMBYTES];

RSP_DATA * HsmCmdHashBlockSha256(uint8_t * dataIn,  
                                 int       numDataInBytes, 
                                 uint8_t * dataOut);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif //_HASH_H

/* *****************************************************************************
 End of File
 */