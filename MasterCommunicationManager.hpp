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

#define MASTER_SYNC_BUTTON_PIN  10 // pin number
#define RESET_KEY_HOLD_DURATION 7 // secs

class MasterCommunicationManager: BaseCommunicationManager {
private:
    MasterCommunicationManager();
    char *findFirstSlave();
    bool tryConnectingWithSlave(char slave[BL_ADDRESS_LENGTH]);
    void initializeAndFindSlaveIfNeeded();
    
    bool isInMiddleOfPattern = false;
    unsigned long patternStartTime;
    void handleKeyHoldReset();
    void searchForNewSlave();
protected:
    virtual bool performModuleInit();
    
public:
    static BaseCommunicationManager *shared();
    virtual void update();
};

#endif /* MasterCommunicationManager_hpp */
