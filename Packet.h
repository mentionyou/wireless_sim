//
//  Packet.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#ifndef Packet_h
#define Packet_h

#include <iostream>

typedef unsigned long address;

struct DATA
{
    address source=0;
    address destination=0;
    address peer=0;/// after will added
    int payload=1280;
    int type=0;
};

struct ACK
{
    address destination=0;
    int payload=20;
    int type=0;
};


#endif /* Packet_h */
