/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_vdm_user.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_vdm_user.c file
  ******************************************************************************
  *
  * Copyright (c) 2018 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __USBPD_VDM_USER_H_
#define __USBPD_VDM_USER_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
/** @addtogroup STM32_USBPD_APPLICATION
  * @{
  */

/** @addtogroup STM32_USBPD_APPLICATION_VDM_USER
  * @{
  */

/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
USBPD_StatusTypeDef USBPD_VDM_UserInit(uint8_t PortNum);
void                USBPD_VDM_UserReset(uint8_t PortNum);
void                USBPD_VDM_UserCallback(uint8_t PortNum);
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_VDM_USER_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
