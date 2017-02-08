/**
 * \file
 * \brief Interface for the Non-Volatile-Memory
 * \author Erich Styger
 *
 * This provides an interface to store and retrieve data from the on-chip memory.
 */

#ifndef CONFIGNVM_H_
#define CONFIGNVM_H_

#define NVMC_FLASH_BLOCKS		 32
//#define NVMC_FLASH_START_ADDR    0x1FFFE000 /* NVRM_Config, start address of configuration data in flash */
#define NVMC_FLASH_START_ADDR    0x38000
#define NVMC_FLASH_ERASED_UINT8  0xFF
#define NVMC_FLASH_ERASED_UINT16 0xFFFF

#define NVMC_SSID_PASS_DATA_START_ADDR  (NVMC_FLASH_START_ADDR)
#define NVMC_SSID_PASS_DATA_SIZE               (0x0400U) /* 1024 bytes of data */
#define NVMC_SSID_PASS_END_ADDR                (NVMC_SSID_PASS_DATA_START_ADDR+NVMC_SSID_PASS_DATA_SIZE)

char NVMC_SaveSSID_PASSData(void *data, int id);
void *NVMC_GetSSID_PASSData(int id);

#endif /* CONFIGNVM_H_ */
