//
//  PHY.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include "PHY.h"
#include "Event.h"
#include "Node.h"
#include <iostream>

using namespace std;

PHY::PHY()
{
    this->rx_state=PHY_IDLE;
    this->tx_state=PHY_IDLE;
    this->node=NULL;
    this->index1=0;
    this->index2=0;
}


void PHY::phy_send()
{
    if(tx_state==PHY_IDLE)
        tx_state=PHY_BUSY;
    else
    {
        cout<<"PHY::send error"<<endl;
        exit(-1);
    }
}

void PHY::phy_receive(const Event & event)
{
    if(rx_state==PHY_IDLE)
        rx_state=PHY_BUSY;
    else if (rx_state==PHY_BUSY)
        rx_state=PHY_COLLISION;
    else if(rx_state==PHY_COLLISION)
        rx_state=PHY_COLLISION;
    
    int id=event.nodeid-1;
    if(id<=63)
        index1= index1  | ( (unsigned long) 1<<id );
    else if(id<=127)
        index2 = index2 | ( (unsigned long) 1<<(id-64) );
    else
    {
        cout<<"Number of node out of range"<<endl;
        exit(-1);
    }
    
}

void PHY::phy_send_end()
{
    if (tx_state==PHY_BUSY) {
        tx_state=PHY_IDLE;
    }
}

void PHY::phy_receive_end(const Event & event)
{
    int id=event.nodeid-1;
    if(id<=63)
        index1= index1 & (~((unsigned long)1<<id));
    else if(id<=127)
        index2 = index2 & (~((unsigned long)1<< (id-64) ));
    else
    {
        cout<<"Number of node out of range"<<endl;
        exit(-1);
    }
    
    if(index1==0 && index2==0 )
        rx_state=PHY_IDLE;
}

