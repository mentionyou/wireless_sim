//
//  Traffic.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2017-8-15.
//  Copyright © 2017年 段荣昌. All rights reserved.
//

#ifndef Traffic_h
#define Traffic_h



#include "Node.h"

#define BASE_TRAFFIC 10

//#define PACKETS_IN_1_SECONDS  5
class Traffic
{
public:
    Traffic();
    ~Traffic(){};
    void start();
    void start(int,int);
    void set_traffic();
    void set_traffic(int,int);
};

int base_traffic = BASE_TRAFFIC;

void Traffic::start()
{
    extern Node Nodelist[size_of_Nodelist];
    this->set_traffic();
    for (int i=1; i<size_of_Nodelist;i++)
        Nodelist[i].start(0);
}

void Traffic::start(int traffic_input,int node_input)
{
    extern Node Nodelist[size_of_Nodelist];
    this->set_traffic(traffic_input,node_input);
    for (int i=1; i<size_of_Nodelist;i++)
        Nodelist[i].start(0);
}


void Traffic::set_traffic()
{
    extern Node Nodelist[size_of_Nodelist];
    // client set itself's up & down traffic independently
    // AP set it's up taffic base on clients down traffic ( AP's up traffic is the golbal down traffic)
    // AP is node[1]
    
    for(int i=2;i<size_of_Nodelist;i++)
    {
        Nodelist[i].MAClayer.up_traffic= base_traffic*pow(10,3);
        Nodelist[i].MAClayer.down_traffic= base_traffic*pow(10,3);
    }
    Nodelist[1].MAClayer.up_traffic=0;
    Nodelist[1].MAClayer.down_traffic=0;
    for(int i=2;i<size_of_Nodelist;i++)
        Nodelist[1].MAClayer.up_traffic += Nodelist[i].MAClayer.down_traffic;
}

void Traffic::set_traffic(int traffic_input,int node_input)
{

    extern Node Nodelist[size_of_Nodelist];
    for(int i=2;i<node_input;i++)
    {
        Nodelist[i].MAClayer.up_traffic= base_traffic*pow(10,3)*5;
        Nodelist[i].MAClayer.down_traffic= base_traffic*pow(10,3)*5;
    }
    for(int i=node_input;i<size_of_Nodelist;i++)
    {
        Nodelist[i].MAClayer.up_traffic= base_traffic*pow(10,3)*25;
        Nodelist[i].MAClayer.down_traffic= base_traffic*pow(10,3)*25;
    }
    Nodelist[1].MAClayer.up_traffic=0;
    Nodelist[1].MAClayer.down_traffic=0;
    for(int i=2;i<size_of_Nodelist;i++)
        Nodelist[1].MAClayer.up_traffic += Nodelist[i].MAClayer.down_traffic;
}


#endif /* Traffic_h */
