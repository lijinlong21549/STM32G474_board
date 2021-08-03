#include "BNO055.h"
#include "i2c.h"
#include "usart.h"

#define BNO055_IIC_ADD 0x28
#define BNO055_Interface IIC
//通过地址位，来定义八位读写地址
#if BNO055_IIC_ADD == 0x28
#define BNO055_read 0x51
#define BNO055_write 0x50
#elif BNO055_IIC_ADD == 0x29
#define BNO055_read 0x53
#define BNO055_write 0x52
#endif


/**
  * @brief BNO055的写操作
  * @param Register_page:寄存器所在的页
  * @param Register:寄存器地址
  * @param DATA:所写的数据
  * @retval 操作结果
  */
int BNO055_Write(int Register_page,uint8_t Register,uint8_t DATA)
{
		
    return 0;
}

/**
  * @brief BNO055的读操作
  * @param Register_page:寄存器所在的页
  * @param Register:寄存器地址
  * @param DATA:所读取的数据
  * @retval 操作结果
  */
int BNO055_Read(int Register_page,uint8_t Register,uint8_t *DATA)
{
    if(HAL_I2C_Master_Transmit(&hi2c1,BNO055_write,&Register,1,1000)== HAL_OK)
		{
				printf("发送失败\r\n");
		}
		else
		{
			
		}
    HAL_I2C_Master_Receive(&hi2c1,BNO055_read,DATA,1,1000);
    return 0;
}

/**
  * @brief BNO055初始化操作
  * @retval 操作结果
  */
int BNO055_Init()
{

	HAL_GPIO_WritePin(BNO055_RST_GPIO_Port,BNO055_RST_Pin, GPIO_PIN_RESET);
	printf("复位BNO055\r\n");
	HAL_Delay(50);
	HAL_GPIO_WritePin(BNO055_RST_GPIO_Port,BNO055_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	
	uint8_t Register=0xA0;

	if(HAL_I2C_Master_Transmit(&hi2c1,BNO055_IIC_ADD>>1,&Register,2,1000)!= HAL_OK)
	{
			printf("发送失败\r\n");
	}
	else
	{
		
	}
	HAL_Delay(1);
	uint8_t DATA;
	HAL_I2C_Master_Receive(&hi2c1,BNO055_IIC_ADD>>1,&DATA,1,1000);
	printf("%X\r\n",DATA);
	return 0;
}


