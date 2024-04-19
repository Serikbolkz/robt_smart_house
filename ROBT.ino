// Adding the necessary libraries for the sensors
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
// Initializing the variables for each sensor
#define DHTTYPE DHT11
#define DHTPIN 6 
#define ONE_WIRE_BUS 5
#define WATER_PIN  A1
#define relay 11

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial BTSerial(3,4); 

const int redLED = 13;
const int vib_pin = 8; 
float t;
long previousMillis = 0;
const int buzzerPin = 9;
const int soundPin = A2;
bool button = false;

// initial setup of sensors and arduino
void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(vib_pin, INPUT);
  pinMode(7, INPUT);
  pinMode(WATER_PIN, INPUT);
  pinMode(buzzerPin, OUTPUT);
  dht.begin();
  sensors.begin();
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  sensors.setResolution(12);
  sensors.setWaitForConversion(false);  // makes it async asincrono=non blocca il programma durante la lettura

  sensors.requestTemperatures(); // Send the command to get temperatures
  previousMillis = millis();

}
void loop() {
  // reading the analog/digital values from each sensor
  int curr = analogRead(A0);
  int flame = digitalRead(7);
  int water = analogRead(WATER_PIN);
  float h = dht.readHumidity();
  int vib = digitalRead(vib_pin);
  int sound = analogRead(soundPin);
  int buttondata = digitalRead(10);
  Serial.println(buttondata);
  // line 60-66, turning on/off the sound sensor
  if(buttondata == 1 && button == false){
    button = true;
    buttondata = 0;
  }
  if(buttondata == 1 && button == true){
    button = false;
  }
  // line 68-79, showing the temperature and humidity on the screen 
  if (millis() - previousMillis > 1200){
    t = sensors.getTempCByIndex(0);
    sensors.requestTemperatures(); 
    previousMillis = millis(); 
    lcd.clear(); 
    lcd.setCursor(1,0);  
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.setCursor(1,1);   
    lcd.print("Humid: ");
    lcd.print(h);
  }
  // line 81-91, if flame sensor detects flame, it activates buzzer, RGB LED and shows corresponding warning on the screen. Also activates water pomp mechanism.
  if(flame == LOW){
    lcd.clear();
    lcd.setCursor(3,0);   
    lcd.print("WARNING!!");
    lcd.setCursor(3,1);   
    lcd.print("FLAME!!");
    digitalWrite(relay, LOW);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(4000);
  }
  // line 93-102, if gas sensor detects gas, it activates buzzer, RGB LED and shows corresponding warning on the screen. Also, sends signal through Bluetooth.
  if(curr > 720){
    lcd.clear();
    lcd.setCursor(3,0);   
    lcd.print("WARNING!");
    lcd.setCursor(1,1);   
    lcd.print("GAS LEAKAGE!!");
    digitalWrite(redLED, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(2000);
    BTSerial.println("aaaaaaaaaaaaa");
  }
  // line 105- 114, if vibration sensor detects vibration, it activates buzzer, RGB LED and shows corresponding warning on the screen.
  if(vib == LOW){
    digitalWrite(redLED, LOW);
    lcd.clear();
    lcd.setCursor(2,0);   
    lcd.print("WARNING!!");
    lcd.setCursor(0,1);   
    lcd.print("EARTHQUAKE");
    digitalWrite(buzzerPin, LOW);
    delay(2000);
  }
  // line 116-126, if water sensors detects water, it activates buzzer, RGB LED and shows corresponding warning on the screen. Also, sends signal through Bluetooth.
  if(water > 200){
    lcd.clear();
    lcd.setCursor(4,0);   
    lcd.print("WARNING!!");
    lcd.setCursor(0,1);   
    lcd.print("WATER LEAKAGE");
    digitalWrite(redLED, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(3000);
    BTSerial.println("a");
  }
  // if sound sensor is on and it detects sound, it activates buzzer, RGB LED and shows corresponding warning on the screen.
  if(sound > 700 && button == true){
    lcd.clear();
    lcd.setCursor(4,0);   
    lcd.print("WARNING!!");
    lcd.setCursor(4,1);   
    lcd.print("BREAK IN!!");
    digitalWrite(redLED, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }
  // turning the water pomp, RGB LED and buzzer
  delay(300);
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(redLED, HIGH);
  digitalWrite(relay, HIGH);
}
