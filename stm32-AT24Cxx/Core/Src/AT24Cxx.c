/*
февраль, 2021
Автор: Жуйков Дмитрий
github: DmitriyVentil

*/

/*
Библиотека позволяет записывать, читать и удалять, начиная с любого адреса и заканчивая любым адресом памяти,
без привязки к страницам самой микросхемы.
*/

#include "AT24Cxx.h"
#include "string.h"
static uint16_t PageSize[13]={8,8,8,16,16,16,32,32,64,64,128,256,256};

/*
 Функция записи в EEPROM AT24Cxx
 Dev_Adr - адрес AT24Cxx
 Mem_Adr - адрес начала записи
 data - буфер записи
 Data_Size - размер данных записи
*/
void AT24Cxx_Write(AT24CxxType type,uint8_t Dev_Adr, uint32_t Mem_Adr,uint8_t* data, uint32_t Data_Size)
{
	uint16_t Page_count=Mem_Adr%PageSize[type];
	uint8_t Dev_Adr_t;
	uint8_t adrSize= (type <= AT24C16) ? 1 : 2;

	if(type>=AT24C04 && type <=AT24C16)
	{
		Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>7);
	}
	else if(type>=AT24CM01)
	{
		Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);
	}  else  Dev_Adr_t=Dev_Adr;

	if((PageSize[type]-Page_count)>Data_Size)
	{
		while(HAL_I2C_Mem_Write(AT24Cxx_I2C, Dev_Adr_t, Mem_Adr, adrSize, data, Data_Size, 100)){};
	}
	else
	{
		while(HAL_I2C_Mem_Write(AT24Cxx_I2C, Dev_Adr_t, Mem_Adr, adrSize, data, (PageSize[type]-Page_count), 100)){};
		Data_Size-=(PageSize[type]-Page_count);
		Mem_Adr+=(PageSize[type]-Page_count);
		data+=(PageSize[type]-Page_count);
	}
	for(uint16_t i=0;i<Data_Size/PageSize[type];i++)
	{
		if(type>=AT24C04 && type <=AT24C16)
		{
			Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>7);
		}
		else if(type>=AT24CM01)
		{
			Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);
		}
		while(HAL_I2C_Mem_Write(AT24Cxx_I2C, Dev_Adr_t, Mem_Adr, adrSize, data+PageSize[type]*i, PageSize[type], 100)){};
		Mem_Adr+=PageSize[type];
	}
	if(Data_Size%PageSize[type]!=0)
	{
		if(type>=AT24C04 && type <=AT24C16)
		{
			Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>7);
		}
		else if(type>=AT24CM01)
		{
			Dev_Adr_t= Dev_Adr | (uint8_t)(Mem_Adr>>15);
		}
		while(HAL_I2C_Mem_Write(AT24Cxx_I2C, Dev_Adr_t, Mem_Adr, adrSize, data+PageSize[type]*(Data_Size/PageSize[type]), Data_Size%PageSize[type], 100)){};
	}
}
/*
 Функция чтения из EEPROM AT24Cxx
 Dev_Adr - адрес AT24Cxx
 Mem_Adr - адрес начала чтения
 data - буфер чтения
 Data_Size - размер данных чтения
*/
void AT24Cxx_Read(AT24CxxType type,uint8_t Dev_Adr, uint32_t Mem_Adr,uint8_t* data, uint32_t Data_Size)
{
	uint8_t Dev_Adr_t;
	uint8_t adrSize = (type <= AT24C16) ? 1 : 2;
	if(type>=AT24C04 && type <=AT24C16)
	{
		Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr>>7);
	}
	else if(type>=AT24CM01)
	{
		Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr>>15);
	} else Dev_Adr_t=Dev_Adr;

	if(Data_Size<32768)
	{
		while(HAL_I2C_Mem_Read(AT24Cxx_I2C, Dev_Adr_t, (uint16_t)Mem_Adr, adrSize, data,Data_Size, HAL_MAX_DELAY)){};
	}
	else
	{
		for(uint32_t i=0;i<Data_Size/32768;i++)
		{
			while(HAL_I2C_Mem_Read(AT24Cxx_I2C, Dev_Adr_t, (uint16_t)Mem_Adr, adrSize, &data[i*32768],32768, HAL_MAX_DELAY)){};
			Mem_Adr+=32768;
			if(type>=AT24C04 && type <=AT24C16)
			{
				Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr>>7);
			}
			else if(type>=AT24CM01)
			{
				Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr>>15);
			}
		}
		if(Data_Size%32768!=0)
		{
			while(HAL_I2C_Mem_Read(AT24Cxx_I2C, Dev_Adr_t, (uint16_t)Mem_Adr, adrSize, &data[(Data_Size/32768)*32768],Data_Size%32768, HAL_MAX_DELAY)){};
		}
	}
}
/*
 Функция очистки по начальному и конечному адресу
 Dev_Adr - адрес AT24Cxx
 Mem_Adr_Start - Начальный адрес(включая указанный адрес), с которого начинается удаление
 Mem_Adr_Stop - Конечный адрес(включая указанный адрес), на котором заканчивается удаление
*/
void AT24Cxx_Erase(AT24CxxType type,uint8_t Dev_Adr, uint32_t Mem_Adr_Start, uint32_t Mem_Adr_Stop)
{
	if(Mem_Adr_Start<Mem_Adr_Stop)
	{
		uint8_t EraseBuff[PageSize[type]];
		memset(EraseBuff,0xFF,PageSize[type]);
	    uint16_t Page_count=Mem_Adr_Start%PageSize[type];
        uint32_t Data_Size=Mem_Adr_Stop+1-Mem_Adr_Start;
    	uint8_t Dev_Adr_t;
    	uint8_t adrSize = (type <= AT24C16) ? 1 : 2;
    	if(type>=AT24C04 && type <=AT24C16)
    	{
    		Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr_Start>>7);
    	}
    	else if(type>=AT24CM01)
    	{
    		Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr_Start>>15);
    	} else  Dev_Adr_t = Dev_Adr;

		if((PageSize[type]-Page_count)>Data_Size)
		{
			while(HAL_I2C_Mem_Write(AT24Cxx_I2C, Dev_Adr_t, Mem_Adr_Start, adrSize, EraseBuff, Data_Size, 100)){};
		}
		else
		{
			while(HAL_I2C_Mem_Write(AT24Cxx_I2C, Dev_Adr_t, Mem_Adr_Start, adrSize, EraseBuff, (PageSize[type]-Page_count), 100)){};
			Data_Size=Data_Size-(PageSize[type]-Page_count);
			Mem_Adr_Start+=(PageSize[type]-Page_count);
		}

		for(uint16_t i=0;i<Data_Size/PageSize[type];i++)
		{
	    	if(type>=AT24C04 && type <=AT24C16)
	    	{
	    		Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr_Start>>7);
	    	}
	    	else if(type>=AT24CM01)
	    	{
	    		Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr_Start>>15);
	    	}
			while(HAL_I2C_Mem_Write(AT24Cxx_I2C, Dev_Adr_t, Mem_Adr_Start, adrSize, EraseBuff, PageSize[type], 100)){};
			Mem_Adr_Start+=PageSize[type];
		}
		if(Data_Size%PageSize[type]!=0)
		{
	    	if(type>=AT24C04 && type <=AT24C16)
	    	{
	    		Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr_Start>>7);
	    	}
	    	else if(type>=AT24CM01)
	    	{
	    		Dev_Adr_t=Dev_Adr|(uint8_t)(Mem_Adr_Start>>15);
	    	}
			while(HAL_I2C_Mem_Write(AT24Cxx_I2C, Dev_Adr_t, Mem_Adr_Start, adrSize, EraseBuff, Data_Size%PageSize[type], 100)){};
		}
	}
}
