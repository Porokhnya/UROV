//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "InterruptScreen.h"
#include "DS3231.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
InterruptScreen* ScreenInterrupt = NULL;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen* InterruptScreen::create()
{
  if(ScreenInterrupt)
    return ScreenInterrupt;
    
  ScreenInterrupt = new InterruptScreen();

  // назначаем обработчика прерываний по умолчанию - наш экран
  InterruptHandler.setSubscriber(ScreenInterrupt);
  
  return ScreenInterrupt;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
InterruptScreen::InterruptScreen() : AbstractTFTScreen("INTERRUPT")
{
  startSeenTime = 0;
  timerDelta = 0;
  canAcceptInterruptData = true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::onDeactivate()
{
    // после деактивирования нашего экрана мы опять можем принимать данные прерываний, чтобы показать новые графики
    canAcceptInterruptData = true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::OnInterruptRaised(CurrentOscillData* oscData, EthalonCompareResult compareResult)
{

  if(!canAcceptInterruptData)
  {
    DBGLN("InterruptScreen::OnInterruptRaised - CAN'T ACCEPT INTERRUPT DATA!");
    return;
  }

  // говорим, что до нового цикла мы не можем больше ничего принимать.
  canAcceptInterruptData = false;

  // пришли результаты серии прерываний с одного из списков.
  // мы запоминаем результаты в локальный список.
  EthalonCompareBox box;
  oscillData = oscData;
  box.chartColor = LGRAY;
  box.compareColor = LGRAY;
  box.foreCompareColor = LGRAY;
  box.compareCaption = "-";

  switch(compareResult)
  {
    case COMPARE_RESULT_NoSourcePulses:
    case COMPARE_RESULT_NoEthalonFound:
    case COMPARE_RESULT_RodBroken:
	{
		box.compareColor = LGRAY;
		box.foreCompareColor = BLACK;
		box.compareCaption = "-";
	}
    break;

    case COMPARE_RESULT_MatchEthalon:
	{
		box.compareColor = GREEN;
		box.foreCompareColor = BLACK;
		box.compareCaption = "OK";
	}
    break;

  case COMPARE_RESULT_MismatchEthalon:
	{
		box.compareColor = RED;
		box.foreCompareColor = WHITE;
		box.compareCaption = "ERR";
	}
    break;
  }

 compareBox = box;
   


  // сначала делаем пересчёт точек на график, т.к. у нас ограниченное кол-во точек - это раз.
  // два - когда в списках прерываний точек заведомо меньше, чем точек на графике (например, 20 вместо 150) - без пересчёта получим
  // куцый график, в этом случае нам надо его растянуть по-максимуму.

//  Drawing::ComputeChart(InterruptData, serie);

  // вычисляем моторесурс
  computeMotoresource();

  // запоминаем время начала показа и переключаемся на экран
  startSeenTime = millis();
  Screen.switchToScreen(this);
  Screen.update(); // после вызова update наш экран покажется сразу, т.е. валидность всех переданных в OnInterruptRaised мы можем гарантировать.  
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::drawTime(TFTMenu* menu)
{
    DS3231Time tm = RealtimeClock.getTime();

    // получаем компоненты даты в виде строк
    TFT_Class* dc = menu->getDC();
    //dc->setColor(WHITE);
    //dc->setBackColor(BLACK);
    dc->setFreeFont(TFT_SMALL_FONT);
    String strDate = RealtimeClock.getDateStr(tm);
    String strTime = RealtimeClock.getTimeStr(tm);

    // печатаем их
    menu->getRusPrinter()->print(strDate.c_str(), 5, 1, BLACK, WHITE);
    menu->getRusPrinter()->print(strTime.c_str(), 90, 1, BLACK, WHITE);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::doSetup(TFTMenu* menu)
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::doUpdate(TFTMenu* menu)
{
    uint32_t now = millis();
    uint32_t dT = now - timerDelta;
    timerDelta = now;
    
    if(now - startSeenTime > INTERRUPT_SCREEN_SEE_TIME)
    {
      // время показа истекло, переключаемся на главный экран
		DBGLN(F("График показан, переключаемся на главный экран!"));

      startSeenTime = 0;
      Screen.switchToScreen("Main");
      return;
    }

    bool canRedrawMotoresource = false;
    
    if(channelMotoresourcePercents >= (100 - MOTORESOURCE_BLINK_PERCENTS) )
    {
      // ресурс по системе на канале 1 исчерпан, надо мигать надписью
      motoresourceBlinkTimer += dT;
      
      if(motoresourceBlinkTimer > MOTORESOURCE_BLINK_DURATION)
      { 
        motoresourceBlinkTimer -= MOTORESOURCE_BLINK_DURATION;
               
        if(motoresourceLastFontColor == RED)
        {
          motoresourceLastFontColor = BLACK;
        }
        else
        {
          motoresourceLastFontColor = RED;
        }

          canRedrawMotoresource = true;
      }
    }

    if(canRedrawMotoresource)
    {
      drawMotoresource(menu);
    }
      
      
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::drawMotoresource(TFTMenu* menu)
{
	DBGLN("Рисуем моторесурс...");

  TFT_Class* dc = menu->getDC();
  dc->setFreeFont(TFT_SMALL_FONT);


 // uint32_t channelResourceCurrent = Settings.getMotoresource();  
//  uint32_t channelResourceMax = Settings.getMotoresourceMax();

  // рисуем моторесурс системы по каналам
  uint16_t curX = 5;
  uint16_t curY = 130;
  uint8_t fontHeight = FONT_HEIGHT(dc);


  String str;

  str = F("Импульсов: ");
  str += InterruptData.size();
  
  
  str += F("; ресурс: ");
  str += channelMotoresourcePercents;
  str += "%";

  menu->getRusPrinter()->print(str.c_str(),curX,curY,BLACK,motoresourceLastFontColor);
  curY += fontHeight + 4;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::computeMotoresource()
{
  
  uint32_t channelResourceCurrent = Settings.getMotoresource();
  uint32_t channelResourceMax = Settings.getMotoresourceMax();
  channelMotoresourcePercents = (channelResourceCurrent*100)/(channelResourceMax ? channelResourceMax : 1);
  motoresourceLastFontColor = channelMotoresourcePercents < (100 - MOTORESOURCE_BLINK_PERCENTS) ? WHITE : RED;
  timerDelta = millis();
  motoresourceBlinkTimer = (MOTORESOURCE_BLINK_DURATION/3)*2;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::drawCompareResult(TFTMenu* menu)
{
  TFT_Class* dc = menu->getDC();
  dc->setFreeFont(TFT_SMALL_FONT);
  uint8_t fontHeight = FONT_HEIGHT(dc);
  
  uint16_t curX = 162;
  uint16_t curY = 20; 
  uint8_t boxWidth = 30;  
  uint8_t boxHeight = 18;
  uint8_t spacing = 4;


	  String channelNum = "1";
    uint8_t captionLen = menu->getRusPrinter()->textWidth(channelNum.c_str());
    menu->getRusPrinter()->print(channelNum.c_str(), curX, curY + (boxHeight - captionLen)/2, BLACK, compareBox.chartColor );

    uint16_t boxLeft = curX + captionLen + spacing;
    dc->fillRoundRect(boxLeft, curY, boxWidth, boxHeight,2,compareBox.compareColor);

    captionLen = menu->getRusPrinter()->textWidth(compareBox.compareCaption);
    menu->getRusPrinter()->print(compareBox.compareCaption, boxLeft + (boxWidth - captionLen)/2, curY + (boxHeight - fontHeight)/2,compareBox.compareColor,compareBox.foreCompareColor );  


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::doDraw(TFTMenu* menu)
{
	//Drawing::DrawChart(this, serie);
  Drawing::DrawChartFromList(this,InterruptData);
	drawTime(menu);
	drawMotoresource(menu);
	drawCompareResult(menu);

  // ОЧИСТКА ПАМЯТИ
 // serie.clear();
  // возобновляем обработчик прерываний
  InterruptHandler.resume();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InterruptScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  // обработчик нажатия на кнопку. Номера кнопок начинаются с 0 и идут в том порядке, в котором мы их добавляли
 /*
  if (pressedButton == 0)
	  menu->switchToScreen("SCREEN5"); // переключаемся на 5 экран
  else if (pressedButton == 1)
	  menu->switchToScreen("SCREEN6"); // переключаемся на 6 экран
  else if (pressedButton == 2)
	  menu->switchToScreen("Main"); // переключаемся на первый экран
*/    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
