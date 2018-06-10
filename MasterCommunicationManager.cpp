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
    if (sendCommand("AT+ORGL",              1).isOK == false) { return false; }
    if (sendCommand("AT+RMAAD",             1).isOK == false) { return false; }
    if (sendCommand("AT+UART=9600,0,0",     1).isOK == false) { return false; }
    if (sendCommand("AT+NAME=NES Receiver", 1).isOK == false) { return false; }
    if (sendCommand("AT+PSWD=0000",         1).isOK == false) { return false; }
    if (sendCommand("AT+ROLE=1",            1).isOK == false) { return false; }
    if (sendCommand("AT+CMODE=1",           1).isOK == false) { return false; }
    if (sendCommand("AT+CLASS=73F4",        1).isOK == false) { return false; } // custom so that its harder to be discovered
    if (sendCommand("AT+IAC=9E8B33",        1).isOK == false) { return false; } // liac
    if (sendCommand("AT+INQM=1,1,10",       1).isOK == false) { return false; } // (rssiSearch, CountOfResults, durationOfSearch)

    return true;
}

#pragma mark - Searching For Slave

char* MasterCommunicationManager::searchForSlave() {
    
    char responce[MAX_MESSAGE_LENGTH];
    bool initSend = false;
    size_t byteCount = 0;
    
    do {
        
        //MARK: here i can add check to sleep the system after
        
        if (initSend == false) {
            initSend = sendCommand("AT+INIT", 2).isOK;
            continue;
        }
        
        CommandResult result = sendCommand("AT+INQ", 11);
        
        byteCount = result.byteCount;
        strcpy(responce, result.responce);

    } while (byteCount == 0);

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
    strcat(pairCommand, ",10");
    
    char linkCommand[BL_ADDRESS_LENGTH + 8] = "AT+LINK=";
    strcat(linkCommand, slave);
    
    bool pairPassed = false;
    bool linkPassed = false;
    
    int8_t numberOfAttempts = 0;
    
    do {
    
        if (numberOfAttempts >= 10) { return false; }
        
        numberOfAttempts++;
        
        if (pairPassed && linkPassed == false) {
            linkPassed = sendCommand(linkCommand, 11).isOK; // 20 sec timeout for responce
            continue;
        }
        
        if (pairPassed == false) {
            pairPassed = sendCommand(pairCommand, 11).isOK; // 20 sec timeout for responce
            continue;
        }
        
    } while (linkPassed == false);
    
    return true;
}
