#include <Wire.h>               // Include Wire library for I2C
#include <LiquidCrystal_I2C.h>  // Include LiquidCrystal I2C library
#include <SoftwareSerial.h>     // Include SoftwareSerial library for GSM communication

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize the LCD with the I2C address, and dimensions (16x2)
SoftwareSerial gsmSerial(2, 3);      // RX, TX for GSM module

const int heartbeatPin = A0;  // Analog pin connected to the heartbeat sensor
const int alarmPin = 8;       // Digital pin connected to the alarm

char remoteNumber[] = "+1234567890";  // Replace with your recipient's phone number

void setup() {
  pinMode(alarmPin, OUTPUT);  // Set the alarm pin as an output
  
  lcd.init();        // Initialize the LCD
  lcd.backlight();   // Turn on the backlight
  
  lcd.setCursor(0, 0);
  lcd.print("Heartbeat Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);       // Display the initializing message for 2 seconds
  
  lcd.clear();       // Clear the LCD screen
  
  // Initialize SoftwareSerial for GSM communication
  gsmSerial.begin(9600);
  
  // Wait a few seconds for the GSM module to initialize
  delay(3000);
  
  // Set SMS mode to text
  gsmSerial.println("AT+CMGF=1");
  delay(100);
  
  // Clear any existing SMS messages in the GSM module's buffer
  gsmSerial.println("AT+CMGD=1,4");
  delay(1000);
}

void loop() {
  int heartbeat = analogRead(heartbeatPin);  // Read the analog value from the heartbeat sensor
  int heartbeatThreshold = 100;  // Example threshold for testing

  // Display heartbeat value on LCD
  lcd.setCursor(0, 0);
  lcd.print("Heartbeat: ");
  lcd.print(heartbeat);

  // Check if heartbeat exceeds the threshold
  if (heartbeat > heartbeatThreshold) {
    lcd.setCursor(0, 1);
    lcd.print("Alert: High HR");
    digitalWrite(alarmPin, HIGH);  // Activate the alarm
    
    // Send SMS alert
    sendSMSAlert();
    
    delay(5000);  // Delay to prevent continuous SMS sending
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Normal");
    digitalWrite(alarmPin, LOW);  // Deactivate the alarm
  }

  delay(500);  // Delay for stability and to reduce LCD flicker
}

void sendSMSAlert() {
  gsmSerial.println("AT+CMGS=\"" + String(remoteNumber) + "\"");  // Specify the recipient's phone number
  delay(100);
  
  gsmSerial.print("Heart rate alert! HR is above 100 bpm.");  // SMS message content
  delay(100);
  
  gsmSerial.write((char)26);  // End SMS with Ctrl+Z
  delay(100);
  
  gsmSerial.println();
  delay(1000);  // SMS sending time
}
