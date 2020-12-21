#include "Endstops.h"
#include "CONFIG.h"
#include "Settings.h"
//--------------------------------------------------------------------------------------------------------------------------------
volatile uint32_t upEndstopLastInterrupt = 0;
volatile uint32_t downEndstopLastInterrupt = 0;
const uint32_t MIN_ENDSTOP_INT_INTERVAL = 1000000.0 / (1.*(ENDSTOP_FREQUENCY - ENDSTOP_HISTERESIS));
const uint32_t MAX_ENDSTOP_INT_INTERVAL = 1000000.0 / (1.*(ENDSTOP_FREQUENCY + ENDSTOP_HISTERESIS));
//--------------------------------------------------------------------------------------------------------------------------------
void readUpEndstop() // читаем верхний концевик
{
	upEndstopLastInterrupt = micros();
}
//--------------------------------------------------------------------------------------------------------------------------------
void readDownEndstop() // читает нижний концевик
{
	downEndstopLastInterrupt = micros();
}
//--------------------------------------------------------------------------------------------------------------------------------
void SetupEndstops() // настраивает концевики
{
	pinMode(ROD_ENDSTOP_UP, INPUT_PULLUP);
	pinMode(ROD_ENDSTOP_DOWN, INPUT_PULLUP);

	attachInterrupt((ROD_ENDSTOP_UP), readUpEndstop, ENDSTOPS_INT_LEVEL);
	attachInterrupt((ROD_ENDSTOP_DOWN), readDownEndstop, ENDSTOPS_INT_LEVEL);
}
//--------------------------------------------------------------------------------------------------------------------------------
bool checkInterval(uint32_t interval) // проверяет на достижение интервала
{
  if (interval >= MIN_ENDSTOP_INT_INTERVAL && interval <= MAX_ENDSTOP_INT_INTERVAL)
  {
		return false;
  }

	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------
bool RodUpEndstopTriggered(bool noInt) // проверяет на срабатывание верхнего концевики
{
	if(noInt)
  {
		noInterrupts();
  }

	uint32_t lastIntTime = upEndstopLastInterrupt;

	if(noInt)
  {
		interrupts();
  }

	return checkInterval(micros() - lastIntTime);
}
//--------------------------------------------------------------------------------------------------------------------------------
bool RodDownEndstopTriggered(bool noInt) // проверяет на срабатывание нижнего концевика
{
	if(noInt)
  {
		noInterrupts();
  }

	uint32_t lastIntTime = downEndstopLastInterrupt;

	if(noInt)
  {
		interrupts();
  }

	return checkInterval(micros() - lastIntTime);

}
//--------------------------------------------------------------------------------------------------------------------------------
