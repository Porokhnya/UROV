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
  if (SD.exists("/FILE4.A") && SD.exists("/FILE4.B"))
  {
	  screenButtons->enableButton(file4Button);
  }
  else
  {
	  screenButtons->disableButton(file4Button);
  }
  if (SD.exists("/FILE5.A") && SD.exists("/FILE5.B"))
  {
	  screenButtons->enableButton(file5Button);
  }
  else
  {
	  screenButtons->disableButton(file5Button);
  }
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::doSetup(TFTMenu* menu)
{
  // тут настраиваемся, например, можем добавлять кнопки

	
	int menu_height = 30;
	int button_gap = 6;
	int height_button = 35;
	int width_button = 125;

 
  file1Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 1");
  menu_height += height_button + button_gap;
  file2Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 2");
  menu_height += height_button + button_gap;
  file3Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 3");
  menu_height += height_button + button_gap;
  file4Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 4");
  menu_height += height_button + button_gap;
  file5Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 5");
  menu_height += height_button + button_gap;
  file_selection = screenButtons->addButton(340, menu_height, width_button, height_button, "ВЫБОР");
  menu_height += height_button + button_gap;
  backButton = screenButtons->addButton(340, menu_height, width_button, height_button, "НАЗАД");
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::doUpdate(TFTMenu* menu)
{

    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::drawGrid(TFTMenu* menu)
{

	UTFT* dc = menu->getDC();
	dc->setColor(VGA_WHITE);
	dc->setBackColor(VGA_BLACK);

	dc->setFont(BigRusFont);
	// тут рисуем, что надо именно нам, кнопки прорисуются сами после того, как мы тут всё отрисуем
	menu->print("Экран выбора графика", 70, 10);
	menu->print("Точек графика", 20, 245);
	dc->setFont(SmallRusFont);

	// рисуем сетку
	RGBColor gridColor = { 0,200,0 }; // цвет сетки
	int gridX = 20; // стартовая координата по X для сетки
	int gridY = 30; // стартовая координата по Y для сетки
	int columnsCount = 6; // количество столбцов сетки
	int rowsCount = 4; // количество строк сетки
	int columnWidth = 50; // ширина столбца
	int rowHeight = 50; // высота строки

	dc->setColor(VGA_BLACK);
	dc->fillRect(gridX, gridY, gridX + 5 + (columnWidth*columnsCount), gridY + 5 + (rowHeight*rowsCount)); // Очистить экран
	Drawing::DrawGrid(gridX, gridY, columnsCount, rowsCount, columnWidth, rowHeight, gridColor);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SelectEncoderChartScreen::doDraw(TFTMenu* menu)
{
  UTFT* dc = menu->getDC();
  dc->setColor(VGA_WHITE);
  dc->setBackColor(VGA_BLACK);

  dc->setFont(BigRusFont);
  // тут рисуем, что надо именно нам, кнопки прорисуются сами после того, как мы тут всё отрисуем
  //menu->print("Экран выбора графика", 70, 10);
  drawGrid(menu);
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
	else if (pressedButton == file4Button)
	{
		// Загрузить файл 4
		chartLoader->LoadChart(lcmFromFile, 4);
	}
	else if (pressedButton == file5Button)
	{
		// Загрузить файл 5
		chartLoader->LoadChart(lcmFromFile, 5);
	}


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SelectEncoderChartScreen::clear_Grid(TFTMenu* menu)
{
	drawGrid(menu); // рисуем сетку снова

	//chartPoints.clear(); // очищаем список наших экранных точек
	//pulsesList.clear(); // очищаем список сгенеренных импульсов
	//touch_x_min = TOUCH_X_MIN; // сбрасываем начальную координату по X

	//screenButtons->disableButton(calculateButton, screenButtons->buttonEnabled(calculateButton));
	//enableSaveButtons(false, true);

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

