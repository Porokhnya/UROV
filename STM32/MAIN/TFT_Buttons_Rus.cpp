//--------------------------------------------------------------------------------------------------------------------------------------
#include "TFT_Buttons_Rus.h"
#include "TFTMenu.h"
//--------------------------------------------------------------------------------------------------------------------------------------
TFT_Buttons_Rus::TFT_Buttons_Rus(TFT_Class *ptrUTFT, TOUCH_Class *ptrURTouch, TFTRus* pTFTRus, int count_btns) // конструктор
{
  // сохраняем указатели на экран, тачскрин, и класс поддержки русских шрифтов
	_UTFT = ptrUTFT;
	_URTouch = ptrURTouch;
  pRusPrinter = pTFTRus;

  // создаём массив под кнопки
  countButtons = count_btns;
  buttons = new button_type[count_btns];

  // инициализируем массив, как полностью неиспользуемый
	deleteAllButtons();

  // настройки цветов по умолчанию
	_color_text				= WHITE;
	_color_text_inactive	= DGRAY;
	_color_background		= BLUE;
	_color_border			= WHITE;
	_color_hilite			= RED;
	_font_text				= NULL;
	_font_symbol			= NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------
TFT_Buttons_Rus::~TFT_Buttons_Rus() // деструктор
{
  delete [] buttons; // удаляем массив кнопок
}
//--------------------------------------------------------------------------------------------------------------------------------------
word TFT_Buttons_Rus::getButtonBackColor(int buttonID) // возвращаем фоновый цвет кнопки
{
   if(buttonID < 0 || buttonID >= countButtons)
   {
    return 0;
   }

  return buttons[buttonID].backColor;
}
//--------------------------------------------------------------------------------------------------------------------------------------
word TFT_Buttons_Rus::getButtonFontColor(int buttonID) // возвращаем цвет текста кнопки
{
   if(buttonID < 0 || buttonID >= countButtons)
   {
    return 0;
   }

  return buttons[buttonID].fontColor;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::setButtonBackColor(int buttonID, word color) // устанавливаем фоновый цвет кнопки
{
   if(buttonID < 0 || buttonID >= countButtons)
   {
    return;
   }

    buttons[buttonID].backColor = color;
    buttons[buttonID].flags |= BUTTON_HAS_BACK_COLOR;
    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::selectButton(int buttonID, bool selected, boolean redraw) // выделяем кнопку
{
   if(buttonID < 0 || buttonID >= countButtons)
   {
    return;
   }

    if(selected)
    {
      buttons[buttonID].flags |= BUTTON_SELECTED;
    }
    else
    {
      buttons[buttonID].flags &= ~BUTTON_SELECTED;
    }

    if (redraw)
    {
      drawButton(buttonID);
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::setButtonFontColor(int buttonID, word cl) // устанавливаем цвет текста кнопки
{
  if(buttonID < 0 || buttonID >= countButtons)
  {
    return;
  }

  buttons[buttonID].fontColor = cl;
  buttons[buttonID].flags |= BUTTON_HAS_FONT_COLOR;
    
}
//--------------------------------------------------------------------------------------------------------------------------------------
int TFT_Buttons_Rus::addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char *label, uint16_t flags) // добавляем кнопку
{
	int btcnt = 0;
  
	while (((buttons[btcnt].flags & BUTTON_UNUSED) == 0) and (btcnt<countButtons))
  {
		btcnt++;
  }
  
	if (btcnt == countButtons)
  {
		return -1;
  }
	else
	{
		buttons[btcnt].pos_x  = x;
		buttons[btcnt].pos_y  = y;
		buttons[btcnt].width  = width;
		buttons[btcnt].height = height;
		buttons[btcnt].flags  = flags | BUTTON_VISIBLE;
		buttons[btcnt].label  = label;
//		buttons[btcnt].data   = NULL;
		return btcnt;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::hideButton(int buttonID, boolean redraw) // скрываем кнопку
{
	if (buttonID < 0 || buttonID >= countButtons)
 {
		return;
 }

  if (!(buttons[buttonID].flags & BUTTON_UNUSED))
  {
    buttons[buttonID].flags = buttons[buttonID].flags & ~BUTTON_VISIBLE;
    if (redraw)
    {
      drawButton(buttonID);
    }
  }  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::showButton(int buttonID, boolean redraw) // показываем кнопку
{
  if (buttonID < 0 || buttonID >= countButtons)
 {
		return;
 }

  if (!(buttons[buttonID].flags & BUTTON_UNUSED))
  {
    buttons[buttonID].flags = buttons[buttonID].flags | BUTTON_VISIBLE;
    if (redraw)
    {
      drawButton(buttonID);
    }
  }  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::drawButtons(DrawButtonsUpdateFunc func) // рисуем все кнопки
{
  
	for (int i=0;i<countButtons;i++)
	{
		if ((buttons[i].flags & BUTTON_UNUSED) == 0)
    {
			drawButton(i);
      
      if(func)
      {
        func();
      }

     yield();
     
     
    }
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::setIconFont(FONTTYPE font) // устанавливаем шрифт для иконок
{
  _font_icon = font;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::setButtonHasIcon(int buttonID) // устанавливаем флаг, что первый символ текста кнопки - это иконка
{
   if(buttonID < 0 || buttonID >= countButtons)
   {
    return;
   }

    buttons[buttonID].flags |= BUTTON_HAS_ICON;
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::drawButton(int buttonID) // рисуем кнопку
{  

	if (buttonID < 0 || buttonID >= countButtons)
 {
		return;
 }


	int		text_x, text_y;
  if (!(buttons[buttonID].flags & BUTTON_VISIBLE)) // если кнопка скрыта - заливаем её прямоугольник фоновым цветом
  {
    _UTFT->fillRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].width, buttons[buttonID].height, TFT_BACK_COLOR);    
    return;
  }

    uint16_t bkColor = _color_background;
  

    if(buttons[buttonID].flags & BUTTON_HAS_BACK_COLOR && !(buttons[buttonID].flags & BUTTON_DISABLED)) // если у кнопки есть свой фоновый цвет - используем его
    {
       bkColor = buttons[buttonID].backColor;
    }
    else
    {
       bkColor = _color_background; // иначе используем фоновый цвет по умолчанию
    }
     

    // рисуем прямоугольник кнопки фоновым цветом
    _UTFT->fillRoundRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].width, buttons[buttonID].height, 2, bkColor);
    yield();


    if(buttons[buttonID].flags & BUTTON_SELECTED) // если кнопка выделена - применяем цвет выделения
    {
      bkColor = _color_hilite;
    }
    else
    {
      bkColor = _color_border; // иначе - применяем цвет рамки
    }

    // рисуем рамку
    _UTFT->drawRoundRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].width, buttons[buttonID].height, 2, bkColor);

    
    yield();

    uint16_t textColor;
    FONTTYPE curFont = _font_text;


    if (buttons[buttonID].flags & BUTTON_DISABLED) // если кнопка неактивна - применяем цвет неактивного текста
    {
      textColor = _color_text_inactive;
    }
    else
    {
      if (buttons[buttonID].flags & BUTTON_HAS_FONT_COLOR) // если у кнопки есть свой цвет текста - применяем его
      {
       textColor = buttons[buttonID].fontColor;
      }
      else
      {
        textColor = _color_text; // иначе - применяем цвет текста по умолчанию
      }
    }

    const char* label = buttons[buttonID].label; // текст на кнопке
    int icon_x = 0;
    int icon_y = 0;    
 
    if (buttons[buttonID].flags & BUTTON_SYMBOL) // если кнопка с текстом в виде символа, то вычисляем координаты символа
    {
      _UTFT->setFreeFont(_font_symbol);
      text_x = (buttons[buttonID].width/2) - (pRusPrinter->textWidth(buttons[buttonID].label)/2) + buttons[buttonID].pos_x;
      text_y = (buttons[buttonID].height/2) - (_UTFT->fontHeight(1)/2) + buttons[buttonID].pos_y;

      // FIX: залезает на рамку, если разница в координатах меньше 2 !!!
      if(text_y >= buttons[buttonID].pos_y)
      {
        while(text_y - buttons[buttonID].pos_y < 2)
        {
          text_y++;
        }
      }
     
      
    }
    else // кнопка с обычным текстом
    {
        if (buttons[buttonID].flags & BUTTON_HAS_FONT) // если у кнопки есть свой шрифт - применяем его
        {
          _UTFT->setFreeFont(buttons[buttonID].font);
          curFont = buttons[buttonID].font;
        }
        else
        {
          _UTFT->setFreeFont(_font_text); // иначе применяем шрифт по умолчанию
        }

        if((buttons[buttonID].flags & BUTTON_HAS_ICON) && _font_icon) // если первый символ текста - иконка, то переходим на второй символ текста
        {
          label++;
        }        

      /*
      text_x = ((buttons[buttonID].width/2) - (pRusPrinter->textWidth(buttons[buttonID].label)/2)) + buttons[buttonID].pos_x;
      text_y = (buttons[buttonID].height/2) - (_UTFT->fontHeight(1)/2) + buttons[buttonID].pos_y;
      */

      // вычисляем позицию текста внутри прямоугольника кнопки
      int labelWidth = pRusPrinter->textWidth(label);
      int fontHeight = _UTFT->fontHeight(1);
      text_x = ((buttons[buttonID].width/2) - (labelWidth/2)) + buttons[buttonID].pos_x;
      text_y = (buttons[buttonID].height/2) - (fontHeight/2) + buttons[buttonID].pos_y;

      if((buttons[buttonID].flags & BUTTON_HAS_ICON) && _font_icon) // если у кнопки есть иконка - смещаем координаты текста, учитывая ширину иконки
      {
        _UTFT->setFreeFont(_font_icon);

        char lbl[2] = {0};
        lbl[0] = buttons[buttonID].label[0];

        int iconWidth = pRusPrinter->textWidth(lbl);
        
        icon_x = text_x - iconWidth/2;
        text_x += iconWidth/2;
        icon_y = (buttons[buttonID].height/2) - (_UTFT->fontHeight(1)/2) + buttons[buttonID].pos_y;

        _UTFT->setFreeFont(curFont);
      }      
      
    } // else text label

    if(buttons[buttonID].flags & BUTTON_HAS_BACK_COLOR && !(buttons[buttonID].flags & BUTTON_DISABLED)) // если у кнопки есть фоновый цвет - применяем его
    {
      bkColor = buttons[buttonID].backColor;
    }
    else
    {
      bkColor = _color_background; // иначе - применяем фоновый цвет по умолчанию
    }

     // перачаем текст на экране по рассчитанным координатам
     pRusPrinter->print(label, text_x, text_y, bkColor, textColor);

    if((buttons[buttonID].flags & BUTTON_HAS_ICON) && _font_icon) // если у кнопки есть иконка - рисуем её
    {
      _UTFT->setFreeFont(_font_icon);

      char icon[2] = {0};
      icon[0] = buttons[buttonID].label[0];
      pRusPrinter->print(icon, icon_x, icon_y, bkColor, textColor);

      _UTFT->setFreeFont(curFont);
    }     

}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::setButtonFont(int buttonID, FONTTYPE someFont) // устанавливаем шрифт для кнопки
{
    if(buttonID < 0)
    {
      return;
    }

    buttons[buttonID].font = someFont;
    buttons[buttonID].flags |= BUTTON_HAS_FONT; 

}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::enableButton(int buttonID, boolean redraw) // включаем кнопку
{
	if (buttonID < 0 || buttonID >= countButtons)
 {
		return;
 }

	if (!(buttons[buttonID].flags & BUTTON_UNUSED))
	{
		buttons[buttonID].flags = buttons[buttonID].flags & ~BUTTON_DISABLED;
		if (redraw)
   {
			drawButton(buttonID);
   }
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::disableButton(int buttonID, boolean redraw) // выключаем кнопку
{
	if (buttonID < 0 || buttonID >= countButtons)
 {
		return;
 }

	if (!(buttons[buttonID].flags & BUTTON_UNUSED))
	{
		buttons[buttonID].flags = buttons[buttonID].flags | BUTTON_DISABLED;
		if (redraw)
   {
			drawButton(buttonID);
   }
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* TFT_Buttons_Rus::getLabel(int buttonID) // возвращаем текст кнопки
{
	if (buttonID < 0 || buttonID >= countButtons)
 {
		return "";
 }

  if (!(buttons[buttonID].flags & BUTTON_UNUSED))
  {
    return buttons[buttonID].label;
  }  
  return "";
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::relabelButton(int buttonID, const char *label, boolean redraw) // меняем текст на кнопке
{
	if (buttonID < 0 || buttonID >= countButtons)
 {
		return;
 }

	if (!(buttons[buttonID].flags & BUTTON_UNUSED))
	{
		buttons[buttonID].label = label;
		if (redraw)
   {
			drawButton(buttonID);
   }
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
boolean TFT_Buttons_Rus::buttonEnabled(int buttonID) // проверяем, активна ли кнопка
{
	if (buttonID < 0 || buttonID >= countButtons)
 {
		return false;
 }

	return !(buttons[buttonID].flags & BUTTON_DISABLED);
}
//-------------------------------------------------------------------------------------------------------------------------------------- 
void TFT_Buttons_Rus::deleteButton(int buttonID) // удаляем кнопку
{
	if (buttonID < 0 || buttonID >= countButtons)
 {
		return;
 }

	if (!(buttons[buttonID].flags & BUTTON_UNUSED))
 {
		buttons[buttonID].flags = BUTTON_UNUSED;
 }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::deleteAllButtons() // удаляем все кнопки
{
	for (int i=0;i<countButtons;i++)
	{
		buttons[i].pos_x=0;
		buttons[i].pos_y=0;
		buttons[i].width=0;
		buttons[i].height=0;
		buttons[i].flags=BUTTON_UNUSED;
    buttons[i].font = NULL;
		buttons[i].label="";
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
int TFT_Buttons_Rus::checkButtons(OnCheckButtonsFunc pressed, OnCheckButtonsFunc released) // проверяем кнопки на нажатие
{
  
    if (_URTouch->TouchPressed()) // если есть нажатие на тачскрин, то
    //if (_URTouch->dataAvailable() == true)
    {

      //_URTouch->read();
      TS_Point p = _URTouch->getPoint(); // получаем координаты

		int		result = -1;
		int		touch_x = p.x;
		int		touch_y = p.y;
    //int    touch_x = _URTouch->getX();
    //int   touch_y = _URTouch->getY();

      
		for (int i=0;i<countButtons;i++) // проверяем, есть ли попадание координат тача в координаты любой из кнопок
		{
			if (((buttons[i].flags & BUTTON_UNUSED) == 0) and ((buttons[i].flags & BUTTON_DISABLED) == 0) and ((buttons[i].flags & BUTTON_VISIBLE) ) and (result == -1))
			{
				if ((touch_x >= buttons[i].pos_x) and (touch_x <= (buttons[i].pos_x + buttons[i].width)) and (touch_y >= buttons[i].pos_y) and (touch_y <= (buttons[i].pos_y + buttons[i].height)))
       {
					result = i;
       }
			}
		}
		if (result != -1) // если кнопка найдена, то
		{
			if (!(buttons[result].flags & BUTTON_NO_BORDER)) // рисуем вокруг неё рамку
			{       
					_UTFT->drawRoundRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].width, buttons[result].height, 2, _color_hilite);
          yield();
			}
       if(pressed) // если есть обработчик нажатия - вызываем его
       {
        pressed(result);
       }
		}
   
		if (result != -1) // если кнопка найдена - ждём, пока тач не отпустят
		{
      while (_URTouch->TouchPressed()) 
      //while (_URTouch->dataAvailable() == true) 
      {
//        Ticker.tick(); 
        yield(); 
      }

      if(released) // если есть обработчик отпускания кнопки - вызываем его
      {
        released(result);
      }

			if (!(buttons[result].flags & BUTTON_NO_BORDER)) // если можно рисовать рамку на кнопке - снимаем рамку
			{

        uint16_t cl;
				if(buttons[result].flags & BUTTON_SELECTED)
       {
           cl = _color_hilite;
       }
				else
        {
				  cl = _color_border;
        }
         
				_UTFT->drawRoundRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].width, buttons[result].height,2, cl);

        yield();
			}
		}
		return result;
	}
	else
  {
		return -1;
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::setTextFont(FONTTYPE font) // устанавливаем шрифт текста по умолчанию
{
	_font_text = font;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::setSymbolFont(FONTTYPE font) // устанавливаем шрифт символов по умолчанию
{
	_font_symbol = font;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void TFT_Buttons_Rus::setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back) // устанавливаем цвета по умолчанию
{
	_color_text				= atxt;
	_color_text_inactive	= iatxt;
	_color_background		= back;
	_color_border			= brd;
	_color_hilite			= brdhi;
}
//--------------------------------------------------------------------------------------------------------------------------------------
