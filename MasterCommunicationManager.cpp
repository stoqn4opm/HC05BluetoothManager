//
//  MasterCommunicationManager.cpp
//  AVRSleep
//
//  Created by Stoyan Stoyanov on 6.06.18.
//  Copyright © 2018 Stoyan Stoyanov. All rights reserved.
//

#include "MasterCommunicationManager.hpp"

#pragma mark - Master Singleton Reference

MasterCommunicationManager::MasterCommunicationManager() {
    
    pinMode(CONNECTION_CHECK_PIN, INPUT);
    pinMode(MODE_CONTROL_KEY_PIN, OUTPUT);
    pinMode(POWER_CONTROL_PIN, OUTPUT);
    
    if (!AVRUserDefaults::isBluetoothAlreadyConfigured()) {
        Serial.begin(BAUD_RATE_ATMODE);
        enterMode(MODE_ATCOMMAND);
        bool result = performModuleInit();
        while (result == false) {
            enterMode(MODE_SLEEP);
            delay(1000);
            result = performModuleInit();
        }
        char *slave = searchForSlave();
        Serial.print("slave found :");
        Serial.println(slave);
        Serial.flush();
        Serial.end();
//        AVRUserDefaults::setIsBluetoothAlreadyConfigured(true);
    }
    enterMode(MODE_NORMAL);
    Serial.begin(BAUD_RATE_NORMAL);
}

BaseCommunicationManager* MasterCommunicationManager::shared() {
    if (instance == 0) { instance = new MasterCommunicationManager(); }
    return instance;
}

#pragma mark - Module Specific Init

bool MasterCommunicationManager::performModuleInit() {
    delay(BL_BOOT_TIME); // because i saw it fail on 700
    if (sendCommand("AT+ORGL")                  == false) { return false; }
    if (sendCommand("AT+RMAAD")                 == false) { return false; }
    if (sendCommand("AT+UART=9600,0,0")         == false) { return false; }
    if (sendCommand("AT+NAME=NES Receiver")     == false) { return false; }
    if (sendCommand("AT+PSWD=0000")             == false) { return false; }
    if (sendCommand("AT+ROLE=1")                == false) { return false; }
    if (sendCommand("AT+CMODE=1")               == false) { return false; }
    if (sendCommand("AT+CLASS=73F4")            == false) { return false; } // custom so that its harder to be discovered
    if (sendCommand("AT+IAC=9E8B33")            == false) { return false; } // liac
    if (sendCommand("AT+INQM=1,1,10")           == false) { return false; }; // (rssiSearch, CountOfResults, durationOfSearch)

    return true;
}

#pragma mark - Searching For Slave

void MasterCommunicationManager::sendInitOrBlock() {
    bool initSend = sendCommand("AT+INIT");
    while (initSend == false) {
        initSend = sendCommand("AT+INIT");
    }
}

char* MasterCommunicationManager::searchForSlave() {
    
    sendInitOrBlock();
    
    char responce[MAX_MESSAGE_LENGTH];
    
    delay(300); // works just fine without it but lets be on the safe side
    Serial.println("AT+INQ");
    Serial.flush(); // Waits for the transmission of outgoing serial data to complete.
    
    Serial.setTimeout(10000);
    size_t countOfBytes = Serial.readBytesUntil('\r', responce, MAX_MESSAGE_LENGTH - 1);
    responce[MAX_MESSAGE_LENGTH - 1] = '\0';
//    do {
//
//        while (Serial.available() > 0) {
//        }
//
//    } while (responce[0] != '\0');
    
    
    Serial.print("responce ");
    Serial.println(responce);
    
    static char result[BL_ADDRESS_LENGTH];
//    slave found :98D3:,2:,168EC  || 98D3,21,FC7AF7
    for (int8_t i = 0; i < 14; i++) {
        result[i] = (i == 5 || i == 8) ? ',' : responce[i + 5];
    }
    result[14] = '\0';
    return result; // sample result: 98D3,21,FC7AF7
}
