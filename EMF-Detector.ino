#include <EEPROM.h>

#define inputPinR A0
#define buzzer 9
#define LED1 A2
#define LED2 A3
#define LED3 A4
#define LED4 A5
#define LED5 2
#define btn 3

int peak = 0;
volatile int totalR = 0;
volatile int avg = 0;                       
int curFreq = 0;

int ledConfig[5] = { 60,52,45,38,20 };
int nMeasures[5] = { 400, 300, 200, 150, 50 }; 
int ledcfg = 0;

void setup()
{ 
  Serial.begin(115200);
  pinMode(inputPinR, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  //LEDS
  pinMode(LED5, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED1, OUTPUT);
  //TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
  //TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
  TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz
  //TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 divisor to   256 for PWM frequency of   122.55 Hz
  //TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz
  totalR = analogRead(inputPinR);
  curFreq = totalR;
  ledsOff();

  //READ CONFIG
  ledcfg = EEPROM.read(0);

  //SHOW CURR CONFIG
  ledShowConf(ledcfg);
  delay(500);
  ledsOff();
  delay(500);
  ledShowConf(ledcfg);
  delay(500);
  ledsOff();
}

void loop()
{ 
  if(digitalRead(btn) == 0)
  {
    analogWrite(buzzer, 0);
    ledsOff();
    if(ledcfg >= 0 && ledcfg < 4){
      ledcfg++;
      EEPROM.write(0, ledcfg);
    }else{
      ledcfg = 0;
      EEPROM.write(0, ledcfg);
    }
    ledShowConf(ledcfg);
    Serial.println(ledConfig[ledcfg]);
    delay(1000);
    ledsOff();
  }

  for(int i = 0; i < nMeasures[ledcfg]; i++){
    totalR += analogRead(inputPinR);
    delayMicroseconds(10);
  }
  totalR = totalR / nMeasures[ledcfg];
  totalR = constrain(totalR, 0, 100);
  totalR = map(totalR, 0, 50, 0, 255);  
  if(totalR >= 1){    
    freqSoft(1); 
    ledmap(); 
  }else if(totalR <= 0){
    ledsOff();                                                 
    analogWrite(buzzer, 0);                                                                            
  }
  totalR = 0;
}


void ledShowConf(int num){
  switch (num){
    case 0:
      digitalWrite(LED5, LOW);
      digitalWrite(LED4, LOW);
      digitalWrite(LED3, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED1, HIGH);
      break;
    case 1:
      digitalWrite(LED5, LOW);
      digitalWrite(LED4, LOW);
      digitalWrite(LED3, LOW);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED1, HIGH);
      break;
    case 2:
      digitalWrite(LED5, LOW);
      digitalWrite(LED4, LOW);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED1, HIGH);
      break;
    case 3:
      digitalWrite(LED5, LOW);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED1, HIGH);
      break;
    case 4:
      digitalWrite(LED5, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED1, HIGH);
      break;
  }
}

void ledmap(){
  int tmp = ledConfig[ledcfg];
  int nLed = map(totalR,0, tmp , 0, 5);
  if(nLed == 0){
    digitalWrite(LED5, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED1, LOW);
  }else if(nLed == 1){
    digitalWrite(LED5, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED1, HIGH);
  }else if(nLed == 2){
    digitalWrite(LED5, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED1, HIGH);
  }else if(nLed == 3){
    digitalWrite(LED5, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED1, HIGH);
  }else if(nLed == 4){
    digitalWrite(LED5, LOW);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED1, HIGH);
  }else if(nLed == 5){
    digitalWrite(LED5, HIGH);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED1, HIGH);
  }
}

void ledsOff(){
  digitalWrite(LED5, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, LOW);
}
void ledsOn(){
  digitalWrite(LED5, HIGH);
  digitalWrite(LED4, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED1, HIGH);
}


void freqSoft(int step){
  if(totalR > curFreq){
    for(; totalR > curFreq; curFreq += step){
      analogWrite(buzzer, curFreq);
      delay(2);
    }
  }else if(totalR < curFreq){
    for(; totalR < curFreq; curFreq -= step){
      analogWrite(buzzer, curFreq);
      delay(2);
    }
  }
}
