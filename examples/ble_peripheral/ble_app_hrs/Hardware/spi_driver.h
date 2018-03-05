#ifndef _SPI_DRIVER_H
#define _SPI_DRIVER_H


#include <stdbool.h>
#include <stdint.h>


#define KX022_PIN_SCL         15    //SPI and I2C Serial Clock
#define KX022_PIN_ADDR_MISO   14    //Serial Data Out pin during 4 wire SPI communication and part of the device address during I2C communication.
#define KX022_PIN_SDA_MOSI    13    //SPI Data input / I2C Serial Data
#define KX022_PIN_IO_VDD      12    //The power supply input for the digital communication bus.
#define KX022_PIN_TRIG        11    //Trigger pin for FIFO buffer control
#define KX022_PIN_INT1        10
#define KX022_PIN_INT2        9
#define KX022_PIN_VDD_C_GPIO  8     //供电引脚 The power supply input.
#define KX022_PIN_CS          16    // GND:SPI   VCC:IIC


#define LIS3DH_PIN_SPC         27   
#define LIS3DH_PIN_SDI         26   
#define LIS3DH_PIN_SDO         25  
#define LIS3DH_PIN_CS          24   
#define LIS3DH_PIN_INT1        23
#define LIS3DH_PIN_INT2        22
#define LIS3DH_PIN_VDD         8     //供电引脚 The power supply input.

/* 三轴加速度传感器SPI */
//#define SPI_SCK_PIN           KX022_PIN_SCL
//#define SPI_MISO_PIN          KX022_PIN_ADDR_MISO
//#define SPI_MOSI_PIN          KX022_PIN_SDA_MOSI
//#define SPI_SS_PIN            KX022_PIN_CS

#define SPI_SCK_PIN           LIS3DH_PIN_SPC 
#define SPI_MISO_PIN          LIS3DH_PIN_SDO 
#define SPI_MOSI_PIN          LIS3DH_PIN_SDI
#define SPI_SS_PIN            LIS3DH_PIN_CS


#define SPI_READBIT           (0x80)


void    SPI_Init(void) ;
uint8_t SPI_WriteReg(uint8_t WriteAddr, uint8_t Data) ;
uint8_t SPI_ReadReg(uint8_t Reg, uint8_t* Data) ;


#endif /* _SPI_DRIVER_H */


