//
//  MasterCommunicationManager.hpp
//  AVRSleep
//
//  Created by Stoyan Stoyanov on 6.06.18.
//  Copyright © 2018 Stoyan Stoyanov. All rights reserved.
//

#ifndef MasterCommunicationManager_hpp
#define MasterCommunicationManager_hpp

#include "BaseCommunicationManager.hpp"

#define MASTER_SYNC_BUTTON_PIN 10

class MasterCommunicationManager: BaseCommunicationManager {
private:
    MasterCommunicationManager();
    char *findFirstSlave();
    bool tryConnectingWithSlave(char slave[BL_ADDRESS_LENGTH]);
    void initializeAndFindSlaveIfNeeded();
    
    bool setupNewSlaveForAutoConnection = false;
    char slaveForAutoConnection[BL_ADDRESS_LENGTH];
    
    void autoConnectSlaveIfNeeded();
    
protected:
    virtual bool performModuleInit();
    
public:
    static BaseCommunicationManager *shared();
    virtual void update();
};

#endif /* MasterCommunicationManager_hpp */
