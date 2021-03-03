/*
февраль, 2021
Автор: Жуйков Дмитрий
github: DmitriyVentil

*/

#ifndef AT24Cxx_H_
#define AT24Cxx_H_

#include "main.h"
#include "stm32l4xx_hal.h"


//  i2c для AT24Cxx
extern I2C_HandleTypeDef hi2c1;
#define AT24Cxx_I2C  &hi2c1
// Адрес AT24Cxx
#define Adr_AT24Cxx   0xA0

// Используемая микросхема
typedef enum
{
	AT24C00, 	//128 бит (16 байта)
	AT24C01, 	//1024 бит (128 байта)
	AT24C02, 	//2048 бит (256 байта)
	AT24C04, 	//4096 бит (512 байта)
	AT24C08, 	//8192 бит (1024 байта)
	AT24C16, 	//16384 бит (2048 байта)
	AT24C32, 	//32768 бит (4096 байта)
	AT24C64, 	//65536 бит (8192 байта)
	AT24C128,	//131072 бит (16384 байта)
	AT24C256,	//262144 бит (32768 байта)
	AT24C512, 	//524288 бит (65536 байта)
	AT24CM01,	//1048576 бит (131072 байта)
	AT24CM02,	//2097152 бит (262144 байта)
} AT24CxxType;

/*
 Функция записи в EEPROM AT24Cxx
 Dev_Adr - адрес AT24Cxx
 Mem_Adr - адрес начала записи
 data - буфер записи
 Data_Size - размер данных записи
*/
void AT24Cxx_Write(AT24CxxType type,uint8_t Dev_Adr, uint32_t Mem_Adr,uint8_t* data, uint32_t Data_Size);
/*
 Функция чтения из EEPROM AT24Cxx
 Dev_Adr - адрес AT24Cxx
 Mem_Adr - адрес начала чтения
 data - буфер чтения
 Data_Size - размер данных чтения
*/
void AT24Cxx_Read(AT24CxxType type,uint8_t Dev_Adr, uint32_t Mem_Adr,uint8_t* data, uint32_t Data_Size);
/*
 Функция очистки по начальному и конечному адресу
 Dev_Adr - адрес AT24Cxx
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается удаление
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается удаление
*/
void AT24Cxx_Erase(AT24CxxType type,uint8_t Dev_Adr, uint32_t Mem_Adr_Start, uint32_t Mem_Adr_Stop);



#endif /* AT24Cxx_H_ */
