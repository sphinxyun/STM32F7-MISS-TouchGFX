#include "audio_if.h"

/* External variables --------------------------------------------------------*/
static AUDIO_IFTypeDef AudioIf;

AUDIO_ProcessTypdef haudio __attribute__((section(".RamData1")));

/**
 * @brief  Register Audio callbacks
 * @param  callbacks
 * @retval None
 */
void AUDIO_IF_RegisterCallbacks(pFunc tc_cb, pFunc ht_cb, pFunc err_cb) {
	AudioIf.TransferComplete_CallBack = tc_cb;
	AudioIf.HalfTransfer_CallBack = ht_cb;
	AudioIf.Error_CallBack = err_cb;
}
/**
 * @brief  Manages the DMA Transfer complete interrupt.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
	if (AudioIf.TransferComplete_CallBack) {
		AudioIf.TransferComplete_CallBack();
	}
}

/**
 * @brief  Manages the DMA Half Transfer complete interrupt.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void) {
	if (AudioIf.HalfTransfer_CallBack) {
		AudioIf.HalfTransfer_CallBack();
	}
}

/**
 * @brief  Manages the DMA FIFO error interrupt.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_OUT_Error_CallBack(void) {
	if (AudioIf.Error_CallBack) {
		AudioIf.Error_CallBack();
	}
}
