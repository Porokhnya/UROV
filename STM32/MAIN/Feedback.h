#pragma once
//--------------------------------------------------------------------------------------------------
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------
// Визуальная обрабтая связь
//--------------------------------------------------------------------------------------------------
class FeedbackList
{
  public:
    FeedbackList();
    
    void begin(); // начало работы

    void readyDiode(bool on=true); // установка уровня светодиода ГОТОВ
    void failureDiode(bool on=true); // установка уровня светодиода АВАРИЯ
    void testDiode(bool on=true); // установка уровня светодиода ТЕСТ

    void setFailureLineLevel(bool on=true); // установка уровня линии АВАРИЯ

  private:
};
//--------------------------------------------------------------------------------------------------
extern FeedbackList Feedback;
//--------------------------------------------------------------------------------------------------
