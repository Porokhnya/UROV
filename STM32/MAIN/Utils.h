#pragma once

#include <Arduino.h>

String formatFloat(float f, uint8_t decimalPlaces)
{
	int n = 10;
	for(uint8_t i=0;i<decimalPlaces;i++)
	{
		n *= 10;
	}
	
	f *= n;
	
	int32_t t = f;
	uint32_t d = t%n;
	t -= d;	
	t/=n;
	
	String result;
	result = t;
	result += '.';
	result += d;
	
	return result;
}