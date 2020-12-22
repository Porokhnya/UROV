/**
// класс работы с EEPROM-памятью

AT24CX.cpp
Library for using the EEPROM AT24C32/64

Copyright (c) 2014 Christian Paul

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

 */
#include "AT24CX.h"
#include "ConfigPin.h"
#include <Wire.h>
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор базового класса
 */
AT24CX::AT24CX(TwoWire& w) 
{
  wire = &w;
	init(0, 32);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор базового класса, с указанным индексом микросхемы на шине и размером страницы
 */
AT24CX::AT24CX(TwoWire& w,byte index, uint16_t pageSize) 
{
  wire = &w;
	init(index, pageSize);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C32 EEPROM с индексом 0
 */
AT24C32::AT24C32(TwoWire& w) : AT24CX(w) 
{
	init(0, 32);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C32 EEPROM с переданным индексом
 */
AT24C32::AT24C32(TwoWire& w,byte index) : AT24CX(w)  
{
	init(index, 32);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C64 EEPROM с индексом 0
 */
AT24C64::AT24C64(TwoWire& w) : AT24CX(w)  
{
	init(0, 32);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C64 EEPROM с переданным индексом
 */
AT24C64::AT24C64(TwoWire& w,byte index) : AT24CX(w)  
{
	init(index, 32);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C128 EEPROM с индексом 0
 */
AT24C128::AT24C128(TwoWire& w) : AT24CX(w)  
{
	init(0, 64);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C128 EEPROM с переданным индексом
 */
AT24C128::AT24C128(TwoWire& w,byte index) : AT24CX(w)  
{
	init(index, 64);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C256 EEPROM с индексом 0
 */
AT24C256::AT24C256(TwoWire& w) : AT24CX(w)  
{
	init(0, 64);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C256 EEPROM с переданным индексом
 */
AT24C256::AT24C256(TwoWire& w,byte index) : AT24CX(w)  
{
	init(index, 64);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C512 EEPROM с индексом 0
 */
AT24C512::AT24C512(TwoWire& w) : AT24CX(w)  
{
	init(0, 128);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C512 EEPROM с переданным индексом
 */
AT24C512::AT24C512(TwoWire& w,byte index) : AT24CX(w)  
{
	init(index, 128);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C1024 EEPROM с индексом 0
 */
AT24C1024::AT24C1024(TwoWire& w) : AT24CX(w)  
{
  init(0, 256);
}
//--------------------------------------------------------------------------------------------------
/**
 * Конструктор AT24C1024 EEPROM с переданным индексом
 */
AT24C1024::AT24C1024(TwoWire& w,byte index) : AT24CX(w)  
{
  init(index, 256);
}
//--------------------------------------------------------------------------------------------------
/**
 * Инициализация
 */
void AT24CX::init(byte index, uint16_t pageSize) 
{
	_id = AT24CX_ID | (index & 0x7);
	_pageSize = pageSize;
//	Wire.begin(); 
}
//--------------------------------------------------------------------------------------------------
/**
 * Запись байта
 */
void AT24CX::write(unsigned int address, byte data) 
{
    wire->beginTransmission(_id);
    
    if(wire->endTransmission()==0) 
    {
    	wire->beginTransmission(_id);
    	wire->write(address >> 8);
    	wire->write(address & 0xFF);
      	wire->write(data);
    	wire->endTransmission();
    	delay(10);
    }
}
//--------------------------------------------------------------------------------------------------
/**
 * Запись двух байт
 */
void AT24CX::writeInt(unsigned int address, unsigned int data) 
{
	write(address, (byte*)&data, 2);
}
//--------------------------------------------------------------------------------------------------
/**
 * Запись четырёх байт
 */
void AT24CX::writeLong(unsigned int address, unsigned long data) 
{
	write(address, (byte*)&data, 4);
}
//--------------------------------------------------------------------------------------------------
/**
 * Запись числа с плавающей точкой
 */
void AT24CX::writeFloat(unsigned int address, float data) 
{
	write(address, (byte*)&data, 4);
}
//--------------------------------------------------------------------------------------------------
/**
 * Запись числа с плавающей точкой двойной точности
 */
void AT24CX::writeDouble(unsigned int address, double data) 
{
	write(address, (byte*)&data, 8);
}
//--------------------------------------------------------------------------------------------------
/**
 * Запись строки
 */
void AT24CX::writeChars(unsigned int address, char *data, int length) 
{
	write(address, (byte*)data, length);
}
//--------------------------------------------------------------------------------------------------
/**
 * Чтение двух байт
 */
unsigned int AT24CX::readInt(unsigned int address) 
{
	read(address, _b, 2);
	return *(unsigned int*)&_b[0];
}
//--------------------------------------------------------------------------------------------------
/**
 * Чтение четырёх байт
 */
unsigned long AT24CX::readLong(unsigned int address) 
{
	read(address, _b, 4);
	return *(unsigned long*)&_b[0];
}
//--------------------------------------------------------------------------------------------------
/**
 * Чтение числа с плавающей точкой
 */
float AT24CX::readFloat(unsigned int address) 
{
	read(address, _b, 4);
	return *(float*)&_b[0];
}
//--------------------------------------------------------------------------------------------------
/**
 * Чтение числа с плавающей точкой повышенной точности
 */
double AT24CX::readDouble(unsigned int address) 
{
	read(address, _b, 8);
	return *(double*)&_b[0];
}
//--------------------------------------------------------------------------------------------------
/**
 * Чтение строки
 */
void AT24CX::readChars(unsigned int address, char *data, int n) 
{
	read(address, (byte*)data, n);
}
//--------------------------------------------------------------------------------------------------
/**
 * Запись последовательности из n байт
 */
void AT24CX::write(unsigned int address, byte *data, int n) 
{
	// status quo
	int c = n;						// bytes left to write
	int offD = 0;					// current offset in data pointer
	int offP;						// current offset in page
	int nc = 0;						// next n bytes to write

	// write alle bytes in multiple steps
	while (c > 0) 
	{
		// calc offset in page
		offP = address % _pageSize;
		// maximal 30 bytes to write
		nc = min(min(c, 30), _pageSize - offP);
		write(address, data, offD, nc);
		c-=nc;
		offD+=nc;
		address+=nc;
	}
}
//--------------------------------------------------------------------------------------------------
/**
 * Запись последовательности из n байт со смещением
 */
void AT24CX::write(unsigned int address, byte *data, int offset, int n) 
{
    wire->beginTransmission(_id);
    
    if (wire->endTransmission()==0) 
    {
     	wire->beginTransmission(_id);
    	wire->write(address >> 8);
    	wire->write(address & 0xFF);
    	byte *adr = data+offset;
    	wire->write(adr, n);
    	wire->endTransmission();
    	delay(20);
    }
}
//--------------------------------------------------------------------------------------------------
/**
 * чтение байта
 */
byte AT24CX::read(unsigned int address) 
{
	byte b = 0;
	int r = 0;
	wire->beginTransmission(_id);
 
    if (wire->endTransmission()==0) 
    {
     	wire->beginTransmission(_id);
    	wire->write(address >> 8);
    	wire->write(address & 0xFF);
    	if (wire->endTransmission()==0) 
    	{
  			wire->requestFrom(_id, 1);
  			while (wire->available() > 0 && r<1) 
  			{
  				b = (byte)wire->read();
  				r++;
  			}
    	}
    }
    return b;
}
//--------------------------------------------------------------------------------------------------
/**
 * чтение последовательности из n байт
 */
void AT24CX::read(unsigned int address, byte *data, int n) 
{
	int c = n;
	int offD = 0;
	// read until are n bytes read
	while (c > 0) 
	{
		// read maximal 32 bytes
		int nc = c;
		if (nc > 32)
			nc = 32;
		read(address, data, offD, nc);
		address+=nc;
		offD+=nc;
		c-=nc;
	}
}
//--------------------------------------------------------------------------------------------------
/**
 * чтение последовательности из n байт по смещению
 */
void AT24CX::read(unsigned int address, byte *data, int offset, int n) 
{
	wire->beginTransmission(_id);
 
    if (wire->endTransmission()==0) 
    {
     	wire->beginTransmission(_id);
    	wire->write(address >> 8);
    	wire->write(address & 0xFF);
      
    	if (wire->endTransmission()==0) 
    	{
			  int r = 0;
    		wire->requestFrom(_id, n);
        
			  while (wire->available() > 0 && r<n) 
			  {
				  data[offset+r] = (byte)wire->read();
				  r++;
			  }
    	}
    }
}
//--------------------------------------------------------------------------------------------------
