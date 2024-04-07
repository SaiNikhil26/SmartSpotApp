#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ThingSpeak.h>
WiFiClient client;

const char* ssid = "Even Detector";
const char* password = "@#Gavrishb";

unsigned long myChannelNumber = 2487047;
const char* myWriteAPIKey = "WZJCOLNULY7HWTY0";
int statusCode;

const int trigPin1 = 14;
const int echoPin1 = 34;
const int trigPin2 = 15;
const int echoPin2 = 35;

int value1 = 0;
int value2 = 0;
int freeSlot;
int count;

unsigned long PreviousTime = 0;
unsigned long lastTime = 0;
unsigned long timeDelay = 10000;

#define LED1 33
#define LED2 12
#define BUZZER1 32
#define BUZZER2 26

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration1;
long duration2;

float distanceCm1;
float distanceInch1;
float distanceCm2;
float distanceInch2;

LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(BUZZER1, OUTPUT);

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER2, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Not Connected");
  }
  ThingSpeak.begin(client);
}

void loop() {
  count = 2;

  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distanceCm1 = duration1 * SOUND_SPEED / 2;
  distanceInch1 = distanceCm1 * CM_TO_INCH;

  if (distanceCm1 < 20) {
    digitalWrite(LED1, HIGH);
    digitalWrite(BUZZER1, HIGH);
    value1 = 1;
    count--;
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(BUZZER1, LOW);
    value1 = 0;
  }

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distanceCm2 = duration2 * SOUND_SPEED / 2;
  distanceInch2 = distanceCm2 * CM_TO_INCH;

  if (distanceCm2 < 20) {
    digitalWrite(LED2, HIGH);
    digitalWrite(BUZZER2, HIGH);
    value2 = 1;
    count--;
  } else {
    digitalWrite(LED2, LOW);
    digitalWrite(BUZZER2, LOW);
    value2 = 0;
  }

  Serial.println("No.of Free Slots: ");
  Serial.println(count);
  lcd.clear();

  if (value1 == 1 && value2 == 1) {
    freeSlot = 0;
  } else if (value1 == 1 && value2 == 0) {
    freeSlot = 1;
  } else if (value1 == 0 && value2 == 1) {
    freeSlot = 2;
  }
  else{
    freeSlot = 3;
  }

  if (value1 == 0 && value2 == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Slot1: FREE");
    lcd.setCursor(0, 1);
    lcd.print("Slot2: FREE");
  } else if (value1 == 0 && value2 == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Slot1: FREE");
    lcd.setCursor(0, 1);
    lcd.print("Slot2: FULL");
  } else if (value1 == 1 && value2 == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Slot1: FULL");
    lcd.setCursor(0, 1);
    lcd.print("Slot2: FREE");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("NO PARKING");
    lcd.setCursor(0, 1);
    lcd.print("AVAILABLE");
  }

  if ((millis() - PreviousTime - lastTime) > timeDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      ThingSpeak.setField(1, String(count));
      ThingSpeak.setField(2, freeSlot); // Field 2 to store the status of free slot
      int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if (statusCode == 200) {
        Serial.println("Channel Update Successful");
      } else {
        Serial.println("Problem writing in data: " + String(statusCode));
      }
    }
    lastTime = millis() - PreviousTime;
  }
  delay(1000);
}
