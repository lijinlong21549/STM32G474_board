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
 * @brief  QSPI��������
 *
 * @param   instruction    Ҫ���͵�ָ��
 * @param   address      ���͵���Ŀ�ĵ�ַ
 * @param   dummyCycles    ��ָ��������
 * @param   addressMode    ��ַģʽ; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINES,QSPI_ADDRESS_4_LINES
 * @param   dataMode    ����ģʽ; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINES,QSPI_DATA_4_LINES
 * @param   dataSize        ����������ݳ���
 *
 * @return  uint8_t      QSPI_OK:����
 *                      QSPI_ERROR:����
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
 * @brief  ��ȡW25QXXоƬID
 *
 * @param   void
 *
 * @return  uint16_t    ����ֵ����
 *         0XEF13,��ʾоƬ�ͺ�ΪW25Q80
 *         0XEF14,��ʾоƬ�ͺ�ΪW25Q16
 *         0XEF15,��ʾоƬ�ͺ�ΪW25Q32
 *         0XEF16,��ʾоƬ�ͺ�ΪW25Q64
 *         0XEF17,��ʾоƬ�ͺ�ΪW25Q128
 *         0XEF18,��ʾоƬ�ͺ�ΪW25Q256
 */
uint16_t W25QXX_ReadID(void)
{
    uint8_t ID[2];
    uint16_t deviceID;
    QSPI_Send_CMD(W25X_ManufactDeviceID, 0x00, 0, QSPI_ADDRESS_1_LINE, QSPI_DATA_1_LINE, sizeof(ID));
    HAL_QSPI_Receive(&hqspi1, ID, 5000);
		/*
		printf("Flash�ͺŲ�����");
		printf("%X",ID[0]);
		printf("%X\r\n",ID[1]);
		*/
    deviceID = (ID[0] << 8) | ID[1];
    return deviceID;
}

/**
 * @brief  ��ȡSPI FLASH����
 *
 * @param   pBuf      ���ݴ洢��
 * @param   ReadAddr    ��ʼ��ȡ�ĵ�ַ(���32bit)
 * @param   ReadSize      Ҫ��ȡ���ֽ���(���32bit)
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
 * @brief  �ȴ�����
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
 * @brief  ����һ������
 *
 * @param   EraseAddr Ҫ������������ַ
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
 * @brief  ��ָ����ַ��ʼд�����һҳ������
 *
 * @param   pBuf      ���ݴ洢��
 * @param   WriteAddr    ��ʼд��ĵ�ַ(���32bit)
 * @param   WriteSize      Ҫд����ֽ���(���1 Page = 256 Bytes),������Ӧ�ó�����ҳ��ʣ���ֽ���
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
 * @brief  �޼���дSPI FLASH
 *       ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
 *       �����Զ���ҳ����
 *       ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
 *
 * @param   pBuf      ���ݴ洢��
 * @param   WriteAddr    ��ʼд��ĵ�ַ(���32bit)
 * @param   WriteSize      Ҫд����ֽ���(���32bit)
 *
 * @return  void
 */
void W25QXX_Write_NoCheck(uint8_t *pBuf, uint32_t WriteAddr, uint32_t WriteSize)
{
    uint32_t pageremain = W25X_PAGE_SIZE - WriteAddr % W25X_PAGE_SIZE; //��ҳʣ����ֽ���
    if(WriteSize <= pageremain)
        pageremain = WriteSize;
    while(1)
    {
        W25QXX_Write_Page(pBuf, WriteAddr, pageremain);
        if(WriteSize == pageremain)
            break;              //д�������
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
 * @brief  дSPI FLASH
 *       ��ָ����ַ��ʼд��ָ�����ȵ�����
 *       �ú�������������!
 *
 * @param   pBuf      ���ݴ洢��
 * @param   WriteAddr    ��ʼд��ĵ�ַ(���32bit)
 * @param   WriteSize      Ҫд����ֽ���(���32bit)
 *
 * @return  void
 */
void W25QXX_Write(uint8_t *pBuf, uint32_t WriteAddr, uint32_t WriteSize)
{
    uint32_t secpos = WriteAddr / W25X_SECTOR_SIZE; //������ַ
    uint32_t secoff = WriteAddr % W25X_SECTOR_SIZE; //�������ڵ�ƫ��
    uint32_t secremain = W25X_SECTOR_SIZE - secoff; //����ʣ��ռ��С
    uint32_t i;
    uint8_t * W25QXX_BUF = W25QXX_BUFFER;
    if(WriteSize <= secremain)
        secremain = WriteSize; //������4096���ֽ�
    while(1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * W25X_SECTOR_SIZE, W25X_SECTOR_SIZE); //������������������
        for(i = 0; i < secremain; i++) //У������
        {
            if(W25QXX_BUF[secoff + i] != 0XFF)
                break; //��Ҫ����
        }
        if(i < secremain) //��Ҫ����
        {
            W25QXX_Erase_Sector(secpos * W25X_SECTOR_SIZE);//�����������
            for(i = 0; i < secremain; i++)   //����
            {
                W25QXX_BUF[i + secoff] = pBuf[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * W25X_SECTOR_SIZE, W25X_SECTOR_SIZE); //д����������
        }
        else
            W25QXX_Write_NoCheck(pBuf, WriteAddr, secremain); //д�Ѿ������˵�,ֱ��д������ʣ������.
        if(WriteSize == secremain)
            break; //д�������
        else//д��δ����
        {
            secpos++;       //������ַ��1
            secoff = 0;     //ƫ��λ��Ϊ0
            pBuf += secremain;          //ָ��ƫ��
            WriteAddr += secremain;     //д��ַƫ��
            WriteSize -= secremain;    //�ֽ����ݼ�
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
