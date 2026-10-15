/**
 * @file
 * @brief	Platform-specific system clock interface
 */

#include <string.h>

#include <mik32_hal_pcc.h>

#include <sysclock.h>

bool sysclock_init(void)
{
	const PCC_ConfigErrorsTypeDef no_errors = {HAL_OK};
	PCC_ConfigErrorsTypeDef errors;
	PCC_InitTypeDef pcc = {0};

	pcc.OscillatorEnable            = PCC_OSCILLATORTYPE_ALL;
	pcc.FreqMon.OscillatorSystem    = PCC_OSCILLATORTYPE_OSC32M;
	pcc.FreqMon.ForceOscSys         = PCC_FORCE_OSC_SYS_UNFIXED;
	pcc.FreqMon.Force32KClk         = PCC_FREQ_MONITOR_SOURCE_OSC32K;
	pcc.AHBDivider                  = 0;
	pcc.APBMDivider                 = 0;
	pcc.APBPDivider                 = 0;
	pcc.HSI32MCalibrationValue      = 128;
	pcc.LSI32KCalibrationValue      = 8;
	pcc.RTCClockSelection           = PCC_RTC_CLOCK_SOURCE_AUTO;
	pcc.RTCClockCPUSelection        = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
	errors = HAL_PCC_Config(&pcc);

	return (memcmp(&errors, &no_errors, sizeof(errors)) == 0);
}
