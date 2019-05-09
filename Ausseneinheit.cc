char                 databuffer[35];
double               temp;

//#define Wetterpin 8
#include <ELECHOUSE_CC1101.h>


void getBuffer()                                                                    //Get weather status data
{
  int index;
  for (index = 0;index < 35;index ++)
  {
    if(Serial.available())
    {
      databuffer[index] = Serial.read();
      if (databuffer[0] != 'c')
      {
        index = -1;
      }
    }
    else
    {
      index --;
    }
  }
}

int transCharToInt(char *_buffer,int _start,int _stop)                               //char to int）
{
  int _index;
  int result = 0;
  int num = _stop - _start + 1;
  int _temp[num];
  for (_index = _start;_index <= _stop;_index ++)
  {
    _temp[_index - _start] = _buffer[_index] - '0';
    result = 10*result + _temp[_index - _start];
  }
  return result;
}

int WindDirection()                                                                  //Wind Direction
{
  return transCharToInt(databuffer,1,3);
}

float WindSpeedAverage()                                                             //air Speed (1 minute)
{
  temp = 0.44704 * transCharToInt(databuffer,5,7);
  return temp;
}

float WindSpeedMax()                                                                 //Max air speed (5 minutes)
{
  temp = 0.44704 * transCharToInt(databuffer,9,11);
  return temp;
}

float Temperature()                                                                  //Temperature ("C")
{
  temp = (transCharToInt(databuffer,13,15) - 32.00) * 5.00 / 9.00;
  return temp;
}

float RainfallOneHour()                                                              //Rainfall (1 hour)
{
  temp = transCharToInt(databuffer,17,19) * 25.40 * 0.01;
  return temp;
}

float RainfallOneDay()                                                               //Rainfall (24 hours)
{
  temp = transCharToInt(databuffer,21,23) * 25.40 * 0.01;
  return temp;
}

int Humidity()                                                                       //Humidity
{
  return transCharToInt(databuffer,25,26);
}

float BarPressure()                                                                  //Barometric Pressure
{
  temp = transCharToInt(databuffer,28,32);
  return temp / 10.00;
}

void setup()
{
 for (int i = 0; i < 20; i++) {
    if(i != 2)//just because the button is hooked up to digital pin 2
    pinMode(i, OUTPUT);
  }

  pinMode(0, INPUT);
  pinMode(1, INPUT);
 // pinMode(Wetterpin, OUTPUT);
  Serial.begin(9600);
  
  ELECHOUSE_cc1101.Init(F_433);

  
  //SETUP WATCHDOG TIMER
  WDTCSR = (24);//change enable and WDE - also resets
  WDTCSR = (33);//prescalers only - get rid of the WDE and WDCE bit
  WDTCSR |= (1<<6);//enable interrupt mode
  
  //Disable ADC - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
  ADCSRA &= ~(1 << 7);
  
  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep

  
  //BOD DISABLE - this must be called right before the __asm__ sleep instruction
  MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
  __asm__  __volatile__("sleep");//in line assembler to go to sleep
}
void loop()
{  
   getBuffer();                                                                      //Begin!
  String tx_weather = "{\"t\":" +String(Temperature())+"," + "\"h\":" +String(Humidity())+"," + "\"p\":" +String(BarPressure())+"," +"\"r\":" +String(RainfallOneHour())+"," +"\"w\":" +String(WindSpeedAverage())+"," +"\"d\":" +String(WindDirection())+"}\n";
  int m_lengthweather = tx_weather.length();
  byte txbyteweather[m_lengthweather];
  tx_weather.getBytes(txbyteweather, m_lengthweather +1);
  Serial.println((char *)txbyteweather);
  Serial.println(m_lengthweather);
  ELECHOUSE_cc1101.SendData(txbyteweather, m_lengthweather);
  delay(12999);
  
  //BOD DISABLE - this must be called right before the __asm__ sleep instruction
  MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
  __asm__  __volatile__("sleep");//in line assembler to go to sleep
}


ISR(WDT_vect){


  //DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up
}// watchdog interrupt

