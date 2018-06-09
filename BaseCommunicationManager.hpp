//
//  CommunicationManager.hpp
//  NESController
//
//  Created by Stoyan Stoyanov on 30.05.18.
//  Copyright © 2018 Stoyan Stoyanov. All rights reserved.
//

#ifndef CommunicationManager_hpp
#define CommunicationManager_hpp

#pragma mark - Communication Configuration

#define BAUD_RATE_ATMODE 38400
#define BAUD_RATE_NORMAL 9600

#define CONNECTION_CHECK_PIN 13
#define MODE_CONTROL_KEY_PIN 12
#define POWER_CONTROL_PIN    9

#define MAX_MESSAGE_LENGTH  10 // bytes

#define MODE_NORMAL    0x00
#define MODE_ATCOMMAND 0x02
#define MODE_SLEEP     0x01

#include "Arduino.h"
#include <AVRUserDefaults.h>

#pragma mark - Base Abstract Class Definition

class BaseCommunicationManager {

  protected:
    static BaseCommunicationManager *instance;
    virtual bool performModuleInit() = 0;
    void enterMode(int8_t mode); // use MODE_NORMAL and MODE_ATCOMMAND
    bool sendCommand(String command);
  public:
    bool isConnected();
    void update();
    void send(int16_t data);
    char *getData();
    int8_t countOfBytesAvailable();
};

#endif /* CommunicationManager_hpp */
