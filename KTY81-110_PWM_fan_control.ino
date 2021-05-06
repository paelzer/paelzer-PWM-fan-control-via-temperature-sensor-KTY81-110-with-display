
/*
    7 Segment Display related
*/

// pin defines for 7 segment Display
#define dig_1 2 // digit 1 pin
#define dig_2 3 // digit 2 pin
#define dig_3 4 // digit 3 pin
#define dig_4 5 // digit 4 pin

// defines for the positions of degree, Celsius and dash symbols in the digit array
#define degree  10
#define celsius 11
#define dash    12

// pin defines for the Arduino Nano to control the 74HC595
#define resetPin  12 // MR    
#define clockPin   6 // SH_CP
#define storePin   7 // ST_CP
#define dataPin    8 // DS

#define segmentsOff  B11111111 // pattern to turn off all segments (common cathode display)


#define sensorPin A1 // temperatur sensor pin is connected to A1 on the nano board

int8_t intTemp;

// integers to take the 2 temperature digits for the display
uint8_t ones = 0, tens = 0;

//define digits for 7 segm display
char digit[13] = {
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
  B00011110, // C
  B11111011  // -

};


/*
    PWM fan control related
*/

float resistor = 3275;

#define fan1Pin 9
#define fan2Pin 10

#define snail  100
#define slow   130
#define fast   200

#define interval 2000
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

    // Carry out 10 ADC value measurements
    float temp = 0;

    float totalADCvalues = 0;
    for (int i = 0; i < 10; i++)
    {
      int singleADCvalue = analogRead(sensorPin);
      totalADCvalues += singleADCvalue;
    }

    // get the average measurement value
    float avgADCvalue = totalADCvalues / 10;

    // calculate the resistance value
    float resistance = avgADCvalue / (1023 - avgADCvalue) * resistor;
    avgADCvalue = 0;

    Serial.print("Resistance: ");
    Serial.println(resistance, 1);

    // polynomial equation to calculate the temperature from the resistance value
    temp = 0.0000000000000158856096691663 * pow(resistance, 5) - 0.000000000107707814667084 * pow(resistance, 4) + 0.000000296930015588067 * pow(resistance, 3) - 0.000439914565065421 * pow(resistance, 2) + 0.467289261526917 * resistance - 207.534348970219;

    Serial.print("Temperature: ");
    Serial.println(temp);

    // 2 digits for the temperature is enough
    intTemp = (int)round(temp);

    // 1st digit
    ones = (intTemp % 10);
    // 2nd digit
    tens = ((intTemp / 10) % 10);

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


  // send data to the display

  if (intTemp >= 0) // OK
  {
    sendDigit(digit[tens], dig_1);
    sendDigit(digit[ones], dig_2);
  }
  else // don't show values below "0" because we prefer higher temperatures
  {
    sendDigit(digit[dash], dig_1);
    sendDigit(digit[dash], dig_2);
  }

  sendDigit(digit[degree], dig_3);
  sendDigit(digit[celsius], dig_4);

}


// function to set the fan speed

void spinFan(uint8_t fanspeed)
{
  analogWrite(fan1Pin, fanspeed);
  analogWrite(fan2Pin, fanspeed);
  return 0;
}

// function to send the temperature value to the shift register

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
