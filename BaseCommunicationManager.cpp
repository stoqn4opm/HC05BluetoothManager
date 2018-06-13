//
//  CommunicationManager.cpp
//  NESController
//
//  Created by Stoyan Stoyanov on 30.05.18.
//  Copyright © 2018 Stoyan Stoyanov. All rights reserved.
//

#include "BaseCommunicationManager.hpp"

#pragma mark - Common Init Parts

BaseCommunicationManager* BaseCommunicationManager::instance = 0;

CommandResult BaseCommunicationManager::sendCommand(char command[], int8_t timeOutInSecs) {
//    delay(300); // works just fine without it but lets be on the safe side
    Serial.end();
    Serial.begin(BAUD_RATE_ATMODE);
    Serial.println(command);
    Serial.flush(); // Waits for the transmission of outgoing serial data to complete.
    
    size_t countOfBytes = 0;
    char responce[MAX_MESSAGE_LENGTH]; // OK\0
    
    Serial.setTimeout(timeOutInSecs * 1000);
    
    do {
        countOfBytes = Serial.readBytesUntil('\n', responce, MAX_MESSAGE_LENGTH - 2);
        responce[countOfBytes] = '\n';
        responce[countOfBytes + 1] = '\0';
    } while (countOfBytes == 0);
    
    Serial.setTimeout(1000);
    
    CommandResult result;
    result.byteCount = countOfBytes;
    
    strcpy(result.responce, responce);
    
    if ((responce[0] != 'O')  ||
        (responce[1] != 'K')  ||
        (responce[2] != '\r') ||
        (responce[3] != '\n')) { result.isOK = false; return result; }
    
    result.isOK = true;
    return result;
}

void BaseCommunicationManager::enterMode(int8_t mode) {
    
    if (mode == MODE_SLEEP) {
        digitalWrite(POWER_CONTROL_PIN, LOW);
        digitalWrite(MODE_CONTROL_KEY_PIN, LOW);
        return;
    }
    
    digitalWrite(POWER_CONTROL_PIN, LOW); // for N Channel mosfet base control
    digitalWrite(MODE_CONTROL_KEY_PIN, mode == MODE_NORMAL ? LOW : HIGH);
    delay(10);
    digitalWrite(POWER_CONTROL_PIN, HIGH); // for N Channel mosfet base control
    delay(BL_BOOT_TIME); // because i saw it fail on 700
}

#pragma mark - Common Send/Receive Data

#define PACKET_TYPE_1 0b00000000 // for future use UNSAFE, because non set uint defaults to 0
#define PACKET_TYPE_2 0b01000000 // used for sending 2 keypresses
#define PACKET_TYPE_3 0b10000000 // used for sending battery percentage
#define PACKET_TYPE_4 0b11000000 // used for sending 6 keypresses

void BaseCommunicationManager::send(BluetoothPacket data, bool sendDeviceState) {
    data.isPopulated = true;
    if (isConnected() == false) { return; }
    
    uint8_t packet1 = PACKET_TYPE_4;
    packet1 = packet1 | (data.buttonData >> 2);
    Serial.write(packet1);
    
    uint8_t packet2 = PACKET_TYPE_2;
    packet2 = packet2 | ((uint8_t)(data.buttonData << 6) >> 2);
    Serial.write(packet2);
    
    if (sendDeviceState == false) { return; }
    
    uint8_t packet3 = PACKET_TYPE_3;
    packet3 = packet3 | ((uint8_t)(data.deviceState << 2) >> 2);
    Serial.write(packet3);
}

BluetoothPacket BaseCommunicationManager::getData() {
    
    static BluetoothPacket result;
    result.buttonData = 0;
    result.deviceState = 0;
    result.isPopulated = false;
    
    if (isConnected() == false)  { return result; }

    uint8_t rawData[3];
    
    Serial.readBytes(rawData, sizeof(rawData));
    
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t currentByte = rawData[i];
        
        if ((currentByte >> 6) == (PACKET_TYPE_4 >> 6)) {
            result.buttonData = result.buttonData | (currentByte << 2);
        }
        else if ((currentByte >> 6) == (PACKET_TYPE_2 >> 6)) {
            result.buttonData = result.buttonData | ((uint8_t)(currentByte << 2) >> 6);
        }
        else if ((currentByte >> 6) == (PACKET_TYPE_3 >> 6)) {
            result.deviceState = result.deviceState | ((uint8_t)(currentByte << 2) >> 2);
        }
    }
    
    result.isPopulated = true;
    
    return result;
}

#pragma mark - Computed Variables

bool BaseCommunicationManager::isConnected() {
    return digitalRead(CONNECTION_CHECK_PIN) == HIGH;
}
