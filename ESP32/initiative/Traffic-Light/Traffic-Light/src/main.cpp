#include <Arduino.h>

void printSystemInfo() {
    Serial.println("========================================");
    Serial.println("        ESP32 SYSTEM INFORMATION");
    Serial.println("========================================");

    Serial.printf("Chip Model     : %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision  : %d\n", ESP.getChipRevision());
    Serial.printf("CPU Frequency  : %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash Size     : %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.printf("Flash Speed    : %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
    Serial.printf("Free Heap      : %d Bytes\n", ESP.getFreeHeap());
    Serial.printf("SDK Version    : %s\n", ESP.getSdkVersion());

    Serial.println("========================================");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    printSystemInfo();
}

void loop() {

    static uint32_t counter = 1;

    Serial.printf(
        "[%03lu] ESP32 Running | Uptime : %lu sec | Free Heap : %d Bytes\n",
        counter++,
        millis() / 1000,
        ESP.getFreeHeap()
    );

    delay(1000);
}