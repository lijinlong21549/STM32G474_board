#ifndef __BNO055_H__
#define __BNO055_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

int BNO055_Read(int Register_page,uint8_t Register,uint8_t *DATA);
int BNO055_Write(int Register_page,uint8_t Register,uint8_t DATA);
int BNO055_Init(void);




#ifdef __cplusplus
}
#endif

#endif /* __BNO055_H__ */
