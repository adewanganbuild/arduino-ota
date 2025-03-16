#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

// WiFi Credentials
const char* ssid = "myguest";
const char* password = "123123123";

// URL of the version file
const char* version_url = "http://192.168.0.102:8080/version-url.txt";  

// Current firmware version (Update this when flashing manually)
String CURRENT_VERSION = "1.0.0";  

// Timer Variables
unsigned long lastCheckTime = 0;
const unsigned long updateInterval = 60 * 1000;  // 1 minute in milliseconds

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop() {
    unsigned long currentTime = millis();

    // Check for OTA update every 1 minute
    if (currentTime - lastCheckTime >= updateInterval) {
        lastCheckTime = currentTime;
        checkForUpdate();
    }
    Serial.println("Done 10");
    delay(60000);  // Small delay to avoid excessive CPU usage
}

void checkForUpdate() {
    Serial.println("Checking for update...");

    HTTPClient http;
    http.begin(version_url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        response.trim();  // Remove whitespace
        http.end();

        // Extract the version and firmware URL
        int newLineIndex = response.indexOf('\n');
        if (newLineIndex == -1) {
            Serial.println("Invalid version file format!");
            return;
        }

        String newVersion = response.substring(0, newLineIndex);
        String firmware_url = response.substring(newLineIndex + 1);
        newVersion.trim();
        firmware_url.trim();

        Serial.print("Current version: ");
        Serial.println(CURRENT_VERSION);
        Serial.print("Latest version: ");
        Serial.println(newVersion);
        Serial.print("Firmware URL: ");
        Serial.println(firmware_url);

        if (newVersion.equals(CURRENT_VERSION)) {
            Serial.println("Firmware is up to date. No update needed.");
            return;
        } else {
            Serial.println("New firmware version available! Updating...");
            performOTAUpdate(firmware_url);
        }
    } else {
        Serial.printf("Failed to check version, HTTP response: %d\n", httpCode);
    }

    http.end();
}

void performOTAUpdate(String firmware_url) {
    HTTPClient http;
    http.begin(firmware_url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        int contentLength = http.getSize();
        if (contentLength <= 0) {
            Serial.println("Invalid content length");
            return;
        }

        // Ensure enough space for OTA update
        if (!Update.begin(contentLength)) {
            Serial.println("Not enough space for update.");
            return;
        }

        WiFiClient *stream = http.getStreamPtr();
        size_t written = Update.writeStream(*stream);

        if (written == contentLength) {
            Serial.println("Update successfully downloaded.");
        } else {
            Serial.println("Update failed.");
            return;
        }

        if (Update.end()) {
            Serial.println("Update complete. Restarting...");
            ESP.restart();
        } else {
            Serial.println("Update failed.");
        }
    } else {
        Serial.printf("Update check failed, HTTP response: %d\n", httpCode);
    }

    http.end();
}
