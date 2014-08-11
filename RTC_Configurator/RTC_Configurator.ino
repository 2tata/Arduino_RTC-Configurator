/*
 #***********************************************
 #  Dateiname:  ........  RTC-Configurator.ino  *
 #  Autor:  ..................  Jan-Tarek Butt  *
 #  Sprache:  .............................  C  *
 #  Dialekt:  .......................  Arduino  *
 #  Hardware:  ..................  Arduino Uno  *
 #  Datum:  .......................  26.6.2013  *
 #***********************************************
 */

//bibeliothek einbriden um I2c zu sprechen
#include "Wire.h"

//Pin Belegung Definieren
#define CL A1
#define VCC A2
#define GND A3

//adresse der RTC im I2c Bus
#define ADD 0x68

//Start adresse auf der RTC
#define STARTADD 0x00

void setup(){
  //Pins Initzialisiren
  pinMode(GND,OUTPUT);
  pinMode(VCC,OUTPUT);
  pinMode(CL,INPUT);
  digitalWrite(GND,0);
  digitalWrite(VCC,1);
  Wire.begin();
  Serial.begin(9600);

  Serial.println(" ");
  Serial.println("    Um die RTC zu Programmieren muessen nach folgendem Schema ");
  Serial.println("    die Daten eingegeben werden 'JJ.MM.mDmD.wDwD.hh.mm.ss'");
  Serial.println(" ");
  Serial.println("    z.B. 13061802150832 =           PIN belegung");
  Serial.println("    +--------------------+        +-----------+");
  Serial.println("    |Jahr= 13            |        |VCC=  A2   |");
  Serial.println("    |Monat= 06           |        |CL=   A1   |");
  Serial.println("    |Monatstage= 18      |        |SCl=  A5   |");
  Serial.println("    |Wochentage= 02      |        |SDa=  A4   |");
  Serial.println("    |Stunde= 15          |        |GND=  A3   |");
  Serial.println("    |Minute= 08          |        +-----------+");
  Serial.println("    |Sekunde= 32         |");
  Serial.println("    +--------------------+");
  Serial.println(" ");
  Serial.println("    Wenn die Eingabe Korrekt war wird ein ok wider gegeben!");
  Serial.println(" ");
  Serial.println("    Gib 'time' ein um die aktuelle zeit der RTC auszulesen.");
}

void loop(){
  //variablen definiren
  byte SEC = 0, MIN = 0, HOUR = 0, WEEKDAY = 0, MONTHDAY = 0, MONTH = 0, YEAR = 0;
  byte rSEC = 0, rMIN = 0, rHOUR = 0, rWEEKDAY = 0, rMONTHDAY = 0, rMONTH = 0, rYEAR = 0;

  delay(10);
  //Wird ausgefürt wenn der Arduino Seriel 14 zeichen empfängt.
  if(Serial.available()==14)
  {
    Serial.println("OK!");
    Serial.println(" ");

    //Serielle Decodirung
    Serial.println("Serielle Decodirung...");

    YEAR+=(Serial.read()-48)*10;
    YEAR+=(Serial.read()-48);

    Serial.print("YEAR: ");
    Serial.println(YEAR);

    MONTH+=(Serial.read()-48)*10;
    MONTH+=(Serial.read()-48);

    Serial.print("MONTH: ");
    Serial.println(MONTH);

    MONTHDAY+=(Serial.read()-48)*10;
    MONTHDAY+=(Serial.read()-48);

    Serial.print("MONTHDAY: ");
    Serial.println(MONTHDAY);

    WEEKDAY+=(Serial.read()-48)*10;
    WEEKDAY+=(Serial.read()-48);

    Serial.print("WEEKDAY: ");
    Serial.println(WEEKDAY);

    HOUR+=(Serial.read()-48)*10;
    HOUR+=(Serial.read()-48);

    Serial.print("HOUR: ");
    Serial.println(HOUR);

    MIN+=(Serial.read()-48)*10;
    MIN+=(Serial.read()-48);

    Serial.print("MIN: ");
    Serial.println(MIN);

    SEC+=(Serial.read()-48)*10;
    SEC+=(Serial.read()-48);

    Serial.print("SEC: ");
    Serial.println(SEC);
    Serial.println(" ");

    //RTC Programmieren
    Serial.println("i2c Transmission...");

    Wire.beginTransmission(ADD);
    Wire.write(STARTADD);

    Wire.write(BCD_encode(SEC));
    Serial.print("Transmission Sec: ");
    Serial.println(BCD_encode(SEC));

    Wire.write(BCD_encode(MIN));
    Serial.print("Transmission Min: ");
    Serial.println(BCD_encode(MIN));

    Wire.write(BCD_encode(HOUR));
    Serial.print("Transmission Hour: ");
    Serial.println(BCD_encode(HOUR));

    Wire.write(BCD_encode(WEEKDAY));
    Serial.print("Transmission WeekDay: ");
    Serial.println(BCD_encode(WEEKDAY));

    Wire.write(BCD_encode(MONTHDAY));
    Serial.print("Transmission MonthDay: ");
    Serial.println(BCD_encode(MONTHDAY));

    Wire.write(BCD_encode(MONTH));
    Serial.print("Transmission Month: ");
    Serial.println(BCD_encode(MONTH));

    Wire.write(BCD_encode(YEAR));
    Serial.print("Transmission Year: ");
    Serial.println(BCD_encode(YEAR));
    Serial.println(" ");

    Wire.write(B00010000);  //SQWE enablen

    Wire.write(STARTADD);
    Wire.endTransmission();

    //RTC Auslesen
    Serial.println("i2c.request...");
    byte auslesen = 0;
    while (true)
    {
      //Neu setzen der Register Startadresse
      Wire.beginTransmission(ADD);
      Wire.write(STARTADD);
      Wire.endTransmission();

      Wire.requestFrom(ADD, 7);

      rSEC = BCD_decode(Wire.read());
      rMIN = BCD_decode(Wire.read());
      rHOUR = BCD_decode(Wire.read() & 0b111111); //24 Stunden Zeit
      rWEEKDAY = BCD_decode(Wire.read()); //0-6 -> Sonntag - Samstag
      rMONTHDAY = BCD_decode(Wire.read());
      rMONTH = BCD_decode(Wire.read());
      rYEAR = BCD_decode(Wire.read());
      if (auslesen == 0)
      {
        if((SEC+MIN+HOUR+WEEKDAY+MONTHDAY+MONTH+YEAR)==(rSEC+rMIN+rHOUR+rWEEKDAY+rMONTHDAY+rMONTH+rYEAR))
        {
          Serial.println("Programmierunge der RTC wahr erfolgreich!");
          //Ausgabe:   Datum:DD/MM/JJ Zeit:HH:MM:SS
          Serial.print("Datum: ");
          Serial.print(rMONTHDAY);
          Serial.print("/");
          Serial.print(rMONTH);
          Serial.print("/");
          Serial.print(rYEAR);
          Serial.print(" Zeit: ");
          Serial.print(rHOUR);
          Serial.print(":");
          Serial.print(rMIN);
          Serial.print(":");
          Serial.println(rSEC);
          auslesen = 1;
        }
        else
        {
          Serial.println("Fehler bitte ueberpruefen sie die verkabelung!");
          break;
        }
      }
      else
      {
          Serial.print("Datum: ");
          Serial.print(rMONTHDAY);
          Serial.print("/");
          Serial.print(rMONTH);
          Serial.print("/");
          Serial.print(rYEAR);
          Serial.print(" Zeit: ");
          Serial.print(rHOUR);
          Serial.print(":");
          Serial.print(rMIN);
          Serial.print(":");
          Serial.println(rSEC);
      }
    }
  }
  else if(Serial.available()==4)
  {
    char time[8] = "";
    time[0] = Serial.read();
    time[1] = Serial.read();
    time[2] = Serial.read();
    time[3] = Serial.read();
    if (time[0] == 't' && time[1] == 'i' && time[2] == 'm' && time[3] == 'e')
    {
      byte auslesen = 0;
      while (true)
      {
        //Neu setzen der Register Startadresse
        Wire.beginTransmission(ADD);
        Wire.write(STARTADD);
        Wire.endTransmission();

        Wire.requestFrom(ADD, 7);

        rSEC = BCD_decode(Wire.read());
        rMIN = BCD_decode(Wire.read());
        rHOUR = BCD_decode(Wire.read() & 0b111111); //24 Stunden Zeit
        rWEEKDAY = BCD_decode(Wire.read()); //0-6 -> Sonntag - Samstag
        rMONTHDAY = BCD_decode(Wire.read());
        rMONTH = BCD_decode(Wire.read());
        rYEAR = BCD_decode(Wire.read());
        Serial.print("Datum: ");
        Serial.print(rMONTHDAY);
        Serial.print("/");
        Serial.print(rMONTH);
        Serial.print("/");
        Serial.print(rYEAR);
        Serial.print(" Zeit: ");
        Serial.print(rHOUR);
        Serial.print(":");
        Serial.print(rMIN);
        Serial.print(":");
        Serial.println(rSEC);
      }
    }
  }
}

// DEC zu BCD
byte BCD_encode(byte X)
{
  return ((X/10*16)+(X%10));
}

// BCD zu DEC
byte BCD_decode(byte X)
{ 
  return ((X/16*10)+(X%16));
}


