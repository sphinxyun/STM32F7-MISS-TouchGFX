#include "hy4614.h"
//#include "main.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"

static I2C_HandleTypeDef hI2cAudioHandler = { 0 };



/******************************* I2C Routines *********************************/
/**
 * @brief  Initializes I2C MSP.
 * @param  i2c_handler : I2C handler
 * @retval None
 */
static void I2Cx_MspInit(I2C_HandleTypeDef *i2c_handler) {
	GPIO_InitTypeDef gpio_init_structure;

	/* AUDIO and LCD I2C MSP init */

	/*** Configure the GPIOs ***/
	/* Enable GPIO clock */
	DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()
	;

	/* Configure I2C Tx as alternate function */
	gpio_init_structure.Pin = DISCOVERY_AUDIO_I2Cx_SCL_PIN;
	gpio_init_structure.Mode = GPIO_MODE_AF_OD;
	gpio_init_structure.Pull = GPIO_NOPULL;
	gpio_init_structure.Speed = GPIO_SPEED_FAST;
	gpio_init_structure.Alternate = DISCOVERY_AUDIO_I2Cx_SCL_SDA_AF;
	HAL_GPIO_Init(DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

	/* Configure I2C Rx as alternate function */
	gpio_init_structure.Pin = DISCOVERY_AUDIO_I2Cx_SDA_PIN;
	HAL_GPIO_Init(DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

	/*** Configure the I2C peripheral ***/
	/* Enable I2C clock */
	DISCOVERY_AUDIO_I2Cx_CLK_ENABLE()
	;

	/* Force the I2C peripheral clock reset */
	DISCOVERY_AUDIO_I2Cx_FORCE_RESET();

	/* Release the I2C peripheral clock reset */
	DISCOVERY_AUDIO_I2Cx_RELEASE_RESET();

	/* Enable and set I2Cx Interrupt to a lower priority */
	HAL_NVIC_SetPriority(DISCOVERY_AUDIO_I2Cx_EV_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(DISCOVERY_AUDIO_I2Cx_EV_IRQn);

	/* Enable and set I2Cx Interrupt to a lower priority */
	HAL_NVIC_SetPriority(DISCOVERY_AUDIO_I2Cx_ER_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(DISCOVERY_AUDIO_I2Cx_ER_IRQn);
}

static void I2Cx_Init(I2C_HandleTypeDef *i2c_handler) {
	if (HAL_I2C_GetState(i2c_handler) == HAL_I2C_STATE_RESET) {
		i2c_handler->Instance = DISCOVERY_AUDIO_I2Cx;
		i2c_handler->Init.Timing = DISCOVERY_I2Cx_TIMING;
		i2c_handler->Init.OwnAddress1 = 0;
		i2c_handler->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		i2c_handler->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		i2c_handler->Init.OwnAddress2 = 0;
		i2c_handler->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		i2c_handler->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

		/* Init the I2C */
		I2Cx_MspInit(i2c_handler);
		HAL_I2C_Init(i2c_handler);
	}
}

static void I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr) {
	/* De-initialize the I2C communication bus */
	HAL_I2C_DeInit(i2c_handler);

	/* Re-Initialize the I2C communication bus */
	I2Cx_Init(i2c_handler);
}

/**
 * @brief  Reads multiple data.
 * @param  i2c_handler : I2C handler
 * @param  Addr: I2C address
 * @param  Reg: Reg address
 * @param  MemAddress: Memory address
 * @param  Buffer: Pointer to data buffer
 * @param  Length: Length of the data
 * @retval Number of read data
 */
static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler,
		uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer,
		uint16_t Length) {
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Read(i2c_handler, Addr, (uint16_t) Reg, MemAddress,
			Buffer, Length, 1000);

	/* Check the communication status */
	if (status != HAL_OK) {
		/* I2C error occurred */
		I2Cx_Error(i2c_handler, Addr);
	}
	return status;
}

/* Global HY4614 handle */
static HY4614_handle_TypeDef HY4614_handle =
		{ HY4614_I2C_NOT_INITIALIZED, 0, 0 };

/**
 * @brief  Return the status of I2C was initialized or not.
 * @param  None.
 * @retval : I2C initialization status.
 */
static uint8_t HY4614_Get_I2C_InitializedStatus(void);

/**
 * @brief  I2C initialize if needed.
 * @param  None.
 * @retval : None.
 */
static void HY4614_I2C_InitializeIfRequired(void);

/**
 * @brief  Initialize the HY4614 communication bus
 *         from MCU to HY4614 : ie I2C channel initialization (if required).
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @retval None
 */
void HY4614_Init(void) {
	/* Initialize I2C link if needed */
	HY4614_I2C_InitializeIfRequired();
}

/**
 * @brief  Software Reset the HY4614.
 *         @note : Not applicable to HY4614.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @retval None
 */
void HY4614_Reset(uint16_t DeviceAddr) {
	/* Do nothing */
	/* No software reset sequence available in HY4614 IC */
}

/**
 * @brief  Read the HY4614 device ID, pre initialize I2C in case of need to be
 *         able to read the HY4614 device ID, and verify this is a HY4614.
 * @param  DeviceAddr: I2C HY4614 Slave address.
 * @retval The Device ID (two bytes).
 */
uint16_t HY4614_ReadID(uint16_t DeviceAddr) {
	volatile uint8_t ucReadId = 0, ucReadVerH = 0, ucReadVerL = 0;
	uint8_t nbReadAttempts = 0;
	uint8_t bFoundDevice = 0;

	/* Initialize I2C link if needed */
	HY4614_I2C_InitializeIfRequired();

    /* Read register HY4614_CHIP_ID_REG as DeviceID detection */
	I2Cx_ReadMultiple(&hI2cAudioHandler, DeviceAddr, 0xA9, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &ucReadId, 1);

	if (ucReadId == 0x15) {
		/* At maximum 4 attempts to read ID : exit at first finding of the searched device ID */
		for (nbReadAttempts = 0; ((nbReadAttempts < 3) && !(bFoundDevice)); nbReadAttempts++) {
			/* Read register HY4614_CHIP_ID_REG as DeviceID detection */
			I2Cx_ReadMultiple(&hI2cAudioHandler, DeviceAddr, 0xA6, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &ucReadVerH, 1);
			I2Cx_ReadMultiple(&hI2cAudioHandler, DeviceAddr, 0xA7, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &ucReadVerL, 1);
		}
	}

	/* Return the device ID value */
	return 0;
}

/**
 * @brief  Return if there is touches detected or not.
 *         Try to detect new touches and forget the old ones (reset internal global
 *         variables).
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval : Number of active touches detected (can be 0, 1 or 2).
 */
uint8_t HY4614_TS_DetectTouch(uint16_t DeviceAddr) {
	volatile uint8_t nbTouch = 0;

	/* Read register HY4614_TD_STAT_REG to check number of touches detection */
	I2Cx_ReadMultiple(&hI2cAudioHandler, DeviceAddr, HY4614_TD_STAT_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &nbTouch, 1);

	nbTouch &= HY4614_TD_STAT_MASK;

	if (nbTouch > HY4614_MAX_DETECTABLE_TOUCH) {
		/* If invalid number of touch detected, set it to zero */
		nbTouch = 0;
	}

	/* Update HY4614 driver internal global : current number of active touches */
	HY4614_handle.currActiveTouchNb = nbTouch;

	/* Reset current active touch index on which to work on */
	HY4614_handle.currActiveTouchIdx = 0;

	return (nbTouch);
}

/**
 * @brief  Get the touch screen X and Y positions values
 *         Manage multi touch thanks to touch Index global
 *         variable 'HY4614_handle.currActiveTouchIdx'.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  X: Pointer to X position value
 * @param  Y: Pointer to Y position value
 * @retval None.
 */
void HY4614_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y) {
	volatile uint8_t ucReadData[4] = { 0, 0, 0, 0 };

	static uint16_t coord;
	uint8_t baseRegAddress = 0;

	if (HY4614_handle.currActiveTouchIdx < HY4614_handle.currActiveTouchNb) {
		switch (HY4614_handle.currActiveTouchIdx) {
		case 0:
			baseRegAddress = HY4614_P1_BASE_REG;
			break;

		case 1:
			baseRegAddress = HY4614_P2_BASE_REG;
			break;

		case 2:
			baseRegAddress = HY4614_P3_BASE_REG;
			break;

		case 3:
			baseRegAddress = HY4614_P4_BASE_REG;
			break;

		case 4:
			baseRegAddress = HY4614_P5_BASE_REG;
			break;

		default:
			break;

		}

		/* Read touch data */
		I2Cx_ReadMultiple(&hI2cAudioHandler, DeviceAddr, baseRegAddress, I2C_MEMADD_SIZE_8BIT, ucReadData, 4);

		/* low part of X position */
		coord = (ucReadData[1] & HY4614_TOUCH_POS_LSB_MASK)
				>> HY4614_TOUCH_POS_LSB_SHIFT;

		/* high part of X position */
		coord |= ((ucReadData[0] & HY4614_TOUCH_POS_MSB_MASK)
				>> HY4614_TOUCH_POS_MSB_SHIFT) << 8;

		/* Send back ready X position to caller */
		*X = (int) ((float) coord * 0.454868514570007);

		/* low part of Y position */
		coord = (ucReadData[3] & HY4614_TOUCH_POS_LSB_MASK)
				>> HY4614_TOUCH_POS_LSB_SHIFT;

		/* high part of Y position */
		coord |= ((ucReadData[2] & HY4614_TOUCH_POS_MSB_MASK)
				>> HY4614_TOUCH_POS_MSB_SHIFT) << 8;

		/* Send back ready Y position to caller */
		*Y = (int) ((float) coord * 0.469208211143695);

		HY4614_handle.currActiveTouchIdx++; /* next call will work on next touch */
	}
}

/**** NEW FEATURES enabled when Multi-touch support is enabled ****/

#if (TS_MULTI_TOUCH_SUPPORTED == 1)

/**
 * @brief  Get the last touch gesture identification (zoom, move up/down...).
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @param  pGestureId : Pointer to get last touch gesture Identification.
 * @retval None.
 */
void HY4614_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId)
{
	volatile uint8_t ucReadData = 0;

//  ucReadData = TS_IO_Read(DeviceAddr, HY4614_GEST_ID_REG);

	* pGestureId = ucReadData;
}

/**
 * @brief  Get the touch detailed informations on touch number 'touchIdx' (0..1)
 *         This touch detailed information contains :
 *         - weight that was applied to this touch
 *         - sub-area of the touch in the touch panel
 *         - event of linked to the touch (press down, lift up, ...)
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of HY4614).
 * @param  touchIdx : Passed index of the touch (0..1) on which we want to get the
 *                    detailed information.
 * @param  pWeight : Pointer to to get the weight information of 'touchIdx'.
 * @param  pArea   : Pointer to to get the sub-area information of 'touchIdx'.
 * @param  pEvent  : Pointer to to get the event information of 'touchIdx'.

 * @retval None.
 */
void HY4614_TS_GetTouchInfo(uint16_t DeviceAddr,
		uint32_t touchIdx,
		uint32_t * pWeight,
		uint32_t * pArea,
		uint32_t * pEvent)
{
	volatile uint8_t ucReadData = 0;
	uint8_t regAddressXHigh = 0;
	uint8_t regAddressPWeight = 0;
	uint8_t regAddressPMisc = 0;

	if(touchIdx < HY4614_handle.currActiveTouchNb)
	{
		switch(touchIdx)
		{
			case 0 :
			regAddressXHigh = HY4614_P1_XH_REG;
			regAddressPWeight = HY4614_P1_WEIGHT_REG;
			regAddressPMisc = HY4614_P1_MISC_REG;
			break;

			case 1 :
			regAddressXHigh = HY4614_P2_XH_REG;
			regAddressPWeight = HY4614_P2_WEIGHT_REG;
			regAddressPMisc = HY4614_P2_MISC_REG;
			break;

			case 2 :
			regAddressXHigh = HY4614_P3_XH_REG;
			regAddressPWeight = HY4614_P3_WEIGHT_REG;
			regAddressPMisc = HY4614_P3_MISC_REG;
			break;

			case 3 :
			regAddressXHigh = HY4614_P4_XH_REG;
			regAddressPWeight = HY4614_P4_WEIGHT_REG;
			regAddressPMisc = HY4614_P4_MISC_REG;
			break;

			case 4 :
			regAddressXHigh = HY4614_P5_XH_REG;
			regAddressPWeight = HY4614_P5_WEIGHT_REG;
			regAddressPMisc = HY4614_P5_MISC_REG;
			break;

//    case 5 :
//      regAddressXHigh   = HY4614_P6_XH_REG;
//      regAddressPWeight = HY4614_P6_WEIGHT_REG;
//      regAddressPMisc   = HY4614_P6_MISC_REG;
//      break;
//
//    case 6 :
//      regAddressXHigh   = HY4614_P7_XH_REG;
//      regAddressPWeight = HY4614_P7_WEIGHT_REG;
//      regAddressPMisc   = HY4614_P7_MISC_REG;
//      break;
//
//    case 7 :
//      regAddressXHigh   = HY4614_P8_XH_REG;
//      regAddressPWeight = HY4614_P8_WEIGHT_REG;
//      regAddressPMisc   = HY4614_P8_MISC_REG;
//      break;
//
//    case 8 :
//      regAddressXHigh   = HY4614_P9_XH_REG;
//      regAddressPWeight = HY4614_P9_WEIGHT_REG;
//      regAddressPMisc   = HY4614_P9_MISC_REG;
//      break;
//
//    case 9 :
//      regAddressXHigh   = HY4614_P10_XH_REG;
//      regAddressPWeight = HY4614_P10_WEIGHT_REG;
//      regAddressPMisc   = HY4614_P10_MISC_REG;
//      break;

			default :
			break;

		} /* end switch(touchIdx) */

		/* Read Event Id of touch index */
		ucReadData = TS_IO_Read(DeviceAddr, regAddressXHigh);
		* pEvent = (ucReadData & HY4614_TOUCH_EVT_FLAG_MASK) >> HY4614_TOUCH_EVT_FLAG_SHIFT;

		/* Read weight of touch index */
		ucReadData = TS_IO_Read(DeviceAddr, regAddressPWeight);
		* pWeight = (ucReadData & HY4614_TOUCH_WEIGHT_MASK) >> HY4614_TOUCH_WEIGHT_SHIFT;

		/* Read area of touch index */
		ucReadData = TS_IO_Read(DeviceAddr, regAddressPMisc);
		* pArea = (ucReadData & HY4614_TOUCH_AREA_MASK) >> HY4614_TOUCH_AREA_SHIFT;

	} /* of if(touchIdx < HY4614_handle.currActiveTouchNb) */
}

#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

/** @defgroup HY4614_Static_Function_Body
 * @{
 */

/* Static functions bodies-----------------------------------------------*/

/**
 * @brief  Return the status of I2C was initialized or not.
 * @param  None.
 * @retval : I2C initialization status.
 */
static uint8_t HY4614_Get_I2C_InitializedStatus(void) {
	return (HY4614_handle.i2cInitialized);
}

/**
 * @brief  I2C initialize if needed.
 * @param  None.
 * @retval : None.
 */
static void HY4614_I2C_InitializeIfRequired(void) {
	if (HY4614_Get_I2C_InitializedStatus() == HY4614_I2C_NOT_INITIALIZED) {
		/* Initialize TS IO BUS layer (I2C) */
		I2Cx_Init(&hI2cAudioHandler);

		/* Set state to initialized */
		HY4614_handle.i2cInitialized = HY4614_I2C_INITIALIZED;
	}
}

