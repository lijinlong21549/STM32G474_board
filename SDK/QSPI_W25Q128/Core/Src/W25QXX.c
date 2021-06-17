#ifndef __W25QXX_H__
#define __W25QXX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "stdio.h"
#include "W25QXX.h"
#include "usart.h"

#define HAL_QPSI_TIMEOUT_DEFAULT_VALUE 5000

/**
 * @brief  QSPI发送命令
 *
 * @param   instruction    要发送的指令
 * @param   address      发送到的目的地址
 * @param   dummyCycles    空指令周期数
 * @param   addressMode    地址模式; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINES,QSPI_ADDRESS_4_LINES
 * @param   dataMode    数据模式; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINES,QSPI_DATA_4_LINES
 * @param   dataSize        待传输的数据长度
 *
 * @return  uint8_t      QSPI_OK:正常
 *                      QSPI_ERROR:错误
 */
uint8_t QSPI_Send_CMD(uint32_t instruction, uint32_t address,uint32_t dummyCycles, 
                    uint32_t addressMode, uint32_t dataMode, uint32_t dataSize)
{
    QSPI_CommandTypeDef Cmdhandler;
    Cmdhandler.Instruction        = instruction;                   
    Cmdhandler.Address            = address;
    Cmdhandler.AlternateBytes     = 0x00;
    Cmdhandler.AddressSize        = QSPI_ADDRESS_24_BITS;
    Cmdhandler.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;                             
    Cmdhandler.DummyCycles        = dummyCycles;                   
    Cmdhandler.InstructionMode    = QSPI_INSTRUCTION_1_LINE;        
    Cmdhandler.AddressMode        = addressMode;
    Cmdhandler.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;                            
    Cmdhandler.DataMode           = dataMode;
    Cmdhandler.NbData             = dataSize;                                  
    Cmdhandler.DdrMode            = QSPI_DDR_MODE_DISABLE;             
    Cmdhandler.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
    Cmdhandler.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
    if(HAL_QSPI_Command(&hqspi1, &Cmdhandler, 5000) != HAL_OK)
		{
			return QSPI_ERROR;
		}
    return QSPI_OK;
}

uint16_t W25QXX_TYPE = W25Q128;
uint8_t W25QXX_BUFFER[4096];

/**
 * @brief  读取W25QXX芯片ID
 *
 * @param   void
 *
 * @return  uint16_t    返回值如下
 *         0XEF13,表示芯片型号为W25Q80
 *         0XEF14,表示芯片型号为W25Q16
 *         0XEF15,表示芯片型号为W25Q32
 *         0XEF16,表示芯片型号为W25Q64
 *         0XEF17,表示芯片型号为W25Q128
 *         0XEF18,表示芯片型号为W25Q256
 */
uint16_t W25QXX_ReadID(void)
{
    uint8_t ID[2];
    uint16_t deviceID;
    QSPI_Send_CMD(W25X_ManufactDeviceID, 0x00, 0, QSPI_ADDRESS_1_LINE, QSPI_DATA_1_LINE, sizeof(ID));
    HAL_QSPI_Receive(&hqspi1, ID, 5000);
		/*
		printf("Flash型号参数：");
		printf("%X",ID[0]);
		printf("%X\r\n",ID[1]);
		*/
    deviceID = (ID[0] << 8) | ID[1];
    return deviceID;
}

/**
 * @brief  读取SPI FLASH数据
 *
 * @param   pBuf      数据存储区
 * @param   ReadAddr    开始读取的地址(最大32bit)
 * @param   ReadSize      要读取的字节数(最大32bit)
 *
 * @return  void
 */
uint8_t W25QXX_Read(uint8_t *pBuf, uint32_t ReadAddr, uint32_t ReadSize)
{
  //QSPI_Send_CMD(W25X_ReadData, ReadAddr, 0, QSPI_ADDRESS_1_LINE, QSPI_DATA_1_LINE, ReadSize);
    QSPI_Send_CMD(W25X_FastReadQuad, ReadAddr, 8, QSPI_ADDRESS_1_LINE, QSPI_DATA_4_LINES, ReadSize);
    if(HAL_QSPI_Receive(&hqspi1, pBuf, 5000) != HAL_OK)
        return W25QXX_ERROR;
    return W25QXX_OK;
}

/**
 * @brief  等待空闲
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Wait_Busy(void)
{
    uint8_t status = 1;
    while((status & 0x01) == 0x01){
        QSPI_Send_CMD(W25X_ReadStatusReg1, 0x00, 0, QSPI_ADDRESS_NONE, QSPI_DATA_1_LINE, 1);
        HAL_QSPI_Receive(&hqspi1, &status, 5000);
    }
}

/**
 * @brief  擦除一个扇区
 *
 * @param   EraseAddr 要擦除的扇区地址
 *
 * @return  void
 */
void W25QXX_Erase_Sector(uint32_t EraseAddr)
{
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
    QSPI_Send_CMD(W25X_SectorErase, EraseAddr, 0, QSPI_ADDRESS_1_LINE, QSPI_DATA_NONE, 1);
    W25QXX_Wait_Busy();
}

/**
 * @brief  在指定地址开始写入最大一页的数据
 *
 * @param   pBuf      数据存储区
 * @param   WriteAddr    开始写入的地址(最大32bit)
 * @param   WriteSize      要写入的字节数(最大1 Page = 256 Bytes),该数不应该超过该页的剩余字节数
 *
 * @return  void
 */
void W25QXX_Write_Page(uint8_t *pBuf, uint32_t WriteAddr, uint32_t WriteSize)
{
    if(WriteSize > W25X_PAGE_SIZE)
        return;
    
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
    //QSPI_Send_CMD(W25X_PageProgram, WriteAddr, 0, QSPI_ADDRESS_1_LINE, QSPI_DATA_1_LINE, WriteSize);
    QSPI_Send_CMD(W25X_QuadPageProgram, WriteAddr, 0, QSPI_ADDRESS_1_LINE, QSPI_DATA_4_LINES, WriteSize);
    HAL_QSPI_Transmit(&hqspi1, pBuf, 5000);
    W25QXX_Wait_Busy();
}

/**
 * @brief  无检验写SPI FLASH
 *       必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
 *       具有自动换页功能
 *       在指定地址开始写入指定长度的数据,但是要确保地址不越界!
 *
 * @param   pBuf      数据存储区
 * @param   WriteAddr    开始写入的地址(最大32bit)
 * @param   WriteSize      要写入的字节数(最大32bit)
 *
 * @return  void
 */
void W25QXX_Write_NoCheck(uint8_t *pBuf, uint32_t WriteAddr, uint32_t WriteSize)
{
    uint32_t pageremain = W25X_PAGE_SIZE - WriteAddr % W25X_PAGE_SIZE; //单页剩余的字节数
    if(WriteSize <= pageremain)
        pageremain = WriteSize;
    while(1)
    {
        W25QXX_Write_Page(pBuf, WriteAddr, pageremain);
        if(WriteSize == pageremain)
            break;              //写入结束了
        else                    //WriteSize > pageremain
        {
            pBuf += pageremain;
            WriteAddr += pageremain;
            WriteSize -= pageremain;
            pageremain = (WriteSize > W25X_PAGE_SIZE) ? W25X_PAGE_SIZE : WriteSize;
        }
    }
}
/**
 * @brief  写SPI FLASH
 *       在指定地址开始写入指定长度的数据
 *       该函数带擦除操作!
 *
 * @param   pBuf      数据存储区
 * @param   WriteAddr    开始写入的地址(最大32bit)
 * @param   WriteSize      要写入的字节数(最大32bit)
 *
 * @return  void
 */
void W25QXX_Write(uint8_t *pBuf, uint32_t WriteAddr, uint32_t WriteSize)
{
    uint32_t secpos = WriteAddr / W25X_SECTOR_SIZE; //扇区地址
    uint32_t secoff = WriteAddr % W25X_SECTOR_SIZE; //在扇区内的偏移
    uint32_t secremain = W25X_SECTOR_SIZE - secoff; //扇区剩余空间大小
    uint32_t i;
    uint8_t * W25QXX_BUF = W25QXX_BUFFER;
    if(WriteSize <= secremain)
        secremain = WriteSize; //不大于4096个字节
    while(1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * W25X_SECTOR_SIZE, W25X_SECTOR_SIZE); //读出整个扇区的内容
        for(i = 0; i < secremain; i++) //校验数据
        {
            if(W25QXX_BUF[secoff + i] != 0XFF)
                break; //需要擦除
        }
        if(i < secremain) //需要擦除
        {
            W25QXX_Erase_Sector(secpos * W25X_SECTOR_SIZE);//擦除这个扇区
            for(i = 0; i < secremain; i++)   //复制
            {
                W25QXX_BUF[i + secoff] = pBuf[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * W25X_SECTOR_SIZE, W25X_SECTOR_SIZE); //写入整个扇区
        }
        else
            W25QXX_Write_NoCheck(pBuf, WriteAddr, secremain); //写已经擦除了的,直接写入扇区剩余区间.
        if(WriteSize == secremain)
            break; //写入结束了
        else//写入未结束
        {
            secpos++;       //扇区地址增1
            secoff = 0;     //偏移位置为0
            pBuf += secremain;          //指针偏移
            WriteAddr += secremain;     //写地址偏移
            WriteSize -= secremain;    //字节数递减
            secremain = (WriteSize > W25X_SECTOR_SIZE) ? W25X_SECTOR_SIZE : WriteSize;
        }
    }
}

void W25QXX_int(void)
{
	W25QXX_ReadID();

}
#ifdef __cplusplus
}
#endif

#endif /* __W25QXX_H__ */
