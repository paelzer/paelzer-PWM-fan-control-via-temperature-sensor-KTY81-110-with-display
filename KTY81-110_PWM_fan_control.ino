/*

    7 Segment Display related

*/

// pin defines for 7 segment Display
#define dig_1 2 // digit 1 pin
#define dig_2 3 // digit 2 pin
#define dig_3 4 // digit 3 pin
#define dig_4 5 // digit 4 pin

#define degree  10
#define celsius 11

// pin defines for the Arduino Nano to control the 74HC595
#define resetPin  12 // MR    
#define clockPin   6 // SH_CP
#define storePin   7 // ST_CP
#define dataPin    8 // DS


#define segmentsOff  B11111111 // pattern to turn off all segments (common cathode display)

#define sensorPin A1

// integers to take the 2 temperature digits for the display
int ones = 0, tens = 0;

// integer to increment measurments
int i = 0;

//define digits for 7 segm display
char digit[12] = {
  B00010100, // 0
  B11110101, // 1
  B00011001, // 2
  B01010001, // 3
  B11110000, // 4
  B01010010, // 5
  B00010010, // 6
  B01110101, // 7
  B00010000, // 8
  B01010000, // 9
  B01111000, // °
  B00011110  // C
};


/*

    PWM fan control related

*/

// resistor value of voltage divider in ohm
// 3.24
// 977/978

float resistor = 3275;

#define fan1Pin 9
#define fan2Pin 10

#define snail  100
#define slow   130
#define fast   200

#define interval 1000
unsigned long waitUntil = 0;

uint8_t fanspeed = 0;
uint8_t fanspeed_new = 0;


void setup() {

  pinMode(resetPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(storePin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(dig_1, OUTPUT);
  pinMode(dig_2, OUTPUT);
  pinMode(dig_3, OUTPUT);
  pinMode(dig_4, OUTPUT);

  digitalWrite(resetPin, HIGH);


  //setup timer1 registers for fast PWM and no timer clock prescaler
  TCCR1B = TCCR1B & 0b11100000 | 0b00001001;

  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}



void loop() {

  if ((unsigned long)(millis() - waitUntil) >= interval) {  // check for rollover

    // temperature float value rounded to an integer
    intTemp = 0;    
    
    // Carry out 20 measurements  
    for(i=0; i<20; i++)
    {
        float temp = kty(sensorPin);
        intTemp += (int)round(temp);
    }
    
    // take the average measurement value
    intTemp = intTemp /20;

    ones = (intTemp % 10);
    tens = ((intTemp / 10) % 10);

    Serial.println("Temperature: ");
    Serial.println(temp);

    if (temp < 40)
    {
      fanspeed_new = snail;
    }
    else if (temp >= 40)
    {
      fanspeed_new = slow;
    }
    else if (temp >= 50)
    {
      fanspeed_new = fast;
    }

    if (fanspeed_new != fanspeed)
    {
      spinFan(fanspeed_new);
      fanspeed = fanspeed_new;
    }

    waitUntil = waitUntil + interval;  // wait another interval cycle
  }

  sendDigit(digit[tens], dig_1);
  sendDigit(digit[ones], dig_2);
  sendDigit(digit[degree], dig_3);
  sendDigit(digit[celsius], dig_4);

}


// Function to run a temperature measurement
float kty(unsigned int port) {
  float sensorValue = analogRead(port);
  float resistance = sensorValue / (1023 - sensorValue) * resistor;

  Serial.print("Resistance: ");
  Serial.println(resistance, 1);
  return -0.000028 * pow(resistance, 2) + 0.1844 * resistance - 129.97;
}

void spinFan(uint8_t fanspeed)
{
  analogWrite(fan1Pin, fanspeed);
  analogWrite(fan2Pin, fanspeed);
  return 0;
}

// Funktion zum Übertragen der Informationen
void sendDigit(char wert, int dig) {
  digitalWrite(dig, HIGH);

  digitalWrite(storePin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, wert);
  digitalWrite(storePin, HIGH);
  delay(5);

  // avoid ghosting by setting all outputs high for a moment (common anode display)
  digitalWrite(storePin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, segmentsOff);
  digitalWrite(storePin, HIGH);
  digitalWrite(dig, LOW);
}
