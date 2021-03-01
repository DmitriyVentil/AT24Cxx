/*
февраль, 2021
Автор: Жуйков Дмитрий
github: DmitriyVentil

*/

/*
Библиотека позволяет записывать, читать и удалять, начиная с любого адреса и заканчивая любым адресом памяти,
без привязки к страницам самой микросхемы.
*/


#include "AT24C32.h"

//32768 бит (4096 байта)
/*
 Функция записи в EEPROM AT24C32
 Dev_Adr - адрес AT24C32
 Mem_Adr - адрес начала записи
 data - буфер записи
 Data_Size - размер данных записи
*/
void AT24C32_Write(uint8_t Dev_Adr, uint16_t Mem_Adr,uint8_t* data, uint16_t Data_Size)
{
	uint8_t Page_count=Mem_Adr%32;
	if((32-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Write(AT24C32_I2C, Dev_Adr, Mem_Adr, 2, data, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Write(AT24C32_I2C, Dev_Adr, Mem_Adr, 2, data, (32-Page_count), 100)){};
		Data_Size=Data_Size-(32-Page_count);
		Mem_Adr=Mem_Adr+(32-Page_count);
		data=data+(32-Page_count);
	}

	for(uint8_t i=0;i<Data_Size/32;i++)
	{
		while(HAL_I2C_Mem_Write(AT24C32_I2C, Dev_Adr, Mem_Adr, 2, data+32*i, 32, 100)){};
		Mem_Adr=Mem_Adr+32;
	}
	if(Data_Size%32!=0)
	{
		while(HAL_I2C_Mem_Write(AT24C32_I2C, Dev_Adr, Mem_Adr, 2, data+32*(Data_Size/32), Data_Size%32, 100)){};
	}
}
/*
 Функция чтения из EEPROM AT24C32
 Dev_Adr - адрес AT24C32
 Mem_Adr - адрес начала чтения
 data - буфер чтения
 Data_Size - размер данных чтения
*/
void AT24C32_Read(uint8_t Dev_Adr, uint16_t Mem_Adr,uint8_t* data, uint16_t Data_Size)
{

	uint8_t Page_count=Mem_Adr%32;

	if((32-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Read(AT24C32_I2C, Dev_Adr, Mem_Adr, 2, data,Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Read(AT24C32_I2C, Dev_Adr, Mem_Adr, 2, data, (32-Page_count), 100)){};
		Data_Size=Data_Size-(32-Page_count);
		Mem_Adr=Mem_Adr+(32-Page_count);
		data=data+(32-Page_count);
	}
	for(uint16_t i=0;i<Data_Size/32;i++)
	{
		while(HAL_I2C_Mem_Read(AT24C32_I2C, Dev_Adr, Mem_Adr, 2, data+32*i, 32, 100)){};
		Mem_Adr=Mem_Adr+32;
	}
	if(Data_Size%32!=0)
	{
		while(HAL_I2C_Mem_Read(AT24C32_I2C, Dev_Adr, Mem_Adr, 2, data+32*(Data_Size/32), Data_Size%32, 100)){};
	}
}
/*
 Функция поиска последнего адреса в EEPROM AT24C32
 Фукнция вернет  ближайший адрес, по которому не записано данных
 Dev_Adr - адрес AT24C32
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается поиск от 0 до 4095 (4096 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается поиск от 0 до 4095 (4096 адресов)
 Возвращается адрес ближайшего пустого элемента
*/
uint16_t AT24C32_Search_Last(uint8_t Dev_Adr, uint16_t Mem_Adr_Start, uint16_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
	    uint8_t Page_count=Mem_Adr_Start%32;
	    uint16_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
	    uint8_t data[32]={0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	    uint8_t last_Adr_count=0;
		if((32-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Read(AT24C32_I2C, Dev_Adr, Mem_Adr_Start, 2, data, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Read(AT24C32_I2C, Dev_Adr, Mem_Adr_Start, 2, data, (32-Page_count), 100)){};
			Data_Size=Data_Size-(32-Page_count);
		}
		for(last_Adr_count=0;last_Adr_count<32;last_Adr_count++)
		{
			if(data[last_Adr_count]==0xFF)
			   return (Mem_Adr_Start+last_Adr_count);
		}
		Mem_Adr_Start=Mem_Adr_Start+(32-Page_count);
		for(uint16_t i=0;i<Data_Size/32;i++)
		{
			while(HAL_I2C_Mem_Read(AT24C32_I2C, Dev_Adr, Mem_Adr_Start, 2, data, 32, 100)){};
			for(last_Adr_count=0;last_Adr_count<32;last_Adr_count++)
			{
				if(data[last_Adr_count]==0xFF)
				   return (Mem_Adr_Start+last_Adr_count);
			}
			Mem_Adr_Start=Mem_Adr_Start+32;
		}
		if(Data_Size%32!=0)
		{
			while(HAL_I2C_Mem_Read(AT24C32_I2C, Dev_Adr, Mem_Adr_Start, 2, data, Data_Size%32, 100)){};
		}
		for(last_Adr_count=0;last_Adr_count<32;last_Adr_count++)
		{
			if(data[last_Adr_count]==0xFF)
			   return (Mem_Adr_Start+last_Adr_count);
	    }
	}


	    return 4096;
}
/*
 Функция очистки по начальному и конечному адресу
 Dev_Adr - адрес AT24C32
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается удаление от 0 до 4095 (4096 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается удаление от 0 до 4095 (4096 адресов)
*/
void AT24C32_Erase(uint8_t Dev_Adr, uint16_t Mem_Adr_Start, uint16_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
	    uint8_t EraseBuff[32]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	    uint8_t Page_count=Mem_Adr_Start%32;
        uint16_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
		if((32-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Write(AT24C32_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Write(AT24C32_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, (32-Page_count), 100)){};
			Data_Size=Data_Size-(32-Page_count);
			Mem_Adr_Start=Mem_Adr_Start+(32-Page_count);
		}

		for(uint8_t i=0;i<Data_Size/32;i++)
		{
			while(HAL_I2C_Mem_Write(AT24C32_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, 32, 100)){};
			Mem_Adr_Start=Mem_Adr_Start+32;
		}
		if(Data_Size%32!=0)
		{
			while(HAL_I2C_Mem_Write(AT24C32_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, Data_Size%32, 100)){};
		}
	}
}
