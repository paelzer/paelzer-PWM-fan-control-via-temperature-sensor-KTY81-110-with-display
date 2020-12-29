#define dig_1 2
#define dig_2 3
#define dig_3 4
#define dig_4 5

#define resetPin 12
#define taktPin 6 // SH_CP
#define speicherPin 7 // ST_CP
#define datenPin 8 // DS
#define segmentsOff B11111111

//define characters for 7 segm display
char zahl[12] = {B00010100, B11110101, B00011001, B01010001, B11110000, B01010010, B00010010, B01110101, B00010000, B01010000, B01111000, B00011110};

void setup() {
  
  pinMode(resetPin, OUTPUT);
  pinMode(taktPin, OUTPUT);
  pinMode(speicherPin, OUTPUT);
  pinMode(datenPin, OUTPUT);

  pinMode(dig_1, OUTPUT);
  pinMode(dig_2, OUTPUT);
  pinMode(dig_3, OUTPUT);
  pinMode(dig_4, OUTPUT);

  digitalWrite(resetPin, HIGH);

}

void loop() {

  //digitalWrite(resetPin, LOW);
  //delay(2000);

  sendDigit(zahl[6], dig_1);
  sendDigit(zahl[7], dig_2);
  sendDigit(zahl[10], dig_3);
  sendDigit(zahl[11], dig_4);
}


// Funktion zum Übertragen der Informationen
void sendDigit(char wert, int dig) {
  digitalWrite(dig, HIGH);
  
  digitalWrite(speicherPin, LOW);
  shiftOut(datenPin, taktPin, LSBFIRST, wert);
  digitalWrite(speicherPin, HIGH);
  delay(5);
  // disable ghosting by setting high all outputs (common anode display)
  digitalWrite(speicherPin, LOW);
  shiftOut(datenPin, taktPin, LSBFIRST, segmentsOff);
  digitalWrite(speicherPin, HIGH);
  digitalWrite(dig, LOW);
}
