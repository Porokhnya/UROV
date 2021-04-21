//--------------------------------------------------------------------------------------------------------------------------------------
// настройки прошивки
//--------------------------------------------------------------------------------------------------------------------------------------
#define MASTER_SERIAL   Serial    // UART, с которого принимаем запросы мастера (USB с ПК, например)
#define SLAVE_SERIAL     Serial3   // UART, куда ретранслируем запросы мастера, и откуда принимаем ответы, передавая их мастеру
#define SERIAL_SPEED  57600     // скорость работы обеих UART
#define SWITCH_PIN    PE1         // номер пина управления приёмом/передачей по RS-485
//--------------------------------------------------------------------------------------------------------------------------------------
bool inSendMode = false;
/*
uint8_t* _msg = NULL; // принятый запрос от мастера
uint32_t _len = 0; // длина запроса, принятого от мастера
uint32_t u32time = 0; // таймер
uint32_t T3_5 = 0;
typedef enum
{
  msWaitMasterQuery,  // ждём запроса от мастера
  msWaitSlaveResponse, // ждём ответа от слейва
  
} MachineState;

MachineState machineState = msWaitMasterQuery;
//--------------------------------------------------------------------------------------------------------------------------------------
void setT3_5()  // высчитываем длительность паузы после окончания фрейма MODBUS
{
  T3_5 = 24000/SERIAL_SPEED;
  
  if (SERIAL_SPEED > 19200)
  {
    T3_5 = 1750; 
  }
  else 
  {
    T3_5 = 35000000/SERIAL_SPEED; 
  }  
}
*/
//--------------------------------------------------------------------------------------------------------------------------------------
void switchToSend() // переводим контроллер RS-485 на передачу
{
    if(inSendMode)
      return;

     inSendMode = true;
     
     digitalWrite(SWITCH_PIN,HIGH); 
}
//--------------------------------------------------------------------------------------------------------------------------------------
void switchToReceive() // переводим контроллер RS-485 на приём
{  
  if(!inSendMode)
    return;

  inSendMode = false;
  
    digitalWrite(SWITCH_PIN,LOW); 
}
//--------------------------------------------------------------------------------------------------------------------------------------
/*
void serialRead(Stream* port)
{
  if(port->available())
  {
    uint8_t b = port->read();
    uint8_t* _new = (uint8_t*) malloc(_len + 1);
    
    if(_msg)
    {
      memcpy(_new,_msg,_len);
      free(_msg);
    }
    
    _new[_len] = b;
    _msg = _new;
    _len++;
    
    u32time = micros();
    
  } // if(available)

}
*/
//--------------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  /*
    setT3_5(); // высчитываем длительность паузы в 3.5 символа, в зависимости от скорости соединения. Эта пауза означает конец передачи фрейма по MODBUS
*/
    // настраиваем пин переключения приём/передача
    pinMode(SWITCH_PIN,OUTPUT);
    switchToSend(); // в режим мастера

    // поднимаем UART
    MASTER_SERIAL.begin(SERIAL_SPEED);
    SLAVE_SERIAL.begin(SERIAL_SPEED);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void loop()
{

  // от слейва к мастеру
  
  switchToReceive();
  while(SLAVE_SERIAL.available())
  {
    MASTER_SERIAL.write(SLAVE_SERIAL.read());
  }
  

  // от мастера к слейву
  while(MASTER_SERIAL.available())
  {
    switchToSend();  
    SLAVE_SERIAL.write(MASTER_SERIAL.read());
  }


  
  
  /*
    switch(machineState) // в какой ветке конечного автомата находимся?
    {
        case msWaitMasterQuery: // ждём запроса от мастера
        {
            serialRead(&SERIAL_FROM); // читаем запрос от мастера

            if ((micros() - u32time) < T3_5) // ещё не поймали паузу между фреймом
            {
              return;
            }
            
            if(_len > 0) // имеем фрейм от мастера, надо его переслать слейву
            {
                switchToSend(); // переключаемся на передачу
    
                // пишем слейву
                SERIAL_TO.write(_msg,_len);
                SERIAL_TO.flush();
    
                free(_msg);
                _msg = NULL;
                _len = 0;
    
                switchToReceive(); // переключаемся на приём
    
                machineState = msWaitSlaveResponse; // ждём ответа от слейва

                delay(10); // дадим слейву 10 миллисекунд на подготовку ответа
                
                u32time = micros();
                
            } // if(_len > 0)
            
        }
        break; // msWaitMasterQuery

        case msWaitSlaveResponse: // ждём ответа от слейва
        {
            serialRead(&SERIAL_TO); // читаем ответ от слейва

            if ((micros() - u32time) < T3_5) // ещё не поймали паузу между фреймом
            {
              return;
            }

            if(_len > 0) // есть ответ от слейва, надо переслать его мастеру
            {
                // пишем мастеру ответ от слейва
                SERIAL_FROM.write(_msg,_len);
                SERIAL_FROM.flush();

                free(_msg);
                _msg = NULL;
                _len = 0;
                
            } // if(_len > 0)

            switchToSend(); // переключаемся на передачу
            machineState = msWaitMasterQuery; // ждём запроса от мастера
            u32time = micros();
        }
        break;
      
    } // switch
    */
}
//--------------------------------------------------------------------------------------------------------------------------------------

