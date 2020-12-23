#pragma once

// класс обработки экранных кнопок

#include <Arduino.h>
#include "TFTRus.h"
#include "TFT_Includes.h"

#define MAX_BUTTONS	25	// Maximum number of buttons available at one time

// флаги состояний кнопок

#define BUTTON_DISABLED			1   // выключена
#define BUTTON_SYMBOL			2   // символ
#define BUTTON_HAS_ICON   4   // имеет иконку
//#define BUTTON_SYMBOL_REP_3X	4
//#define BUTTON_BITMAP			8	
#define BUTTON_NO_BORDER		16  // без границы
#define BUTTON_HAS_BACK_COLOR 32  // имеет фоновый цвет
#define BUTTON_HAS_FONT_COLOR 64  // имеет цвет шрифта
#define BUTTON_VISIBLE 128  // видима
#define BUTTON_SELECTED 256 // выбрана
#define BUTTON_HAS_FONT 512 // имеет шрифт
#define BUTTON_UNUSED      0x8000 // не используется


#pragma pack(push,1)
typedef struct
{
  int16_t			pos_x, pos_y, width, height;
  uint16_t backColor, fontColor;
  uint16_t			flags;
  const char				*label;
  FONTTYPE font;
  
} button_type;
#pragma pack(pop)

typedef void (*DrawButtonsUpdateFunc)(void);
typedef void (*OnCheckButtonsFunc)(int button);

class TFT_Buttons_Rus
{
	public:
		TFT_Buttons_Rus(TFT_Class *ptrUTFT, TOUCH_Class *ptrURTouch, TFTRus* pTFTRus, int count_btns = MAX_BUTTONS);
    ~TFT_Buttons_Rus();

		int		addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char *label, uint16_t flags=0); // добавляет кнопку
		void	drawButtons(DrawButtonsUpdateFunc func=NULL); // рисует кнопки на экране
		void	drawButton(int buttonID); // рисует кнопку
		void	enableButton(int buttonID, boolean redraw=false); // включает кнопку
		void	disableButton(int buttonID, boolean redraw=false); // выключает кнопку
		void	relabelButton(int buttonID, const char *label, boolean redraw=false); // меняет текст на кнопке
		boolean	buttonEnabled(int buttonID); // проверяет, включена ли кнопка
		void	deleteButton(int buttonID); // удаляет кнопку
		void	deleteAllButtons(); // удаляет все кнопки
		int		checkButtons(OnCheckButtonsFunc pressed = NULL, OnCheckButtonsFunc released = NULL); // проверяет кнопки на нажатие
		void	setTextFont(FONTTYPE font); // устанавливает шрифт текста для всех кнопок
		void	setSymbolFont(FONTTYPE font); // устанавливает шрифт символов для всех кнопок
		void	setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back); // устанавливает цвета для всех кнопок
    void selectButton(int buttonID, bool selected, boolean redraw=false); // выделяет кнопку
    
		word getButtonBackColor(int buttonID); // возвращает фоновый цвет кнопки
		void 	setButtonBackColor(int buttonID, word color); // устанавливает фоновый цвет кнопки

   void setButtonFont(int buttonID, FONTTYPE font); // устанавливает шрифт кнопки
		word getButtonFontColor(int buttonID); // возвращает цвет шрифта кнопки
		void 	setButtonFontColor(int buttonID, word color); // устанавливает цвет шрифта кнопки
   
    void showButton(int buttonID, boolean redraw=false); // показывает кнопку
    void hideButton(int buttonID, boolean redraw=false); // скрывает кнопку
    const char* getLabel(int buttonID); // возвращает текст кнопки

    void setIconFont(FONTTYPE font); // устанавливает шрифт иконки кнопки
    void setButtonHasIcon(int buttonID); // устанавливает флаг, что первый символ текста кнопки - это иконка

	protected:
		TFT_Class		*_UTFT;
		TOUCH_Class		*_URTouch;

    TFTRus* pRusPrinter;
    
		button_type*	buttons;
    int countButtons;
    
		word		_color_text, _color_text_inactive, _color_background, _color_border, _color_hilite;
		FONTTYPE	_font_text;
		FONTTYPE _font_symbol;
    FONTTYPE _font_icon;
};
