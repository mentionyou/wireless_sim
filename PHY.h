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
    
    bool phy_check_idle();
    bool phy_only_receive_singal_from (int node_id);
    bool phy_pt_send_check();
    bool phy_st_send_check(int node_id);
    bool phy_pt_receive_check( int node_id);
    bool phy_st_receive_ack_check(int node_id);

private:
    // for more genral phy hearing check;
    unsigned long index1;// index to each neighbour nodes;
    unsigned long index2;
    unsigned long index3;
};



//


#endif /* PHY_h */
