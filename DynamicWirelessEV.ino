#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int irInPin = 2;
const int irOutPin = 3;
const int relayPin = 8;
const int pulsePin = 9;   // MOSFET gate control

int carCount = 0;

bool lastInState = HIGH;
bool lastOutState = HIGH;

void setup() {

  pinMode(irInPin, INPUT);
  pinMode(irOutPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(pulsePin, OUTPUT);

  digitalWrite(relayPin, LOW);
  digitalWrite(pulsePin, LOW);

  lcd.begin(16, 2);
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Car Counter");
  delay(2000);
  lcd.clear();
}

void loop() {

  bool currentInState = digitalRead(irInPin);
  bool currentOutState = digitalRead(irOutPin);

  // -------- Car IN ----------
  if (lastInState == HIGH && currentInState == LOW) {
    carCount++;
    delay(300);  // debounce
  }

  // -------- Car OUT ----------
  if (lastOutState == HIGH && currentOutState == LOW) {
    if (carCount > 0) {
      carCount--;
    }
    delay(300);  // debounce
  }

  lastInState = currentInState;
  lastOutState = currentOutState;

  // -------- Relay + High Frequency Coil ----------
  if (carCount > 0) {

    digitalWrite(relayPin, HIGH);

    // Generate high frequency for wireless transfer
    tone(pulsePin, 25000);   // 25 kHz

  } else {

    digitalWrite(relayPin, LOW);

    noTone(pulsePin);        // Stop frequency
    digitalWrite(pulsePin, LOW);
  }

  // -------- LCD Display ----------
  lcd.setCursor(0, 0);
  lcd.print("Cars Inside:   ");

  lcd.setCursor(0, 1);
  lcd.print("Count: ");
  lcd.print(carCount);
  lcd.print("   ");
}
const int irIn = 2;
const int irOut = 3;
const int relayPin = 8;

int carCount = 0;

bool lastInState;
bool lastOutState;

void setup() {

  // -------- 10kHz Wireless Power on Pin 9 --------
  pinMode(9, OUTPUT);
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A = _BV(COM1A0);              // Toggle OC1A
  TCCR1B = _BV(WGM12) | _BV(CS10);   // CTC mode, prescaler 1
  OCR1A = 799;                      // 10kHz

  // -------- IR Sensors --------
  pinMode(irIn, INPUT_PULLUP);
  pinMode(irOut, INPUT_PULLUP);

  // -------- Relay --------
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  delay(1000);  // allow sensors to stabilize

  lastInState = digitalRead(irIn);
  lastOutState = digitalRead(irOut);
}

void loop() {

  bool currentIn = digitalRead(irIn);
  bool currentOut = digitalRead(irOut);

  // Detect entry (HIGH -> LOW)
  if (lastInState == HIGH && currentIn == LOW) {
    carCount++;
    delay(200);  // debounce
  }

  // Detect exit (HIGH -> LOW)
  if (lastOutState == HIGH && currentOut == LOW) {
    if (carCount > 0) {
      carCount--;
    }
    delay(200);
  }

  lastInState = currentIn;
  lastOutState = currentOut;

  // Relay ON if cars present
  digitalWrite(relayPin, carCount > 0 ? HIGH : LOW);
}