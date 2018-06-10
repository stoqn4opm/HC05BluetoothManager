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

#pragma mark - Common Update Loop

void BaseCommunicationManager::update() {
    
}

CommandResult BaseCommunicationManager::sendCommand(char command[], int8_t timeOutInSecs) {
//    delay(300); // works just fine without it but lets be on the safe side
    Serial.println(command);
    Serial.flush(); // Waits for the transmission of outgoing serial data to complete.
//    delay(600); // > 1000 from datasheet for HC-06, tested and 107 fails, 114 passes
    
    size_t countOfBytes = 0;
    char responce[MAX_MESSAGE_LENGTH]; // OK\0
    
    Serial.setTimeout(timeOutInSecs * 1000);
    
    do {
        countOfBytes = Serial.readBytesUntil('\0', responce, MAX_MESSAGE_LENGTH - 1);
        responce[MAX_MESSAGE_LENGTH - 1] = '\0';
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
}

#pragma mark - Common Send/Receive Data

int8_t BaseCommunicationManager::countOfBytesAvailable() {
    return Serial.available();
}

void BaseCommunicationManager::send(int16_t data) {
    //    Serial.write(&data, sizeof(data));
}

char *BaseCommunicationManager::getData() {
    byte index = 0;
    char endMarker = '\0'; // because every responce should end in \r\n\0 (CR+LF+TERM)
    
    bool newData = false;
    static char receivedChars[MAX_MESSAGE_LENGTH];
    
    while (Serial.available() > 0 && newData == false) {
        char receivedCharacter = Serial.read();
        if (receivedCharacter != endMarker) {
            receivedChars[index] = receivedCharacter;
            index++;
            if (index >= MAX_MESSAGE_LENGTH) {
                index = MAX_MESSAGE_LENGTH - 1;
            }
        }
        else {
            receivedChars[index] = '\0'; // terminate the string
            newData = true;
        }
    }
    return receivedChars;
}

#pragma mark - Computed Variables

bool BaseCommunicationManager::isConnected() {
    return digitalRead(CONNECTION_CHECK_PIN) == HIGH;
}
