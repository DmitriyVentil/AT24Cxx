/*
февраль, 2021
Автор: Жуйков Дмитрий
github: DmitriyVentil

*/

#ifndef AT24C64_H_
#define AT24C64_H_

#include "main.h"
#include "stm32l4xx_hal.h"


//  i2c для AT24C64
extern I2C_HandleTypeDef hi2c1;
#define AT24C64_I2C  &hi2c1
// Адрес микросхемы памяти
#define Adr_AT24C64   0xA0
/*
 Функция записи в EEPROM AT24C64
 Dev_Adr - адрес AT24C64
 Mem_Adr - адрес начала записи
 data - буфер записи
 Data_Size - размер данных записи
*/
void AT24C64_Write(uint8_t Dev_Adr, uint16_t Mem_Adr,uint8_t* data, uint16_t Data_Size);
/*
 Функция чтения из EEPROM AT24C64
 Dev_Adr - адрес AT24C64
 Mem_Adr - адрес начала чтения
 data - буфер чтения
 Data_Size - размер данных чтения
*/
void AT24C64_Read(uint8_t Dev_Adr, uint16_t Mem_Adr,uint8_t* data, uint16_t Data_Size);
/*
 Функция поиска последнего адреса в EEPROM AT24C64
 Фукнция вернет  ближайший адрес, по которому не записано данных
 Dev_Adr - адрес AT24C64
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается поиск от 0 до 8191 (8192 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается поиск от 0 до 8191 (8192 адресов)
 Возвращается адрес ближайшего пустого элемента
*/
uint16_t AT24C64_Search_Last(uint8_t Dev_Adr, uint16_t Mem_Adr_Start, uint16_t Mem_Adr_Stop);
/*
 Функция очистки по начальному и конечному адресу
 Dev_Adr - адрес AT24C64
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается удаление от 0 до 8191 (8192 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается удаление от 0 до 8191 (8192 адресов)
*/
void AT24C64_Erase(uint8_t Dev_Adr, uint16_t Mem_Adr_Start, uint16_t Mem_Adr_Stop);



#endif /* AT24C64_H_ */
