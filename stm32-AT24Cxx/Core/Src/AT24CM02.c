/*
февраль, 2021
Автор: Жуйков Дмитрий
github: DmitriyVentil

*/

/*
Библиотека позволяет записывать, читать и удалять, начиная с любого адреса и заканчивая любым адресом памяти,
без привязки к страницам самой микросхемы.
*/


#include "AT24CM02.h"

//2097152 бит (262144 байта)

/*
 Функция записи в EEPROM AT24CM02
 Dev_Adr - адрес AT24CM02
 Mem_Adr - адрес начала записи
 data - буфер записи
 Data_Size - размер данных записи
*/
void AT24CM02_Write(uint8_t Dev_Adr, uint32_t Mem_Adr,uint8_t* data, uint32_t Data_Size)
{
	uint16_t Page_count=Mem_Adr%256;
	uint8_t Dev_Adr_t;
	Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);
	if((256-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Write(AT24CM02_I2C, Dev_Adr_t, Mem_Adr, 2, data, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Write(AT24CM02_I2C, Dev_Adr_t, Mem_Adr, 2, data, (256-Page_count), 100)){};
		Data_Size=Data_Size-(256-Page_count);
		Mem_Adr=Mem_Adr+(256-Page_count);
		data=data+(256-Page_count);
	}

	for(uint16_t i=0;i<Data_Size/256;i++)
	{
		Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);
		while(HAL_I2C_Mem_Write(AT24CM02_I2C, Dev_Adr_t, Mem_Adr, 2, data+256*i, 256, 100)){};
		Mem_Adr=Mem_Adr+256;
	}
	if(Data_Size%256!=0)
	{
		Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);
		while(HAL_I2C_Mem_Write(AT24CM02_I2C, Dev_Adr_t, Mem_Adr, 2, data+256*(Data_Size/256), Data_Size%256, 100)){};
	}
}
/*
 Функция чтения из EEPROM AT24CM02
 Dev_Adr - адрес AT24CM02
 Mem_Adr - адрес начала чтения
 data - буфер чтения
 Data_Size - размер данных чтения
*/
void AT24CM02_Read(uint8_t Dev_Adr, uint32_t Mem_Adr,uint8_t* data, uint32_t Data_Size)
{

	uint16_t Page_count=Mem_Adr%256;
	uint8_t Dev_Adr_t;
	Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);

	if((256-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Read(AT24CM02_I2C, Dev_Adr_t, Mem_Adr, 2, data,Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Read(AT24CM02_I2C, Dev_Adr_t, Mem_Adr, 2, data, (256-Page_count), 100)){};
		Data_Size=Data_Size-(256-Page_count);
		Mem_Adr=Mem_Adr+(256-Page_count);
		data=data+(256-Page_count);
	}
	for(uint16_t i=0;i<Data_Size/256;i++)
	{
		Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);
		while(HAL_I2C_Mem_Read(AT24CM02_I2C, Dev_Adr_t, Mem_Adr, 2, data+256*i, 256, 100)){};
		Mem_Adr=Mem_Adr+256;
	}
	if(Data_Size%256!=0)
	{
		Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);
		while(HAL_I2C_Mem_Read(AT24CM02_I2C, Dev_Adr_t, Mem_Adr, 2, data+256*(Data_Size/256), Data_Size%256, 100)){};
	}
}
/*
 Функция поиска последнего адреса в EEPROM AT24CM02
 Фукнция вернет  ближайший адрес, по которому не записано данных
 Dev_Adr - адрес AT24CM02
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается поиск от 0 до 262143 (262144 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается поиск от 0 до 262143 (262144 адресов)
 Возвращается адрес ближайшего пустого элемента
*/
uint32_t AT24CM02_Search_Last(uint8_t Dev_Adr, uint32_t Mem_Adr_Start, uint32_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
	    uint16_t Page_count=Mem_Adr_Start%256;
	    uint32_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
	    uint8_t data[256];
	    uint16_t last_Adr_count=0;
    	uint8_t Dev_Adr_t;
    	Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr_Start>>15);

		if((256-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Read(AT24CM02_I2C, Dev_Adr_t, Mem_Adr_Start, 2, data, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Read(AT24CM02_I2C, Dev_Adr_t, Mem_Adr_Start, 2, data, (256-Page_count), 100)){};
			Data_Size=Data_Size-(256-Page_count);
		}
		for(last_Adr_count=0;last_Adr_count<256;last_Adr_count++)
		{
			if(data[last_Adr_count]==0xFF)
			   return (Mem_Adr_Start+last_Adr_count);
		}
		Mem_Adr_Start=Mem_Adr_Start+(256-Page_count);
		for(uint16_t i=0;i<Data_Size/256;i++)
		{
			Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr_Start>>15);
			while(HAL_I2C_Mem_Read(AT24CM02_I2C, Dev_Adr_t, Mem_Adr_Start, 2, data, 256, 100)){};
			for(last_Adr_count=0;last_Adr_count<256;last_Adr_count++)
			{
				if(data[last_Adr_count]==0xFF)
				   return (Mem_Adr_Start+last_Adr_count);
			}
			Mem_Adr_Start=Mem_Adr_Start+256;
		}
		if(Data_Size%256!=0)
		{
			Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr_Start>>15);
			while(HAL_I2C_Mem_Read(AT24CM02_I2C, Dev_Adr_t, Mem_Adr_Start, 2, data, Data_Size%256, 100)){};
		}
		for(last_Adr_count=0;last_Adr_count<256;last_Adr_count++)
		{
			if(data[last_Adr_count]==0xFF)
			   return (Mem_Adr_Start+last_Adr_count);
	    }
	}


	return 262144;
}
/*
 Функция очистки по начальному и конечному адресу
 Dev_Adr - адрес AT24CM02
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается удаление от 0 до 262143 (262144 адресов)
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается удаление от 0 до 262143 (262144 адресов)
*/
void AT24CM02_Erase(uint8_t Dev_Adr, uint32_t Mem_Adr_Start, uint32_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
	    uint8_t EraseBuff[256]=
	    {
	    		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	    		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	    };
	    uint16_t Page_count=Mem_Adr_Start%256;
        uint32_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
    	uint8_t Dev_Adr_t;
    	Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr_Start>>15);
		if((256-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Write(AT24CM02_I2C, Dev_Adr_t, Mem_Adr_Start, 2, EraseBuff, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Write(AT24CM02_I2C, Dev_Adr_t, Mem_Adr_Start, 2, EraseBuff, (256-Page_count), 100)){};
			Data_Size=Data_Size-(256-Page_count);
			Mem_Adr_Start=Mem_Adr_Start+(256-Page_count);
		}

		for(uint16_t i=0;i<Data_Size/256;i++)
		{
			Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr_Start>>15);
			while(HAL_I2C_Mem_Write(AT24CM02_I2C, Dev_Adr_t, Mem_Adr_Start, 2, EraseBuff, 256, 100)){};
			Mem_Adr_Start=Mem_Adr_Start+256;
		}
		if(Data_Size%256!=0)
		{
			Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr_Start>>15);
			while(HAL_I2C_Mem_Write(AT24CM02_I2C, Dev_Adr_t, Mem_Adr_Start, 2, EraseBuff, Data_Size%256, 100)){};
		}
	}
}
