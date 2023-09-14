/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology

  @File Name
    app_button.h

  @Summary
    Button handler

  @Description
    Button handler.
 */
/* ************************************************************************** */

#ifndef _EXAMPLE_APP_BUTTON_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_APP_BUTTON_NAME_H


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#include "bsp/bsp.h"

#define BUTTON_DEBOUNCE_TICKS_MS   50
#define LONG_BUTTON_PRESS_TICKS_MS 1000

//Button States
typedef enum
{
    BUTTON_STATE_IDLE=0,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_BUTTON0_PRESSED,                   
    BUTTON_STATE_WAIT_FOR_RELEASE    
} BUTTON_STATES;

typedef struct _BUTTON
{
    uint32_t      volatile buttonDelay;
    BUTTON_STATES volatile buttonState;

    /* Switch state variables*/
    bool ignoreSwitchPress;

    /* Tracks switch press*/
    bool isSwitchPressed;
    
    /* Indicates the switch was pressed and now released */
    bool wasSwitchPressed; 
} BUTTON;

extern BUTTON button;

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
