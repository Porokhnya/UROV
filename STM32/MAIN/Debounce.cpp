//--------------------------------------------------------------------------------------------------
#include "Debounce.h"
//--------------------------------------------------------------------------------------------------
Debounce::Debounce()
{
  pin = 0;
  debounceTimer = 0;
  pinState = 0;
  lastPinState = 0;
  bounceInterval = 5;
}
//--------------------------------------------------------------------------------------------------
void Debounce::begin(uint8_t _pin, uint16_t _bounceInterval, bool _pullup)
{
  pin = _pin;
  bounceInterval = _bounceInterval;

  pinMode(pin, _pullup ? INPUT_PULLUP : INPUT);
  pinState = lastPinState = digitalRead(pin);
}
//--------------------------------------------------------------------------------------------------
void Debounce::update()
{
  uint8_t nowState = digitalRead(pin);

  // если состояние кнопки изменилось - взводим таймер
  if (nowState != lastPinState) 
  {
    debounceTimer = millis();
  }

  if ((millis() - debounceTimer) >= bounceInterval) // N миллисекунд на подавление дребезга
  {
    // если отсчёт таймера закончен, то смотрим,
    // если прочитанное состояние не равно последнему высчитанному, то считаем,
    // что дребезг прошёл, и сохраняем текущее состояние
    if (nowState != pinState) 
    {
      pinState = nowState;
    }
  }

  lastPinState = nowState;     
}
//--------------------------------------------------------------------------------------------------

