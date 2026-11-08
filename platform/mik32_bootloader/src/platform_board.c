/**
 * @file
 * @brief	Platform-specific board interface
 */

#include <limits.h>

#include <mik32_hal_spifi_w25.h>
#include <csr.h>

#include <board.h>

/* MIK32 SPIFI fast read command with Quad I/O. */
#define MIK32_SPIFI_CMD_FAST_READ_QUAD_IO	0xEB

/**
 * @brief	Configure MIK32 SPIFI for Quad-mode memory-mapped access.
 */
static void mik32_spifi_init(void)
{
	SPIFI_HandleTypeDef spifi = { .Instance = SPIFI_CONFIG };

	HAL_SPIFI_MspInit();
	HAL_SPIFI_Reset(&spifi);

	uint8_t sreg1 = HAL_SPIFI_W25_ReadSREG(&spifi, W25_SREG1);
	uint8_t sreg2 = HAL_SPIFI_W25_ReadSREG(&spifi, W25_SREG2);

	HAL_SPIFI_W25_WriteSREG(&spifi, sreg1, sreg2 | (1 << 1));

	SPIFI_MemoryCommandTypeDef cmd_mem = {
		.OpCode = MIK32_SPIFI_CMD_FAST_READ_QUAD_IO,
		.FieldForm = SPIFI_CONFIG_CMD_FIELDFORM_OPCODE_SERIAL,
		.FrameForm = SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR,
		.InterimData = 0,
		.InterimLength = 3,
	};

	SPIFI_MemoryModeConfig_HandleTypeDef spifi_mem = {
		.Instance = spifi.Instance,
		.CacheEnable = SPIFI_CACHE_ENABLE,
		.CacheLimit = UINT32_MAX,
		.Command = cmd_mem,
	};

	HAL_SPIFI_MemoryMode_Init(&spifi_mem);
}

bool board_init(void)
{
	mik32_spifi_init();

	write_csr(mtvec, 0x80000000);
	asm volatile("la ra, 0x80000000\n\t"
				 "jalr ra");

	while (true)
		;
}
