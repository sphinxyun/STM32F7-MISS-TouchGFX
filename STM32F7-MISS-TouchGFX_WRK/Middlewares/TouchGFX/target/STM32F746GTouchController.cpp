/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.10.0 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
  


#include "STM32F746GTouchController.hpp"

extern "C"
{
#include "stm32746g_discovery_ts.h"

extern __IO TS_StateTypeDef ts;

uint32_t LCD_GetXSize();
uint32_t LCD_GetYSize();
}

using namespace touchgfx;

void STM32F746GTouchController::init()
{
	BSP_TS_Init(640, 480);
	BSP_TS_ITConfig();

//    BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());
}

bool STM32F746GTouchController::sampleTouch(int32_t& x, int32_t& y)
{
//    TS_StateTypeDef state = { 0 };
//    BSP_TS_GetState(&state);
    if (ts.touchDetected)
    {
        x = ts.touchX[0];
        y = ts.touchY[0];

        return true;
    }
    return false;
}
