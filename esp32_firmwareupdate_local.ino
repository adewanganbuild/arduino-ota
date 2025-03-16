#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "xxxxx";        // Replace with your WiFi SSID
const char* password = "xxxxx"; // Replace with your WiFi Password
const char* firmware_url = "http://192.168.0.102:8080/esp32_firmwareupdate_local.ino.bin";  // URL of the firmware file

void checkForUpdates() {
    // Serial.println("Checking for firmware update...");
    
    HTTPClient http;
    http.begin(firmware_url);  // Connect to the firmware file
    http.setTimeout(30000);
    int httpCode = http.GET(); // Make GET request
    
    if (httpCode == HTTP_CODE_OK) { // If response is OK
        int contentLength = http.getSize();
        // Serial.println("content length : " + contentLength);
        if (contentLength <= 0) {
            Serial.println("Invalid content length.");
            return;
        }

        // size_t freeSpace = Update.getFreeSketchSpace();
        // Serial.printf("Available OTA partition size: %d bytes\n", freeSpace);

        WiFiClient* stream = http.getStreamPtr();  // Get the stream for downloading
        
        // Serial.println("Starting update...");
        if (Update.begin(contentLength)) {  // Start the update process
            size_t written = Update.writeStream(*stream);
            if (written == contentLength) {
                // Serial.println("Update successfully written.");
                if (Update.end()) {
                    // Serial.println("Update completed. Rebooting...");
                    ESP.restart();  // Restart ESP32
                } else {
                    Serial.println("Update failed!");
                }
            } else {
                Serial.println("Written only " + String(written) + " / " + String(contentLength));
            }
        } else {
            Serial.println("Not enough space for update.");
        }
    } else {
        Serial.println("Failed to download update. HTTP Code: " + String(httpCode));
    }
    http.end();  // Close the HTTP connection
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    // Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    checkForUpdates();  // Call update function
}

void loop() {
    // Normal program execution
    Serial.printf("Flash Chip Size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("Sketch Size: %d bytes\n", ESP.getSketchSize());
    Serial.printf("Free Space for OTA: %d bytes\n", ESP.getFreeSketchSpace());
    Serial.printf("Max Sketch Size: %d bytes\n", ESP.getFreeSketchSpace());
    Serial.println("\nDone 1000");
    delay(60000);
}
