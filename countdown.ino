#include <Wire.h>
#include <EEPROM.h>
#include "RTClib.h"

#define N_SEGMENTS 7
#define BASE_DIGITS 10
#define N_DIGITS 3
#define PULSE_MS 300
#define BETWEEN_DIGITS_MS 300
#define PERIOD_S 86400L
#define N_PER_PERIOD 10
#define EEPROM_BASE_ADDRESS 0

RTC_DS1307 rtc;
DateTime goal( 2014, 12, 31, 0, 0, 0);

boolean segments[BASE_DIGITS][N_SEGMENTS] = 
{{true,true,true,true,true,true,false},
{false,true,true,false,false,false,false},
{true,true,false,true,true,false,true},
{true,true,true,true,false,false,true},
{false,true,true,false,false,true,true},
{true,false,true,true,false,true,true},
{true,false,true,true,true,true,true},
{true,true,true,false,false,false,false},
{true,true,true,true,true,true,true},
{true,true,true,true,false,true,true}};

int address_pins[N_SEGMENTS] = {13,12,11,10,9,8,7};

void showDate(const char* txt, const DateTime& dt) {
    Serial.print(txt);
    Serial.print(' ');
    Serial.print(dt.year(), DEC);
    Serial.print('/');
    Serial.print(dt.month(), DEC);
    Serial.print('/');
    Serial.print(dt.day(), DEC);
    Serial.print(' ');
    Serial.print(dt.hour(), DEC);
    Serial.print(':');
    Serial.print(dt.minute(), DEC);
    Serial.print(':');
    Serial.print(dt.second(), DEC);
    
    Serial.print(" = ");
    Serial.print(dt.unixtime());
    Serial.print("s / ");
    Serial.print(dt.unixtime() / 86400L);
    Serial.print("d since 1970");
    
    Serial.println();
}

void clear_digit(){
  for (int k=0; k<N_SEGMENTS; k++){
      digitalWrite(address_pins[k], LOW);      
  }
}

void set_digit( int value){
  for (int k=0; k<N_SEGMENTS; k++){
    if(segments[value][k]){
      digitalWrite(address_pins[k], HIGH); 
      delay(PULSE_MS);
      digitalWrite(address_pins[k], LOW); 
    }
  }
}

void convert_to_digits(int n, int digits[N_DIGITS]){
  digits[2] = n/100;
  
  n = n - digits[2]*100;
  digits[1] = n/10;
  
  n = n - digits[1]*10;
  digits[0] = n;
}

void setup() {                
  Serial.begin(57600);
  Wire.begin();  
  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  for (int k=0; k< N_SEGMENTS; k++){
    pinMode(address_pins[k], OUTPUT);
  }
  
  
}



void loop() {
  DateTime now = rtc.now();
  //DateTime goal( 2014, 12, 31, 0, 0, 0);

  int days_diff = goal.unixtime()/PERIOD_S - now.unixtime()/ PERIOD_S;
  int days_digits[N_DIGITS];
  convert_to_digits( days_diff, days_digits);

  for (int k=0; k< N_DIGITS; k++){
    //set_digit_address(k);
    clear_digit();
    set_digit( days_digits[k]);
    delay(BETWEEN_DIGITS_MS);
  }
   
  delay(PERIOD_S / (N_PER_PERIOD *1.0) * 1000);
}
