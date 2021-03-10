/**

AT24CX.h
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
#ifndef AT24CX_h
#define AT24CX_h

// AT24Cx I2C adress
// 80
// 0x50
#define AT24CX_ID B1010000


// includes
#include <Arduino.h>
#include <Wire.h>

// byte
typedef uint8_t byte;


// general class definition
class AT24CX 
{
public:

  // конструкторы
	AT24CX(TwoWire& w);
	AT24CX(TwoWire& w,byte index, uint16_t pageSize);
 
	void write(unsigned int address, byte data);  // запись байта
	void write(unsigned int address, byte *data, int n); // запись массива байт
	void writeInt(unsigned int address, unsigned int data); // запись двух байт
	void writeLong(unsigned int address, unsigned long data); // запись четырёх байт
	void writeFloat(unsigned int address, float data); // запись числа с плавающей точкой
	void writeDouble(unsigned int address, double data); // запись числа с плавающей точкой двойной точности
	void writeChars(unsigned int address, char *data, int length); // запись строки
	byte read(unsigned int address); // чтение байта
	void read(unsigned int address, byte *data, int n); // чтение массива байт
	unsigned int readInt(unsigned int address); // чтение двух байт
	unsigned long readLong(unsigned int address); // чтение четырёх байт
	float readFloat(unsigned int address); // чтение числа с плавающей точкой
	double readDouble(unsigned int address); // чтение числа с плавающей точкой двойной точности
	void readChars(unsigned int address, char *data, int n); // чтение строки
protected:
	void init(byte index, uint16_t pageSize);
  TwoWire* wire;
private:
	void read(unsigned int address, byte *data, int offset, int n);
	void write(unsigned int address, byte *data, int offset, int n);
	int _id;
	byte _b[8];
	uint16_t _pageSize;
};

// AT24C32 class definiton
class AT24C32 : public AT24CX {
public:
	AT24C32(TwoWire& w);
	AT24C32(TwoWire& w,byte index);
};

// AT24C64 class definiton
class AT24C64 : public AT24CX {
public:
	AT24C64(TwoWire& w);
	AT24C64(TwoWire& w,byte index);
};

// AT24C128 class definiton
class AT24C128 : public AT24CX {
public:
	AT24C128(TwoWire& w);
	AT24C128(TwoWire& w,byte index);
};

// AT24C256 class definiton
class AT24C256 : public AT24CX {
public:
	AT24C256(TwoWire& w);
	AT24C256(TwoWire& w,byte index);
};

// AT24C512 class definiton
class AT24C512 : public AT24CX {
public:
	AT24C512(TwoWire& w);
	AT24C512(TwoWire& w,byte index);
};

// AT24C1024 class definiton
class AT24C1024 : public AT24CX {
public:
  AT24C1024(TwoWire& w);
  AT24C1024(TwoWire& w,byte index);
};


#endif
