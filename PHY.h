//
//  PHY.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#ifndef PHY_h
#define PHY_h

#define PHY_IDLE 0
#define PHY_BUSY 1
#define PHY_COLLISION 2

#include "Event.h"

class PHY
{
public:
    PHY();
    ~PHY(){};
    
    void* node;
    int tx_state; // IDLE, BUSY
    int rx_state;// IDLE, BUSY, COLLISION
    
    
//    bool phy_able_to_send();
//    bool phy_able_to_receive();
    
    void phy_send();
    void phy_receive(const Event&);
    
    void phy_send_end();
    void phy_receive_end(const Event&);

private:
    // for more genral phy hearing check;
    unsigned long index;// index to each neighbour nodes; 
};



//


#endif /* PHY_h */
