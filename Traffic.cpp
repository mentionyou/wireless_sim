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


//int base_traffic = BASE_TRAFFIC;

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
    for (int i=1; i< size_of_Nodelist; i++)
    {
        Nodelist[i].MAClayer.up_traffic = 0;
        Nodelist[i].target_node=0;
    }
    
    Nodelist[1].MAClayer.up_traffic = 500 * pow(10,3);
    Nodelist[1].target_node=size_of_Nodelist-1;
    Nodelist[size_of_Nodelist-1].MAClayer.up_traffic = 500 * pow(10,3);
    Nodelist[size_of_Nodelist-1].target_node=1;
}
