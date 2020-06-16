//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "CreateEncoderChartScreen.h"
#include "Buzzer.h"

const size_t MAX_POINTS_IN_CHART = 8; // максимальное кол-во точек на экране графика
const uint16_t TOUCH_X_MIN = 20; // минимальная координата обработки тача по X
const uint16_t TOUCH_Y_MIN = 30; // минимальная координата обработки тача по Y

const uint16_t START_POINT_X = 20; // начальная координата физической точки графика на экране по X
const uint16_t START_POINT_Y = 230;  // начальная координата физической точки графика на экране по X

const uint16_t END_POINT_X = 320; // конечная координата физической точки графика на экране по X
const uint16_t END_POINT_Y = 230;  // конечная координата физической точки графика на экране по X

const uint16_t TOTAL_POINTS_IN_CHART = 200; // сколько точек надо получить на графике?

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CreateEncoderChartScreen::CreateEncoderChartScreen() : AbstractTFTScreen("CreateEncoderChartScreen")
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::onActivate()
{
  chartPoints.clear(); // очищаем список наших экранных точек
  touch_x_min = TOUCH_X_MIN; // сбрасываем начальную координату по X

  screenButtons->disableButton(calculateButton);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::doSetup(TFTMenu* menu)
{
  // тут настраиваемся, например, можем добавлять кнопки

	
	int menu_height = 30;
	int button_gap = 5;
	int height_button = 29;
	int width_button = 125;

  clearButton = screenButtons->addButton(5, 255, 150, 40, "ОЧИСТИТЬ");
  calculateButton = screenButtons->addButton(160, 255, 150, 40, "ВЫЧИСЛИТЬ");
  backButton = screenButtons->addButton( 315 ,  255, 150,  40, "ЗАВЕРШИТЬ");

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
void CreateEncoderChartScreen::doUpdate(TFTMenu* menu)
{
  if(chartPoints.size() < MAX_POINTS_IN_CHART) // если кол-во точек не превысило максимальное
	{
		get_Point_Screen(menu); // Определить точку на сетке

	}

    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::drawGrid(TFTMenu* menu)
{

  UTFT* dc = menu->getDC();
  dc->setColor(VGA_WHITE);
  dc->setBackColor(VGA_BLACK);

  dc->setFont(BigRusFont);
  // тут рисуем, что надо именно нам, кнопки прорисуются сами после того, как мы тут всё отрисуем
  menu->print("Экран создания графика", 70, 10);
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
  dc->fillRect(gridX, gridY, gridX + 15 + (columnWidth*columnsCount), gridY + 18 + (rowHeight*rowsCount)); // Очистить экран
  Drawing::DrawGrid(gridX, gridY, columnsCount, rowsCount, columnWidth, rowHeight, gridColor);  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::doDraw(TFTMenu* menu)
{
  drawGrid(menu);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
	// обработчик нажатия на кнопку.

	if (pressedButton == backButton)
	{
	   menu->switchToScreen("Main"); // переключаемся на первый экран
    }
    else if (pressedButton == clearButton)
    {
		  clear_Grid(menu);  //  "ОЧИСТИТЬ"
  	}
	else if (pressedButton == calculateButton)
	{
    if(!(chartPoints.size() < MAX_POINTS_IN_CHART)) // формировать график, только если кол-во точек в списке необходимое
    {
		  create_Schedule(menu);  //  Сформировать график
    }
	}
	else if (pressedButton == file1Button)
	{
		// Сохранить в файл 1
	}
	else if (pressedButton == file2Button)
	{
		// Сохранить в файл 2
	}
	else if (pressedButton == file3Button)
	{
		// Сохранить в файл 3
	}
	else if (pressedButton == mem1Button)
	{
		// Сохранить в память 1
	}
	else if (pressedButton == mem2Button)
	{
		// Сохранить в память 2
	}
	else if (pressedButton == mem3Button)
	{
		// Сохранить в память 3
	}
	else if (pressedButton == grid_Button)
	{
		Serial.println("pressedButton");// 
	}
	

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  CreateEncoderChartScreen::get_Point_Screen(TFTMenu* menu)
{
	URTouch* tftTouch_point = menu->getTouch();
	UTFT* dc = menu->getDC();
	dc->setColor(VGA_WHITE);
	dc->setBackColor(VGA_BLACK);

	if (tftTouch_point->dataAvailable() == true)
	{
		tftTouch_point->read();
		int		touch_x = tftTouch_point->getX();
		int		touch_y = tftTouch_point->getY();

		if ((touch_x > touch_x_min && touch_x <= END_POINT_X) && (touch_y > TOUCH_Y_MIN && touch_y <= END_POINT_Y)) //Вычислять только в пределах сетки с увеличением по Х
		{

      // добавляем новую точку в список точек
      Point pt = {touch_x, touch_y};
      chartPoints.push_back(pt);

			menu->print("*", touch_x-2, touch_y - 3);  // Точка на графике
      
			touch_x_min = touch_x; // запрещаем формировать точку по X меньше предыдущей
      
			menu->print(">", touch_x_min, 236);        // стрелка ограничения по Х на графике
			dc->drawLine(20, 240, touch_x_min, 240);   // стрелка ограничения по Х на графике
			dc->setFont(BigRusFont);
      
      String stringVar = String(chartPoints.size(), DEC);
     
			menu->print(stringVar.c_str(), 320, 234);         // стрелка ограничения по Х на графике
			dc->setFont(SmallRusFont);
      
			touch_x -= START_POINT_X;
			touch_y = START_POINT_Y - touch_y;
      
			Serial.print("touch_x : ");
			Serial.print(touch_x);
			Serial.print(", touch_y : ");
			Serial.println(touch_y);

      Buzzer.buzz();

      if(chartPoints.size() >= MAX_POINTS_IN_CHART)
      {
        screenButtons->enableButton(calculateButton, !screenButtons->buttonEnabled(calculateButton));
      }
      
		}
		while (tftTouch_point->dataAvailable() == true) {}
    delay(500);
    
	} // dataAvailable

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::clear_Grid(TFTMenu* menu)
{
  drawGrid(menu); // рисуем сетку снова

  chartPoints.clear(); // очищаем список наших экранных точек
  touch_x_min = TOUCH_X_MIN; // сбрасываем начальную координату по X

  screenButtons->disableButton(calculateButton, screenButtons->buttonEnabled(calculateButton));

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void creteLinePoints(int x1, int x2, int y1, int y2/*, uint16_t pointsCount*/, Points& resultPoints)
{

    unsigned int  dx = (x2 > x1 ? x2 - x1 : x1 - x2);
    short     xstep =  x2 > x1 ? 1 : -1;
    unsigned int  dy = (y2 > y1 ? y2 - y1 : y1 - y2);
    short     ystep =  y2 > y1 ? 1 : -1;
    int       col = x1, row = y1;

    if (dx < dy)
    {
      int t = - (dy >> 1);
      while (true)
      {
        Point pt = {col, row};
        resultPoints.push_back(pt);
        
        if (row == y2)
          return;
          
        row += ystep;
        t += dx;
        if (t >= 0)
        {
          col += xstep;
          t   -= dy;
        }
      } 
    }
    else
    {
      int t = - (dx >> 1);
      while (true)
      {
        Point pt = {col, row};
        resultPoints.push_back(pt);
        
        if (col == x2)
          return;
          
        col += xstep;
        t += dy;
        if (t >= 0)
        {
          row += ystep;
          t   -= dx;
        }
      } 
    }
  
/*
  
   int deltax = abs(x1 - x0); // 2
   int deltay = abs(y1 - y0); // 10
 
   double error = 0;
   double xStep = double(x1-x0)/pointsCount; // 2/100 = 0.02
   double deltaerr = (double(deltay + 0) / double(deltax + 0)) / (double(pointsCount)/deltax); // (10/2) / (100/2) = 0.1
 
   int y = y0;
   int diry = y1 - y0;
 
   if(diry > 0)
       diry = 1;
 
   if(diry < 0)
       diry = -1;
 
 
 double x = x0;
 while(x <= x1)
 {
  
  int pointX = x; // БЕЗ ROUND
  int pointY = y;

  Serial.print("X="); Serial.print(pointX); Serial.print("; Y="); Serial.println(pointY);

  Point pt = {pointX, pointY};
  resultPoints.push_back(pt);
         
  error += deltaerr;
   
    if(error >= 1.0)
   {
           y += diry;
           error -= 1.0;
   }
   
   x += xStep;
 } // while
   */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::create_Schedule(TFTMenu* menu)  //  Сформировать график
{
	UTFT* dc = menu->getDC();
	dc->setColor(VGA_WHITE);
	dc->setBackColor(VGA_BLACK);

  // рисуем наши точки. Начальная точка графика у нас в координатах 20,230, конечная точка графика у нас в координатах 320,230
  if(chartPoints.size())
  {
    // рисуем, только если есть точки в списке
    Point ptPrev = {START_POINT_X,START_POINT_Y};

    for(size_t i=0;i<chartPoints.size();i++)
    {
      Point ptNext = chartPoints[i];
      dc->drawLine(ptPrev.X, ptPrev.Y, ptNext.X, ptNext.Y);

      ptPrev = ptNext;
    } // for

    // рисуем окончание графика
    dc->drawLine(ptPrev.X, ptPrev.Y, END_POINT_X, END_POINT_Y);
  }

  // ТЕСТОВЫЙ КОД - НАЧАЛО

  // у нас ситуация: есть N точек, и M промежутков. Есть общее количество точек на графике. Есть общая дельта по X, соответственно, мы можем выяснить
  // кол-во точек на каждом участке графика по дельте Х этого участка.

  Serial.println("====================================================================");

  // выводим общее количество точек, требуемое на графике
  Serial.print("Total points needed: "); Serial.println(TOTAL_POINTS_IN_CHART);

  // получаем общую дельту по X
  int totalDeltaX = abs(END_POINT_X - START_POINT_X);

  Serial.print("Total X delta: "); Serial.println(totalDeltaX);

  // теперь считаем дельты и кол-во точек, требуемых для каждого участка графика
  if(chartPoints.size())
  {
      Vector<uint16_t> xDeltas;
      
      Point ptPrev = {START_POINT_X,START_POINT_Y};
      for(size_t i=0;i<chartPoints.size();i++)
      {
         Point ptNext = chartPoints[i];

          xDeltas.push_back(abs(ptNext.X - ptPrev.X));
         
         ptPrev = ptNext;
      }
      
     xDeltas.push_back(abs(END_POINT_X - ptPrev.X));

     // выводим список всех дельт по X
     for(size_t i=0;i<xDeltas.size();i++)
     {
        Serial.print("X delta #"); Serial.print((i+1)); Serial.print(": "); Serial.println(xDeltas[i]);
     }

     // теперь рассчитываем кол-во точек на каждом из отрезков
     Vector<uint16_t> xPoints; // кол-во точек на часть графика
     Vector<float> xPartPercents; // процентное соотношение времени для части графика (от общего времени срабатывания всего графика)
     
     float deltaErr = 0.0; // ошибка накопления точек

     uint16_t pointsGenerated = 0;

     for(size_t i=0;i<xDeltas.size();i++)
     {
        uint16_t xDelta = xDeltas[i];
        
        // totalDeltaX = 100%
        // xDelta = x%
        // x% = (xDelta*100)/totalDeltaX;

        float percents = (100.*xDelta)/totalDeltaX;
        xPartPercents.push_back(percents);

        // теперь считаем кол-во точек на отрезок
        // TOTAL_POINTS_IN_CHART = 100%
        // x = percents
        // x = (percents*TOTAL_POINTS_IN_CHART)/100;

        float pointsPerPart = (percents*TOTAL_POINTS_IN_CHART)/100 + deltaErr;
        uint16_t pppInt = pointsPerPart;
        deltaErr = pointsPerPart - pppInt;

        pointsGenerated += pppInt;

        if(i == xDeltas.size() - 1) // добиваем до общего кол-ва точек в конце графика
        {
            while(pointsGenerated < TOTAL_POINTS_IN_CHART)
            {
              pointsGenerated++;
              pppInt++;
            }
        }

        xPoints.push_back(pppInt);
        
     } // for

     // посчитали кол-во точек по частям, выводим это в Serial
     uint32_t sumPoints = 0;
     for(size_t i=0;i<xPoints.size();i++)
     {
        sumPoints += xPoints[i];
        Serial.print("Pulses per part #"); Serial.print((i+1)); Serial.print(": "); Serial.println(xPoints[i]);
     }

     Serial.print("SUM of pulses: "); Serial.println(sumPoints);

     // теперь считаем точки по частям
     
     Points resultPoints; // тут массив с конечными ЭКРАННЫМИ координатами рассчитанных точек
     ptPrev = {START_POINT_X,START_POINT_Y};
     Point ptLast = {END_POINT_X,END_POINT_Y};
     chartPoints.push_back(ptLast);


      uint32_t sumScreenPoints = 0;
      for(size_t i=0;i<chartPoints.size();i++)
      {
        resultPoints.clear();
        Point ptNext = chartPoints[i];
        // пытаемся посчитать точки, поместив их в массив resultPoints
        creteLinePoints(ptPrev.X, ptNext.X, ptPrev.Y, ptNext.Y, resultPoints);
        ptPrev = ptNext;

        sumScreenPoints += resultPoints.size();

        // выводим кол-во рассчитанных ЭКРАННЫХ точек для части
        Serial.print("SCREEN Points per part #"); Serial.print((i+1)); Serial.print(": "); Serial.println(resultPoints.size());
      } // for

      Serial.print("SUM of SCREEN points: "); Serial.println(sumScreenPoints);

      chartPoints.pop();
     
    
  } // if(chartPoints.size())

  
  Serial.println("====================================================================");

/*
  // теперь для теста просто рассчитываем кол-во точек между начальной точкой графика и первой точкой, поставленной пользователем
  // для упрощения теста считаем, что там 1 нас 100 точек.

  if(chartPoints.size())
  {
      Points resultPoints; // тут массив с конечными координатами рассчитанных точек
      const uint8_t POINTS_PER_PART = 50; // сколько точек будет на один отрезок графика

       Point ptPrev = {START_POINT_X,START_POINT_Y};
  
      for(size_t i=0;i<chartPoints.size();i++)
      {
        Point ptNext = chartPoints[i];
        // пытаемся посчитать POINTS_PER_PART точек, поместив их в массив resultPoints
        creteLinePoints(ptPrev.X, ptNext.X, ptPrev.Y, ptNext.Y, POINTS_PER_PART, resultPoints);
        ptPrev = ptNext;
      } // for

    // формируем окончание графика
    creteLinePoints(ptPrev.X, END_POINT_X, ptPrev.Y, END_POINT_Y, POINTS_PER_PART, resultPoints);      
          
    // теперь пытаемся отрисовать эти точки пикселями на экране
    dc->setColor(VGA_YELLOW);
    for(size_t i=0;i<resultPoints.size();i++)
    {
      Point pt = resultPoints[i];
      dc->drawPixel(pt.X,pt.Y);
    } // for

  } // if(chartPoints.size())
  
  */

  // ТЕСТОВЫЙ КОД - КОНЕЦ
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

