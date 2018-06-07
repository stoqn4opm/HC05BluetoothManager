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
  if (btSerial()->available()) {
    Serial.write(btSerial()->read());
  }
  if (Serial.available()) {
    btSerial()->write(Serial.read());
  }
}


void BaseCommunicationManager::enterMode(int8_t mode) {
  digitalWrite(POWER_CONTROL_PIN, HIGH); // for P Channel mosfet base control
  digitalWrite(MODE_CONTROL_KEY_PIN, mode == MODE_NORMAL ? LOW : HIGH);
  delay(100);
  digitalWrite(POWER_CONTROL_PIN, LOW); // for P Channel mosfet base control
  delay(1000);
}

#pragma mark - Common Send/Receive Data

int8_t BaseCommunicationManager::countOfBytesAvailable() {
  return btSerial()->available();
}

void BaseCommunicationManager::send(int16_t data) {

  //    btSerial.write(&data, sizeof(data));
}

int16_t BaseCommunicationManager::getData() {
  btSerial()->read();
}

#pragma mark - Computed Variables

SoftwareSerial * BaseCommunicationManager::btSerial() {
  if (helper) {
    return helper;
  }
  helper = new SoftwareSerial(BLUETOOTH_RX_PIN, BLUETOOTH_TX_PIN);
  return helper;
}

bool BaseCommunicationManager::isConnected() {
  return digitalRead(CONNECTION_CHECK_PIN) == HIGH;
}
