

/* Includes ------------------------------------------------------------------*/
#include "spi_driver.h"
#include "lis3dh_driver.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include <string.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "StepDetector.h"

uint8_t response, lis3dhAddress;
AxesRaw_t dataXYZ;

void lis3dh_Init(void)
{
	/* lis3dh 引脚配置 */
  if(LIS3DH_GetWHO_AM_I(&lis3dhAddress))
  {
     NRF_LOG_INFO("lis3dhAddress =  %x \n\r\0", lis3dhAddress);  
  }
  else
  {
     return ;
  }
  
  /* SPI 初始化 */
  
  
  
  //Inizialize MEMS Sensor
  
  
  
  //set ODR (turn ON device)
  response = LIS3DH_SetODR(LIS3DH_ODR_50Hz);
  if(response==1){
       NRF_LOG_INFO("\n\rSET_ODR_OK    \n\r\0");       
  }
  //set PowerMode 
  response = LIS3DH_SetMode(LIS3DH_NORMAL);
  if(response==1){
        NRF_LOG_INFO("SET_MODE_OK     \n\r\0");       
  }
  //set Fullscale
  response = LIS3DH_SetFullScale(LIS3DH_FULLSCALE_2);
  if(response==1){
        NRF_LOG_INFO("SET_FULLSCALE_OK\n\r\0");
        
  }
  //set axis Enable
  response = LIS3DH_SetAxis(LIS3DH_X_ENABLE | LIS3DH_Y_ENABLE | LIS3DH_Z_ENABLE);
  if(response==1){
        NRF_LOG_INFO("SET_AXIS_OK     \n\r\0");       
  }


}

void lis3dh_example1(void)
{
    response = LIS3DH_GetAccAxesRaw(&dataXYZ);
    if(response==1)
    {
        //print data values
        //NRF_LOG_INFO( "X=%6d Y=%6d Z=%6d \r\n", dataXYZ.AXIS_X, dataXYZ.AXIS_Y, dataXYZ.AXIS_Z);
        onSensorChanged(dataXYZ.AXIS_X, dataXYZ.AXIS_Y, dataXYZ.AXIS_Z);
    }
}


void lis3dh_example2(void)
{
  
  
}


