#pragma once
//--------------------------------------------------------------------------------------------------
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------
// класс для чтения с пина без дребезга
//--------------------------------------------------------------------------------------------------
class Debounce
{
 public:
  
    Debounce();

    void begin(uint8_t pin, uint16_t bounceInterval=5, bool pullup=true); // _pullup == false - подтяжка к земле, иначе - подтяжка к питанию
    void update(); // обновляем внутреннее состояние
    uint8_t getState() { return pinState; };      


 private:

    uint8_t pinState;
    uint8_t pin;
    uint32_t debounceTimer;
    uint8_t lastPinState;
    uint16_t bounceInterval;
    
 };
//--------------------------------------------------------------------------------------------------
