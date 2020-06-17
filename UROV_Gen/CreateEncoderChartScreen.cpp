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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CreateEncoderChartScreen::CreateEncoderChartScreen() : AbstractTFTScreen("CreateEncoderChartScreen")
{
  
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
void CreateEncoderChartScreen::enableSaveButtons(bool en, bool redraw)
{
  if(!en)
  {
    screenButtons->disableButton(file1Button, redraw && screenButtons->buttonEnabled(file1Button));
    screenButtons->disableButton(file2Button, redraw && screenButtons->buttonEnabled(file2Button));
    screenButtons->disableButton(file3Button, redraw && screenButtons->buttonEnabled(file3Button));

    screenButtons->disableButton(mem1Button, redraw && screenButtons->buttonEnabled(mem1Button));
    screenButtons->disableButton(mem2Button, redraw && screenButtons->buttonEnabled(mem2Button));
    screenButtons->disableButton(mem3Button, redraw && screenButtons->buttonEnabled(mem3Button));
    
  }
  else
  {
    screenButtons->enableButton(file1Button, redraw && !screenButtons->buttonEnabled(file1Button));
    screenButtons->enableButton(file2Button, redraw && !screenButtons->buttonEnabled(file2Button));
    screenButtons->enableButton(file3Button, redraw && !screenButtons->buttonEnabled(file3Button));

    screenButtons->enableButton(mem1Button, redraw && !screenButtons->buttonEnabled(mem1Button));
    screenButtons->enableButton(mem2Button, redraw && !screenButtons->buttonEnabled(mem2Button));
    screenButtons->enableButton(mem3Button, redraw && !screenButtons->buttonEnabled(mem3Button));
    
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
      return;
    }
    
    fileB.open(fileBName.c_str(),FILE_WRITE | O_TRUNC);
    if(!fileB.isOpen())
    {
      fileA.close();
      enableSaveButtons(true,true);
      return;
    }

    fileA.rewind();
    fileB.rewind();
    
    // теперь пишем в файлы

    // сначала записываем смещения, чтобы обеспечить эмуляцию энкодера
    uint32_t rec = 0;
    writeToFile(fileA,rec); // нулевое смещение для линии А

    rec = 5;
    writeToFile(fileB,rec); // смещение в 5 микросекунд для линии B

    // теперь пишем все данные
    for(size_t i=0;i<pulsesList.size();i++)
    {
      rec = pulsesList[i];
      writeToFile(fileA,rec); // импульс линии А
      writeToFile(fileB,rec); // импульс линии В
    }

    fileA.close();
    fileB.close();
    
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

   float tStep = 1.0/pointsCount;
   float t = 0;
   
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
  
/*
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

   //  // посчитали кол-во точек по частям, выводим это в Serial
     uint32_t sumPoints = 0;
     for(size_t i=0;i<xPoints.size();i++)
     {
        sumPoints += xPoints[i];
        Serial.print("Pulses per part #"); Serial.print((i+1)); Serial.print(": "); Serial.println(xPoints[i]);
     }

     Serial.print("SUM of pulses: "); Serial.println(sumPoints);

     // теперь считаем точки по частям
     
     Points intermediatePoints; // тут массив с конечными ЭКРАННЫМИ координатами рассчитанных точек для части графика
     Points resultPoints; // тут массив с конечными ЭКРАННЫМИ координатами ВСЕХ опорных точек графика
     
     ptPrev = {START_POINT_X,START_POINT_Y};
     Point ptLast = {END_POINT_X,END_POINT_Y};
     chartPoints.push_back(ptLast);


      uint32_t sumScreenPoints = 0;
      for(size_t i=0;i<chartPoints.size();i++)
      {
        intermediatePoints.clear();
        Point ptNext = chartPoints[i];
        // пытаемся посчитать точки, поместив их в массив resultPoints
        creteLinePoints(ptPrev.X, ptNext.X, ptPrev.Y, ptNext.Y, xPoints[i],intermediatePoints);
        ptPrev = ptNext;

        sumScreenPoints += resultPoints.size();

        // выводим кол-во рассчитанных ЭКРАННЫХ точек для части
        Serial.print("SCREEN Points per part #"); Serial.print((i+1)); Serial.print(": "); Serial.println(resultPoints.size());

        // теперь отрисуем точки на экране в виде кружочков
        dc->setColor(VGA_YELLOW);
        for(size_t k=0;k<intermediatePoints.size();k++)
        {
          Point pt = intermediatePoints[k];
          dc->fillCircle(pt.X,pt.Y,2);

          resultPoints.push_back(pt);
        } // for  
              
      } // for

      Serial.print("SUM of SCREEN points: "); Serial.println(sumScreenPoints);

      chartPoints.pop();


     /* 
      у нас есть:
      
      Vector<uint16_t> xPoints; // кол-во точек (импульсов) на часть графика
      Vector<float> xPartPercents; // процентное соотношение времени для части графика (от общего времени срабатывания всего графика)

      все опорные точки - расположены в resultPoints, их можно разбить на части, руководствуясь массивом кол-ва точек на часть xPoints.
     
    */

    // теперь нам надо сформировать длительности импульсов, исходя из сгенерированных опорных точек.
    // для каждой части у нас есть её временнОй вес (в списке xPartPercents), и N точек (N - в списке xPoints).
    // координата по Y - чем меньше (у нас значения по Y инвертированы) - тем меньше общее время импульса.
    
    pulsesList.clear(); // очищаем результирующий список импульсов

    // вычисляем минимальную и максимальную координаты по Y из списка resultPoints

    int minY, maxY;
    minY = maxY = resultPoints[0].Y;

    for(size_t z=0;z<resultPoints.size();z++)
    {
      Point pt = resultPoints[z];
      minY = min(minY,pt.Y);
      maxY = max(maxY,pt.Y);
    }

    // получили минимальную и максимальную координаты по Y, относительно которых будем потом рассчитывать длительность итерации цикла для одной опорной точки графика
    
    int fullYDia = maxY - minY; // полная дельта размаха по Y, 100% ширины одного самого длительного импульса
    float fullWorkTime = 1000.*(PULSE_CHART_WORK_TIME); // полное время работы графика (100%), микросекунд
        

    // теперь проходим по массиву xPoints, берём оттуда кол-во точек, пробегаемся по точкам, высчитываем паузы между импульсами
    size_t listIterator = 0;

    for(size_t j=0;j<xPoints.size();j++)
    {
      size_t partPointsCount = xPoints[j]; // кол-во точек в текущей части
      float partTimePercents = xPartPercents[j]; // процентовка по времени для текущей части

      // рассчитываем общее время в микросекундах для работы текущей части графика
      // fullWorkTime = 100%
      // x = partTimePercents
      // x = (partTimePercents*fullWorkTime)/100;
      float partWorkTime = (partTimePercents*fullWorkTime)/100; // общее время работы части графика, микросекунд

      // получили общую длительность работы части графика. у нас есть кол-во точек, усреднённая длительность итерации цикла для одной точки,
      // а также полный размах времени импульса по Y (в переменной fullYDia). Следовательно, мы можем вычислить, какой процент по размаху
      // от fullYDia занимает текущая координата по Y.      
      
      // пробегаемся по точкам текущей части, считая весовые доли каждой точки
      Vector<float> partPointsWeight; // список весов каждой точки части графика
      float weightSum = 0; // сумма весов всех точек
      
      for(size_t k=0;k<partPointsCount;k++)
      {
        Point pt = resultPoints[listIterator];
        listIterator++;

        // получили текущую опорную точку. вычисляем, какой процент по размаху
        // от fullYDia занимает текущая координата по Y.      
        // fullYDia = 100%
        // pt.Y = x%
        // x = (pt.Y*100)/fullDia

        float pointWeight = (1.*pt.Y)/fullYDia; // весовая доля точки

        /*
         допустим, у нас размах по Y - 10, длительность работы части графика - 3 секунды, точек - 3, первая и третья
         точки - имеют максимально длительный импульс, вторая - 50% длительности импульса, т.е. график имеет вид /\.
         Среднее время длительности для одной точки - 1 секунда (3 секунды / 3 точки). Весовые доли точек распределяются таким образом:

          1. 1
          2. 0.5
          3. 1

          всего весовых долей - 2.5 на 3 секунды длительности работы отрезка графика, т.е. на 1 весовую долю приходится 3/2.5 = 1.2 секунды
         */

        weightSum += pointWeight;
        partPointsWeight.push_back(pointWeight); // запоминаем вес точки        
        
      } // for

      // в weightSum у нас - сумма весов всех точек. теперь мы можем посчитать, сколько приходится на одну весовую долю времени.
      // для этого надо время работы части графика разделить на сумму весов точек.
      float oneWeightTime = partWorkTime/weightSum;

      // теперь мы имеем общий расклад по весам для каждой точки, и можем вычислить время импульса для каждой точки
      for(size_t w=0;w<partPointsWeight.size();w++)
      {
         float pWeight = partPointsWeight[w];
         uint32_t pulseWidth = (pWeight*oneWeightTime);

         if(pulseWidth < (PULSE_WIDTH)*2) // минимальная ширина импульса - двойная ширина высокого уровня, т.е. минимальное заполнение - 50%
         {
            pulseWidth = (PULSE_WIDTH)*2;
         }

         // отнимаем от ширины имппульса ширину высокого уровня, чтобы обеспечить правильность по длительности времени
         pulseWidth -= (PULSE_WIDTH);

         // печатаем для теста
         Serial.print("Pulse width: "); Serial.println(pulseWidth);

         // сохраняем в список
         pulsesList.push_back(pulseWidth);
      }
      
      
    } // for
    
  } // if(chartPoints.size())

  if(pulsesList.size())
  {
    enableSaveButtons(true,true);    
  }

  
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

