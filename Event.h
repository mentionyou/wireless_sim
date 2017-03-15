//
//  Event.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 16/10/17.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#ifndef Event_h
#define Event_h

#include <iostream>
#include "Packet.h"

#define Sending_data 100
#define Sending_data_end 200
#define Sending_data_collision 300
#define Sending_ack 400
#define Sending_ack_end 500
#define Sending_ack_collision 600
#define Inner_node 700

#define u_seconds unsigned long

// we treat end and collision as a kind of sending case.


class Event
{
public:
    Event(){};
    ~Event(){};
    Event (const int a, const unsigned long b, const int c, const int d): uid(a), t(b), nodeid (c), type(d){}
    
    int uid; // Event unique id.
    u_seconds t;
    int nodeid;
    int type;

};


inline bool operator< (const Event &a , const Event &b)
{
    if(a.t < b.t)
    {
        return true;
    }
    else if (a.t==b.t && a.uid < b.uid)
    {
        return true;
    }
    else
    {
        return false;
    }
    
};

inline bool operator== (const Event&a, const Event &b)
{
    return (a.uid == b.uid) && (a.nodeid==b.nodeid) && (a.t==b.t) && (a.type==b.type);
};

inline bool operator!= (const Event&a, const Event &b)
{
    return !(a == b);
};

inline bool operator > (const Event &a , const Event &b)
{
    if(a.t > b.t)
    {
        return true;
    }
    else if (a.t==b.t && a.uid > b.uid)
    {
        return true;
    }
    else
    {
        return false;
    }
};


#endif /* Event_h */
