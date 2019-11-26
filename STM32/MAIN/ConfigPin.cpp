#include "ConfigPin.h"
#include "CONFIG.h"
#include <Arduino.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ConfigPin::ConfigPin()
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ConfigPin::setI2CPriority(uint8_t priority)
{
//TODO: Закомментировал!!!
/*
  NVIC_SetPriorityGrouping(NVIC_PriorityGroup_1);
  NVIC_DisableIRQ(WIRE_ISR_ID);
  NVIC_ClearPendingIRQ(WIRE_ISR_ID);
  NVIC_SetPriority(WIRE_ISR_ID, priority);
  NVIC_EnableIRQ(WIRE_ISR_ID);

#if defined (__arm__) && defined (__SAM3X8E__) 
  NVIC_DisableIRQ(WIRE1_ISR_ID);
  NVIC_ClearPendingIRQ(WIRE1_ISR_ID);
  NVIC_SetPriority(WIRE1_ISR_ID, priority);
  NVIC_EnableIRQ(WIRE1_ISR_ID);
#endif  
*/
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ConfigPin::setup()
{
  
	pinMode(out_asu_tp1, OUTPUT);                       // Выход на АСУ ТП №1
	digitalWrite(out_asu_tp1, LOW);                     // Выход на АСУ ТП №1
	pinMode(out_asu_tp2, OUTPUT);                       // Выход на АСУ ТП №2
	digitalWrite(out_asu_tp2, LOW);                     // Выход на АСУ ТП №2
	pinMode(out_asu_tp3, OUTPUT);                       // Выход на АСУ ТП №3
	digitalWrite(out_asu_tp3, LOW);                     // Выход на АСУ ТП №3
	pinMode(out_asu_tp4, OUTPUT);                       // Выход на АСУ ТП №4
	digitalWrite(out_asu_tp4, LOW);                     // Выход на АСУ ТП №4


	pinMode(Upr_RS485, OUTPUT);                         // Выход управлениея шиной RS485  
	digitalWrite(Upr_RS485, LOW);                       // Выход управлениея шиной RS485  

													// CAN шина
	pinMode(ID0_Out, OUTPUT);                           // Выход управлениея CAN шиной   
	digitalWrite(ID0_Out, LOW);                         // Выход управлениея CAN шиной   
	pinMode(ID1_Out, OUTPUT);                           // Выход управлениея CAN шиной   
	digitalWrite(ID1_Out, LOW);                         // Выход управлениея CAN шиной   

	pinMode(IDE0_In, INPUT);                            // Вход управлениея CAN шиной   
	digitalWrite(IDE0_In, INPUT_PULLUP);                // Вход управлениея CAN шиной, подключить резисторы
	pinMode(IDE1_In, INPUT);                            // Вход управлениея CAN шиной   
	digitalWrite(IDE1_In, INPUT_PULLUP);                // Вход управлениея CAN шиной, подключить резисторы

	pinMode(PPS_Out, OUTPUT);                           // Выход управлениея CAN шиной   
	digitalWrite(PPS_Out, LOW);                         // Выход управлениея CAN шиной   

	pinMode(TFT_reset, OUTPUT);                         // Выход pin сброса TFT дисплея.  
	digitalWrite(TFT_reset, HIGH);                      // Выход pin сброса TFT дисплея. 
	delay(10);
	digitalWrite(TFT_reset, LOW);                      // Сброс TFT дисплея. 
	delay(10);
	digitalWrite(TFT_reset, HIGH);                      // Выход pin сброса TFT дисплея. 


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

