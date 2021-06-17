/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W25QXX_H
#define __W25QXX_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "quadspi.h"

extern uint16_t W25QXX_TYPE;
// W25Q128 Page size and Sector size
#define W25X_PAGE_SIZE      256
#define W25X_SECTOR_SIZE    4096
// W25QXX API function declaration
#define W25QXX_Write_Enable()   QSPI_Send_CMD(W25X_WriteEnable, 0x00, 0, QSPI_ADDRESS_NONE, QSPI_DATA_NONE, 1)
#define W25QXX_Write_Disable()  QSPI_Send_CMD(W25X_WriteDisable, 0x00, 0, QSPI_ADDRESS_NONE, QSPI_DATA_NONE, 1)

//W25QX chip ID
#define W25Q80   0XEF13   
#define W25Q16   0XEF14
#define W25Q32   0XEF15
#define W25Q64   0XEF16
#define W25Q128  0XEF17
#define W25Q256 0XEF18
//W25X instruction list
#define W25X_WriteEnable    0x06 
#define W25X_WriteDisable    0x04 
#define W25X_ReadStatusReg1    0x05 
#define W25X_ReadStatusReg2    0x35 
#define W25X_ReadStatusReg3    0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData      0x03 
#define W25X_FastReadData    0x0B 
#define W25X_FastReadDual    0x3B
#define W25X_FastReadQuad       0x6B 
#define W25X_PageProgram    0x02
#define W25X_QuadPageProgram    0x32 
#define W25X_BlockErase      0xD8 
#define W25X_SectorErase    0x20 
#define W25X_ChipErase      0xC7 
#define W25X_PowerDown      0xB9 
#define W25X_ReleasePowerDown  0xAB 
#define W25X_DeviceID      0xAB 
#define W25X_ManufactDeviceID  0x90 
#define W25X_JedecDeviceID    0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9
#define W25X_SetReadParam    0xC0 
#define W25X_EnterQPIMode       0x38
#define W25X_ExitQPIMode        0xFF
//W25QXX Error Code
#define W25QXX_OK     0
#define W25QXX_ERROR  1
#define QSPI_OK     0
#define QSPI_ERROR  1
// W25QXX API function declaration
uint8_t QSPI_Send_CMD(uint32_t instruction, uint32_t address,uint32_t dummyCycles, uint32_t addressMode, uint32_t dataMode, uint32_t dataSize);

void W25QXX_int(void);
uint16_t W25QXX_ReadID(void);
void W25QXX_Wait_Busy(void);
void W25QXX_Erase_Sector(uint32_t EraseAddr);
uint8_t W25QXX_Read(uint8_t *pBuf, uint32_t ReadAddr, uint32_t ReadSize);
void W25QXX_Write(uint8_t *pBuf, uint32_t WriteAddr, uint32_t WriteSize);

#ifdef __cplusplus
}
#endif

#endif /* __W25Qx_H */

