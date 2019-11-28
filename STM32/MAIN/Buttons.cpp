//--------------------------------------------------------------------------------------------------
#include "Buttons.h"
#include "Feedback.h"
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------
ButtonsList Buttons;
//--------------------------------------------------------------------------------------------------
ButtonsList::ButtonsList()
{
  inited = false;
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::begin()
{
  redButton.begin(BUTTON_RED);
  blueButton.begin(BUTTON_BLUE);
  yellowButton.begin(BUTTON_YELLOW);

  inited = true;
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::update()
{
  if(!inited)
    return;
    
  redButton.update();
  blueButton.update();
  yellowButton.update();

  if(yellowButton.isClicked())
  {
    DBGLN(F("YELLOW BUTTON CLICKED!"));
    Feedback.testDiode(false); // гасим светодиод ТЕСТ
    Feedback.failureDiode(false); // гасим светодиод ОШИБКА
    Feedback.readyDiode(false); // гасим светодиод УСПЕХ

    Feedback.alarm(false); // сбрасываем сигнал АВАРИЯ
  }

  if(redButton.isClicked())
  {
    DBGLN(F("RED BUTTON CLICKED!"));
  }

  if(blueButton.isClicked())
  {
    DBGLN(F("BLUE BUTTON CLICKED!"));
  }
 
}
//--------------------------------------------------------------------------------------------------

