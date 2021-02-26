/*
февраль, 2021
Автор: Жуйков Дмитрий
github: DmitriyVentil

*/

/*
Библиотека позволяет записывать, читать и удалять, начиная с любого адреса и заканчивая любым адресом памяти,
без привязки к страницам самой микросхемы.

 */

#include "AT24C08.h"

//8192 бит (1024 байта)

// Функция обновления адреса блока 256 байтов
// Не используется пользователем
uint8_t AT24C08_Block_Count(uint8_t Dev_Adr,uint16_t Mem_Adr)
{
	if(Mem_Adr>0x2FF){Dev_Adr=(Dev_Adr | 0x06) & 0xF6;}
	else if(Mem_Adr>0x1FF){Dev_Adr=(Dev_Adr | 0x04)& 0xF4;}
	else if(Mem_Adr>0xFF){Dev_Adr=(Dev_Adr | 0x02)& 0xF2;}
	return Dev_Adr;
}
/*
 Функция записи в EEPROM AT24C08
 Dev_Adr - адрес AT24C08
 Mem_Adr - адрес начала записи
 data - буфер записи
 Data_Size - размер данных записи
*/
void AT24C08_Write(uint8_t Dev_Adr, uint16_t Mem_Adr,uint8_t* data, uint16_t Data_Size)
{
	uint8_t Page_count=Mem_Adr%16;
	Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr);
	if((16-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Write(AT24C08_I2C, Dev_Adr, Mem_Adr, 1, data, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Write(AT24C08_I2C, Dev_Adr, Mem_Adr, 1, data, (16-Page_count), 100)){};
		Data_Size=Data_Size-(16-Page_count);
		Mem_Adr=Mem_Adr+(16-Page_count);
		data=data+(16-Page_count);
	}

	for(uint8_t i=0;i<Data_Size/16;i++)
	{
		Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr);
		while(HAL_I2C_Mem_Write(AT24C08_I2C, Dev_Adr, Mem_Adr, 1, data+16*i, 16, 100)){};
		Mem_Adr=Mem_Adr+16;
	}
	if(Data_Size%16!=0)
	{
		Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr);
		while(HAL_I2C_Mem_Write(AT24C08_I2C, Dev_Adr, Mem_Adr, 1, data+16*(Data_Size/16), Data_Size%16, 100)){};
	}
}
/*
 Функция чтения из EEPROM AT24C08
 Dev_Adr - адрес AT24C08
 Mem_Adr - адрес начала чтения
 data - буфер чтения
 Data_Size - размер данных чтения
*/
void AT24C08_Read(uint8_t Dev_Adr, uint16_t Mem_Adr,uint8_t* data, uint16_t Data_Size)
{

	uint8_t Page_count=Mem_Adr%16;

	Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr);
	if((16-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Read(AT24C08_I2C, Dev_Adr, Mem_Adr, 1, data, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Read(AT24C08_I2C, Dev_Adr, Mem_Adr, 1, data, (16-Page_count), 100)){};
		Data_Size=Data_Size-(16-Page_count);
		Mem_Adr=Mem_Adr+(16-Page_count);
		data=data+(16-Page_count);
	}
	for(uint16_t i=0;i<Data_Size/16;i++)
	{
		Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr);
		while(HAL_I2C_Mem_Read(AT24C08_I2C, Dev_Adr, Mem_Adr, 1, data+16*i, 16, 100)){};
		Mem_Adr=Mem_Adr+16;
	}
	if(Data_Size%16!=0)
	{
		Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr);
		while(HAL_I2C_Mem_Read(AT24C08_I2C, Dev_Adr, Mem_Adr, 1, data+16*(Data_Size/16), Data_Size%16, 100)){};
	}
}
/*
 Функция поиска последнего адреса в EEPROM AT24C08
 Фукнция вернет  ближайший адрес, по которому не записано данных
 Dev_Adr - адрес AT24C08
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается поиск от 0 до 1023 (1024 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается поиск от 0 до 1023 (1024 адресов)
 Возвращается адрес ближайшего пустого элемента
*/
uint16_t AT24C08_Search_Last(uint8_t Dev_Adr, uint16_t Mem_Adr_Start, uint16_t Mem_Adr_Stop)
{
	uint8_t Page_count=Mem_Adr_Start%16;
	uint16_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
	uint8_t data[16]={0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	uint8_t last_Adr_count=0;
	Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr_Start);
	if((16-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Read(AT24C08_I2C, Dev_Adr, Mem_Adr_Start, 1, data, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Read(AT24C08_I2C, Dev_Adr, Mem_Adr_Start, 1, data, (16-Page_count), 100)){};
		Data_Size=Data_Size-(16-Page_count);
	}
	for(last_Adr_count=0;last_Adr_count<16;last_Adr_count++)
	{
		if(data[last_Adr_count]==0xFF)
		   return (Mem_Adr_Start+last_Adr_count);
	}
	Mem_Adr_Start=Mem_Adr_Start+(16-Page_count);
	for(uint16_t i=0;i<Data_Size/16;i++)
	{
		Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr_Start);
		while(HAL_I2C_Mem_Read(AT24C08_I2C, Dev_Adr, Mem_Adr_Start, 1, data, 16, 100)){};
		for(last_Adr_count=0;last_Adr_count<16;last_Adr_count++)
			{
				if(data[last_Adr_count]==0xFF)
				   return (Mem_Adr_Start+last_Adr_count);
			}
		Mem_Adr_Start=Mem_Adr_Start+16;
	}
	if(Data_Size%16!=0)
	{
		Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr_Start);
		while(HAL_I2C_Mem_Read(AT24C08_I2C, Dev_Adr, Mem_Adr_Start, 1, data, Data_Size%16, 100)){};
	}
	for(last_Adr_count=0;last_Adr_count<16;last_Adr_count++)
	{
		if(data[last_Adr_count]==0xFF)
		   return (Mem_Adr_Start+last_Adr_count);
	}

	return 1024;
}
/*
 Функция очистки по начальному и конечному адресу
 Dev_Adr - адрес AT24C08
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается удаление от 0 до 1023 (1024 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается удаление от 0 до 1023 (1024 адресов)
*/
void AT24C08_Erase(uint8_t Dev_Adr, uint16_t Mem_Adr_Start, uint16_t Mem_Adr_Stop)
{
	uint8_t EraseBuff[16]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	uint8_t Page_count=Mem_Adr_Start%16;
	uint16_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
	Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr_Start);
	if((16-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Write(AT24C08_I2C, Dev_Adr, Mem_Adr_Start, 1, EraseBuff, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Write(AT24C08_I2C, Dev_Adr, Mem_Adr_Start, 1, EraseBuff, (16-Page_count), 100)){};
		Data_Size=Data_Size-(16-Page_count);
		Mem_Adr_Start=Mem_Adr_Start+(16-Page_count);
	}

	for(uint8_t i=0;i<Data_Size/16;i++)
	{
		Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr_Start);
		while(HAL_I2C_Mem_Write(AT24C08_I2C, Dev_Adr, Mem_Adr_Start, 1, EraseBuff, 16, 100)){};
		Mem_Adr_Start=Mem_Adr_Start+16;
	}
	if(Data_Size%16!=0)
	{
		Dev_Adr=AT24C08_Block_Count(Dev_Adr,Mem_Adr_Start);
		while(HAL_I2C_Mem_Write(AT24C08_I2C, Dev_Adr, Mem_Adr_Start, 1, EraseBuff, Data_Size%16, 100)){};
	}
}
