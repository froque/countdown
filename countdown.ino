#include <Wire.h>
#include <EEPROM.h>
#include "RTClib.h"
#include "DHT.h"

#define N_SEGMENTS 7
#define BASE_DIGITS 10
#define N_DIGITS 3
#define PULSE_MS 200
#define BETWEEN_DIGITS_MS 300
//#define PERIOD_S 86400L // One day
//#define PERIOD_S 3600L  // One hour
#define PERIOD_S 60L      // One minute
#define N_PER_PERIOD 10
#define EEPROM_BASE_ADDRESS 0
#define DHTPIN 4
#define DHTTYPE DHT11

//DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;
DateTime goal( 2014, 1, 3, 0, 0, 0);
int previous_days_diff = -1;

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

//int address_segments[N_SEGMENTS] = {6,7,8,9,10,11,12};
int address_segments[N_SEGMENTS] = {12, 11, 10, 9, 8, 7, 6};
int reset_pin = 2;
int address_digits[N_DIGITS] = {3,4,5};

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

void clear_segments(){
  /*for (int k=0; k<N_SEGMENTS; k++){
      digitalWrite(address_segments[k], LOW);      
  }*/
  digitalWrite(reset_pin, HIGH);
  delay(PULSE_MS);
  digitalWrite(reset_pin, LOW);
}

void set_segments( int value){
  for (int k=0; k<N_SEGMENTS; k++){
    if(segments[value][k]){
      digitalWrite(address_segments[k], HIGH); 
      delay(PULSE_MS);
      digitalWrite(address_segments[k], LOW); 
    }
  }
}

void set_digit(int k, int value){
  digitalWrite(address_digits[k], HIGH);
  clear_segments();
  set_segments(value);
  digitalWrite(address_digits[k], LOW);
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
  //dht.begin();
  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
  for (int k=0; k< N_SEGMENTS; k++){
    pinMode(address_segments[k], OUTPUT);
  }
  
  pinMode(reset_pin, OUTPUT);
  
  for (int k=0; k< N_DIGITS; k++){
    pinMode(address_digits[k], OUTPUT);
  }
  
}


void loop() {
/*  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  }
  */
  
  DateTime now = rtc.now();

  int days_diff = goal.unixtime()/PERIOD_S - now.unixtime()/ PERIOD_S;
  
  
  Serial.println(days_diff);
  Serial.println(previous_days_diff);
  if( days_diff < previous_days_diff || previous_days_diff == -1){
    
    previous_days_diff = days_diff;
    
    int days_digits[N_DIGITS];
    convert_to_digits( days_diff, days_digits);
  
    for (int k=0; k< N_DIGITS; k++){
      /*
      digitalWrite(address_digits[k], HIGH);
      clear_digit();
      set_digit( days_digits[k]);
      delay(BETWEEN_DIGITS_MS);
      digitalWrite(address_digits[k], LOW);
      */
      set_digit(k,days_digits[k]);
    }
    
  }
  
  delay(PERIOD_S / (N_PER_PERIOD *1.0) * 1000);
// delay(2000);
}
