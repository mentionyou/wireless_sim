//
//  Statistics.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2017-8-15.
//  Copyright © 2017年 段荣昌. All rights reserved.
//

#ifndef Statistics_h
#define Statistics_h

#include "Event.h"
#include "Node.h"
#include "MAC.h"
#include "math.h"
#include "Common.h"


#define Interval (pow(10,6)/2)


class Statistics
{
public:
    u_seconds interval= Interval;
    void record();
};

void Statistics::record()
{
    extern Node Nodelist[size_of_Nodelist];
    //int fairness[size_of_Nodelist];
    int fairness_up=0;
    int fairness_down=0;
    for (int i=num_AP+1+num_client*1; i<num_AP+1+num_client*2;i++)
//    for (int i=num_AP+1; i<size_of_Nodelist;i++)
    {
       MAC* mac= &(Nodelist[i].MAClayer);
        //fairness[i]=mac->delay_count;
        fairness_up += mac->delay_count;
        fairness_down +=mac->delay_count * mac->delay_count;
        if(mac->delay_sum==0)
            continue;
        //cout<<i<<":"<<mac->delay_sum/mac->delay_count<<"+"<<mac->delay_max<<" ";
        cout<<i<<":"<<mac->delay_count<<" ";
        mac->delay_sum=0;
        mac->delay_count=0;
        mac->delay_max=0;
    }
    cout<< "Fairness:"<< (double)(fairness_up*fairness_up)/(size_of_Nodelist*fairness_down)<<endl;
}


#endif /* Statistics_h */
