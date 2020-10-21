//--------------------------------------------------------------------------------------------------
#include "Buttons.h"
#include "Feedback.h"
#include "CONFIG.h"
#include "ADCSampler.h"
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

    Feedback.setFailureLineLevel(false); // сбрасываем сигнал АВАРИЯ

    // сбрасываем все сигналы на АСУ ТП
    digitalWrite(out_asu_tp1, !(asu_tp_level));
    digitalWrite(out_asu_tp2, !(asu_tp_level));
    digitalWrite(out_asu_tp3, !(asu_tp_level));
    digitalWrite(out_asu_tp4, !(asu_tp_level));
    
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
