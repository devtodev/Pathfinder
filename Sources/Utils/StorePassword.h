/**
 * \file
 * \brief Interface for the Non-Volatile-Memory
 * \author Erich Styger
 *
 * This provides an interface to store and retrieve data from the on-chip memory.
 */

#ifndef CONFIGNVM_H_
#define CONFIGNVM_H_

#define NVMC_FLASH_START_ADDR    0x3FC00 /* NVRM_Config, start address of configuration data in flash */
#define NVMC_FLASH_ERASED_UINT8  0xFF
#define NVMC_FLASH_ERASED_UINT16 0xFFFF

#define NVMC_SSID_PASS_DATA_START_ADDR  (NVMC_FLASH_START_ADDR)
#define NVMC_SSID_PASS_DATA_SIZE               (129) /* 4 bytes of data */
#define NVMC_SSID_PASS_END_ADDR                (NVMC_SSID_PASS_DATA_START_ADDR+NVMC_SSID_PASS_DATA_SIZE)

char NVMC_SaveSSID_PASSData(void *data, int dataSize);
void *NVMC_GetSSID_PASSData(void);

#endif /* CONFIGNVM_H_ */
