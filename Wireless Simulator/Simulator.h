//
//  Simulator.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 16/10/18.
//  Copyright © 2016年 段荣昌. All rights reserved.
//


#ifndef Simulator_h
#define Simulator_h

#include <queue>
#include <iostream>
#include "Event.h"
#include "Node.h"



using namespace std;

struct cmp
{
    bool operator () (const Event &a , const Event &b)
    {
        return a>b;
    }
};


class Simulator
{
public:
    priority_queue< Event , vector<Event> , cmp > sim_queue;
    
    void run();
    void sch(const Event &);
    void add(const Event &);
    
//    bool check_available(const Event &);// check the event is or isn't executive?
};



#endif /* Simulator_h */
