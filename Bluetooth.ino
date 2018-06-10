#include "MasterCommunicationManager.hpp"

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
    bool isConnected = MasterCommunicationManager::shared()->isConnected();
    MasterCommunicationManager::shared()->update();
//    Serial.print("MasterCommunicationManager is connected:");
//    Serial.println(isConnected);
}
