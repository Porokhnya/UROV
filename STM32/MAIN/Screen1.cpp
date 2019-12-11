//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Screen1.h"
#include "DS3231.h"
#include "ConfigPin.h"
#include "ADCSampler.h"
#include "CONFIG.h"
#include "InterruptHandler.h"
#include "InterruptScreen.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Screen1* mainScreen = NULL;
extern "C" char* sbrk(int i);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loopADC()
{
#ifndef _ADC_OFF

//  static bool dataHigh_old = false;
  
  if (adcSampler.available()) 
  {
    int bufferLength = 0;
    uint16_t* cBuf = adcSampler.getADCBuffer(&bufferLength);    // Получить буфер с данными


    static uint16_t countOfPoints = 0;    
    static uint16_t* serie1 = NULL;
    static uint16_t* serie2 = NULL;
    static uint16_t* serie3 = NULL;
    
    uint16_t currentCountOfPoints = bufferLength/ NUM_CHANNELS;

    if(currentCountOfPoints != countOfPoints)
    {
      countOfPoints = currentCountOfPoints;
      
      delete [] serie1;
      delete [] serie2;
      delete [] serie3;

      serie1 = new uint16_t[countOfPoints];
      serie2 = new uint16_t[countOfPoints];
      serie3 = new uint16_t[countOfPoints];

    }

 
    uint16_t serieWriteIterator = 0;

    uint32_t raw200V = 0;
    uint32_t raw5V = 0;
    uint32_t raw3V3 = 0;

  /*
    Буфер у нас для четырёх каналов, индексы:

    0 -  Аналоговый вход трансформатора №1
    1 - Аналоговый вход трансформатора №2
    2 -  Аналоговый вход трансформатора №3
    3 - Аналоговый вход контроль питания 3.3в

*/  
    
    for (int i = 0; i < bufferLength; i = i + NUM_CHANNELS, serieWriteIterator++)                // получить результат измерения поканально, с интервалом 3
    {
	  serie1[serieWriteIterator] = cBuf[i + 0];        // Данные 1 графика  (красный)
	  serie2[serieWriteIterator] = cBuf[i + 1];        // Данные 2 графика  (синий)
	  serie3[serieWriteIterator] = cBuf[i + 2];        // Данные 3 графика  (желтый)

	  raw3V3  += cBuf[i + 3];                          // Данные Измерение 3V3
    
    //TODO: ТУТ ЗАКОММЕНТИРОВАЛ, НЕТ КОНТРОЛЯ !!!
	  raw5V   += 0;//cBuf[i + 8];                          // Данные Измерение +5V 
	  raw200V += 0;//cBuf[i + 6];                          // Данные Измерение =200В


	  } // for

    raw200V /= countOfPoints;
    raw3V3 /= countOfPoints;
    raw5V /= countOfPoints;

    Settings.set3V3RawVoltage(raw3V3);
    Settings.set5VRawVoltage(raw5V);
    Settings.set200VRawVoltage(raw200V);
      
    adcSampler.reset();                                  // все данные переданы в ком

    if(mainScreen && mainScreen->isActive())
    {
      mainScreen->requestToDrawChart(serie1, serie2, serie3, countOfPoints);      
    }

  }
  /*
    if (dataHigh_old != adcSampler.dataHigh)
    {
      dataHigh_old = adcSampler.dataHigh;
    }
    */
#endif // !_ADC_OFF
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Screen1::Screen1() : AbstractTFTScreen("Main")
{
  oldsecond = 0;
  mainScreen = this;
  points1 = NULL;
  points2 = NULL;
  points3 = NULL;
  canDrawChart = false;
  inDrawingChart = false;
  last3V3Voltage = last5Vvoltage = last200Vvoltage = -1;
  canLoopADC = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::drawVoltage(TFTMenu* menu)
{
#ifndef _DISABLE_DRAW_VOLTAGE
  if(!isActive())
    return;

  word color = RED;
  TFT_Class* dc = menu->getDC();
  
  dc->setFreeFont(TFT_SMALL_FONT);
  uint8_t fontHeight = FONT_HEIGHT(dc);
  
  uint16_t curX = 190;
  uint16_t curY = 20;

  VoltageData vData = Settings.get3V3Voltage();// Контроль источника питания +3.3в

  if(last3V3Voltage != vData.raw)
  {
    last3V3Voltage = vData.raw;
    
    float threshold = (3.3/100)*VOLTAGE_THRESHOLD;
    float lowBorder = 3.3 - threshold;
    float highBorder = 3.3 + threshold;
  
    if(vData.voltage >= lowBorder && vData.voltage <= highBorder)
      color = GREEN;
  
    //String data = String(vData.voltage,1)+"V";
    int32_t vDataI = vData.voltage*10;
    String data;
    data = int32_t(vDataI/10);
    data += '.';
    data += abs(vDataI%10);
    data += "V";
    
    while(data.length() < 4)
      data += ' ';
      

    menu->getRusPrinter()->print(data.c_str(),curX,curY,BLACK,color);
  }
  
  curY += fontHeight + 2;
  
  
  vData = Settings.get5Vvoltage();        // Контроль источника питания +5.0в

  if(last5Vvoltage != vData.raw)
  {
    last5Vvoltage = vData.raw;
    
    float threshold = (5.0/100)*VOLTAGE_THRESHOLD;
    float lowBorder = 5.0 - threshold;
    float highBorder = 5.0 + threshold;
  
    color = RED;
  
    if(vData.voltage >= lowBorder && vData.voltage <= highBorder)
      color = GREEN;
  
    //String data = String(vData.voltage,1) + "V";
    int32_t vDataI = vData.voltage*10;
    String data;
    data = int32_t(vDataI/10);
    data += '.';
    data += abs(vDataI%10);
    data += "V";
    
    while(data.length() < 4)
      data += ' ';
  
    //dc->setColor(color);  
    menu->getRusPrinter()->print(data.c_str(),curX,curY,BLACK,color);
  }
  
  curY += fontHeight + 2;

  
  vData = Settings.get200Vvoltage();      // Контроль источника питания 200в

  if(last200Vvoltage != vData.raw)
  {
    last200Vvoltage = vData.raw;
    
    float threshold = (200.0/100)*VOLTAGE_THRESHOLD;
    float lowBorder = 200.0 - threshold;
    float highBorder = 200.0 + threshold;
  
    color = RED;
  
    if(vData.voltage >= lowBorder && vData.voltage <= highBorder)
      color = GREEN;
  
    String data = String((uint16_t)vData.voltage) + "V";
    
    while(data.length() < 4)
      data += ' ';
  
   // dc->setColor(color);  
    //dc->print(data.c_str(),curX,curY);
    menu->getRusPrinter()->print(data.c_str(),curX,curY,BLACK,color);
  }

//  dc->setColor(oldColor);
#endif // !_DISABLE_DRAW_VOLTAGE
   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::onDeactivate()
{
  // станем неактивными, надо выключить подписчика результатов прерываний
  InterruptHandler.setSubscriber(NULL);
  
  last3V3Voltage = last5Vvoltage = last200Vvoltage = -1;
  
  // прекращаем отрисовку графика
  chart.stopDraw();
  inDrawingChart = false;
  canDrawChart = false;

#ifndef _ADC_OFF
	canLoopADC = false;
#endif // !_ADC_OFF

  DBGLN(F("MainScreen::onDeactivate()"));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::onActivate()
{
#ifndef _ADC_OFF
  canLoopADC = true;
#endif // !_ADC_OFF

  // мы активизируемся, назначаем подписчика результатов прерываний
  InterruptHandler.setSubscriber(ScreenInterrupt);

  DBGLN(F("MainScreen::onActivate()"));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::doSetup(TFTMenu* menu)
{
#ifndef _DISABLE_MAIN_SCREEN_BUTTONS
	screenButtons->setSymbolFont(VARIOUS_SYMBOLS_32x32);
	// тут настраиваемся, например, можем добавлять кнопки
	screenButtons->addButton(5, 140, 165, 30, "НАСТРОЙКИ");
	screenButtons->addButton(179, 130, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 
#endif // !_DISABLE_MAIN_SCREEN_BUTTONS

	// ТУТ НАСТРАИВАЕМ НАШ ГРАФИК
	// устанавливаем ему начальные точки отсчёта координат

	chart.setCoords(5, 120);
	// говорим, какое у нас кол-во точек по X и по Y
	chart.setPoints(CHART_POINTS_COUNT,100);
	// добавляем наши тестовые графики, количеством 1

	serie1 = chart.addSerie({ 255,0,0 });     // первый график - красного цвета
	serie2 = chart.addSerie({ 0,0,255 });     // второй график - голубого цвета
	serie3 = chart.addSerie({ 255,255,0 });   // третий график - желтого цвета

#ifndef _ADC_OFF

	adcSampler.setLowBorder(Settings.getTransformerLowBorder());
	adcSampler.setHighBorder(Settings.getTransformerHighBorder());
  
	adcSampler.begin();  
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::drawTime(TFTMenu* menu)
{

#ifndef _DISABLE_DRAW_TIME
    DS3231Time tm = RealtimeClock.getTime();
    if (oldsecond != tm.second)
    {
      TFT_Class* dc = menu->getDC();
      dc->setFreeFont(TFT_SMALL_FONT);
      
        oldsecond = tm.second;

      // получаем компоненты даты в виде строк
      String strDate = RealtimeClock.getDateStr(tm);
      String strTime = RealtimeClock.getTimeStr(tm);
  
      // печатаем их
      menu->print(strDate.c_str(), 5, 1);
      menu->print(strTime.c_str(), 90, 1);
  
#ifndef _DISABLE_DRAW_RAM_ON_SCREEN
      
      String str = "RAM: ";
      str += getFreeMemory();      
      Screen.print(str.c_str(), 10,123);
#endif // !_DISABLE_DRAW_RAM_ON_SCREEN
      
    }

#endif // !_DISABLE_DRAW_TIME

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::doUpdate(TFTMenu* menu)
{


  drawTime(menu);
  drawVoltage(menu);
  drawChart();

#ifndef _ADC_OFF
  if(canLoopADC)
    loopADC();
#endif // !_ADC_OFF
	// тут обновляем внутреннее состояние
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t Screen1::getSynchroPoint(uint16_t* points, uint16_t pointsCount)
{
 //Тут синхронизируем график, ища нужную нам точку, с которой мы его выводим
  const uint16_t lowBorder = 100; // нижняя граница, по которой ищем начало
  const uint16_t wantedBorder = 2048; // граница синхронизации
  const uint8_t maxPointToSeek = 48; // сколько точек просматриваем вперёд, для поиска значения синхронизации

  if(pointsCount <= CHART_POINTS_COUNT || pointsCount <= maxPointToSeek)
  {
    // кол-во точек уже равно кол-ву точек на графике, синхронизировать начало - не получится
   // DBGLN(F("Too many points!!!!"));
    return 0;
  }
  
  // у нас условия - ищем первый 0, после него ищём первый 2048. Если найдено за 30 первых точек - выводим следующие 150.
  // если не найдено - просто выводим первые 150 точек
  

  uint8_t iterator = 0;
  bool found = false;
  for(; iterator < maxPointToSeek; iterator++)
  {
    if(points[iterator] <= lowBorder)
    {
      // нашли нижнюю границу
      found = true;
      break;
    }
  }

  if(!found)
  {
    // нижняя граница не найдена, просто рисуем как есть
    //DBGLN(F("Low border not found!!!!"));
    return 0;
  }

  found = false;

  // теперь ищем нужную границу для синхронизации
  for(; iterator < maxPointToSeek; iterator++)
  {
    if(points[iterator] >= wantedBorder)
    {
      // нашли границу синхронизации
      found = true;
      break;
    }
  } // for

  if(!found)
  {
    // за maxPointToSeek мы так и не нашли значение синхронизации, выводим как есть
    //DBGLN(F("High border not found!!!!"));
    return 0;
  }

  //DBGLN(F("Found shift: "));
  //DBGLN((&(points[iterator]) - points));

  // нужная граница синхронизации найдена - выводим график, начиная с этой точки
 return ( (&(points[iterator]) - points) ); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::requestToDrawChart(uint16_t* _points1,   uint16_t* _points2,  uint16_t* _points3, uint16_t pointsCount)
{
  if(inDrawingChart)
  {
    chart.stopDraw();
  }

  canDrawChart = true;
  inDrawingChart = false;
  
  points1 = _points1;
  points2 = _points2;
  points3 = _points3;

  int shift = getSynchroPoint(points1,pointsCount);
  int totalPoints = min(CHART_POINTS_COUNT, (pointsCount - shift));

  serie1->setPoints(&(points1[shift]), totalPoints);
  serie2->setPoints(&(points2[shift]), totalPoints);
  serie3->setPoints(&(points3[shift]), totalPoints);
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::drawChart()
{

  if(!isActive() || !canDrawChart || inDrawingChart)
    return;

  inDrawingChart = true;    
	// рисуем сетку
	int gridX = 5; // начальная координата сетки по X
	int gridY = 20; // начальная координата сетки по Y
	int columnsCount = 6; // 5 столбцов
	int rowsCount = 4; // 6 строк
	int columnWidth = 25; // ширина столбца
	int rowHeight = 25; // высота строки 
	RGBColor gridColor = { 0,200,0 }; // цвет сетки


  static uint32_t fpsMillis = 0;
  uint32_t now = millis();

  if(now - fpsMillis > (1000/CHART_FPS) )
  {
  	// вызываем функцию для отрисовки сетки, её можно вызывать из каждого класса экрана
  	Drawing::DrawGrid(gridX, gridY, columnsCount, rowsCount, columnWidth, rowHeight, gridColor);
  
  	chart.draw();// просим график отрисовать наши серии

   fpsMillis = millis();
  }

  inDrawingChart = false;
  canDrawChart = false;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::doDraw(TFTMenu* menu)
{
  drawTime(menu);

#ifndef _DISABLE_DRAW_SOFTWARE_VERSION

  // рисуем версию ПО
  TFT_Class* dc = menu->getDC();
  dc->setFreeFont(TFT_SMALL_FONT);

  uint16_t w = dc->width();
  String str = SOFTWARE_VERSION;

  int strW = menu->getRusPrinter()->textWidth(str.c_str());

  int top = 1;
  int left = w - strW - 3;

  menu->print(str.c_str(),left,top);
  
#endif // !_DISABLE_DRAW_SOFTWARE_VERSION

 // DBGLN(F("MainScreen::draw()"));
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen1::onButtonPressed(TFTMenu* menu, int pressedButton)
{
#ifndef _DISABLE_MAIN_SCREEN_BUTTONS
  // обработчик нажатия на кнопку. Номера кнопок начинаются с 0 и идут в том порядке, в котором мы их добавляли
	if (pressedButton == 0)
	{
		menu->switchToScreen("Settings"); // переключаемся на экран работы с SD
	}
	else if (pressedButton == 1)
	{
		menu->switchToScreen("SCREEN4"); // переключаемся на третий экран
	}
#endif // !_DISABLE_MAIN_SCREEN_BUTTONS
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Screen1::getFreeMemory()
{
    char top = 't';
    return &top - reinterpret_cast<char*>(sbrk(0));

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------