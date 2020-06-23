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
  
 /* int menu_height = 30;
  int button_gap = 5;
  int height_button = 29;
  int width_button = 80;*/
  
  // тут настраиваемся, например, можем добавлять кнопки
  //reserved = screenButtons->addButton(5, 2, BUTTON_WIDTH, 30, "reserved");
  menu_height += height_button + button_gap - 5; // Выравниваем кнопки относительно текста
  genPeriodButton = screenButtons->addButton(370, menu_height, width_button, height_button, "");
  menu_height += height_button + button_gap;
  pulseWidthButton = screenButtons->addButton(370, menu_height, width_button, height_button, "");
  menu_height += height_button + button_gap;
  workTimeButton = screenButtons->addButton(370, menu_height, width_button, height_button, "");
  menu_height += height_button + button_gap;
  menu_height += height_button + button_gap;
  backButton = screenButtons->addButton(20, menu_height, BUTTON_WIDTH, BUTTON_HEIGHT, "ВЫХОД");

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
  menu_height = 30;
  button_gap = 5;
  
  menu->print("Настройки графика",10, menu_height);
  menu_height += height_button + button_gap;
  menu->print("Интервал таймера, мкс:", 10, menu_height);
  menu_height += height_button + button_gap;
  menu->print("Ширина импульса, мкс:", 10, menu_height);
  menu_height += height_button + button_gap;
  menu->print("Время работы, мс:", 10, menu_height);
  
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

