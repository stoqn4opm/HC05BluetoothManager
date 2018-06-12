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
    pinMode(MASTER_SYNC_BUTTON_PIN, INPUT);
    
    if (AVRUserDefaults::isBluetoothAlreadyConfigured() && !AVRUserDefaults::isSlaveAlreadyConfigured()) {
        searchForNewSlave();
    } else {
        // no need to enterMode(MODE_NORMAL) and wait for boot time
        // because the first thing that will happen in update() is initializeAndFindSlaveIfNeeded()
        // will get called, switching to MODE_ATCOMMAND if bluetooth is not configured!
        if (!AVRUserDefaults::isBluetoothAlreadyConfigured()) { return; }
        enterMode(MODE_NORMAL);
        Serial.begin(BAUD_RATE_NORMAL);
    }
}

BaseCommunicationManager* MasterCommunicationManager::shared() {
    if (instance == 0) { instance = new MasterCommunicationManager(); }
    return instance;
}

#pragma mark - Update Loop

void MasterCommunicationManager::update() {
    
    // forget slave button
    if (digitalRead(MASTER_SYNC_BUTTON_PIN) == HIGH) {
        handleKeyHoldReset();
    } else {
        if (isInMiddleOfPattern) { searchForNewSlave(); }
        isInMiddleOfPattern = false;
    }
    
    initializeAndFindSlaveIfNeeded();
}

void MasterCommunicationManager::handleKeyHoldReset() {
    
    if (!isInMiddleOfPattern) {
        isInMiddleOfPattern = true;
        patternStartTime = millis();
    } else {
        unsigned long currentTime = millis();
        
        if (currentTime >= patternStartTime) {
            if (currentTime - patternStartTime > RESET_KEY_HOLD_DURATION * 1000) {
                // set flag in eeprom for starting a new initialization for the bluetooth module
                AVRUserDefaults::setIsBluetoothAlreadyConfigured(false);
                AVRUserDefaults::setIsSlaveAlreadyConfigured(false);
                isInMiddleOfPattern = false;
            }
        } else {
            isInMiddleOfPattern = false;
        }
    }
}

void MasterCommunicationManager::searchForNewSlave() {

    Serial.end();
    Serial.begin(BAUD_RATE_ATMODE);

    bool connected = false;
    do {
        enterMode(MODE_ATCOMMAND);
        char *slave = findSlave();
        connected = tryConnectingWithSlave(slave);
    }
    while (connected == false);
    enterMode(MODE_NORMAL);
    
    Serial.end();
    Serial.begin(BAUD_RATE_NORMAL);
}

#pragma mark - Module Specific Init

void MasterCommunicationManager::initializeAndFindSlaveIfNeeded() {
    
    if (AVRUserDefaults::isBluetoothAlreadyConfigured() == false) {
        Serial.end();
        Serial.begin(BAUD_RATE_ATMODE);
        enterMode(MODE_ATCOMMAND);
        bool result = performModuleInit();
        while (result == false) {
            enterMode(MODE_ATCOMMAND);
            result = performModuleInit();
        }
        AVRUserDefaults::setIsBluetoothAlreadyConfigured(true);

        bool connected = false;
        do {
            char *slave = findSlave();
            connected = tryConnectingWithSlave(slave);
        }
        while (connected == false);
        enterMode(MODE_NORMAL);
        
        Serial.end();
        Serial.begin(BAUD_RATE_NORMAL);
    }
}

bool MasterCommunicationManager::performModuleInit() {
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

char* MasterCommunicationManager::findSlave() {
    
    char responce[MAX_MESSAGE_LENGTH];
    bool initSend = false;
    size_t byteCount = 0;
    
    do {
        
        //MARK: here i can add check to sleep the system after
        
        Serial.end();   // flushing out the remaining part of old messages
        Serial.begin(BAUD_RATE_ATMODE);
        
        if (initSend == false) {
            CommandResult result = sendCommand("AT+INIT", 2);
            if (result.isOK) {
                initSend = true;
            } else {
                if (result.responce[0] == 'E' &&    // this error is fine
                    result.responce[1] == 'R' &&    // because ERROR:(17)
                    result.responce[2] == 'R' &&    // means SPP Library
                    result.responce[3] == 'O' &&    // already initialized
                    result.responce[4] == 'R' &&
                    result.responce[5] == ':' &&
                    result.responce[6] == '(' &&
                    result.responce[7] == '1' &&
                    result.responce[8] == '7' &&
                    result.responce[9] == ')') {
                    Serial.end();   // flushing out the remaining part of message
                    Serial.begin(BAUD_RATE_ATMODE);
                    initSend = true;
                }
            }
            continue;
        }
        
        CommandResult result = sendCommand("AT+INQ", 11);
        
        byteCount = result.byteCount;
        strcpy(responce, result.responce);

    } while (byteCount <= 20); // because responce will be +INQ:98D3:32:2168EC,73F4,FFB7<0D><0A>OK<0D><0A>

    Serial.end();   // flushing out the remaining part of address, because i read only a portion of it
    Serial.begin(BAUD_RATE_ATMODE);
    
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
    strcat(pairCommand, ",5");
    
    char bindCommand[BL_ADDRESS_LENGTH + 8] = "AT+BIND=";
    strcat(bindCommand, slave);

    bool cmodePassed = false;
    int8_t numberOfAttempts = 0;
    
    do {
    
        if (numberOfAttempts >= 20) { return false; }
        
        numberOfAttempts++;
        sendCommand(pairCommand, 5);
        sendCommand(bindCommand, 1);
        cmodePassed = sendCommand("AT+CMODE=0", 1).isOK;
        
    } while (cmodePassed == false);
    
    AVRUserDefaults::setIsSlaveAlreadyConfigured(true);
    return true;
}
