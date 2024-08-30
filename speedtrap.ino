#include <SoftwareSerial.h>

const float PHOTOELECTRIC_DISTANCE_METERS = 1;
const int PHOTOELECTRIC_1_PIN = 9;
const int PHOTOELECTRIC_2_PIN = 10;
unsigned long PHOTOELECTRIC_1_MILLIS = 0;
unsigned long PHOTOELECTRIC_2_MILLIS = 0;
const int BUZZER_PIN = 11;
const int RESET_TIME_MILLIS = 2000;
bool ESP8266 = false;
SoftwareSerial ESPserial(2, 3); // RX, TX 

void setup() {
  Serial.begin(115200);
  ESPserial.begin(115200);
  Serial.println("Starting...");
  pinMode(BUZZER_PIN, OUTPUT);
  tone(BUZZER_PIN, 500, 50);
  pinMode(A0, OUTPUT);
  pinMode(PHOTOELECTRIC_1_PIN, INPUT_PULLUP);
  pinMode(PHOTOELECTRIC_2_PIN, INPUT_PULLUP);
  //startLCD();
  if (initESP8266()) {
    // Set Wi-Fi mode to station only
    ESPserial.println("AT+CWMODE=1");
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
      if (ESPserial.find("OK")) {
        Serial.println("Mode set to Station");
        break;
      }
    }
    
    if (connectToWiFi("speedtrap", "speedtrap")) {
      ESP8266 = true;
      ESPserial.println("AT+CIPMUX=1");
      delay(1000);
      // Get IP Address
      ESPserial.println("AT+CIFSR");
      delay(1000);
      Serial.println("Server ready!");
    }
  } else {
    Serial.println("ESP8266 initialization failed...");
  }
}

bool connectToWiFi(String ssid, String password) {
  int maxAttempts = 10; // Set the number of maximum attempts here
  for (int attempt = 1; attempt <= maxAttempts; attempt++) {
    Serial.print("Attempting to connect to WiFi: ");
    Serial.print(ssid);
    Serial.print(" (Attempt ");
    Serial.print(attempt);
    Serial.println(")");

    delay(2000);
    ESPserial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
    delay(1000);

    if (ESPserial.find("OK")) {
      Serial.println("Connected to WiFi network successfully.");
      alarm(100);
      delay(100);
      alarm(100);
      delay(100);
      alarm(100);
      return true;
    } else {
      Serial.println("Failed to connect, retrying...");
      delay(5000); // Wait 5 seconds before retrying to avoid rapid reconnection attempts
    }
  }
  Serial.println("Failed to connect to WiFi network after several attempts.");
}

bool initESP8266() {
  Serial.println("Initializing ESP8266...");
  delay(1000);
  ESPserial.println("AT"); 
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    if (ESPserial.find("OK")) {
      Serial.println("Module is responding.");
      return true;
    }
  }
  return false;
}

void backlight(bool enabled) {
  digitalWrite(A0, enabled);
}

double getSpeedKmh(double t) {
  return abs(metersToKilometers(PHOTOELECTRIC_DISTANCE_METERS) / millisToHours(t));
}

double metersToKilometers(double distanceMeters) {
  return distanceMeters / 1000.0;
}

double millisToHours(double milliseconds) {
  return milliseconds / 3600000.0;
}

void alarm(int milliseconds) {
  tone(BUZZER_PIN, 500, milliseconds);
}


void makeHttpPostRequest(double speed) {

  // Start an unsecured connection to the server on port 5000
  ESPserial.println("AT+CIPSTART=\"TCP\",\"192.168.1.200\",5000");
  delay(200);
  // Formulate the HTTP POST request
  String postData = "{\"speed\":" + String(speed,2) + "}";
  String httpRequest = "POST /speed HTTP/1.1\r\n";
  httpRequest += "Host: 192.168.1.102:5000\r\n";
  httpRequest += "Authorization: Basic YWRtaW46YW10\r\n"; // Base64 encoded "admin:amt"
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Content-Length: " + String(postData.length()) + "\r\n";
  httpRequest += "\r\n";
  httpRequest += postData;

  // Send the size of the request to the module
  ESPserial.println("AT+CIPSEND=" + String(httpRequest.length()));
  delay(200); 
  /* startTime = millis(); // Reset the timer for waiting for the '>' prompt

  while (millis() - startTime < 200) { // Wait for 2 seconds for '>' prompt
    if (ESPserial.find(">")) {
      break; // Exit the loop if '>' prompt is received
    }
  }
 */
  // Send the actual HTTP request
  ESPserial.print(httpRequest);

 /*  startTime = millis(); // Reset the timer for waiting for the response
 */
 /*  while (millis() - startTime < 200) { // Wait for 3 seconds for a response
    if (ESPserial.available()) {
      String response = ESPserial.readString();
      Serial.println(response); // Print the response to the serial monitor
      break;
    }
  } */

  // Close the TCP connection
  //ESPserial.println("AT+CIPCLOSE");
}

/* bool waitForResponse(String expectedResponse, unsigned long timeoutMillis) {
  unsigned long startTime = millis();
  while (millis() - startTime < timeoutMillis) {
    if (ESPserial.find(expectedResponse.c_str())) {
      return true;
    }
  }
  return false;
}

void checkWiFiConnection() {
  ESPserial.println("AT+CWJAP?"); // Command to check Wi-Fi connection status
  if (!waitForResponse("OK", 5000)) {
    Serial.println("WiFi connection lost, attempting to reconnect...");
    ESP8266 = connectToWiFi("speedtrap", "speedtrap");
  }
} */

void loop() {

  //checkWiFiConnection();

  int read_1 = digitalRead(PHOTOELECTRIC_1_PIN);
  int read_2 = digitalRead(PHOTOELECTRIC_2_PIN);

  if (read_1 == LOW && PHOTOELECTRIC_1_MILLIS == 0) {
    PHOTOELECTRIC_1_MILLIS = millis();
    alarm(100);
    //debugRead(1);
  }

  if (read_2 == LOW && PHOTOELECTRIC_1_MILLIS != 0 && PHOTOELECTRIC_2_MILLIS == 0) {
    PHOTOELECTRIC_2_MILLIS = millis();
    alarm(100);
    //debugRead(2);
  }

  if (PHOTOELECTRIC_1_MILLIS != 0 && PHOTOELECTRIC_2_MILLIS != 0) {
    //clearRow(0);
    //lcd.setCursor(0, 0);
    if (ESP8266) {
      makeHttpPostRequest(getSpeedKmh(double(PHOTOELECTRIC_2_MILLIS - PHOTOELECTRIC_1_MILLIS)));
    }
    //lcd.print(speed);
    //delay(1000);
    PHOTOELECTRIC_1_MILLIS = 0;
    PHOTOELECTRIC_2_MILLIS = 0;
  }

  if (PHOTOELECTRIC_1_MILLIS != 0 && millis() - PHOTOELECTRIC_1_MILLIS > RESET_TIME_MILLIS) {
    PHOTOELECTRIC_1_MILLIS = 0;
    PHOTOELECTRIC_2_MILLIS = 0;
    //debugReset();
    delay(100);
  }

  /*
  if (PHOTOELECTRIC_2_MILLIS != 0 && millis() - PHOTOELECTRIC_2_MILLIS > RESET_TIME_MILLIS) {
    PHOTOELECTRIC_1_MILLIS = 0;
    PHOTOELECTRIC_2_MILLIS = 0;
    debugReset();
    delay(100);
  }
  */


}