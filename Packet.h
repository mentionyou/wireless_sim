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
    address sender=0; // the direct sender of frame
    address receiver=0; //the direct receiver of frame
    address source=0;
    address destination=0;
//    address peer=0;/// will added later
//    int flag_moredata=0;
//    int payload=1500*8;
    int type=0; // Primary frame =1 Secondary frame =2
    u_seconds time_stamp;
    
}; 

struct ACK
{
    address receiver=0;
};


#endif /* Packet_h */
