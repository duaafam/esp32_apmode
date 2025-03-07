#include <WiFi.h>
#include <WiFiManager.h> // WiFiManager library

// Define GPIO pin for external trigger
#define TRIGGER_PIN 34  

// Define AP mode timeout (15 minutes)
#define AP_MODE_TIMEOUT 900000  

WiFiManager wm;

unsigned long apStartTime = 0;  
bool isAPMode = false;  

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIGGER_PIN, INPUT_PULLDOWN); // Configure GPIO 34 as input with pull-down
  
  WiFi.mode(WIFI_STA);  
  wm.setTimeout(180);  // Set WiFiManager timeout (3 minutes)
  
  if (!wm.autoConnect("ESP32_AP", "password")) {
    Serial.println("Failed to connect to Wi-Fi. Waiting for trigger...");
  } else {
    Serial.println("Connected to Wi-Fi: " + WiFi.SSID());
  }
}

void loop() {
  int triggerState = digitalRead(TRIGGER_PIN);  

  Serial.println("Trigger Pin State: " + String(triggerState));

  if (triggerState == HIGH) {
    Serial.println("3.3V detected on GPIO 34! Switching to AP mode...");
    startAPMode();
  }

  // If AP mode is active, check if 15 minutes have passed
  if (isAPMode && (millis() - apStartTime >= AP_MODE_TIMEOUT)) {
    Serial.println("AP Mode Timeout! Exiting AP mode...");
    exitAPMode();
  }

  delay(1000);
}

// Function to start AP mode
void startAPMode() {
  if (!isAPMode) {  
    WiFi.mode(WIFI_AP);
    wm.startConfigPortal("ESP32_AP", "password");
    apStartTime = millis();  
    isAPMode = true;  
  }
}

// Function to exit AP mode and return to STA mode
void exitAPMode() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  
  Serial.println("Trying to reconnect to Wi-Fi...");
  if (!wm.autoConnect("ESP32_AP", "password")) {
    Serial.println("Reconnection failed. Restarting ESP32...");
    ESP.restart();
  } else {
    Serial.println("Reconnected to Wi-Fi: " + WiFi.SSID());
  }

  isAPMode = false;  
}
