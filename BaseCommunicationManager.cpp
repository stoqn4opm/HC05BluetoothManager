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

bool BaseCommunicationManager::sendCommand(String command) {
  Serial.println(command);
  delay(100); // > 1000 from datasheet for HC-06 !!!!!!!!!!

  bool oLetterReceived = false;
  bool kLetterReceived = false;

  while (oLetterReceived == false && kLetterReceived == false) {

    while (Serial.available() > 0) {
      char readedChar = Serial.read();

      if (readedChar == 'O') {
        oLetterReceived = true;
      }
      else if (readedChar == 'K' && oLetterReceived) {
        kLetterReceived = true;
      }
    }
  }
}

void BaseCommunicationManager::enterMode(int8_t mode) {
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

int16_t BaseCommunicationManager::getData() {
  Serial.read();
}

#pragma mark - Computed Variables

bool BaseCommunicationManager::isConnected() {
  return digitalRead(CONNECTION_CHECK_PIN) == HIGH;
}
