#include <SoftwareSerial.h>

#define relay 9
#define ADC_PIN A0
#define RX_PIN 11  // Define the RX pin for SoftwareSerial
#define TX_PIN 12  // Define the TX pin for SoftwareSerial

SoftwareSerial mySerial(RX_PIN, TX_PIN);  // RX, TX

int threshold = 50; // Default threshold to a reasonable value for testing
unsigned long previousMillis = 0;  // Store the last time the relay was turned on
const long interval = 3000;  // 3 seconds interval

void setup() {
  pinMode(relay, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600);  // Initialize SoftwareSerial
  Serial.println("Setup complete. Awaiting data...");
}

void loop() {
  int val_ADC = analogRead(ADC_PIN);
  int moisturePercentage = map(val_ADC, 435, 175, 0, 100); // Adjust these values based on your sensor calibration

  // Shift the moisture percentage up by 80
  moisturePercentage = min(moisturePercentage + 32, 100); // Ensure it does not exceed 100%

  Serial.print("Moisture Percentage: ");
  Serial.println(moisturePercentage);

  // Check for incoming data from ESP32
  if (mySerial.available()) {
    String input = mySerial.readStringUntil('\n');
    Serial.print("Received input: ");
    Serial.println(input);  // Debug the received input

    if (input.startsWith("THRESHOLD:")) {
      input = input.substring(10); // Remove "THRESHOLD:" prefix
      int newThreshold = input.toInt();

      // Validate the new threshold
      if (newThreshold > 0 && newThreshold < 100) {
        threshold = newThreshold;
        Serial.print("Updated threshold: ");
        Serial.println(threshold);
      } else if(newThreshold < 0) {
        threshold = 0;
        Serial.print("Updated threshold: ");
        Serial.println(threshold);
      }
      else if(newThreshold > 100) {
        threshold = 100;
        Serial.print("Updated threshold: ");
        Serial.println(threshold);
      }
    }
  }

  unsigned long currentMillis = millis();

  // Control the relay based on the moisture percentage
  if (moisturePercentage+5 < threshold) {
    if (currentMillis - previousMillis >= interval) {
      digitalWrite(relay, HIGH);
      Serial.println("Relay ON for 3 seconds");
      delay(3000); // Pump on for 3 seconds
      digitalWrite(relay, LOW);
      Serial.println("Relay OFF");
      delay(10000); // Pump off for 10 seconds
      previousMillis = currentMillis;  // Update the time the relay was turned off
    }
  } else {
    digitalWrite(relay, LOW);
    Serial.println("Relay OFF");
  }

  // Send moisture percentage back to ESP32
  mySerial.println(moisturePercentage);
  Serial.print("Sent moisturePercentage to ESP32: ");
  Serial.println(moisturePercentage);

  delay(500); // Small delay to ensure proper communication
}