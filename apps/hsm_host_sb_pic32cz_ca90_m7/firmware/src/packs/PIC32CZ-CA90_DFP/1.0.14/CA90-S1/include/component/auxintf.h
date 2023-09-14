/*
 * Component description for AUXINTF
 *
 * Copyright (c) 2022 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software and any derivatives
 * exclusively with Microchip products. It is your responsibility to comply with third party license
 * terms applicable to your use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 * APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
 * EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/* file generated from device description version 2022-03-10T17:15:59Z */
#ifndef _PIC32CZCA90_AUXINTF_COMPONENT_H_
#define _PIC32CZCA90_AUXINTF_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR AUXINTF                                      */
/* ************************************************************************** */

/* -------- AUXINTF_AIOFFSET : (AUXINTF Offset: 0x00) (R/W 32) Auxiliary Interface Address Offset -------- */
#define AUXINTF_AIOFFSET_RESETVALUE           _UINT32_(0x00)                                       /*  (AUXINTF_AIOFFSET) Auxiliary Interface Address Offset  Reset Value */

#define AUXINTF_AIOFFSET_OFFSET_Pos           _UINT32_(0)                                          /* (AUXINTF_AIOFFSET) Address Offset Position */
#define AUXINTF_AIOFFSET_OFFSET_Msk           (_UINT32_(0xFFFFFFFF) << AUXINTF_AIOFFSET_OFFSET_Pos) /* (AUXINTF_AIOFFSET) Address Offset Mask */
#define AUXINTF_AIOFFSET_OFFSET(value)        (AUXINTF_AIOFFSET_OFFSET_Msk & (_UINT32_(value) << AUXINTF_AIOFFSET_OFFSET_Pos)) /* Assigment of value for OFFSET in the AUXINTF_AIOFFSET register */
#define AUXINTF_AIOFFSET_Msk                  _UINT32_(0xFFFFFFFF)                                 /* (AUXINTF_AIOFFSET) Register Mask  */


/* -------- AUXINTF_AIATTR : (AUXINTF Offset: 0x04) (R/W 32) Auxiliary Interface Attributes -------- */
#define AUXINTF_AIATTR_RESETVALUE             _UINT32_(0x00)                                       /*  (AUXINTF_AIATTR) Auxiliary Interface Attributes  Reset Value */

#define AUXINTF_AIATTR_NSM_Pos                _UINT32_(0)                                          /* (AUXINTF_AIATTR) Non Secure Master Position */
#define AUXINTF_AIATTR_NSM_Msk                (_UINT32_(0x1) << AUXINTF_AIATTR_NSM_Pos)            /* (AUXINTF_AIATTR) Non Secure Master Mask */
#define AUXINTF_AIATTR_NSM(value)             (AUXINTF_AIATTR_NSM_Msk & (_UINT32_(value) << AUXINTF_AIATTR_NSM_Pos)) /* Assigment of value for NSM in the AUXINTF_AIATTR register */
#define AUXINTF_AIATTR_Msk                    _UINT32_(0x00000001)                                 /* (AUXINTF_AIATTR) Register Mask  */


/** \brief AUXINTF register offsets definitions */
#define AUXINTF_AIOFFSET_REG_OFST      _UINT32_(0x00)      /* (AUXINTF_AIOFFSET) Auxiliary Interface Address Offset Offset */
#define AUXINTF_AIATTR_REG_OFST        _UINT32_(0x04)      /* (AUXINTF_AIATTR) Auxiliary Interface Attributes Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief AUXINTF register API structure */
typedef struct
{  /* Auxiliary Interface */
  __IO  uint32_t                       AUXINTF_AIOFFSET;   /**< Offset: 0x00 (R/W  32) Auxiliary Interface Address Offset */
  __IO  uint32_t                       AUXINTF_AIATTR;     /**< Offset: 0x04 (R/W  32) Auxiliary Interface Attributes */
} auxintf_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _PIC32CZCA90_AUXINTF_COMPONENT_H_ */
