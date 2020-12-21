//--------------------------------------------------------------------------------------------------
#include "Feedback.h"
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------
FeedbackList Feedback;
//--------------------------------------------------------------------------------------------------
FeedbackList::FeedbackList()
{
  
}
//--------------------------------------------------------------------------------------------------
void FeedbackList::begin() // настраиваем визуальную обратную связь
{
  pinMode(LED_READY, OUTPUT);
  pinMode(LED_FAILURE, OUTPUT);
  pinMode(LED_TEST, OUTPUT);

  pinMode(STATUS_LINE,OUTPUT);
  digitalWrite(STATUS_LINE, LOW);
  setFailureLineLevel(false);

  readyDiode(false);
  failureDiode(false);
  testDiode(false);
}
//--------------------------------------------------------------------------------------------------
void FeedbackList::setFailureLineLevel(bool on) // установка уровня на линии АВАРИЯ
{
  digitalWrite(STATUS_LINE, on ? STATUS_ALARM_LEVEL : !STATUS_ALARM_LEVEL);
}
//--------------------------------------------------------------------------------------------------
void FeedbackList::readyDiode(bool on) // установка уровня светодиода ГОТОВ
{
  digitalWrite(LED_READY, on ? LED_ON_LEVEL : !LED_ON_LEVEL);
}
//--------------------------------------------------------------------------------------------------
void FeedbackList::failureDiode(bool on) // установка уровня светодиода АВАРИЯ
{
  digitalWrite(LED_FAILURE, on ? LED_ON_LEVEL : !LED_ON_LEVEL);
}
//--------------------------------------------------------------------------------------------------
void FeedbackList::testDiode(bool on) // установка уровня светодиода ТЕСТ
{
  digitalWrite(LED_TEST, on ? LED_ON_LEVEL : !LED_ON_LEVEL);
}
//--------------------------------------------------------------------------------------------------
