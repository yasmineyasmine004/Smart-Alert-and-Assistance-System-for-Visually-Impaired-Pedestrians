#include <LiquidCrystal.h>

// Définition des broches
const int trigPin = 3;
const int echoPin = 2;
const int buzzerPin = 11;
const int ledPin = 12;
const int buttonPin = 13;
const int motorPin = 7; // Contrôle du transistor pour moteur vibreur

LiquidCrystal lcd(10, 9, 8, 6, 5, 4);

int ultrasonic = 0;
bool systemActive = false;
bool lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

void setup() {
  //Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(buttonPin, INPUT); // avec résistance pull-down externe
  digitalWrite(motorPin, LOW); // Éteindre le moteur au départ
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  // Gestion du bouton avec anti-rebond
  bool reading = digitalRead(buttonPin);

   if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == HIGH && systemActive == false) {
      systemActive = true; // activation système
    }
    else if (reading == HIGH && systemActive == true) {
      systemActive = false; // désactivation système
    }
  }
  lastButtonState = reading;


  if (systemActive) {
    ultrasonic = 0.01723 * readUltrasonicDistance(trigPin, echoPin);

    Serial.print("Distance (cm): ");
    Serial.println(ultrasonic);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distance: ");
    lcd.print(ultrasonic);
    lcd.print(" cm");
    lcd.setCursor(0, 1);

    if (ultrasonic <= 300) {
      lcd.print("Ralentir aveugle!");
      
      digitalWrite(motorPin, HIGH); // Allume le moteur

      // LED clignotante
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      delay(500);

      // Buzzer continu
      tone(buzzerPin, 523);
    } else {
      lcd.print("Attention aveugle");
      digitalWrite(motorPin, LOW); // Éteint le moteur

      // LED allumée fixe
      digitalWrite(ledPin, HIGH);

      // Buzzer intermittent
      tone(buzzerPin, 523);
      delay(100);
      noTone(buzzerPin);
      delay(400);
    }

  } else {
    // Système inactif : tout est éteint
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
    digitalWrite(motorPin, LOW); // Moteur éteint

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Systeme OFF");
    lcd.setCursor(0, 1);
    lcd.print("Appuyez bouton");
    delay(200);
  }
}
