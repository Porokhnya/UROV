//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ChartSettingsScreen.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ChartSettingsScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChartSettingsScreen::ChartSettingsScreen() : AbstractTFTScreen("ChartSettingsScreen")
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChartSettingsScreen::onActivate()
{
  currentEditedButton = -1;
  
  genPeriodVal = Settings.getChartGenPeriod();
  pulseWidthVal = Settings.getChartPulseWidth();
  workTimeVal = Settings.getChartWorkTime();

  screenButtons->relabelButton(genPeriodButton,genPeriodVal.c_str());
  screenButtons->relabelButton(pulseWidthButton,pulseWidthVal.c_str());
  screenButtons->relabelButton(workTimeButton,workTimeVal.c_str());
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChartSettingsScreen::doSetup(TFTMenu* menu)
{
  screenButtons->setButtonColors(TFT_BUTTON_COLORS2);
  
  
  // тут настраиваемся, например, можем добавлять кнопки
  //reserved = screenButtons->addButton(5, 2, BUTTON_WIDTH, 30, "reserved");
  genPeriodButton = screenButtons->addButton(120, 30, 95, BUTTON_HEIGHT, "");
  pulseWidthButton = screenButtons->addButton(120, 65, 95, 30, "");
  workTimeButton = screenButtons->addButton(120, 100, 95, 30, "");
  backButton = screenButtons->addButton(5, 142, BUTTON_WIDTH, BUTTON_HEIGHT, "ВЫХОД");

  screenButtons->setButtonBackColor(genPeriodButton,VGA_BLACK);
  screenButtons->setButtonBackColor(pulseWidthButton,VGA_BLACK);
  screenButtons->setButtonBackColor(workTimeButton,VGA_BLACK);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChartSettingsScreen::onKeyboardInput(bool enterPressed, const String& enteredValue)
{
  if(!enterPressed)
    return;

    if(currentEditedButton == genPeriodButton)
    {
      genPeriodVal = enteredValue;
      screenButtons->relabelButton(genPeriodButton,genPeriodVal.c_str());
      Settings.setChartGenPeriod(genPeriodVal.toInt());
    }
    else if(currentEditedButton == pulseWidthButton)
    {
      pulseWidthVal = enteredValue;
      screenButtons->relabelButton(pulseWidthButton,pulseWidthVal.c_str());
      Settings.setChartPulseWidth(pulseWidthVal.toInt());
    }
    else if(currentEditedButton == workTimeButton)
    {
      workTimeVal = enteredValue;
      screenButtons->relabelButton(workTimeButton,workTimeVal.c_str());
      Settings.setChartWorkTime(workTimeVal.toInt());
    }

  currentEditedButton = -1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChartSettingsScreen::doUpdate(TFTMenu* menu)
{
    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChartSettingsScreen::doDraw(TFTMenu* menu)
{
  UTFT* dc = menu->getDC();
  uint8_t* oldFont = dc->getFont();

  dc->setFont(BigRusFont);
  dc->setColor(VGA_WHITE);

  menu->print("Настройки графика",2,2);
  menu->print("Интервал таймера, мкс:", 2, 37);
  menu->print("Ширина импульса, мкс:", 2, 72);
  menu->print("Время работы, мс:", 2, 107);
  
  dc->setFont(oldFont);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChartSettingsScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  if(pressedButton == backButton)
  {
    menu->switchToScreen("ParamsScreen");
  }
  else
  {
    currentEditedButton = pressedButton;
    String strValToEdit = screenButtons->getLabel(currentEditedButton);
    ScreenKeyboard->show(ktDigits,strValToEdit,this,this, 10);
  }
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

