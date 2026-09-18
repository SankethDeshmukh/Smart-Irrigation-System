#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>

// --- Pin Definitions ---
#define DHTPIN 2
#define DHTTYPE DHT11 // Change to DHT22 if using that version
#define RAIN_PIN A1
#define RELAY_PIN 7
#define BUZZER_PIN 8
#define SOIL_PIN A0

// --- SIM800L SoftwareSerial Pins ---
SoftwareSerial sim800l(10, 11); // RX, TX

// --- LCD Initialization ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// --- DHT Initialization ---
DHT dht(DHTPIN, DHTTYPE);

// --- System Variables ---
bool isAutoMode = true;
bool pumpStatus = false;
int soilMoisturePercent = 0;
bool isRaining = false;
float temperature = 0.0;
float humidity = 0.0;
unsigned long previousLcdTime = 0;
bool showScreen1 = true;

// Thresholds
const int dryThreshold = 15; // Turn pump ON if moisture is below 30%
const int wetThreshold = 75; // Turn pump OFF if moisture is above 70%

// Phone number to send SMS alerts to (Include country code, e.g., +91 for India)
String masterNumber = "+918431848510"; 

void setup() {
  Serial.begin(9600);
  sim800l.begin(9600);
  dht.begin();
  
  pinMode(RAIN_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Ensure pump is off initially (Relays are often active LOW)
  digitalWrite(RELAY_PIN, LOW); 
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("    AGRO-TEC    ");
  lcd.setCursor(0, 1);
  lcd.print("SMART IRRIGATION");
  delay(2000);

  lcd.setCursor(0, 0);
  lcd.print("  STARTING THE ");
  lcd.setCursor(0, 1);
  lcd.print("     SYSTEM     ");
  delay(2000);
  lcd.clear();

  // Initialize GSM Module
  lcd.print("Init GSM...");
  sim800l.println("AT"); 
  delay(500);
  sim800l.println("ATE0"); // CRITICAL FIX: Turn off command echoing
  delay(500);
  sim800l.println("AT+CMGF=1"); // Set SMS to Text Mode
  delay(500);
  sim800l.println("AT+CNMI=2,2,0,0,0"); // Route incoming SMS directly to serial
  delay(500);
  
  // Clear any junk in the serial buffer
  while(sim800l.available()) {
    sim800l.read();
  }
  
  lcd.clear();
}

void loop() {
  readSensors();
  updateLCD();
  checkSMS();

  if (isAutoMode) {
    handleAutoMode();
  }
  
  delay(500); // Loop delay for stability
}

// --- Sensor Reading ---
void readSensors() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Read Rain Sensor (Usually LOW when raining)
  isRaining = (digitalRead(RAIN_PIN) == LOW);

  // Read Soil Moisture and map to 0-100%
  // Adjust 1023 and 0 if your sensor reads differently
  int rawSoil = analogRead(SOIL_PIN);
  soilMoisturePercent = map(rawSoil, 1023, 0, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
}

// --- Auto Mode Logic ---
void handleAutoMode() {
  if (isRaining) {
    if (pumpStatus == true) {
      controlPump(false);
      sendSMS("Rain detected. Pump turned OFF automatically.");
    }
  } else {
    if (soilMoisturePercent < dryThreshold && pumpStatus == false) {
      controlPump(true);
      sendSMS("Soil is dry. Pump turned ON automatically.");
    } else if (soilMoisturePercent > wetThreshold && pumpStatus == true) {
      controlPump(false);
      sendSMS("Soil is wet enough. Pump turned OFF automatically.");
    }
  }
}

// --- Pump & Buzzer Control ---
void controlPump(bool turnOn) {
  if (turnOn && !pumpStatus) {
    digitalWrite(RELAY_PIN, HIGH); // Turn pump ON
    pumpStatus = true;
    
    // 2 Small Beeps for Pump ON
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    delay(150);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    
  } else if (!turnOn && pumpStatus) {
    digitalWrite(RELAY_PIN, LOW); // Turn pump OFF
    pumpStatus = false;
    
    // 1 Long Beep for Pump OFF
    digitalWrite(BUZZER_PIN, HIGH);
    delay(800);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// --- LCD Display Update (Alternating Screens) ---
void updateLCD() {
  unsigned long currentMillis = millis();
  
  // Switch screens every 10000 milliseconds (10 seconds)
  if (currentMillis - previousLcdTime >= 10000) {
    previousLcdTime = currentMillis;
    showScreen1 = !showScreen1; // Toggle between true and false
    lcd.clear(); // Clear the screen to prevent leftover characters
  }

  if (showScreen1) {
    // --- Screen 1: Soil, Mode, Pump, Rain ---
    lcd.setCursor(0, 0);
    lcd.print("S:");
    lcd.print(soilMoisturePercent);
    lcd.print("%  ");
    lcd.print(isAutoMode ? "AUTO  " : "MANUAL");

    lcd.setCursor(0, 1);
    lcd.print("PUMP:");
    lcd.print(pumpStatus ? "ON " : "OFF");
    lcd.print(" R:");
    lcd.print(isRaining ? "YES" : "NO ");
  } 
  else {
    // --- Screen 2: Temperature & Humidity ---
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature, 1); // Prints with 1 decimal point
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Hum:  ");
    lcd.print(humidity, 1);
    lcd.print(" %");
  }
}

// --- SMS Receiving & Processing ---
void checkSMS() {
  if (sim800l.available()) {
    String smsContent = sim800l.readString();
    smsContent.toUpperCase(); // Make it uppercase for easier comparison
    Serial.println(smsContent); // Print to serial monitor for debugging

    // CRITICAL FIX: Using '#' prevents the system from reading its own status messages
    if (smsContent.indexOf("#AUTO") > -1) {
      isAutoMode = true;
      sendSMS("System set to AUTO mode.");
    } 
    else if (smsContent.indexOf("#MANUAL") > -1) {
      isAutoMode = false;
      sendSMS("System set to MANUAL mode.");
    } 
    else if (smsContent.indexOf("#PUMP ON") > -1) {
      if (!isAutoMode) {
        controlPump(true);
        sendSMS("Pump turned ON manually.");
      } else {
        sendSMS("Cannot control pump. Change to #MANUAL mode first.");
      }
    } 
    else if (smsContent.indexOf("#PUMP OFF") > -1) {
      if (!isAutoMode) {
        controlPump(false);
        sendSMS("Pump turned OFF manually.");
      } else {
        sendSMS("Cannot control pump. Change to #MANUAL mode first.");
      }
    }
    else if (smsContent.indexOf("#STATUS") > -1) {
      String statusMsg = "System Status:\nMode: " + String(isAutoMode ? "AUTO" : "MANUAL") + 
                         "\nSoil: " + String(soilMoisturePercent) + "%" +
                         "\nTemp: " + String(temperature) + "C" +
                         "\nRain: " + String(isRaining ? "Yes" : "No") +
                         "\nPump: " + String(pumpStatus ? "ON" : "OFF");
      sendSMS(statusMsg);
    }
  }
}

// --- SMS Sending Function ---
void sendSMS(String message) {
  sim800l.println("AT+CMGS=\"" + masterNumber + "\"");
  delay(500);
  sim800l.print(message);
  delay(500);
  sim800l.write(26); // ASCII code for CTRL+Z to send the message
  delay(3000);       // Wait for message to send over the network
  
  // CRITICAL FIX: Clear out the SIM module's output buffer (like "OK") 
  // so it doesn't accidentally trigger the checkSMS() function on the next loop
  while(sim800l.available()) {
    sim800l.read(); 
  }
}