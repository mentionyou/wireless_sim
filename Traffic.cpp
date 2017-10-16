//
//  Traffic.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 2017-10-9.
//  Copyright © 2017年 段荣昌. All rights reserved.
//

#include <stdio.h>
#include "math.h"
#include "Traffic.h"
#include "Common.h"


int base_traffic = BASE_TRAFFIC;

void AP_traffic(int AP_id);

void Traffic::start()
{
    extern Node Nodelist[size_of_Nodelist];
    this->set_traffic();
    for (int i=1; i<size_of_Nodelist;i++)
        Nodelist[i].start(0);
}

void Traffic::set_traffic()
{
    extern Node Nodelist[size_of_Nodelist];
    // client set itself's up & down traffic independently
    // AP set it's up taffic base on clients down traffic ( AP's up traffic is the golbal down traffic)
    // AP is node[1]
    
    int i=1+num_AP;
    for(;i<1+num_AP+num_client*2;i++)
    {
        Nodelist[i].MAClayer.up_traffic= base_traffic*pow(10,3);
        Nodelist[i].MAClayer.down_traffic= base_traffic*pow(10,3);
    }
    for(;i<size_of_Nodelist;i++)
    {
        Nodelist[i].MAClayer.up_traffic= 1;
        Nodelist[i].MAClayer.down_traffic= 1;
    }
    
    for(int i=1; i<1+num_AP;i++)
    {
        Nodelist[i].MAClayer.up_traffic=0;
        Nodelist[i].MAClayer.down_traffic=0;
        AP_traffic(i);
    }
}

void AP_traffic(int AP_id)
{
    extern Node Nodelist[size_of_Nodelist];
    Node* AP= & (Nodelist[AP_id]);
    int count_1= AP->num_assocaition_node;
    int count_2=0;
    for(int i=1;i<size_of_Nodelist;i++)
    {
        if(AP->assocaition_node[i])
        {
            count_2++;
            AP->MAClayer.neighbour[count_2][1]=Nodelist[i].MAClayer.down_traffic;
            AP->MAClayer.up_traffic += Nodelist[i].MAClayer.down_traffic;
        }
    }
    if(count_2!=count_1)
    {
        cout<<"Traffic setting error"<<endl;
        exit(-1);
    }
}






//void Traffic::start(int traffic_input,int node_input)
//{
//    extern Node Nodelist[size_of_Nodelist];
//    this->set_traffic(traffic_input,node_input);
//    for (int i=1; i<size_of_Nodelist;i++)
//        Nodelist[i].start(0);
//}


//void Traffic::set_traffic(int traffic_input,int node_input)
//{
//    
//    extern Node Nodelist[size_of_Nodelist];
//    for(int i=2;i<node_input;i++)
//    {
//        Nodelist[i].MAClayer.up_traffic= base_traffic*pow(10,3)*5;
//        Nodelist[i].MAClayer.down_traffic= base_traffic*pow(10,3)*5;
//    }
//    for(int i=node_input;i<size_of_Nodelist;i++)
//    {
//        Nodelist[i].MAClayer.up_traffic= base_traffic*pow(10,3)*25;
//        Nodelist[i].MAClayer.down_traffic= base_traffic*pow(10,3)*25;
//    }
//    Nodelist[1].MAClayer.up_traffic=0;
//    Nodelist[1].MAClayer.down_traffic=0;
//    for(int i=2;i<size_of_Nodelist;i++)
//        Nodelist[1].MAClayer.up_traffic += Nodelist[i].MAClayer.down_traffic;
//}
