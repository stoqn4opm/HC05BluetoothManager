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
        performModuleInit();
    }
    enterMode(MODE_NORMAL);
    Serial.begin(BAUD_RATE_NORMAL);
}

BaseCommunicationManager* SlaveCommunicationManager::shared() {
    if (instance == 0) {
        instance = new SlaveCommunicationManager();
    }
    return instance;
}

#pragma mark - Module Specific Init

void SlaveCommunicationManager::performModuleInit() {
    Serial.begin(BAUD_RATE_ATMODE);
    enterMode(MODE_ATCOMMAND);
    delay(700);
    sendCommand("AT+ORGL");
    sendCommand("AT+RMAAD");
    sendCommand("AT+UART=9600,0,0");
    sendCommand("AT+NAME=NES Controller");
    sendCommand("AT+PSWD=0000");
    sendCommand("AT+ROLE=0");
    sendCommand("AT+CLASS=73F4"); // custom so that its harder to be discovered
    sendCommand("AT+IAC=9E8B3F"); // not relevant but just in case, for hiding
    
//    AVRUserDefaults::setIsBluetoothAlreadyConfigured(true);
    Serial.end();
}
