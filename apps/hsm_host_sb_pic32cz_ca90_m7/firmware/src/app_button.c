/* ************************************************************************** */
/** 

  @Company
    Microchip Technology 
 

  @File Name
    app_button.c

  @Summary
    Button handling 

  @Description
    Button handling
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "app.h"
#include "app_button.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

BUTTON button =  { 
    .buttonDelay = 0, 
    .isSwitchPressed = false,
    .buttonState = BUTTON_STATE_IDLE,
    .ignoreSwitchPress = false,
    .wasSwitchPressed = false,
};


//******************************************************************************
//******************************************************************************
void APP_Button_Tasks()
{
   //BUTTON PROCESSING
    /* Check the buttons' currentPlayBuffer state. */      
    switch ( button.buttonState )
    {
        case BUTTON_STATE_IDLE:
        {
            button.isSwitchPressed  = false;
            button.wasSwitchPressed = false;
            if ( (button.buttonDelay==0)&&
                 (SWITCH_Get()==SWITCH_STATE_PRESSED))                
            {
                button.buttonDelay=BUTTON_DEBOUNCE_TICKS_MS;       
                button.buttonState=BUTTON_STATE_PRESSED;               
            }
        }
        break;
        
        case BUTTON_STATE_PRESSED:
        { 
            if (button.buttonDelay>0)
            {
                break;      // still debouncing
            }
            
            if(SWITCH_Get()==SWITCH_STATE_PRESSED) 
            {                
                button.isSwitchPressed  = true;
                button.wasSwitchPressed = false;
                button.buttonDelay=LONG_BUTTON_PRESS_TICKS_MS;          // 1 sec is long press
                button.buttonState=BUTTON_STATE_BUTTON0_PRESSED;                  
            }
        }
        break;
          
        //Short Press - change volume/mute on/off
        //Long Press - volume/mute mode
        case BUTTON_STATE_BUTTON0_PRESSED:
        {
            //Wait for button to be released
            if ((button.buttonDelay>0)&&
                (SWITCH_Get()!=SWITCH_STATE_PRESSED))     // SW01 pressed and released < 1 sec
            {
                //SHORT PRESS - Button 1
                button.isSwitchPressed  = false;
                button.wasSwitchPressed = true;
                //New volume
                //button.volumeIndex++;
                //if (button.volumeIndex >= VOLUME_STEPS)
                //{
                //    button.volumeIndex = 0;    
                //}
                //button.volume = volumeLevels[button.volumeIndex];
                
                //if (0==button.volume)
                //{
                //    button.ledState = LED_MUTE_BLINK;
                //    button.muteEn = true;
                    //DRV_CODEC_MuteOn(button.codecClientWriteRead.handle);
                //}
                //else
                //{
                //    button.ledState = LED_ON;
                //    button.muteEn = false;
                    //DRV_CODEC_MuteOff(button.codecClientWriteRead.handle);
                    //DRV_CODEC_VolumeSet(button.codecClientWriteRead.handle, 
                    //                   DRV_CODEC_CHANNEL_LEFT_RIGHT, 
                    //                   button.volume);

                //}

                //Set the debounce value
                button.buttonDelay=BUTTON_DEBOUNCE_TICKS_MS;                
                button.buttonState=BUTTON_STATE_IDLE;              
            }
            else if ((button.buttonDelay==0)&&
                     (SWITCH_Get()==SWITCH_STATE_PRESSED))  
            {
                //LONG PRESS - Button 1
                button.isSwitchPressed  = false;
                button.wasSwitchPressed = true;
                //if (button.muteEn == true)
                //{
                //    button.ledState = LED_ON;
                //    button.muteEn   =false;
                //    DRV_CODEC_MuteOff(button.codecClientWriteRead.handle);
                //}
                //else
                //{
                //    button.ledState = MUTE_BLINK;
                //    button.muteEn   =true;
                //    DRV_CODEC_MuteOn(button.codecClientWriteRead.handle);
                //}
            }                          
        } 
        break;

        //NOT USED
        case BUTTON_STATE_WAIT_FOR_RELEASE:
        {
            if (SWITCH_Get()!=SWITCH_STATE_PRESSED)
            {
                button.buttonDelay=BUTTON_DEBOUNCE_TICKS_MS;
                button.buttonState=BUTTON_STATE_IDLE;
            }
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
} //End APP_Button_Tasks()


/* *****************************************************************************
 End of File
 */
