char                 databuffer[35];                                                //Datenbuffer hinzugefügt
double               temp;                                                          //Variable für Temperatur deklarieren
#include <ELECHOUSE_CC1101.h>                                                       //Libary für Funkmodul einbinden


void getBuffer()                                                                                                  
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

int transCharToInt(char *_buffer,int _start,int _stop)                               //Konvertiert char zu int
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

int WindDirection()                                                                  //Windvariable erstellen
{
  return transCharToInt(databuffer,1,3);
}

float WindSpeedAverage()                                                             //Windgeschwindigkeitsvariable erstellen
{
  temp = 0.44704 * transCharToInt(databuffer,5,7);
  return temp;
}

float WindSpeedMax()                                                                 //Windgeschwindigkeitsmaximumvariable erstellen
{
  temp = 0.44704 * transCharToInt(databuffer,9,11);
  return temp;
}

float Temperature()                                                                  //Temperaturvariable erstellen
{
  temp = (transCharToInt(databuffer,13,15) - 32.00) * 5.00 / 9.00;
  return temp;
}

float RainfallOneHour()                                                              //1h Regenmengevariable erstellen
{
  temp = transCharToInt(databuffer,17,19) * 25.40 * 0.01;
  return temp;
}

float RainfallOneDay()                                                               //24h Regenmengevariable erstellen
{
  temp = transCharToInt(databuffer,21,23) * 25.40 * 0.01;
  return temp;
}

int Humidity()                                                                       //Luftfeuchtigskeitvariable erstellen
{
  return transCharToInt(databuffer,25,26);
}

float BarPressure()                                                                  //Luftdruckvariable erstellen
{
  temp = transCharToInt(databuffer,28,32);
  return temp / 10.00;
}

void setup()
{
 for (int i = 0; i < 20; i++) {                                                     //Pins am ATMega deklarieren, um Strom zu sparen
    if(i != 2)                                                                    
    pinMode(i, OUTPUT);
  }

  pinMode(0, INPUT);                                                                //RX Pin als Eingang deklarieren
  pinMode(1, INPUT);                                                                //TX Pin als Eingang deklarieren
  Serial.begin(9600);                                                               //Serial Monitor starten
  
  ELECHOUSE_cc1101.Init(F_433);                                                     //Funkmodul intialisieren

  
  //Watchdog Timer aufsetzten
  WDTCSR = (24);
  WDTCSR = (33);
  WDTCSR |= (1<<6);                                                                //Aktiviert Interupt Modus
  
  
  ADCSRA &= ~(1 << 7);                                                             //Deaktiviert analog digital converter
  
  //Aktiviert Schlafmodus
  SMCR |= (1 << 2);                                                                //Power down Modus auswählen
  SMCR |= 1;                                                                       //Aktiviere Schlafmodus

  
  //BrownOut detection deaktivieren
  MCUCR |= (3 << 5);                                                               //BrownOut detection deaktivieren
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6);                                          //
  __asm__  __volatile__("sleep");                                                  //in line assembler um schlafen zu gehen
}
void loop()
{  
   getBuffer();                                                                    //Ausgelesene Daten zu einem String zusammenfügen
  String tx_weather = "{\"t\":" +String(Temperature())+"," + "\"h\":" +String(Humidity())+"," + "\"p\":" +String(BarPressure())+"," +"\"r\":" +String(RainfallOneHour())+"," +"\"w\":" +String(WindSpeedAverage())+"," +"\"d\":" +String(WindDirection())+"}\n";
  int m_lengthweather = tx_weather.length();                                       //Stringlänge erkennen
  byte txbyteweather[m_lengthweather];
  tx_weather.getBytes(txbyteweather, m_lengthweather +1);                          
  Serial.println((char *)txbyteweather);                                           //Zwischenwerte im Serial Monitor anzeigen
  Serial.println(m_lengthweather);                                                 //
  ELECHOUSE_cc1101.SendData(txbyteweather, m_lengthweather);                       //String über Funkmodul senden
  delay(12999);                                                                    //Pause
  
  
  MCUCR |= (3 << 5);                                                               //BrownOut detection deaktivieren
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); 
  __asm__  __volatile__("sleep");                                                  //in line assembler um schlafen zu gehen
}


ISR(WDT_vect){                                                                     //Interrupt um Arduino kurzzeitig aufzuwecken


  
}

