//
//  SlaveCommunicationManager.cpp
//  AVRSleep
//
//  Created by Stoyan Stoyanov on 6.06.18.
//  Copyright © 2018 Stoyan Stoyanov. All rights reserved.
//

#include "SlaveCommunicationManager.hpp"

#pragma mark - Slave Singleton Reference

SlaveCommunicationManager::SlaveCommunicationManager() {
    
    pinMode(CONNECTION_CHECK_PIN, INPUT);
    pinMode(MODE_CONTROL_KEY_PIN, OUTPUT);
    pinMode(POWER_CONTROL_PIN, OUTPUT);
    
    if (!AVRUserDefaults::isBluetoothAlreadyConfigured()) {
        bool result = performModuleInit();
        while (result == false) {
            enterMode(MODE_SLEEP);
            delay(1000);
            result = performModuleInit();
        }
    }
    enterMode(MODE_NORMAL);
    Serial.begin(BAUD_RATE_NORMAL);
}

BaseCommunicationManager* SlaveCommunicationManager::shared() {
    if (instance == 0) { instance = new SlaveCommunicationManager(); }
    return instance;
}

#pragma mark - Module Specific Init

bool SlaveCommunicationManager::performModuleInit() {
    Serial.begin(BAUD_RATE_ATMODE);
    enterMode(MODE_ATCOMMAND);
    delay(BL_BOOT_TIME); // because i saw it fail on 700
    if (sendCommand("AT+ORGL",                  1).isOK == false) { return false; }
    if (sendCommand("AT+RMAAD",                 1).isOK == false) { return false; }
    if (sendCommand("AT+UART=9600,0,0",         1).isOK == false) { return false; }
    if (sendCommand("AT+NAME=NES Controller",   1).isOK == false) { return false; }
    if (sendCommand("AT+PSWD=0000",             1).isOK == false) { return false; }
    if (sendCommand("AT+ROLE=0",                1).isOK == false) { return false; }
    if (sendCommand("AT+CMODE=1",               1).isOK == false) { return false; }
    if (sendCommand("AT+CLASS=73F4",            1).isOK == false) { return false; } // custom so that its harder to be discovered
    if (sendCommand("AT+IAC=9E8B33",            1).isOK == false) { return false; } // liac
    
    Serial.end();
//    AVRUserDefaults::setIsBluetoothAlreadyConfigured(true);
    return true;
}
