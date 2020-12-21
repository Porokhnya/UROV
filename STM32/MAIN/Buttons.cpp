//--------------------------------------------------------------------------------------------------
#include "Buttons.h"
#include "Feedback.h"
#include "CONFIG.h"
#include "ADCSampler.h"
//--------------------------------------------------------------------------------------------------
ButtonsList Buttons; // список кнопок
//--------------------------------------------------------------------------------------------------
ButtonsList::ButtonsList()
{
  inited = false; // флаг инициализации
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::begin() // начинаем работу
{
  // инициализируем кнопки
  redButton.begin(BUTTON_RED);
  blueButton.begin(BUTTON_BLUE);
  yellowButton.begin(BUTTON_YELLOW);

  inited = true; // устанавливаем флаг инициализации
}
//--------------------------------------------------------------------------------------------------
void ButtonsList::update() // обновляем внутреннее состояние
{
  if(!inited) // возврат, если не инициализированы
  {
    return;
  }

  // обновляем кнопки
  redButton.update();
  blueButton.update();
  yellowButton.update();

  if(yellowButton.isClicked()) // если кликнута жёлтая кнопка
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

  if(redButton.isClicked()) // если кликнута красная кнопка
  {
    DBGLN(F("RED BUTTON CLICKED!"));
  }

  if(blueButton.isClicked()) // если кликнута синяя кнопка
  {
    DBGLN(F("BLUE BUTTON CLICKED!"));
  }
 
}
//--------------------------------------------------------------------------------------------------
