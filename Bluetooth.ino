#include "SlaveCommunicationManager.hpp"

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
    bool isConnected = SlaveCommunicationManager::shared()->isConnected();
    SlaveCommunicationManager::shared()->update();
    Serial.println(isConnected);
}
