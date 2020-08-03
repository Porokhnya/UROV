//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "CreateEncoderChartScreen.h"
#include "Buzzer.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const size_t MAX_POINTS_IN_CHART = 8; // максимальное кол-во точек на экране графика
const uint16_t TOUCH_X_MIN = 20; // минимальная координата обработки тача по X
const uint16_t TOUCH_Y_MIN = 30; // минимальная координата обработки тача по Y

const uint16_t START_POINT_X = 20; // начальная координата физической точки графика на экране по X
const uint16_t START_POINT_Y = 230;  // начальная координата физической точки графика на экране по X

const uint16_t END_POINT_X = 320; // конечная координата физической точки графика на экране по X
const uint16_t END_POINT_Y = 230;  // конечная координата физической точки графика на экране по X

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CreateEncoderChartScreen::CreateEncoderChartScreen() : AbstractTFTScreen("CreateEncoderChartScreen")
{
  countPulses = 200;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::onDeactivate()
{
  chartPoints.clear(); // очищаем список наших экранных точек
  pulsesList.clear(); // очищаем список сгенерённых импульсов
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::onActivate()
{
  chartPoints.clear(); // очищаем список наших экранных точек
  pulsesList.clear(); // очищаем список сгенерённых импульсов
  
  touch_x_min = TOUCH_X_MIN; // сбрасываем начальную координату по X

  screenButtons->disableButton(calculateButton);

  countPulsesCaption = countPulses;
  screenButtons->relabelButton(countPulsesButton,countPulsesCaption.c_str());

  enableSaveButtons(false);
  
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
  file4Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 4");
  menu_height += height_button + button_gap;
  file5Button = screenButtons->addButton(340, menu_height, width_button, height_button, "FILE 5");
  menu_height += height_button + button_gap;
  countPulsesButton = screenButtons->addButton(340, menu_height, width_button, height_button, ""); //  // кнопка кол-ва импульсов



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
void CreateEncoderChartScreen::enableControlButtons(bool en, bool redraw)
{
  if(!en)
  {
    screenButtons->disableButton(clearButton, redraw && screenButtons->buttonEnabled(clearButton));
    screenButtons->disableButton(calculateButton, redraw && screenButtons->buttonEnabled(calculateButton));
    screenButtons->disableButton(backButton, redraw && screenButtons->buttonEnabled(backButton));
    screenButtons->disableButton(grid_Button, redraw && screenButtons->buttonEnabled(grid_Button));
    screenButtons->disableButton(countPulsesButton, redraw && screenButtons->buttonEnabled(countPulsesButton));
       
  }
  else
  {
    screenButtons->enableButton(clearButton, redraw && !screenButtons->buttonEnabled(clearButton));
    screenButtons->enableButton(calculateButton, redraw && !screenButtons->buttonEnabled(calculateButton));
    screenButtons->enableButton(backButton, redraw && !screenButtons->buttonEnabled(backButton));
    screenButtons->enableButton(grid_Button, redraw && !screenButtons->buttonEnabled(grid_Button));
    screenButtons->enableButton(countPulsesButton, redraw && !screenButtons->buttonEnabled(countPulsesButton));
    }
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::enableSaveButtons(bool en, bool redraw)
{
  if(!en)
  {
    screenButtons->disableButton(file1Button, redraw && screenButtons->buttonEnabled(file1Button));
    screenButtons->disableButton(file2Button, redraw && screenButtons->buttonEnabled(file2Button));
    screenButtons->disableButton(file3Button, redraw && screenButtons->buttonEnabled(file3Button));
    screenButtons->disableButton(file4Button, redraw && screenButtons->buttonEnabled(file4Button));
    screenButtons->disableButton(file5Button, redraw && screenButtons->buttonEnabled(file5Button));
       
  }
  else
  {
    screenButtons->enableButton(file1Button, redraw && !screenButtons->buttonEnabled(file1Button));
    screenButtons->enableButton(file2Button, redraw && !screenButtons->buttonEnabled(file2Button));
    screenButtons->enableButton(file3Button, redraw && !screenButtons->buttonEnabled(file3Button));
    screenButtons->enableButton(file4Button, redraw && !screenButtons->buttonEnabled(file4Button));
    screenButtons->enableButton(file5Button, redraw && !screenButtons->buttonEnabled(file5Button));
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::writeToFile(SdFile& f, uint32_t rec)
{
  uint8_t* p = (uint8_t*) &rec;
  for(size_t i=0;i<sizeof(rec);i++)
  {
    f.write(*p);
    p++;
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CreateEncoderChartScreen::saveToFile(const char* fileName)
{
    enableSaveButtons(false,true);
    enableControlButtons(false,true);
  
    // тут сохраняем список pulsesList в файлы
    SdFile fileA, fileB; 
    String fileAName, fileBName;

    fileAName = fileName;
    fileBName = fileName;

    fileAName += ".A";
    fileBName += ".B";
    
    
    fileA.open(fileAName.c_str(),FILE_WRITE | O_TRUNC);
    if(!fileA.isOpen())
    {
      enableSaveButtons(true,true);
      enableControlButtons(true,true);
      return;
    }
    
    fileB.open(fileBName.c_str(),FILE_WRITE | O_TRUNC);
    if(!fileB.isOpen())
    {
      fileA.close();
      enableSaveButtons(true,true);
      enableControlButtons(true,true);
      return;
    }

    fileA.rewind();
    fileB.rewind();
    
    // теперь пишем в файлы

    // сначала записываем смещения, чтобы обеспечить эмуляцию энкодера
    uint32_t rec = 0;
 //   writeToFile(fileA,rec); // нулевое смещение для линии А

//    rec = 5;
//    writeToFile(fileB,rec); // смещение в 5 микросекунд для линии B

    // теперь пишем все данные
    for(size_t i=0;i<pulsesList.size();i++)
    {
      rec = pulsesList[i];
      writeToFile(fileA,rec); // импульс линии А
      writeToFile(fileB,rec); // импульс линии В
    }

    fileA.close();
    fileB.close();

    enableControlButtons(true,true);
    
    // показываем сообщение, что данные сохранены
    Vector<const char*> lines;
    lines.push_back("Данные");
    lines.push_back("сохранены.");    
    MessageBox->show(lines,"CreateEncoderChartScreen");    
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
   saveToFile("/FILE1");
	}
	else if (pressedButton == file2Button)
	{
		// Сохранить в файл 2
   saveToFile("/FILE2");
	}
	else if (pressedButton == file3Button)
	{
		// Сохранить в файл 3
   saveToFile("/FILE3");
	}
	else if (pressedButton == file4Button)
	{
		// Сохранить в файл 3
		saveToFile("/FILE4");
	}
	else if (pressedButton == file5Button)
	{
		// Сохранить в файл 3
		saveToFile("/FILE5");
	}
	else if (pressedButton == grid_Button)
	{
		DBGLN("pressedButton");// 
	}
  else if (pressedButton == countPulsesButton)
  {
    countPulses += 50;
    if(countPulses > 1000)
    {
      countPulses = 200;
    }

    countPulsesCaption = countPulses;
    screenButtons->relabelButton(countPulsesButton,countPulsesCaption.c_str(),true);
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
      
			DBG("touch_x : ");
			DBG(touch_x);
			DBG(", touch_y : ");
			DBGLN(touch_y);

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
  pulsesList.clear(); // очищаем список сгенеренных импульсов
  touch_x_min = TOUCH_X_MIN; // сбрасываем начальную координату по X

  screenButtons->disableButton(calculateButton, screenButtons->buttonEnabled(calculateButton));
  enableSaveButtons(false,true);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void creteLinePoints(int x1, int x2, int y1, int y2, uint16_t pointsCount, Points& resultPoints)
{

  // работаем с эквидистантами
  /*
   x = x1 + (x2-x1) * t
   y = y1 + (y2-y1) * t

   где t - шаг между точками, в диапазоне 0-1
   */

   double tStep = 1.0/pointsCount;
   double t = 0;
   
   int deltax = (x2-x1);
   int deltay = (y2-y1);

   for(uint16_t i=0;i<pointsCount;i++)
   {
     int x = x1 + t * deltax;
     int y = y1 + t * deltay;
     t += tStep;

     Point pt = {x,y};
     resultPoints.push_back(pt);
   } // for
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double map(double x, double in_min, double in_max, double out_min, double out_max)
{
   // return x;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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

      if(ptPrev.X != ptNext.X && ptPrev.Y != ptNext.Y)
      {
        dc->drawLine(ptPrev.X, ptPrev.Y, ptNext.X, ptNext.Y);
      }

      ptPrev = ptNext;
    } // for

    // рисуем окончание графика
    if(ptPrev.X != END_POINT_X && ptPrev.Y != END_POINT_Y)
    {
      dc->drawLine(ptPrev.X, ptPrev.Y, END_POINT_X, END_POINT_Y);
    }
  }

  // ТЕСТОВЫЙ КОД - НАЧАЛО

  // у нас ситуация: есть N точек, и M промежутков. Есть общее количество точек на графике. Есть общая дельта по X, соответственно, мы можем выяснить
  // кол-во точек на каждом участке графика по дельте Х этого участка.

  DBGLN("====================================================================");

  // выводим общее количество точек, требуемое на графике
  DBG("Total points needed: "); DBGLN(countPulses);

  // получаем общую дельту по X
  int totalDeltaX = abs(END_POINT_X - START_POINT_X);

  DBG("Total X delta: "); DBGLN(totalDeltaX);

  // теперь считаем дельты и кол-во точек, требуемых для каждого участка графика
  if(chartPoints.size())
  {

    // 1. считаем дельты по Х для каждого участка

      Vector<uint16_t> xDeltas;
      
      Point ptPrev = {START_POINT_X,START_POINT_Y};
      for(size_t i=0;i<chartPoints.size();i++)
      {
         Point ptNext = chartPoints[i];

          xDeltas.push_back(abs(ptNext.X - ptPrev.X));
         
         ptPrev = ptNext;
      }
      
     xDeltas.push_back(abs(END_POINT_X - ptPrev.X));

    #ifdef _DEBUG
     // выводим список всех дельт по X
     for(size_t i=0;i<xDeltas.size();i++)
     {
       DBG("X delta #"); DBG((i+1)); DBG(": "); DBGLN(xDeltas[i]);
     }
     #endif

     // теперь рассчитываем кол-во точек на каждом из отрезков
     Vector<uint16_t> xPoints; // кол-во точек на часть графика
     
     double deltaErr = 0.0; // ошибка накопления точек
     uint16_t pointsGenerated = 0;

    // 2. считаем процентный вес участка к общему времени
    // 3. считаем кол-во точек для каждого участка

     for(size_t i=0;i<xDeltas.size();i++)
     {
        uint16_t xDelta = xDeltas[i];
        
        // totalDeltaX = 100%
        // xDelta = x%
        // x% = (xDelta*100)/totalDeltaX;
        double percents = (100.*xDelta)/totalDeltaX;

        // теперь считаем кол-во точек на отрезок
        // countPulses = 100%
        // x = percents
        // x = (percents*countPulses)/100;

        double pointsPerPart = (percents*countPulses)/100 + deltaErr;
        uint16_t pppInt = pointsPerPart;
        deltaErr = pointsPerPart - pppInt;

        pointsGenerated += pppInt;

        if(i == xDeltas.size() - 1) // добиваем до общего кол-ва точек в конце графика
        {
            while(pointsGenerated < countPulses)
            {
              pointsGenerated++;
              pppInt++;
            }
        }

        xPoints.push_back(pppInt);
        
     } // for


   //  посчитали кол-во точек по частям, выводим это в Serial
   #ifdef _DEBUG
     uint32_t sumPoints = 0;
     for(size_t i=0;i<xPoints.size();i++)
     {
        sumPoints += xPoints[i];
        DBG("Pulses per part #"); DBG((i+1)); DBG(": "); DBGLN(xPoints[i]);
     }

     DBG("SUM of pulses: "); DBGLN(sumPoints);
     
     #endif     
         
    // 4. генерируем опорные точки

    // теперь считаем точки по частям
     
     Points intermediatePoints; // тут массив с конечными ЭКРАННЫМИ координатами рассчитанных точек для части графика
     Points resultPoints; // тут массив с конечными ЭКРАННЫМИ координатами ВСЕХ опорных точек графика
     
     ptPrev = {START_POINT_X,START_POINT_Y};
     Point ptLast = {END_POINT_X,END_POINT_Y};
     
     chartPoints.push_back(ptLast); // добавляем временную последнюю точку

      #ifdef _DEBUG
      uint32_t sumScreenPoints = 0;
      #endif
      
      for(size_t i=0;i<chartPoints.size();i++)
      {
        intermediatePoints.clear();
        Point ptNext = chartPoints[i];
        // пытаемся посчитать точки, поместив их в массив resultPoints
        creteLinePoints(ptPrev.X, ptNext.X, ptPrev.Y, ptNext.Y, xPoints[i],intermediatePoints);
        ptPrev = ptNext;

        #ifdef _DEBUG
        sumScreenPoints += intermediatePoints.size();
        #endif

        // выводим кол-во рассчитанных ЭКРАННЫХ точек для части
        DBG("SCREEN Points per part #"); DBG((i+1)); DBG(": "); DBGLN(intermediatePoints.size());

        // 5. отрисовываем опорные точки на экране

        dc->setColor(VGA_YELLOW);
        for(size_t k=0;k<intermediatePoints.size();k++)
        {
          Point pt = intermediatePoints[k];
          dc->fillCircle(pt.X,pt.Y,2);

          resultPoints.push_back(pt);
        } // for  
              
      } // for

      chartPoints.pop(); // убираем последнюю временную точку
      
      DBG("SUM of SCREEN points: "); DBGLN(sumScreenPoints);
    
    // 6. по опорным точкам строим график прерываний

    pulsesList.clear(); // очищаем результирующий список импульсов

    // вычисляем минимальную и максимальную координаты по Y из списка resultPoints

    int minY, maxY, minX, maxX;
    minY = maxY = resultPoints[0].Y;
    minX = maxX = resultPoints[0].X;

    for(size_t z=0;z<resultPoints.size();z++)
    {
      Point pt = resultPoints[z];
      
      minY = min(minY,pt.Y);
      maxY = max(maxY,pt.Y);
      
      minX = min(minX,pt.X);
      maxX = max(maxX,pt.X);

      DBG("pt.X="); DBG(pt.X); DBG(", pt.Y="); DBGLN(pt.Y);
    } // for
    
    int fullYDia = maxY - minY; // полная дельта размаха по Y
    int fullXDia = maxX - minX; // полная дельта размаха по X
    double fullWorkTime = 1000.0 * Settings.getChartWorkTime(); // полное время работы графика (100%), микросекунд

    // считаем веса точек по Y.
    double weightYSum = 0; // сумма весов всех точек, по Y

    // сумма весов считается как сумма Yi*dt, где dt = длительность участка графика по X

        for(size_t z=0;z<resultPoints.size()-1;z++)
        {
            Point ptCur = resultPoints[z];
            Point ptNext = resultPoints[z + 1];

            double deltaX = ptNext.X - ptCur.X; // промежуток времени для отрезка
            double pointWeight = (1.0 * ptCur.Y) / maxY; // весовая доля точки
            pointWeight = map(pointWeight, 0.0, 1.0, 1.0, 10.0);
            double pointWeightMapped = map(pointWeight, 0.0, 1.0, 1.0, 10.0);

            double dt = (deltaX / fullXDia);

            weightYSum += pointWeightMapped * dt; // приплюсовали к сумме весов

        } // for

        DBG("weightYSum = "); DBGLN(weightYSum);
        
    // сумму весов высчитали, теперь считаем относительный вес каждой точки
        double relativePulseWidthSum = 0;

            Vector<double> relativePulseWidthList; // список относительных ширин импульсов

            for(size_t z=0;z<resultPoints.size()-1;z++)
            {
                Point ptCur = resultPoints[z];
                double pointWeight = (1.0 * ptCur.Y) / maxY; // весовая доля точки
                double pointWeightMapped = map(pointWeight, 0.0, 1.0, 1.0, 10.0);

                double relativePulseWidth = (pointWeightMapped * resultPoints.size()) / weightYSum; // импульсов на единицу времени для точки
                relativePulseWidth /= 10; // 10 - максимальная величина интервала весов точки

                relativePulseWidthSum += relativePulseWidth;

                relativePulseWidthList.push_back(relativePulseWidth);

                // печатаем для теста
                DBG("Point weight: "); DBG(pointWeight);
                DBG(",\tpointWeightMapped: "); DBG(pointWeightMapped);
                DBG(",\t\trelativePulseWidth: "); DBGLN(relativePulseWidth);


            } // for

            double relativeTimeUnit = fullWorkTime / relativePulseWidthSum; // относительная единица времени на график

            DBG("SUM OF RELATIVE WIDTH: "); DBGLN(relativePulseWidthSum);
            DBG("RELATIVE TIME UNIT: "); DBGLN(relativeTimeUnit);


            // теперь высчитываем абсолютные ширины импульсов
            double absPulseWidthSum = 0;

           // double maxPulseWidth = 0;

            uint32_t pulseWidthSetting = Settings.getChartPulseWidth();

            for(size_t i=0;i< relativePulseWidthList.size();i++)
            {
                double relW = relativePulseWidthList[i];

                double pulseWidth = relW * relativeTimeUnit; // абсолютная нирина импульса для точки

                absPulseWidthSum += pulseWidth;

                DBG("PULSE WIDTH: "); DBGLN(pulseWidth);

                if (pulseWidth < pulseWidthSetting * 2) // минимальная ширина импульса - двойная ширина высокого уровня, т.е. минимальное заполнение - 50%
                {
                    pulseWidth = pulseWidthSetting * 2;
                }

                // отнимаем от ширины импульса ширину высокого уровня, чтобы обеспечить правильность по длительности времени
                pulseWidth -= pulseWidthSetting;

                // сохраняем в список
                pulsesList.push_back(pulseWidth);

              //  maxPulseWidth = max(maxPulseWidth,pulseWidth);

                // всё, посчитали ширину импульса
            } // for

            // убираем последний пик
            if(pulsesList.size() > 1)
            {
              pulsesList[pulsesList.size()-1] = pulsesList[pulsesList.size()-2];
            }

            DBG("SUM OF ABSOLUTE WIDTH: "); DBGLN(absPulseWidthSum);
            
    
  } // if(chartPoints.size())

  if(pulsesList.size())
  {
    enableSaveButtons(true,true);    
  }

  
 DBGLN("====================================================================");

  // ТЕСТОВЫЙ КОД - КОНЕЦ
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
