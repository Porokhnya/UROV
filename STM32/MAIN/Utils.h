#pragma once

#include <Arduino.h>

// форматируем число с плавающей точкой в строку, с учётом десятичных знаков
String formatFloat(float f, uint8_t decimalPlaces)
{
	int n = 1;
	for(uint8_t i=0;i<decimalPlaces;i++) // делаем множитель по кол-ву десятичных знаков
	{
		n *= 10;
	}

 // делаем форматтер с учётом множителя
 String formatter;
 formatter = "%d.%0";
 formatter += decimalPlaces;
 formatter += "d";

 // вспомогательный буфер
 char buff[50] = {0};

 // печатаем число с учётом форматтера в буфер
 sprintf(buff,formatter.c_str(), (int32_t)f, abs(int32_t(f*n))%n);

 // возврат результата
 return buff;
}
