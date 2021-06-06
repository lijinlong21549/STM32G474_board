/**
  ******************************************************************************
  * @file    tracer_emb.c
  * @author  MCD Application Team
  * @brief   This file contains embeded tracer control functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tracer_emb.h"
#include "tracer_emb_hw.h"
#include "string.h"

/** @addtogroup TRACER_EMB
  * @{
  */

/** @addtogroup TRACER_EMB
  * @{
  */

/** @addtogroup TRACER_EMB
  * @{
  */

/* Private enums -------------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACER_EMB_Private_TypeDef EMB TRACER Private typedef
  * @{
  */
 typedef enum {
  TRACER_OVERFLOW_NONE,
  TRACER_OVERFLOW_DETECTED,
  TRACER_OVERFLOW_SENT
} TRACER_OverFlowTypedef;
/**
  * @}
  */


/** @defgroup USBPD_CORE_TRACER_EMB_Private_TypeDef EMB TRACER Private typedef
  * @{
  */
 typedef enum {
  TRACER_OK,
  TRACER_KO
} TRACER_StatusTypedef;
/**
  * @}
  */

/** @defgroup USBPD_CORE_TRACER_EMB_Private_TypeDef EMB TRACER Private typedef
  * @{
  */
typedef struct  {
  uint32_t PtrTx_Read;
  uint32_t PtrTx_Write;
  uint32_t SizeSent;
  volatile uint8_t Counter;
  const uint8_t *OverFlow_Data;
  uint8_t OverFlow_Size;
  uint8_t discontinue;
  TRACER_OverFlowTypedef OverFlow_Status;
  uint8_t PtrDataTx[TRACER_EMB_BUFFER_SIZE];
} TRACER_ContextTypedef;
/**
  * @}
  */

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACER_EMB_Private_Macros USBPD TRACE Private Macros
  * @{
  */

#define TRACER_WRITE_DATA(_POSITION_,_DATA_)  TracerContext.PtrDataTx[(_POSITION_)% TRACER_EMB_BUFFER_SIZE] = (_DATA_);  \
                                             (_POSITION_) = ((_POSITION_) + 1u) % TRACER_EMB_BUFFER_SIZE;

#define TRACER_ENTER_CRITICAL_SECTION()  uint32_t primask= __get_PRIMASK();\
                                        __disable_irq();

#define TRACER_LEAVE_CRITICAL_SECTION() __set_PRIMASK(primask)


/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_CORE_TRACER_EMB_Private_Functions USBPD TRACE Private Functions
  * @{
  */
uint32_t                    TRACER_CallbackRX(uint8_t character, uint8_t error);
static TRACER_StatusTypedef TRACER_CheckLook(uint32_t *Begin, uint32_t *End);

/* Function prototypes -----------------------------------------------*/
uint8_t TRACER_EMB_ReadData(void);

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_CORE_TRACER_EMB_Private_Variables USBPD TRACE Private Variables
  * @{
  */
static TRACER_ContextTypedef TracerContext;
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/
extern void USBPD_TRACE_Add(uint8_t Type, uint8_t PortNum, uint8_t Sop, uint8_t *Ptr, uint32_t Size);


/** @addtogroup USBPD_CORE_TRACER_EMB_Exported_Functions
  * @{
  */
void TRACER_EMB_Init(void)
{
  /* initialize the Ptr for Read/Write */
  memset(&TracerContext, 0, sizeof(TRACER_ContextTypedef));
  
  /* Initialize trace BUS */
  HW_TRACER_EMB_Init();
  
  /* Initialize the lowpower aspect */
  TRACER_EMB_LowPowerInit();
}

void TRACER_EMB_Add(uint8_t *Ptr, uint32_t Size)
{
  int32_t _writepos;
  uint8_t *data_to_write = Ptr;
  uint32_t index;

  TRACER_EMB_Lock();

  _writepos = TRACER_EMB_AllocateBufer(Size);

  /* if allocation is ok, write data into the bufffer */
  if (_writepos  != -1)
  {
    /* initialize the Ptr for Read/Write */
    for (index = 0u; index < Size; index++)
    {
      TRACER_WRITE_DATA(_writepos, data_to_write[index]);
    }
  }
  TRACER_EMB_UnLock();

  /* Wakeup the trace system */
  TRACER_EMB_WakeUpProcess();
}

#if TRACER_EMB_DMA_MODE == 1UL
void TRACER_EMB_IRQHandlerDMA(void)
{
  HW_TRACER_EMB_IRQHandlerDMA();
}
#endif

void TRACER_EMB_IRQHandlerUSART(void)
{
  HW_TRACER_EMB_IRQHandlerUSART();
}

uint32_t TRACER_EMB_TX_Process(void)
{
  uint32_t _timing = 2u;
  uint32_t _begin, _end;

  if (TRACER_OK == TRACER_CheckLook(&_begin, &_end))
  {
    if (_begin == _end)
    {
      /* nothing to do */
      _timing = 0xFFFFFFFFu;
    }
    else
    {
      TRACER_EMB_Lock();
      /*  */
      if (_end > _begin)
      {
        TracerContext.SizeSent = _end - _begin;
        TracerContext.discontinue = 0;
      }
      else  /* _begin > _end */
      {
        TracerContext.SizeSent = TRACER_EMB_BUFFER_SIZE - _begin;
        TracerContext.discontinue = 1;
      }
      TRACER_EMB_LowPowerSendData();
      HW_TRACER_EMB_SendData(&(TracerContext.PtrDataTx[_begin]), TracerContext.SizeSent);
    }
  }
  return _timing;
}

void TRACER_EMB_WriteData(uint16_t pos, uint8_t data)
{
  TracerContext.PtrDataTx[pos % TRACER_EMB_BUFFER_SIZE] = data;
}

void TRACER_EMB_StartRX(void (*callbackRX)(uint8_t, uint8_t))
{
  HW_TRACER_EMB_RegisterRxCallback(callbackRX);
  HW_TRACER_EMB_StartRX();
}

int32_t TRACER_EMB_EnableOverFlow(const uint8_t *Data, uint8_t Size)
{
  if(Size != 0)
  {
    TracerContext.OverFlow_Data = Data;
    TracerContext.OverFlow_Size = Size;
    return 0;
  }
  return -1;
}

uint8_t TRACER_EMB_ReadData()
{
  return HW_TRACER_EMB_ReadData();
}


/**
  * @}
  */

/** @defgroup USBPD_CORE_TRACER_EMB_Private_Functions USBPD TRACE Private Functions
  * @{
  */

/**
  * @brief  callback called to end a transfert.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_CALLBACK_TX(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  TracerContext.PtrTx_Read = (TracerContext.PtrTx_Read + TracerContext.SizeSent) % TRACER_EMB_BUFFER_SIZE;
  
  if((TracerContext.OverFlow_Data != NULL) && (TracerContext.OverFlow_Status == TRACER_OVERFLOW_DETECTED)
     && (TracerContext.discontinue == 0))
  {
    TracerContext.OverFlow_Status = TRACER_OVERFLOW_SENT;
    HW_TRACER_EMB_SendData((uint8_t *)TracerContext.OverFlow_Data, TracerContext.OverFlow_Size);
    TRACER_LEAVE_CRITICAL_SECTION();
  }
  else
  {
    TRACER_LEAVE_CRITICAL_SECTION();
    TRACER_EMB_UnLock();
  }
  TRACER_EMB_LowPowerSendDataComplete();
}

/**
  * @brief  Lock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_Lock(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  TracerContext.Counter++;
  TRACER_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  UnLock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_UnLock(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  TracerContext.Counter--;
  TRACER_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  if buffer is not locked return a Begin / end @ to transfert over the media.
  * @param  address begin of the data
  * @param  address end of the data
  * @retval USBPD_TRUE if a transfer can be execute else USBPD_FALSE.
  */
static TRACER_StatusTypedef TRACER_CheckLook(uint32_t *Begin, uint32_t *End)
{
  TRACER_StatusTypedef _status = TRACER_KO;
  TRACER_ENTER_CRITICAL_SECTION();

  if (0u == TracerContext.Counter)
  {
    *Begin = TracerContext.PtrTx_Read;
    *End = TracerContext.PtrTx_Write;
    _status = TRACER_OK;
  }

  TRACER_LEAVE_CRITICAL_SECTION();
  return _status;
}

/**
  * @brief  allocate space inside the buffer to push data
  * @param  data size
  * @retval write position inside the buffer is -1 no space available.
  */
int32_t TRACER_EMB_AllocateBufer(uint32_t Size)
{
  uint32_t _freesize;
  int32_t _pos = -1;

  TRACER_ENTER_CRITICAL_SECTION();

  if (TracerContext.PtrTx_Write == TracerContext.PtrTx_Read)
  {
    // need to add buffer full managment
    _freesize = TRACER_EMB_BUFFER_SIZE;
  }
  else
  {
    if (TracerContext.PtrTx_Write > TracerContext.PtrTx_Read)
    {
      _freesize = TRACER_EMB_BUFFER_SIZE - TracerContext.PtrTx_Write + TracerContext.PtrTx_Read;
    }
    else
    {
      _freesize = TracerContext.PtrTx_Read - TracerContext.PtrTx_Write;
    }
  }

  if (_freesize > Size)
  {
    _pos = (int32_t)TracerContext.PtrTx_Write;
    TracerContext.PtrTx_Write = (TracerContext.PtrTx_Write + Size) % TRACER_EMB_BUFFER_SIZE;
    if(TRACER_OVERFLOW_SENT == TracerContext.OverFlow_Status)
      TracerContext.OverFlow_Status = TRACER_OVERFLOW_NONE;
  }
  else
  {
    if(TRACER_OVERFLOW_NONE == TracerContext.OverFlow_Status)
      TracerContext.OverFlow_Status = TRACER_OVERFLOW_DETECTED;
  }

  TRACER_LEAVE_CRITICAL_SECTION();
  return _pos;
}

__weak void TRACER_EMB_LowPowerInit(void)
{
}

__weak void TRACER_EMB_LowPowerSendData(void)
{
}

__weak void TRACER_EMB_LowPowerSendDataComplete(void)
{
}

__weak void TRACER_EMB_WakeUpProcess(void)
{
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

