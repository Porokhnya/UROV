//--------------------------------------------------------------------------------------------------------------------------------------
#define ENCODER_PIN1 PC4 // номер пина А энкодера
#define ENCODER_PIN2 PC2 // номер пина В энкодера
#define SERIAL_SPEED 57600 // скорость работы Serial
//--------------------------------------------------------------------------------------------------------------------------------------
volatile uint8_t aFlag = 0;
volatile uint8_t bFlag = 0;
volatile uint8_t rotationDirection = 0xFF;
volatile uint8_t transitionState = 0; // таблица переходов энкодера
volatile uint8_t lastKnownDirection = 0xFF;     // последнее известное направление движения штанги
volatile bool rotationChanged = false;
volatile uint8_t catchedDirection = 0xFF;
#define CW 1
#define CCW 2
//--------------------------------------------------------------------------------------------------------------------------------------
void saveTransitionState()
{
  uint8_t aState = digitalRead(ENCODER_PIN1);
  uint8_t bState = digitalRead(ENCODER_PIN2);

  transitionState <<= 1;
  transitionState |= aState;
  transitionState <<= 1;
  transitionState |= bState;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void handleDirection()
{
    if(transitionState == 11 || transitionState == 14)
    {
      //CW!
      rotationDirection = CW; // clockwise
    }
    else if(transitionState == 7 || transitionState == 13)
    {
      //CCW!
      rotationDirection = CCW; // counter-clockwise
    }
    else
    {
      //UNKNOWN!
      rotationDirection = 0xFF;
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void pinA()
{
  noInterrupts();
  aFlag = 1;
  saveTransitionState();

  if(bFlag)
  {
    aFlag = 0;
    bFlag = 0;
    handleDirection();
    transitionState = 0;

    if(rotationDirection != 0xFF && rotationDirection != lastKnownDirection)
    {
      rotationChanged = true;
      catchedDirection = rotationDirection;
      lastKnownDirection = rotationDirection;
    }
    
  }
  interrupts();  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void pinB()
{
  noInterrupts();
  bFlag = 1;
  saveTransitionState();

  if(aFlag)
  {
    aFlag = 0;
    bFlag = 0;
    handleDirection();
    transitionState = 0;
  }  
  interrupts();  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(SERIAL_SPEED);
  Serial.println("Init...");

  attachInterrupt((ENCODER_PIN1),pinA, RISING);
  attachInterrupt((ENCODER_PIN2),pinB, RISING);
  
  Serial.println("Ready.");
}
//--------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{
  noInterrupts();
  uint8_t changes = rotationChanged;
  rotationChanged = false;
  uint8_t dir = catchedDirection;
  interrupts();

  if(changes)
  {
    switch(dir)
    {
      case 0xFF: Serial.println("UNKNOWN DIRECTION STATE"); break;
      case CW: Serial.println("CLOCKWISE ROTATION"); break;
      case CCW: Serial.println("COUNTER-CLOCKWISE ROTATION"); break;
    }
  }

}
//--------------------------------------------------------------------------------------------------------------------------------------

