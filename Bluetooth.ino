//#include "MasterCommunicationManager.hpp"
#include "SlaveCommunicationManager.hpp"

void setup() {
  // put your setup code here, to run once:
}

//void loop() {
//  // put your main code here, to run repeatedly:
//    MasterCommunicationManager::shared()->update();
//
//    BluetoothPacket receivedPacket = MasterCommunicationManager::shared()->getData();
//
//    if (receivedPacket.isPopulated) {
//        Serial.print("Button Data ");
//        Serial.println(receivedPacket.buttonData, BIN);
//        Serial.print("State ");
//        Serial.println(receivedPacket.deviceState, BIN);
//    }
//}

void loop() {
    BluetoothPacket packet;
    packet.buttonData = 0x0F;
    packet.deviceState = 0x0A; 
    SlaveCommunicationManager::shared()->send(packet, false);
    SlaveCommunicationManager::shared()->update();
}
 
