#include "stm32746g_discovery_ts.h"

//#include "GUI.h"

__IO TS_StateTypeDef ts;

static uint16_t tsXBoundary, tsYBoundary;
static uint8_t tsOrientation;
static uint8_t I2cAddress;

/**
 * @brief  Initializes and configures the touch screen functionalities and
 *         configures all necessary hardware resources (GPIOs, I2C, clocks..).
 * @param  ts_SizeX: Maximum X size of the TS area on LCD
 * @param  ts_SizeY: Maximum Y size of the TS area on LCD
 * @retval TS_OK if all initializations are OK. Other value if error.
 */
uint8_t BSP_TS_Init(uint16_t ts_SizeX, uint16_t ts_SizeY) {
	uint8_t status = TS_OK;

	tsXBoundary = ts_SizeX;
	tsYBoundary = ts_SizeY;

	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOH_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_13, GPIO_PIN_SET);

	/* Wait at least 200ms after power up before accessing registers
	 * Trsi timing (Time of starting to report point after resetting) from HY4614GQQ datasheet */
	HAL_Delay(200);

	/* Read ID and verify if the touch screen driver is ready */
	HY4614_Init();

	HY4614_ReadID(HY4614_I2C_SLAVE_ADDRESS);

	//  if(ft5336_ts_drv.ReadID(TS_I2C_ADDRESS) == FT5336_ID_VALUE)
//  {
	/* Initialize the TS driver structure */
//	tsDriver = &ft5336_ts_drv;
	I2cAddress = HY4614_I2C_SLAVE_ADDRESS;
	tsOrientation = TS_SWAP_NONE;

	return status;
}

/**
 * @brief  DeInitializes the TouchScreen.
 * @retval TS state
 */
uint8_t BSP_TS_DeInit(void) {
	/* Actually ts_driver does not provide a DeInit function */
	return TS_OK;
}

/**
 * @brief  Configures and enables the touch screen interrupts.
 * @retval TS_OK if all initializations are OK. Other value if error.
 */
uint8_t BSP_TS_ITConfig(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure Interrupt mode for SD detection pin */
	GPIO_InitStruct.Pin = TS_INT_PIN;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; //GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(TS_INT_GPIO_PORT, &GPIO_InitStruct);

	/* Enable and set Touch screen EXTI Interrupt to the lowest priority */
	HAL_NVIC_SetPriority((IRQn_Type) (TS_INT_EXTI_IRQn), 0x0F, 0x00);
	HAL_NVIC_EnableIRQ((IRQn_Type) (TS_INT_EXTI_IRQn));

	return TS_OK;
}

/**
 * @brief  Provide the GUI with current state of the touch screen
 * @param  None
 * @retval None
 */
void BSP_Pointer_Update(void) {
//	static GUI_PID_STATE TS_State = { 0, 0, 0, 0 };

//	uint16_t xDiff, yDiff;

	BSP_TS_GetState((TS_StateTypeDef *) &ts);

	if ((ts.touchX[0] >= LCD_GetXSize()) || (ts.touchY[0] >= LCD_GetYSize())) {
		ts.touchX[0] = 0;
		ts.touchY[0] = 0;
		ts.touchDetected = 0;
	}

//	xDiff = (TS_State.x > ts.touchX[0]) ?
//			(TS_State.x - ts.touchX[0]) : (ts.touchX[0] - TS_State.x);
//	yDiff = (TS_State.y > ts.touchY[0]) ?
//			(TS_State.y - ts.touchY[0]) : (ts.touchY[0] - TS_State.y);
//
//	if ((TS_State.Pressed != ts.touchDetected) || (xDiff > 30)
//			|| (yDiff > 30)) {
//		TS_State.Pressed = ts.touchDetected;
//		TS_State.Layer = 0;
//		if (ts.touchDetected) {
//			TS_State.x = ts.touchX[0];
//			TS_State.y = ts.touchY[0];
//			GUI_TOUCH_StoreStateEx(&TS_State);
//		} else {
//			GUI_TOUCH_StoreStateEx(&TS_State);
//			TS_State.x = 0;
//			TS_State.y = 0;
//		}
//	}
}

void EXTI15_10_IRQHandler(void) {
	if (__HAL_GPIO_EXTI_GET_IT(TS_INT_PIN) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(TS_INT_PIN);

		//looking for the falling edge...
		if (HAL_GPIO_ReadPin(TS_INT_GPIO_PORT, TS_INT_PIN) == GPIO_PIN_RESET) {
			BSP_Pointer_Update();
		}
	}
}

/**
 * @brief  Returns status and positions of the touch screen.
 * @param  TS_State: Pointer to touch screen current state structure
 * @retval TS_OK if all initializations are OK. Other value if error.
 */
uint8_t BSP_TS_GetState(TS_StateTypeDef *TS_State) {
	static uint32_t _x[TS_MAX_NB_TOUCH] = { 0, 0 };
	static uint32_t _y[TS_MAX_NB_TOUCH] = { 0, 0 };
	uint8_t ts_status = TS_OK;
	uint16_t x[TS_MAX_NB_TOUCH];
	uint16_t y[TS_MAX_NB_TOUCH];
	uint16_t brute_x[TS_MAX_NB_TOUCH];
	uint16_t brute_y[TS_MAX_NB_TOUCH];
	uint16_t x_diff;
	uint16_t y_diff;
	uint32_t index;

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
	uint32_t weight = 0;
	uint32_t area = 0;
	uint32_t event = 0;
#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

	/* Check and update the number of touches active detected */
	TS_State->touchDetected = HY4614_TS_DetectTouch(I2cAddress);

	//COMM_SendDebugTextFrame("BSP_TS_GetState nbTouch=%d", TS_State->touchDetected);

	if (TS_State->touchDetected) {
		for (index = 0; index < TS_State->touchDetected; index++) {
			/* Get each touch coordinates */
			HY4614_TS_GetXY(I2cAddress, &(brute_x[index]), &(brute_y[index]));

			//COMM_SendDebugTextFrame("  raw #%d -> X=%d | Y=%d", index, brute_x[index], brute_y[index]);

			if (tsOrientation == TS_SWAP_NONE) {
				x[index] = brute_x[index];
				y[index] = brute_y[index];
			}

			if (tsOrientation & TS_SWAP_X) {
				x[index] = 4096 - brute_x[index];
			}

			if (tsOrientation & TS_SWAP_Y) {
				y[index] = 4096 - brute_y[index];
			}

			if (tsOrientation & TS_SWAP_XY) {
				y[index] = brute_x[index];
				x[index] = brute_y[index];
			}

			x_diff =
					x[index] > _x[index] ?
							(x[index] - _x[index]) : (_x[index] - x[index]);
			y_diff =
					y[index] > _y[index] ?
							(y[index] - _y[index]) : (_y[index] - y[index]);

			if ((x_diff + y_diff) > 5) {
				_x[index] = x[index];
				_y[index] = y[index];
			}

			if (I2cAddress == HY4614_I2C_SLAVE_ADDRESS) {
				TS_State->touchX[index] = x[index];
				TS_State->touchY[index] = y[index];
			} else {
				/* 2^12 = 4096 : indexes are expressed on a dynamic of 4096 */
				TS_State->touchX[index] = (tsXBoundary * _x[index]) >> 12;
				TS_State->touchY[index] = (tsYBoundary * _y[index]) >> 12;
			}

			//COMM_SendDebugTextFrame("  end #%d -> X=%d | Y=%d", index, TS_State->touchX[index], TS_State->touchY[index]);

#if (TS_MULTI_TOUCH_SUPPORTED == 1)

			/* Get touch info related to the current touch */
			ft5336_TS_GetTouchInfo(I2cAddress, index, &weight, &area, &event);

			/* Update TS_State structure */
			TS_State->touchWeight[index] = weight;
			TS_State->touchArea[index] = area;

			/* Remap touch event */
			switch(event)
			{
				case FT5336_TOUCH_EVT_FLAG_PRESS_DOWN :
				TS_State->touchEventId[index] = TOUCH_EVENT_PRESS_DOWN;
				break;
				case FT5336_TOUCH_EVT_FLAG_LIFT_UP :
				TS_State->touchEventId[index] = TOUCH_EVENT_LIFT_UP;
				break;
				case FT5336_TOUCH_EVT_FLAG_CONTACT :
				TS_State->touchEventId[index] = TOUCH_EVENT_CONTACT;
				break;
				case FT5336_TOUCH_EVT_FLAG_NO_EVENT :
				TS_State->touchEventId[index] = TOUCH_EVENT_NO_EVT;
				break;
				default :
				ts_status = TS_ERROR;
				break;
			} /* of switch(event) */

#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

		} /* of for(index=0; index < TS_State->touchDetected; index++) */

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
		/* Get gesture Id */
		ts_status = BSP_TS_Get_GestureId(TS_State);
#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

	} /* end of if(TS_State->touchDetected != 0) */

	return (ts_status);
}

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
/**
 * @brief  Update gesture Id following a touch detected.
 * @param  TS_State: Pointer to touch screen current state structure
 * @retval TS_OK if all initializations are OK. Other value if error.
 */
uint8_t BSP_TS_Get_GestureId(TS_StateTypeDef *TS_State)
{
	uint32_t gestureId = 0;
	uint8_t ts_status = TS_OK;

	/* Get gesture Id */
	ft5336_TS_GetGestureID(I2cAddress, &gestureId);

	/* Remap gesture Id to a TS_GestureIdTypeDef value */
	switch(gestureId)
	{
		case FT5336_GEST_ID_NO_GESTURE :
		TS_State->gestureId = GEST_ID_NO_GESTURE;
		break;
		case FT5336_GEST_ID_MOVE_UP :
		TS_State->gestureId = GEST_ID_MOVE_UP;
		break;
		case FT5336_GEST_ID_MOVE_RIGHT :
		TS_State->gestureId = GEST_ID_MOVE_RIGHT;
		break;
		case FT5336_GEST_ID_MOVE_DOWN :
		TS_State->gestureId = GEST_ID_MOVE_DOWN;
		break;
		case FT5336_GEST_ID_MOVE_LEFT :
		TS_State->gestureId = GEST_ID_MOVE_LEFT;
		break;
		case FT5336_GEST_ID_ZOOM_IN :
		TS_State->gestureId = GEST_ID_ZOOM_IN;
		break;
		case FT5336_GEST_ID_ZOOM_OUT :
		TS_State->gestureId = GEST_ID_ZOOM_OUT;
		break;
		default :
		ts_status = TS_ERROR;
		break;
	} /* of switch(gestureId) */

	return(ts_status);
}
#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

/**
 * @brief  Function used to reset all touch data before a new acquisition
 *         of touch information.
 * @param  TS_State: Pointer to touch screen current state structure
 * @retval TS_OK if OK, TE_ERROR if problem found.
 */
uint8_t BSP_TS_ResetTouchData(TS_StateTypeDef *TS_State) {
	uint8_t ts_status = TS_ERROR;
	uint32_t index;

	if (TS_State != (TS_StateTypeDef *) NULL) {
#if (TS_MULTI_TOUCH_SUPPORTED == 1)
		TS_State->gestureId = GEST_ID_NO_GESTURE;
#endif

		TS_State->touchDetected = 0;

		for (index = 0; index < TS_MAX_NB_TOUCH; index++) {
			TS_State->touchX[index] = 0;
			TS_State->touchY[index] = 0;

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
			TS_State->touchArea[index] = 0;
			TS_State->touchEventId[index] = TOUCH_EVENT_NO_EVT;
			TS_State->touchWeight[index] = 0;
#endif
		}

		ts_status = TS_OK;

	} /* of if (TS_State != (TS_StateTypeDef *)NULL) */

	return (ts_status);
}
