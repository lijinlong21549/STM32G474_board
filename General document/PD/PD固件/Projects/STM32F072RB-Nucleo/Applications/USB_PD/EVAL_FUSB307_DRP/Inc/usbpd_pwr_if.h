/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_pwr_if.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of usbpd_pw_if.h.
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

#ifndef __USBPD_PW_IF_H_
#define __USBPD_PW_IF_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbpd_def.h"

/** @addtogroup STM32_USBPD_APPLICATION
  * @{
  */

/** @addtogroup STM32_USBPD_APPLICATION_POWER_IF
  * @{
  */

/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define BSP_PWR_INVALID_VALUE           0xFFFFFFFFu   /* Invalid value set during issue with voltage setting */
#define BSP_PWR_TIMEOUT_PDO             250u          /* Timeout for PDO to PDO or PDO to APDO at 250ms */

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/** @defgroup USBPD_USER_PWR_IF_Exported_Macros USBPD PWR IF Exported Macros
  * @{
  */

/* enumeration of the different power status available for VBUS */    
typedef enum{
  USBPD_PWR_BELOWVSAFE0V,
  USBPD_PWR_VSAFE5V,
  USBPD_PWR_SNKDETACH
} USBPD_VBUSPOWER_STATUS;

/* Macros used to convert values into PDO representation */
#define PWR_V_20MV(_V_)        ((uint16_t)(( (_V_) * 1000) / 20))   /* From Volt to 20mV multiples      */
#define PWR_V_50MV(_V_)        ((uint16_t)(( (_V_) * 1000) / 50))   /* From Volt to 50mV multiples      */
#define PWR_V_100MV(_V_)       ((uint16_t)(( (_V_) * 1000) / 100))  /* From Volt to 100mV multiples     */
#define PWR_A_10MA(_A_)        ((uint16_t)(( (_A_) * 1000) / 10))   /* From Ampere to 10mA multiples    */
#define PWR_A_50MA(_A_)        ((uint16_t)(( (_A_) * 1000) / 50))   /* From Ampere to 50mA multiples    */
#define PWR_W(_W_)             ((uint16_t)(( (_W_) * 1000) / 250))  /* From Watt to 250mW multiples     */

/* Macros used to get values from PDO representation */
#define PWR_DECODE_50MV(_Value_)           ((uint16_t)(((_Value_) * 50)))     /* From 50mV multiples to mV        */
#define PWR_DECODE_100MV(_Value_)          ((uint16_t)(((_Value_) * 100)))    /* From 100mV multiples to mV       */
#define PWR_DECODE_10MA(_Value_)           ((uint16_t)(((_Value_) * 10)))     /* From 10mA multiples to mA        */
#define PWR_DECODE_50MA(_Value_)           ((uint16_t)(((_Value_) * 50)))     /* From 50mA multiples to mA        */
#define PWR_DECODE_MW(_Value_)             ((uint16_t)(((_Value_) * 250)))    /* From 250mW multiples to mW       */

#define USBPD_PORT_IsValid(__Port__) ((__Port__) < (USBPD_PORT_COUNT))

/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup STM32_USBPD_APPLICATION_POWER_IF_Exported_Functions USBPD PWR IF Exported Functions
  * @{
  */
/**
  * @brief  Initialize structures and variables related to power board profiles
  *         used by Sink and Source, for all available ports.
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_Init(void);

/**
  * @brief  Sets the required power profile, now it works only with Fixed ones  
  * @param  PortNum               Port number
  * @param  Profile          Number of the required Power Data Objects
  * @param  PreviousPowerProfile  Number of the previous required Power Data Objects
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_SetProfile(uint8_t PortNum, uint8_t Profile, uint8_t PreviousPowerProfile);

/**
  * @brief  Checks if the power on a specified port is ready
  * @param  PortNum Port number
  * @param  Vsafe   Vsafe status based on @ref USBPD_VSAFE_StatusTypeDef
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_SupplyReady(uint8_t PortNum, USBPD_VSAFE_StatusTypeDef Vsafe);

/**
  * @brief  Enable VBUS power on a specified port
  * @param  PortNum Port number
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_VBUSEnable(uint8_t PortNum);

/**
  * @brief  Disable VBUS power on a specified port
  * @param  PortNum Port number
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_VBUSDisable(uint8_t PortNum);

/**
  * @brief  Checks if the power on a specified port is enabled
  * @param  PortNum Port number
  * @retval ENABLE or DISABLE
  */
USBPD_FunctionalState USBPD_PWR_IF_VBUSIsEnabled(uint8_t PortNum);

/**
  * @brief  Reads the voltage and the current on a specified port
  * @param  PortNum Port number
  * @param  pVoltage The Voltage in mV
  * @param  pCurrent The Current in mA
  * @retval ENABLE or DISABLE
  */
USBPD_StatusTypeDef USBPD_PWR_IF_ReadVA(uint8_t PortNum, uint16_t *pVoltage, uint16_t *pCurrent);

/**
  * @brief  Enables the VConn on the port.
  * @param  PortNum Port number
  * @param  CC      Specifies the CCx to be selected based on @ref CCxPin_TypeDef structure
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_Enable_VConn(uint8_t PortNum, CCxPin_TypeDef CC);

/**
  * @brief  Disable the VConn on the port.
  * @param  PortNum Port number
  * @param  CC      Specifies the CCx to be selected based on @ref CCxPin_TypeDef structure
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_Disable_VConn(uint8_t PortNum, CCxPin_TypeDef CC);

/**
  * @brief  Allow PDO data reading from PWR_IF storage.
  * @param  PortNum Port number
  * @param  DataId Type of data to be read from PWR_IF
  *         This parameter can be one of the following values:
  *           @arg @ref USBPD_CORE_DATATYPE_SRC_PDO Source PDO reading requested
  *           @arg @ref USBPD_CORE_DATATYPE_SNK_PDO Sink PDO reading requested
  * @param  Ptr Pointer on address where PDO values should be written (u8 pointer)
  * @param  Size Pointer on nb of u32 written by PWR_IF (nb of PDOs)
  * @retval None
  */
void USBPD_PWR_IF_GetPortPDOs(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t *Size);

/**
  * @brief  Find out SRC PDO pointed out by a position provided in a Request DO (from Sink).
  * @param  PortNum Port number
  * @param  RdoPosition RDO Position in list of provided PDO
  * @param  Pdo Pointer on PDO value pointed out by RDO position (u32 pointer)
  * @retval Status of search
  *         USBPD_OK : Src PDO found for requested DO position (output Pdo parameter is set)
  *         USBPD_FAIL : Position is not compliant with current Src PDO for this port (no corresponding PDO value)
  */
USBPD_StatusTypeDef USBPD_PWR_IF_SearchRequestedPDO(uint8_t PortNum, uint32_t RdoPosition, uint32_t *Pdo);


/**
  * @brief  Function to check validity between SNK PDO and power user settings
  * @param  PortNum Port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_PWR_IF_CheckUpdateSNKPower(uint8_t PortNum);

/**
  * @brief the function is called to get VBUS power status.
  * @param PortNum
  * @param PowerTypeStatus
  * @retval UBBPD_TRUE or USBPD_FALSE
  */
uint8_t USBPD_PWR_IF_GetVBUSStatus(uint8_t PortNum, USBPD_VBUSPOWER_STATUS PowerTypeStatus);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_PW_IF_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

