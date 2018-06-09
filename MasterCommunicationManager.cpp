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
        bool isConnected = tryConnectingWithSlave(slave);
        Serial.print("slave found");
        Serial.println(slave);
        Serial.print("is connected ");
        Serial.println(isConnected);
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
    if (sendCommand("AT+INQM=1,1,10")           == false) { return false; } // (rssiSearch, CountOfResults, durationOfSearch)

    return true;
}

#pragma mark - Searching For Slave

char* MasterCommunicationManager::searchForSlave() {
    
    size_t countOfBytes = 0;
    char responce[MAX_MESSAGE_LENGTH];
    bool initSend = false;
    
    do {
        
        //MARK: here i can add check to sleep the system after
        
        if (initSend == false) {
            initSend = sendCommand("AT+INIT");
            continue;
        }
        
        delay(300); // works just fine without it but lets be on the safe side
        Serial.println("AT+INQ");
        Serial.flush(); // Waits for the transmission of outgoing serial data to complete.
        
        Serial.setTimeout(10500); // 10.5 secs. Its important to be more that the scan period thats currently set to 10 (AT+INQM=1,1,10)
        countOfBytes = Serial.readBytesUntil('\r', responce, MAX_MESSAGE_LENGTH - 1);
        responce[MAX_MESSAGE_LENGTH - 1] = '\0';

    } while (countOfBytes < 3);
    
    Serial.setTimeout(1000); // reverting to the default value

    static char result[BL_ADDRESS_LENGTH];

    // Responce will be in the form: +INQ:98D3:21:FC7AF7,73F4,7FFF
    // and should be converted to 98D3,21,FC7AF7
    
    for (int8_t i = 5; i < 19; i++) {
        result[i - 5] = (i == 9 ||i == 12) ? ',' : responce[i];
    }
    result[14] = '\0';
    return result; // sample result: 98D3,21,FC7AF7
}

#pragma mark - Connect with Slave

bool MasterCommunicationManager::tryConnectingWithSlave(char slave[BL_ADDRESS_LENGTH]) {
    
    char pairCommand[BL_ADDRESS_LENGTH + 11] = "AT+PAIR=";
    strcat(pairCommand, slave);
    strcat(pairCommand, ",20");
    
    char linkCommand[BL_ADDRESS_LENGTH + 8] = "AT+LINK=";
    strcat(linkCommand, slave);
    
    Serial.setTimeout(20000); // 20 sec timeout for responce
    
    bool pairPassed = false;
    bool linkPassed = false;
    
    int8_t numberOfAttempts = 0;
    
    do {
    
        if (numberOfAttempts >= 10) {
            Serial.setTimeout(1000);
            return false;
        }
        
        numberOfAttempts++;
        
        
        if (pairPassed == false) {
            pairPassed = sendCommand(pairCommand);
            continue;
        }
        
        if (linkPassed == false) {
            linkPassed = sendCommand(linkCommand);
            continue;
        }
        
    } while (linkPassed == false);
    
    Serial.setTimeout(1000);
    
    return true;
}
