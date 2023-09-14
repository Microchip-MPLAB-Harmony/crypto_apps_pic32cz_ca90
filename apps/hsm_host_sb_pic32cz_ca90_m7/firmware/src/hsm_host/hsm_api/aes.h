/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    aes.h

  @Summary
    AES Encrypt/Decrypt

  @Description
    AES Encrypt/Decrypt
 */
/* ************************************************************************** */

#ifndef _AES_H    /* Guard against multiple inclusion */
#define _AES_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdlib.h>
//#include <stdbool.h>
#include "hsm_command.h"
#include "hsm_host\hsm_command.h"
#include "hsm_host\hsm_command_globals.h"
#include "vsm.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */



// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

//AES Command
typedef enum _CmdAesKeySize
{
    CMD_AES_KEY_128 = 0,
    CMD_AES_KEY_192 = 1,
    CMD_AES_KEY_256 = 2
} CmdAesKeySize;

typedef enum _CmdAesCommandTypes
{
    CMD_AES_ENCRYPT     = 0,
    CMD_AES_DECRYPT     = 1,
    CMD_AES_GCM_ENCRYPT = 2,
    CMD_AES_GCM_DECRYPT = 3,
    CMD_AES_CMAC        = 4,
    CMD_AES_CCM_ENCRYPT = 5,
    CMD_AES_CCM_DECRYPT = 6
} CmdAesCommandTypes;

typedef enum _CmdAesMode
{
    CMD_AES_ECB = 0,
    CMD_AES_CBC = 1,
    CMD_AES_CTR = 2,
    CMD_AES_CFB = 3,
    CMD_AES_OFB = 4,
    CMD_AES_XTS = 5
} CmdAesMode;


typedef union 
{
    struct
    {
        CmdCommandGroups   cmdGroup:8;
        CmdAesCommandTypes cmdType:8;
        CmdAesKeySize      keySize:2;
        unsigned char      :6;
        unsigned char      authInc:1;
        unsigned char      slotParamInc:1;
        unsigned char      :6;
    } s;
    uint32_t v;
} CmdAesCommandHeader;

typedef union 
{
    struct
    {
        CmdCommandGroups   cmdGroup     :8;
        CmdAesCommandTypes cmdType      :8;
        CmdAesKeySize      keySize      :2;
        CmdAesMode         aesMode      :4;
        unsigned char                   :2;
        unsigned char      authInc      :1;
        unsigned char      slotParamInc :1;
        unsigned char                   :6;
    } s;
    uint32_t v;
} CmdAesEcbCommandHeader;


// cmdAes Encrypt/Decrypt Command
typedef struct
{
    uint32_t  dataSize;
} CmdAesEcbParameter1;

typedef union 
{
    struct 
    {
        unsigned char  useCtx       :1;
        unsigned char  resetCtxToIv :1;
        unsigned char               :6;
        unsigned char  slotIndex;
        unsigned short              :16;
    } s;
    uint32_t v;
} CmdAesEcbParameter2;

typedef struct
{
    CmdMailboxHeader       mbHeader;
    CmdAesEcbCommandHeader cmdHeader;
    CmdSGDescriptor*       inputs;
    CmdSGDescriptor*       outputs;
    CmdAesEcbParameter1    param1;
    CmdAesEcbParameter2    param2;
} CmdAesEcbRequest;

typedef struct
{
    CmdMailboxHeader    mbHeader;
    CmdAesCommandHeader cmdHeader;
    CmdResultCodes      resultCode;
} CmdAesEcbResponse;

// TODO:  Other AES Cmd Modes
// cmdAesGcmEcbCommand  

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

RSP_DATA * HsmCmdAesEcbEncryptDecrypt( 
    int              vsSlotNum, //Encryption/Decryption Key
    bool             encrypt,   //not decrypt
    uint32_t *       key,       //key
    CmdAesKeySize    keySize,
    uint32_t *       aesInputDataPtr,
    uint32_t *       aesOutputDataPtr,
    uint32_t         numDataWords);

extern int aesKeyLength[3];

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _AES_H */

/* *****************************************************************************
 End of File
 */
