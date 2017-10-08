//
//  Packet.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#ifndef Packet_h
#define Packet_h

#include "Event.h"
#include <iostream>

typedef unsigned long address;

struct DATA
{
    address source=0;
    address destination=0;
    address peer=0;/// will added later
    int flag_moredata;
    int payload=1500*8-120;
    int type=0;
    u_seconds time;
};

struct ACK
{
    address destination=0;
    int payload=112;
    int type=0;
};


#endif /* Packet_h */
