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
void FeedbackList::begin()
{
	/*
	 pinMode(STATUS_LINE,OUTPUT);
	 digitalWrite(STATUS_LINE, LOW);
	 alarm(false);*/

}
//--------------------------------------------------------------------------------------------------
void FeedbackList::alarm(bool on)
{
	// digitalWrite(STATUS_LINE, on ? STATUS_ALARM_LEVEL : !STATUS_ALARM_LEVEL);
}
//--------------------------------------------------------------------------------------------------

