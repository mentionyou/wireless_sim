//
//  main.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 16/10/17.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include <iostream>
#include "Event.h"
#include "Simulator.h"
#include "Node.h"
#include "Packet.h"
#include "MAC.h"
#include "Common.h"
#include <cmath>


using namespace std;

Node Nodelist[size_of_Nodelist]; // 0 is not used
Simulator sim;
random_number ran_generator;
int unique_id;
int success,failed,collided,fd;
int loc[11][2]={ {0,0},{1,-2},{1,-1},{1,0},{1,1},{1,2},{-1,-2},{-1,-1},{-1,0},{-1,1},{-1,2} };


int main(int argc, const char * argv[]) {
    
    unique_id=1;
    address tmp= (1<<10);
    int loc[11][2];
    
    for(int i=1;i<size_of_Nodelist;i++)
    {
        Nodelist[i].nodeid=i;
        Nodelist[i].MAClayer.m_adrress=tmp+i;
        Nodelist[i].x=loc[i][0];
        Nodelist[i].y=loc[i][1];
        Nodelist[i].start(0);
    }
    
    success=0;
    failed=0;
    collided=0;
    fd=0;
    
    //Start sim
    sim.run();
    
    cout<<"success:"<<success<<"; failed:"<<failed<<"; collided:"<<collided<<"; FD:"<<fd<<endl;
    cout<<"FD ratio:"<< ((double)fd/(failed+success)) <<endl;
    cout<<"collided ratio:"<<((double)collided/(failed+success+collided))<<endl;
    return 0;
}
