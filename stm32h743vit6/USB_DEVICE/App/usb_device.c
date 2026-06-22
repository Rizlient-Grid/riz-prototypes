/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */
#include "usbd_composite_builder.h"
#include "usbd_hid.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t CDC_ID, HID_ID;
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceHS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */
  /* Composite CDC + HID 初始化（与 .ioc 的单 CDC 配置不同，整段在此托管，
     防止 CubeMX 重新生成时覆盖。末尾 return 让下方 CubeMX 默认代码成为死代码）。 */
  uint8_t CDC_EpAddr[] = {CDC_IN_EP, CDC_OUT_EP, CDC_CMD_EP};
  uint8_t HID_EpAddr[] = {HID_EPIN_ADDR};

  if (USBD_Init(&hUsbDeviceHS, &HS_Desc, DEVICE_HS) != USBD_OK)
  {
    Error_Handler();
  }

  if (USBD_RegisterClassComposite(&hUsbDeviceHS, USBD_CDC_CLASS, CLASS_TYPE_CDC, CDC_EpAddr) != USBD_OK)
  {
    Error_Handler();
  }

  if (USBD_RegisterClassComposite(&hUsbDeviceHS, USBD_HID_CLASS, CLASS_TYPE_HID, HID_EpAddr) != USBD_OK)
  {
    Error_Handler();
  }

  /* SetClassID 返回 classId 本身（非状态码），失败时返回 0xFF */
  CDC_ID = USBD_CMPSIT_SetClassID(&hUsbDeviceHS, CLASS_TYPE_CDC, 0);
  if (CDC_ID == 0xFFU)
  {
    Error_Handler();
  }

  if (USBD_CDC_RegisterInterface(&hUsbDeviceHS, &USBD_Interface_fops_HS) != USBD_OK)
  {
    Error_Handler();
  }

  HID_ID = USBD_CMPSIT_SetClassID(&hUsbDeviceHS, CLASS_TYPE_HID, 0);
  if (HID_ID == 0xFFU)
  {
    Error_Handler();
  }

  if (USBD_Start(&hUsbDeviceHS) != USBD_OK)
  {
    Error_Handler();
  }

  HAL_PWREx_EnableUSBVoltageDetector();

  return; /* 跳过 CubeMX 按 .ioc 生成的单 CDC 默认实现 */
#if 0
  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceHS, &HS_Desc, DEVICE_HS) != USBD_OK)
  {
    Error_Handler();
  }

  if (USBD_RegisterClass(&hUsbDeviceHS, &USBD_CDC) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_CDC_RegisterInterface(&hUsbDeviceHS, &USBD_Interface_fops_HS) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_Start(&hUsbDeviceHS) != USBD_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
  /* HAL_PWREx_EnableUSBVoltageDetector() 已在上方 PreTreatment 中调用 */
#endif
  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

