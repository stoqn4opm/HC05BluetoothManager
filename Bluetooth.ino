#include "SlaveCommunicationManager.hpp"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  Serial.println("Enter AT commands:");
}

void loop() {
  // put your main code here, to run repeatedly:
    bool isConnected = SlaveCommunicationManager::shared()->isConnected();
    SlaveCommunicationManager::shared()->update();
}
