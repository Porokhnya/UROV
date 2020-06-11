//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "CreateEncoderChartScreen.h"
#include "Drawing.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CreateEncoderChartScreen::CreateEncoderChartScreen() : AbstractTFTScreen("CreateEncoderChartScreen")
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::doSetup(TFTMenu* menu)
{
  // тут настраиваемся, например, можем добавлять кнопки
  backButton = screenButtons->addButton( 10 ,  30, 200,  50, "ВЕРНУТЬСЯ");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::doUpdate(TFTMenu* menu)
{
    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::doDraw(TFTMenu* menu)
{
  UTFT* dc = menu->getDC();


  // рисуем сетку
  RGBColor gridColor = { 0,200,0 }; // цвет сетки
  int gridX = 10; // стартовая координата по X для сетки
  int gridY = 10; // стартовая координата по Y для сетки
  int columnsCount = 5; // количество столбцов сетки
  int rowsCount = 10; // количество строк сетки
  int columnWidth = 20; // ширина столбца
  int rowHeight = 20; // высота строки
  
  Drawing::DrawGrid(gridX, gridY, columnsCount, rowsCount, columnWidth, rowHeight, gridColor);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  // обработчик нажатия на кнопку.
  if(pressedButton == backButton)
  {
    menu->switchToScreen("Main"); // переключаемся на первый экран
  }

    

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

