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

class MasterCommunicationManager: BaseCommunicationManager {
private:
    MasterCommunicationManager();
    char *searchForSlave();
    bool tryConnectingWithSlave(char slave[BL_ADDRESS_LENGTH]);
protected:
    virtual bool performModuleInit();
public:
    static BaseCommunicationManager *shared();
};

#endif /* MasterCommunicationManager_hpp */
