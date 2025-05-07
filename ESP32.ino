#include <IOXhop_FirebaseESP32.h>
#include <WiFi.h>

#define WIFI_SSID "wifi-name"           //change to your wifi ssid
#define WIFI_PASSWORD "wifi-password"   //change to your wifi password
#define FIREBASE_AUTH "ScVI8MzXuczQaFfNl03LP2p7rENzHFDSmFYzo2n3"        // change to your database authentication
#define FIREBASE_HOST "https://grad-bba94-default-rtdb.firebaseio.com/"      // change to your database address
#define RXp2 16
#define tXp2 17
float moisturePercentage;
float a;

void setup() {
  Serial.begin(115200);          // Set Serial Monitor window comm speed
  Serial2.begin(9600, SERIAL_8N1, RXp2, tXp2);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected:");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  // Reading moisturePercentage from Serial2
if (Serial2.available() > 0) {
  String input = Serial2.readStringUntil('\n'); // Read a line of data from Serial2
  moisturePercentage = input.toFloat(); // Convert the input string to a float
  Serial.print("Received moisturePercentage: ");
  Serial.println(moisturePercentage);

  // Send moisturePercentage to Firebase
  Firebase.setFloat("Humidity", moisturePercentage);
  if (Firebase.failed()) {
    Serial.print("Setting/number failed: ");
    Serial.println(Firebase.error());
    return;
  }
  delay(500); // Small delay to avoid flooding Firebase

  // Clear the serial buffer
  while (Serial2.available() > 0) {
    Serial2.read(); // Read and discard any remaining characters in the buffer
  }
} else {
  Serial.println("No data available from Serial2");
}

  // Retrieve 'a' from Firebase
  a = Firebase.getFloat("smts");
  if (Firebase.failed()) {
    Serial.print("Getting 'a' failed: ");
    Serial.println(Firebase.error());
    return;
  }
  Serial.print("Retrieved 'a' from Firebase: ");
  Serial.println(a);

  // Send 'a' to Arduino via Serial2
  Serial2.print("THRESHOLD:");
  Serial2.println(a);  // Ensure end of line for the receiver to read properly
  delay(500); // Small delay to avoid overwhelming the Arduino
}
