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

#define MAX_MESSAGE_LENGTH  35 // bytes longest messege will be +INQ:98D3:32:2168EC,73F4,FFB7<0D><0A>OK<0D><0A>
#define BL_ADDRESS_LENGTH   15 // bytes: 14 chars address + '\0'
#define BL_BOOT_TIME        3000 // miliseconds

#define MODE_NORMAL    0x00
#define MODE_ATCOMMAND 0x02
#define MODE_SLEEP     0x01

#include "Arduino.h"
#include <AVRUserDefaults.h>
#include "BluetoothPacket.hpp"

#pragma mark - Base Abstract Class Definition

struct BluetoothPacket {
    uint8_t buttonData;
    uint8_t deviceState;
    bool isPopulated;
};

struct CommandResult {
    bool isOK;
    size_t byteCount;
    char responce[MAX_MESSAGE_LENGTH];
};

class BaseCommunicationManager {

  protected:
    static BaseCommunicationManager *instance;
    virtual bool performModuleInit() = 0;
    void enterMode(int8_t mode); // use MODE_NORMAL and MODE_ATCOMMAND
    CommandResult sendCommand(char command[], int8_t timeOutInSecs);
  public:
    bool isConnected();
    virtual void update() = 0;
    void send(BluetoothPacket data, bool sendDeviceState);
    BluetoothPacket getData();
};

#endif /* CommunicationManager_hpp */
