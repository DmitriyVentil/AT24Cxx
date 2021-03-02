/*
февраль, 2021
Автор: Жуйков Дмитрий
github: DmitriyVentil

*/

/*
Библиотека позволяет записывать, читать и удалять, начиная с любого адреса и заканчивая любым адресом памяти,
без привязки к страницам самой микросхемы.
*/


#include "AT24C512.h"

//524288 бит (65536 байта)

/*
 Функция записи в EEPROM AT24C512
 Dev_Adr - адрес AT24C512
 Mem_Adr - адрес начала записи
 data - буфер записи
 Data_Size - размер данных записи
*/
void AT24C512_Write(uint8_t Dev_Adr, uint32_t Mem_Adr,uint8_t* data, uint32_t Data_Size)
{
	uint16_t Page_count=Mem_Adr%128;
	if((128-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Write(AT24C512_I2C, Dev_Adr, Mem_Adr, 2, data, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Write(AT24C512_I2C, Dev_Adr, Mem_Adr, 2, data, (128-Page_count), 100)){};
		Data_Size=Data_Size-(128-Page_count);
		Mem_Adr=Mem_Adr+(128-Page_count);
		data=data+(128-Page_count);
	}

	for(uint16_t i=0;i<Data_Size/128;i++)
	{
		while(HAL_I2C_Mem_Write(AT24C512_I2C, Dev_Adr, Mem_Adr, 2, data+128*i, 128, 100)){};
		Mem_Adr=Mem_Adr+128;
	}
	if(Data_Size%128!=0)
	{
		while(HAL_I2C_Mem_Write(AT24C512_I2C, Dev_Adr, Mem_Adr, 2, data+128*(Data_Size/128), Data_Size%128, 100)){};
	}
}
/*
 Функция чтения из EEPROM AT24C512
 Dev_Adr - адрес AT24C512
 Mem_Adr - адрес начала чтения
 data - буфер чтения
 Data_Size - размер данных чтения
*/
void AT24C512_Read(uint8_t Dev_Adr, uint32_t Mem_Adr,uint8_t* data, uint32_t Data_Size)
{

	uint16_t Page_count=Mem_Adr%128;

	if((128-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Read(AT24C512_I2C, Dev_Adr, Mem_Adr, 2, data,Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Read(AT24C512_I2C, Dev_Adr, Mem_Adr, 2, data, (128-Page_count), 100)){};
		Data_Size=Data_Size-(128-Page_count);
		Mem_Adr=Mem_Adr+(128-Page_count);
		data=data+(128-Page_count);
	}
	for(uint16_t i=0;i<Data_Size/128;i++)
	{
		while(HAL_I2C_Mem_Read(AT24C512_I2C, Dev_Adr, Mem_Adr, 2, data+128*i, 128, 100)){};
		Mem_Adr=Mem_Adr+128;
	}
	if(Data_Size%128!=0)
	{
		while(HAL_I2C_Mem_Read(AT24C512_I2C, Dev_Adr, Mem_Adr, 2, data+128*(Data_Size/128), Data_Size%128, 100)){};
	}
}
/*
 Функция поиска последнего адреса в EEPROM AT24C512
 Фукнция вернет  ближайший адрес, по которому не записано данных
 Dev_Adr - адрес AT24C512
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается поиск от 0 до 65535 (65536 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается поиск от 0 до 65535 (65536 адресов)
 Возвращается адрес ближайшего пустого элемента
*/
uint32_t AT24C512_Search_Last(uint8_t Dev_Adr, uint32_t Mem_Adr_Start, uint32_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
	    uint16_t Page_count=Mem_Adr_Start%128;
	    uint32_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
	    uint8_t data[128];
	    uint16_t last_Adr_count=0;

		if((128-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Read(AT24C512_I2C, Dev_Adr, Mem_Adr_Start, 2, data, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Read(AT24C512_I2C, Dev_Adr, Mem_Adr_Start, 2, data, (128-Page_count), 100)){};
			Data_Size=Data_Size-(128-Page_count);
		}
		for(last_Adr_count=0;last_Adr_count<128;last_Adr_count++)
		{
			if(data[last_Adr_count]==0xFF)
			   return (Mem_Adr_Start+last_Adr_count);
		}
		Mem_Adr_Start=Mem_Adr_Start+(128-Page_count);
		for(uint16_t i=0;i<Data_Size/128;i++)
		{
			while(HAL_I2C_Mem_Read(AT24C512_I2C, Dev_Adr, Mem_Adr_Start, 2, data, 128, 100)){};
			for(last_Adr_count=0;last_Adr_count<128;last_Adr_count++)
			{
				if(data[last_Adr_count]==0xFF)
				   return (Mem_Adr_Start+last_Adr_count);
			}
			Mem_Adr_Start=Mem_Adr_Start+128;
		}
		if(Data_Size%128!=0)
		{
			while(HAL_I2C_Mem_Read(AT24C512_I2C, Dev_Adr, Mem_Adr_Start, 2, data, Data_Size%128, 100)){};
		}
		for(last_Adr_count=0;last_Adr_count<128;last_Adr_count++)
		{
			if(data[last_Adr_count]==0xFF)
			   return (Mem_Adr_Start+last_Adr_count);
	    }
	}


	return 65536;
}
/*
 Функция очистки по начальному и конечному адресу
 Dev_Adr - адрес AT24C512
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается удаление от 0 до 65535 (65536 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается удаление от 0 до 65535 (65536 адресов)
*/
void AT24C512_Erase(uint8_t Dev_Adr, uint32_t Mem_Adr_Start, uint32_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
	    uint8_t EraseBuff[128]=
	    {
	    		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	    		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	    };
	    uint16_t Page_count=Mem_Adr_Start%128;
        uint32_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
		if((128-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Write(AT24C512_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Write(AT24C512_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, (128-Page_count), 100)){};
			Data_Size=Data_Size-(128-Page_count);
			Mem_Adr_Start=Mem_Adr_Start+(128-Page_count);
		}

		for(uint16_t i=0;i<Data_Size/128;i++)
		{
			while(HAL_I2C_Mem_Write(AT24C512_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, 128, 100)){};
			Mem_Adr_Start=Mem_Adr_Start+128;
		}
		if(Data_Size%128!=0)
		{
			while(HAL_I2C_Mem_Write(AT24C512_I2C, Dev_Adr, Mem_Adr_Start, 2, EraseBuff, Data_Size%128, 100)){};
		}
	}
}
