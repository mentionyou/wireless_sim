//
//  Statistics.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 2017-10-26.
//  Copyright © 2017年 段荣昌. All rights reserved.
//

#include <stdio.h>
#include "Node.h"
#include "MAC.h"
#include "math.h"
#include "Statistics.h"

extern int number_of_clients_in_statistic;

//void Statistics::record()
//{
//    extern Node Nodelist[size_of_Nodelist];
//    //int fairness[size_of_Nodelist];
//    int fairness_up=0;
//    int fairness_down=0;
//    double delay_up=0;
//    double delay_down=0;
//    double delay_ap_avg=0;
//    double delay_client_sum=0;
//    double delay_client_count=0;
//    for (int i=1; i<=num_AP;i++)
//    {
//        MAC* mac= &(Nodelist[i].MAClayer);
//        if(mac->delay_sum==0)
//            continue;
//        delay_ap_avg=mac->delay_sum/mac->delay_count/pow(10,6);
////        cout<<"AP"<<i<<":"<<mac->delay_sum/mac->delay_count/pow(10,6)<<"+"<<mac->delay_max/pow(10,6)<<" "<<endl;
//        extern double avg_ap_delay;
//        avg_ap_delay +=delay_ap_avg;
//        mac->delay_sum=0;
//        mac->delay_count=0;
//        mac->delay_max=0;
//    }
//    
//    for (int i=num_AP+1+number_of_clients_in_statistic*0; i<=num_AP+number_of_clients_in_statistic*1;i++)
//    {
//        MAC* mac= &(Nodelist[i].MAClayer);
//        fairness_up += mac->delay_count;
//        fairness_down +=mac->delay_count * mac->delay_count;
//        delay_up +=(double)mac->delay_sum/mac->delay_count/pow(10,6);
//        delay_down +=((double)mac->delay_sum/mac->delay_count/pow(10,6)) *((double)mac->delay_sum/mac->delay_count/pow(10,6));
//        if(mac->delay_sum==0)
//            continue;
////        cout<<i<<":"<<mac->delay_sum/mac->delay_count/pow(10,6)<<"+"<<mac->delay_max/pow(10,6)<<" "<< mac->neighbour[1][4]<<endl;
//        delay_client_sum +=(double)(mac->delay_sum)/pow(10,6);
//        delay_client_count += mac->delay_count;
//        mac->delay_sum=0;
//        mac->delay_count=0;
//        mac->delay_max=0;
//    }
////    cout<< "Fairness:"<< (double)(fairness_up*fairness_up)/(1*number_of_clients_in_statistic*fairness_down)<<endl;
////    cout<< "Delay fairness:"<< (double)(delay_up*delay_up)/(1*number_of_clients_in_statistic*delay_down)<<endl;
//    extern double avg_fair;
//    avg_fair += (double)(fairness_up*fairness_up)/(1*number_of_clients_in_statistic*fairness_down);
//    extern double avg_client_delay;
//    avg_client_delay += delay_client_sum/delay_client_count;
//}