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
    btSerial()->begin(BAUD_RATE_NORMAL);
}

BaseCommunicationManager* SlaveCommunicationManager::shared() {
    if (instance == 0) {
        instance = new SlaveCommunicationManager();
    }
    return instance;
}

#pragma mark - Module Specific Init

void SlaveCommunicationManager::performModuleInit() {
    btSerial()->begin(BAUD_RATE_ATMODE);
    enterMode(MODE_ATCOMMAND);
    btSerial()->println("AT+ORGL");
    delay(1000);
    Serial.println("AT+RMAAD");
    delay(1000);
    Serial.println("AT+NAME=NES Controller");
    delay(1000);
    Serial.println("AT+PSWD=0000");
    delay(1000);
    Serial.println("AT+ROLE=0");
    delay(1000);
    Serial.println("AT+CLASS=002508");
    delay(1000);
    Serial.println("AT+IAC=9E8B32");
    delay(1000);
        
//    AVRUserDefaults::setIsBluetoothAlreadyConfigured(true);
    btSerial()->end();
}
