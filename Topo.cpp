//
//  Topo.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 2017-10-8.
//  Copyright © 2017年 段荣昌. All rights reserved.
//

#include "Topo.h"
#include "Node.h"

void Topo::start()
{
    this->set_topo();
}


bool Topo::dist(int a, int b) // nodeid a,b
{
    extern Node Nodelist[size_of_Nodelist];
    Node* na= &(Nodelist[a]);
    Node* nb= &(Nodelist[b]);
    double tmp=(na->x-nb->x)*(na->x-nb->x) + (na->y-nb->y)*(na->y-nb->y);
    if( tmp < INTERFERENCE)
    {
        return true;
    }
    return false;
}

void Topo::set_topo()
{
    extern Node Nodelist[size_of_Nodelist];
    
    address tmp= (1<<10);
    for(int i=1;i<size_of_Nodelist;i++)
    {
        Nodelist[i].nodeid=i;
        Nodelist[i].MAClayer.m_adrress=tmp+i;
    }

    for (int i=1; i< size_of_Nodelist; i++)
    {
        Nodelist[i].x= 40*i;
        Nodelist[i].y= 0;
    }
    
    extern int route_table[size_of_Nodelist][size_of_Nodelist];
    // 1->2->3->4->5
    for (int i=1; i< size_of_Nodelist ;i++)
    {
        if (i!= num_client)
            route_table[i][5]= i+1 +(1<<10);
    }
    // 1<-2<-3<-4<-5
    for (int i=1; i< size_of_Nodelist ;i++)
    {
        if (i!= 1)
            route_table[i][1]= i-1 + (1<<10);
    }
    
}
