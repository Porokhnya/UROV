//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "SelectEncoderChartScreen.h"
#include "FileUtils.h"
#include "CONFIG.h"
#include "PulsesGen.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LoadEncoderChartScreen* chartLoader = NULL;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SelectEncoderChartScreen::SelectEncoderChartScreen() : AbstractTFTScreen("SelectEncoderChartScreen")
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::onDeactivate()
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::onActivate()
{
  // тут проверяем, какие кнопки активировать


  screenButtons->disableButton(mem1Button);
  screenButtons->disableButton(mem2Button);
  screenButtons->disableButton(mem3Button);

  if(SD.exists("/FILE1.A") && SD.exists("/FILE1.B"))
  {
    screenButtons->enableButton(file1Button);
  }
  else
  {
    screenButtons->disableButton(file1Button);
  }

  if(SD.exists("/FILE2.A") && SD.exists("/FILE2.B"))
  {
    screenButtons->enableButton(file2Button);
  }
  else
  {
    screenButtons->disableButton(file2Button);
  }  

  if(SD.exists("/FILE3.A") && SD.exists("/FILE3.B"))
  {
    screenButtons->enableButton(file3Button);
  }
  else
  {
    screenButtons->disableButton(file3Button);
  }  
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::doSetup(TFTMenu* menu)
{
  // тут настраиваемся, например, можем добавлять кнопки

	
	int menu_height = 30;
	int button_gap = 5;
	int height_button = 29;
	int width_button = 125;

  backButton = screenButtons->addButton( 315 ,  255, 150,  40, "НАЗАД");

  file1Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 1");
  menu_height += height_button + button_gap;
  file2Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 2");
  menu_height += height_button + button_gap;
  file3Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 3");
  menu_height += height_button + button_gap;
  mem1Button = screenButtons->addButton(340, menu_height, width_button, height_button, "MEMO 1");
  menu_height += height_button + button_gap;
  mem2Button = screenButtons->addButton(340, menu_height, width_button, height_button, "MEMO 2");
  menu_height += height_button + button_gap;
  mem3Button = screenButtons->addButton(340, menu_height, width_button, height_button, "MEMO 3");

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::doUpdate(TFTMenu* menu)
{

    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::doDraw(TFTMenu* menu)
{
  UTFT* dc = menu->getDC();
  dc->setColor(VGA_WHITE);
  dc->setBackColor(VGA_BLACK);

  dc->setFont(BigRusFont);
  // тут рисуем, что надо именно нам, кнопки прорисуются сами после того, как мы тут всё отрисуем
  menu->print("Экран выбора графика", 70, 10);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
	// обработчик нажатия на кнопку.

	if (pressedButton == backButton)
	{
	   menu->switchToScreen("Main"); // переключаемся на первый экран
    }
	else if (pressedButton == file1Button)
	{
		// Загрузить файл 1
   chartLoader->LoadChart(lcmFromFile,1);
	}
	else if (pressedButton == file2Button)
	{
		// СЗагрузить файл 2
   chartLoader->LoadChart(lcmFromFile,2);
	}
	else if (pressedButton == file3Button)
	{
		// Загрузить файл 3
   chartLoader->LoadChart(lcmFromFile,3);
	}
	else if (pressedButton == mem1Button)
	{
		// Загрузить память 1
   chartLoader->LoadChart(lcmFromMemory,0);
	}
	else if (pressedButton == mem2Button)
	{
		// Загрузить память 2
   chartLoader->LoadChart(lcmFromMemory,1);
	}
	else if (pressedButton == mem3Button)
	{
		// Загрузить память 3
   chartLoader->LoadChart(lcmFromMemory,2);
	}
	

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// LoadEncoderChartScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LoadEncoderChartScreen::LoadEncoderChartScreen() : AbstractTFTScreen("LoadEncoderChartScreen")
{
  chartLoader = this;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::LoadChart(LoadChartMode mode, int index)
{
  loadMode = mode;
  loadIndex = index;
  Screen.switchToScreen(this);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::onDeactivate()
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::onActivate()
{
  lines.clear();
  lines.push_back("ПОДОЖДИТЕ,");
  lines.push_back("ИДЁТ ЗАГРУЗКА...");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::doSetup(TFTMenu* menu)
{
  // тут настраиваемся, например, можем добавлять кнопки

  UTFT* dc = menu->getDC();
  int displayWidth = dc->getDisplayXSize();
  int displayHeight = dc->getDisplayYSize();
  
  int btnLeft = (displayWidth - BUTTON_WIDTH)/2;
  int btnTop = displayHeight - BUTTON_HEIGHT - 10;

  doneButton = screenButtons->addButton( btnLeft ,  btnTop, BUTTON_WIDTH,  BUTTON_HEIGHT, "ЗАВЕРШИТЬ");
  screenButtons->hideButton(doneButton);


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::drawLines(TFTMenu* menu,int color)
{
  if(!lines.size())
  {
    return;
  }

  UTFT* dc = menu->getDC();
  dc->setColor(color);
  dc->setBackColor(VGA_BLACK);  

 int fontHeight = dc->getFontYsize();
 int fontWidth = dc->getFontXsize();
 int displayWidth = dc->getDisplayXSize();
 int lineSpacing = 2;  

 int curX = 0;
 int curY = 40;

  for(size_t i=0;i<lines.size();i++)
  {
    int lineLength = menu->print(lines[i],curX,curY,0,true);
    curX = (displayWidth - lineLength*fontWidth)/2;    
    menu->print(lines[i],curX,curY);
    curY += fontHeight + lineSpacing;
  }      
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::doUpdate(TFTMenu* menu)
{

    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::prepareFromFile()
{
  ImpulseGeneratorA.stop();
  ImpulseGeneratorB.stop();

  String fileNameA, fileNameB;

  fileNameA = "/FILE";
  fileNameB = "/FILE";

  fileNameA += loadIndex;
  fileNameB += loadIndex;


  fileNameA += ".A";
  fileNameB += ".B";

  ImpulseGeneratorA.prepare(fileNameA);
  ImpulseGeneratorB.prepare(fileNameB);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::doDraw(TFTMenu* menu)
{
  UTFT* dc = menu->getDC();
  dc->setColor(VGA_WHITE);
  dc->setBackColor(VGA_BLACK);

  dc->setFont(BigRusFont);
  drawLines(menu,VGA_WHITE);

  // тут загружаем данные
  switch(loadMode)
  {
    case lcmFromFile:
      prepareFromFile();
    break;
    
    case lcmFromMemory:
    break;
  } // switch

  // стираем старое сообщение
  drawLines(menu,VGA_BLACK);

  // пишем новое сообщение
  lines.clear();
  lines.push_back("ЗАГРУЗКА");
  lines.push_back("ЗАВЕРШЕНА.");
  drawLines(menu,VGA_WHITE);

  // в конце - показываем кнопку
   screenButtons->showButton(doneButton,true);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoadEncoderChartScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  // обработчик нажатия на кнопку.

  if (pressedButton == doneButton)
  {
     menu->switchToScreen("Main"); // переключаемся на первый экран
    }
  
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

