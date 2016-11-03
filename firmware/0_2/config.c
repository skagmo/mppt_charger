#include "config.h"

#include <stdint.h>

config_t config;
const char* STATE_STRING[] = {"SHDN", "INC", "DEC", "CV"};
uint8_t mppt_state = SHUTDOWN;
uint16_t duty_cycle = DUTY_CYCLE_MAX;
uint16_t pv_v, pv_i, bat_v, bat_i_in, bat_i_out, bat_v_old;

void config_default_load(void){
	config.pv_v_min = 16000;
	config.bat_i_in_min = 5;
	config.bat_v_max = 12000;
	config.aux1_shdn = 10500;
	config.aux2_shdn = 10500;
	config.shdn_hyst = 500;
	config.aux1_dstate = 1;
	config.aux2_dstate = 1;
	config.print_int = 1000;
}
//
//uint8_t config_flash_load(void){
//    volatile unsigned char value = 0x09;
//    unsigned char address = 0xE5;
//    eeprom_write(address, value);     // Writing value 0x9 to EEPROM address 0xE5        
//    value = eeprom_read (address);    // Reading the value from address 0xE5
//	
//	config_t temp;
//	uint8_t* temp_u8 = &temp;
//
//	for (j=0; j<sizeof(config_t); j++){
//		
//	}
//	if ( (flash_config->checksum == crc_ccitt((uint8_t*)APP_CONFIG_FLASH_ADDRESS+2, sizeof(config_t)-2)) &&
//	     (flash_config->config_version == CONFIG_VERSION) ){
//		memcpy(&config, (void*)APP_CONFIG_FLASH_ADDRESS, sizeof(config_t));
//		return RET_OK;
//	}
//	return RET_ERROR;
//}
//
//uint8_t config_flash_save(void){
//	unsigned int j, k;
//	uint32_t* config_32 = (uint32_t*)&config;
//
//	// Calculate CRC
//	config.checksum = crc_ccitt((uint8_t*)&config+2, sizeof(config_t)-2);
//		
//	// Erase whole page in flash
//	if (NVMErasePage((uint8_t*)APP_CONFIG_FLASH_ADDRESS)) return RET_ERROR;
//
//	for (j=0; j<sizeof(config_t); j+=4){
//		if ( NVMWriteWord( (uint32_t*)(APP_CONFIG_FLASH_ADDRESS+j), config_32[j/4]) ) return RET_ERROR;
//	}
//
//	return RET_OK;
//}
//	