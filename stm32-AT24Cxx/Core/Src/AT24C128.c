/*
февраль, 2021
Автор: Жуйков Дмитрий
github: DmitriyVentil

*/

/*
Библиотека позволяет записывать, читать и удалять, начиная с любого адреса и заканчивая любым адресом памяти,
без привязки к страницам самой микросхемы.
*/


#include "AT24C128.h"

//131072 бит (16384 байта)
/*
 Функция записи в EEPROM AT24C128
 Dev_Adr - адрес AT24C128
 Mem_Adr - адрес начала записи
 data - буфер записи
 Data_Size - размер данных записи
*/
void AT24C128_Write(uint8_t Dev_Adr, uint16_t Mem_Adr,uint8_t* data, uint32_t Data_Size)
{
	uint16_t Page_count=Mem_Adr%64;
	if((64-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Write(AT24C128_I2C, Dev_Adr, Mem_Adr, 2, data, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Write(AT24C128_I2C, Dev_Adr, Mem_Adr, 2, data, (64-Page_count), 100)){};
		Data_Size=Data_Size-(64-Page_count);
		Mem_Adr=Mem_Adr+(64-Page_count);
		data=data+(64-Page_count);
	}

	for(uint16_t i=0;i<Data_Size/64;i++)
	{
		while(HAL_I2C_Mem_Write(AT24C128_I2C, Dev_Adr, Mem_Adr, 2, data+64*i, 64, 100)){};
		Mem_Adr=Mem_Adr+64;
	}
	if(Data_Size%64!=0)
	{
		while(HAL_I2C_Mem_Write(AT24C128_I2C, Dev_Adr, Mem_Adr, 2, data+64*(Data_Size/64), Data_Size%64, 100)){};
	}
}
/*
 Функция чтения из EEPROM AT24C128
 Dev_Adr - адрес AT24C128
 Mem_Adr - адрес начала чтения
 data - буфер чтения
 Data_Size - размер данных чтения
*/
void AT24C128_Read(uint8_t Dev_Adr, uint16_t Mem_Adr,uint8_t* data, uint32_t Data_Size)
{

	uint16_t Page_count=Mem_Adr%64;

	if((64-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Read(AT24C128_I2C, Dev_Adr, Mem_Adr, 2, data,Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Read(AT24C128_I2C, Dev_Adr, Mem_Adr, 2, data, (64-Page_count), 100)){};
		Data_Size=Data_Size-(64-Page_count);
		Mem_Adr=Mem_Adr+(64-Page_count);
		data=data+(64-Page_count);
	}
	for(uint16_t i=0;i<Data_Size/64;i++)
	{
		while(HAL_I2C_Mem_Read(AT24C128_I2C, Dev_Adr, Mem_Adr, 2, data+64*i, 64, 100)){};
		Mem_Adr=Mem_Adr+64;
	}
	if(Data_Size%64!=0)
	{
		while(HAL_I2C_Mem_Read(AT24C128_I2C, Dev_Adr, Mem_Adr, 2, data+64*(Data_Size/64), Data_Size%64, 100)){};
	}
}
/*
 Функция поиска последнего адреса в EEPROM AT24C128
 Фукнция вернет  ближайший адрес, по которому не записано данных
 Dev_Adr - адрес AT24C128
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается поиск от 0 до 16383 (16384 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается поиск от 0 до 16383 (16384 адресов)
 Возвращается адрес ближайшего пустого элемента
*/
uint16_t AT24C128_Search_Last(uint8_t Dev_Adr, uint16_t Mem_Adr_Start, uint16_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
	    uint16_t Page_count=Mem_Adr_Start%64;
	    uint16_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
	    uint8_t data[64]={0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	    uint8_t last_Adr_count=0;
		if((64-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Read(AT24C128_I2C, Dev_Adr, Mem_Adr_Start, 2, data, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Read(AT24C128_I2C, Dev_Adr, Mem_Adr_Start, 2, data, (64-Page_count), 100)){};
			Data_Size=Data_Size-(64-Page_count);
		}
		for(last_Adr_count=0;last_Adr_count<64;last_Adr_count++)
		{
			if(data[last_Adr_count]==0xFF)
			   return (Mem_Adr_Start+last_Adr_count);
		}
		Mem_Adr_Start=Mem_Adr_Start+(64-Page_count);
		for(uint16_t i=0;i<Data_Size/64;i++)
		{
			while(HAL_I2C_Mem_Read(AT24C128_I2C, Dev_Adr, Mem_Adr_Start, 2, data, 64, 100)){};
			for(last_Adr_count=0;last_Adr_count<64;last_Adr_count++)
			{
				if(data[last_Adr_count]==0xFF)
				   return (Mem_Adr_Start+last_Adr_count);
			}
			Mem_Adr_Start=Mem_Adr_Start+64;
		}
		if(Data_Size%64!=0)
		{
			while(HAL_I2C_Mem_Read(AT24C128_I2C, Dev_Adr, Mem_Adr_Start, 2, data, Data_Size%64, 100)){};
		}
		for(last_Adr_count=0;last_Adr_count<64;last_Adr_count++)
		{
			if(data[last_Adr_count]==0xFF)
			   return (Mem_Adr_Start+last_Adr_count);
	    }
	}


	return 16384;
}
/*
 Функция очистки по начальному и конечному адресу
 Dev_Adr - адрес AT24C128
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается удаление от 0 до 16383 (16384 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается удаление от 0 до 16383 (16384 адресов)
*/
void AT24C128_Erase(uint8_t Dev_Adr, uint16_t Mem_Adr_Start, uint16_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
	    uint8_t EraseBuff[64]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	    		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	    uint16_t Page_count=Mem_Adr_Start%64;
        uint16_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
		if((64-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Write(AT24C128_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Write(AT24C128_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, (64-Page_count), 100)){};
			Data_Size=Data_Size-(64-Page_count);
			Mem_Adr_Start=Mem_Adr_Start+(64-Page_count);
		}

		for(uint16_t i=0;i<Data_Size/64;i++)
		{
			while(HAL_I2C_Mem_Write(AT24C128_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, 64, 100)){};
			Mem_Adr_Start=Mem_Adr_Start+64;
		}
		if(Data_Size%64!=0)
		{
			while(HAL_I2C_Mem_Write(AT24C128_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, Data_Size%64, 100)){};
		}
	}
}
