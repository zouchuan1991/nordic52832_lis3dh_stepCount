/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : example_main.c
* Author             : MSH Application Team
* Author             : Abhishek Anand,Fabio Tota
* Revision           : $Revision: 1.5 $
* Date               : $Date: 16/06/2011 12:19:08 $
* Description        : Example main file for MKI109V1 board
* HISTORY:
* Date        | Modification                                | Author
* 16/06/2011  | Initial Revision                            | Fabio Tota
* 11/06/2012  |	Support for multiple drivers in the same program |	Abhishek Anand

********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
//include files for MKI109V1 board 
#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "led.h"
#include "button.h"
#include "adc_mems.h"
#include "string.h"
#include "spi_mems.h"
#include <stdio.h>

//include MEMS driver
#include "lis3dh_driver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t response;
uint8_t USBbuffer[64];

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//define for example1 or example2
//#define __EXAMPLE1__H 
#define __EXAMPLE2__H 

/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
  uint8_t buffer[26]; 
  uint8_t position=0, old_position=0;
  AxesRaw_t data;
  //Initialize your hardware here
  
  //function for MKI109V1 board 
  InitHardware();
  SPI_Mems_Init();
  
  EKSTM32_LEDOff(LED1);
  EKSTM32_LEDOff(LED2);
  EKSTM32_LEDOff(LED3);    
 
  //wait until the USB is ready (MKI109V1 board)
  while(bDeviceState != CONFIGURED);
 
  //Inizialize MEMS Sensor
  //set ODR (turn ON device)
  response = LIS3DH_SetODR(LIS3DH_ODR_100Hz);
  if(response==1){
        sprintf((char*)buffer,"\n\rSET_ODR_OK    \n\r\0");
        USB_SIL_Write(EP1_IN, buffer, 19);
        SetEPTxValid(ENDP1);
  }
  //set PowerMode 
  response = LIS3DH_SetMode(LIS3DH_NORMAL);
  if(response==1){
        sprintf((char*)buffer,"SET_MODE_OK     \n\r\0");
        USB_SIL_Write(EP1_IN, buffer, 19);
        SetEPTxValid(ENDP1);
  }
  //set Fullscale
  response = LIS3DH_SetFullScale(LIS3DH_FULLSCALE_2);
  if(response==1){
        sprintf((char*)buffer,"SET_FULLSCALE_OK\n\r\0");
        USB_SIL_Write(EP1_IN, buffer, 19);
        SetEPTxValid(ENDP1);
  }
  //set axis Enable
  response = LIS3DH_SetAxis(LIS3DH_X_ENABLE | LIS3DH_Y_ENABLE | LIS3DH_Z_ENABLE);
  if(response==1){
        sprintf((char*)buffer,"SET_AXIS_OK     \n\r\0");
        USB_SIL_Write(EP1_IN, buffer, 19);
        SetEPTxValid(ENDP1);
  }

  
/*********************/  
/******Example 1******/ 
#ifdef __EXAMPLE1__H 
  while(1){
  //get Acceleration Raw data  
  response = LIS3DH_GetAccAxesRaw(&data);
  if(response==1){
    //print data values
    //function for MKI109V1 board 
    EKSTM32_LEDToggle(LED1);
    sprintf((char*)buffer, "X=%6d Y=%6d Z=%6d \r\n", data.AXIS_X, data.AXIS_Y, data.AXIS_Z);
    USB_SIL_Write(EP1_IN, buffer, 29);
    SetEPTxValid(ENDP1);  
    old_position = position;
  }
 }
#endif /* __EXAMPLE1__H  */ 
 
 
/*********************/
/******Example 2******/
#ifdef __EXAMPLE2__H
 //configure Mems Sensor
 //set Interrupt Threshold 
 response = LIS3DH_SetInt1Threshold(20);
 if(response==1){
        sprintf((char*)buffer,"SET_THRESHOLD_OK\n\r\0");
        USB_SIL_Write(EP1_IN, buffer, 19);
        SetEPTxValid(ENDP1);
  }
 //set Interrupt configuration (all enabled)
 response = LIS3DH_SetIntConfiguration(LIS3DH_INT1_ZHIE_ENABLE | LIS3DH_INT1_ZLIE_ENABLE |
									   LIS3DH_INT1_YHIE_ENABLE | LIS3DH_INT1_YLIE_ENABLE |
									   LIS3DH_INT1_XHIE_ENABLE | LIS3DH_INT1_XLIE_ENABLE ); 
 if(response==1){
        sprintf((char*)buffer,"SET_INT_CONF_OK \n\r\0");
        USB_SIL_Write(EP1_IN, buffer, 19);
        SetEPTxValid(ENDP1);
  }
 //set Interrupt Mode
 response = LIS3DH_SetIntMode(LIS3DH_INT_MODE_6D_POSITION);
 if(response==1){
        sprintf((char*)buffer,"SET_INT_MODE    \n\r\0");
        USB_SIL_Write(EP1_IN, buffer, 19);
        SetEPTxValid(ENDP1);
  }

 while(1){
  //get 6D Position
  response = LIS3DH_Get6DPosition(&position);
  if((response==1) && (old_position!=position)){
    EKSTM32_LEDToggle(LED1);
    switch (position){
    case LIS3DH_UP_SX:   sprintf((char*)buffer,"\n\rposition = UP_SX  \n\r\0");   break;
    case LIS3DH_UP_DX:   sprintf((char*)buffer,"\n\rposition = UP_DX  \n\r\0");   break;
    case LIS3DH_DW_SX:   sprintf((char*)buffer,"\n\rposition = DW_SX  \n\r\0");   break;              
    case LIS3DH_DW_DX:   sprintf((char*)buffer,"\n\rposition = DW_DX  \n\r\0");   break; 
    case LIS3DH_TOP:     sprintf((char*)buffer,"\n\rposition = TOP    \n\r\0");   break; 
    case LIS3DH_BOTTOM:  sprintf((char*)buffer,"\n\rposition = BOTTOM \n\r\0");   break; 
    default:      sprintf((char*)buffer,"\n\rposition = unknown\n\r\0");   break;
    }
  
  //function for MKI109V1 board   
  USB_SIL_Write(EP1_IN, buffer, 23);
  SetEPTxValid(ENDP1);  
  old_position = position;
  }
 }
#endif /*__EXAMPLE2__H */ 
 
} // end main


#ifdef USE_FULL_ASSERT


//function for MKI109V1 board 
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
