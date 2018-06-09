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
        performModuleInit();
    }
    enterMode(MODE_NORMAL);
    Serial.begin(BAUD_RATE_NORMAL);
}

BaseCommunicationManager* MasterCommunicationManager::shared() {
    if (instance == 0) {
        instance = new MasterCommunicationManager();
    }
    return instance;
}

#pragma mark - Module Specific Init

bool MasterCommunicationManager::performModuleInit() {
    Serial.begin(BAUD_RATE_ATMODE);
    enterMode(MODE_ATCOMMAND);
    
//    AVRUserDefaults::setIsBluetoothAlreadyConfigured(true);
    Serial.end();
}
