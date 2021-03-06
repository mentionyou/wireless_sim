//
//  main.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 16/10/17.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include "Event.h"
#include "Simulator.h"
#include "Node.h"
#include "Packet.h"
#include "MAC.h"
#include "Traffic.h"
#include "Topo.h"
#include "Statistics.h"
#include <math.h>

using namespace std;

Node Nodelist[size_of_Nodelist]; // 0 is not used
Simulator sim;
random_number ran_generator;
int route_table[size_of_Nodelist][size_of_Nodelist];
int unique_id;
//double avg_fair=0;
//double avg_client_delay=0;
//double avg_ap_delay=0;
//int number_of_clients_in_statistic=0;
int overall_suc,overall_drop;

int main(int argc, const char * argv[]) {
    
    unique_id=1;
//    int traffic_input=1000;
//    traffic_input= atoi(argv[1]);
//    cout<<"Traffic:"<<traffic_input<<endl;
//    int clients=50;
//    clients = atoi(argv[1]);
//    cout<<"clients:"<<clients<<endl;
//    number_of_clients_in_statistic=num_client;
//    cout<<"NODE NUMBER:"<<number_of_clients_in_statistic<<endl;
    
    
    Topo* topo;
    topo->start();
    Traffic* traffic;
    traffic->start();
    
    
    if (STATISTIC) {
        cout<<"Number of Nodes:"<<size_of_Nodelist-1<<endl;
//        cout<<"Data arrival rate:"<<BASE_TRAFFIC <<endl;
        cout<<"CWmin:"<<CWMIN<<" CWmax:"<<CWMAX <<endl;
        cout<<"Queue size:"<< QUEUE_SIZE<<endl;
        cout<<"SLOTS:"<<SLOTS_TO_HANDLE_COLLISION<<endl;
        cout<<"CWmin & CWmax"<<CWMIN <<" "<<CWMAX<<endl;
//        if(AP_BACKOFF_PATTERN==0)
//            cout<<"Normal backoff pattern";
//        else if(AP_BACKOFF_PATTERN ==1)
//            cout<<"Dynamic queue backoff pattern";
        cout<<"\n";
    }
    
//    overall_suc=0;
//    overall_drop=0;

    sim.run();
//    cout<<sizeof(int)<< sizeof(long)<<sizeof(long long)<<endl;
    
    int suc=0;
    int coll=0;
//    int p_suc=0;
//    int s_suc=0;
    
    cout<<endl;
    
    for(int i=1;i < size_of_Nodelist;i++)
    {
        MAC* mac = &(Nodelist[i].MAClayer);
        int ssuc=0;
        int scoll=0;
        ssuc +=  mac->HD_pt_suc + mac->BFD_pt_suc + mac->BFD_st_suc + mac->DBFD_pt_suc +mac->DBFD_st_suc;
        scoll += mac->PT_coll + mac->HD_pt_coll+ mac->HD_pr_coll + mac->BFD_pt_coll + mac->BFD_st_coll + mac->DBFD_pt_coll + mac->DBFD_st_coll;
        cout<<"NODE"<<i<<endl;
        cout<<"Throughput:"<< ((double)ssuc)/ENDTIME*1500*8<<endl;
        cout<<"FD ratio:"<<(double)(ssuc- mac->HD_pt_suc)/ssuc<<endl;
        cout<<"BFD ratio:"<<(double)(mac->BFD_pt_suc+mac->BFD_st_suc)/ssuc<<endl;
        cout<<"COLL ratio:"<<(double)(scoll)/(scoll+ssuc)<<endl;
        cout<<"RECV suc:"<<((double)mac->recv_suc)/ENDTIME*1500*8<<endl;
        cout<<"RECV drop:"<<((double)mac->recv_drop)/ENDTIME*1500*8<<endl;
        
//        p_suc+=mac->pt_suc;
//        s_suc+=mac->st_suc;
//        coll +=mac->pt_coll;
//        if(STATISTIC)
//        {
//            if(i>TOPO_MODE && i<=num_AP)
//                continue;
//            cout<<endl<<"Node"<<i<<":"<< mac->pt_suc+ mac->st_suc<<endl <<" PT_SUC:"<<mac->pt_suc<< "; PT_FD_SUC:"<<mac->pt_fd_suc <<"; PT_OP:"<<mac->pt_fd_op <<"; ST_SUC:"<<mac->st_suc <<" PT_COLL:"<<mac->pt_coll<<"; ST_COLL:"<< mac->st_coll<<"; PT_COLLsignal_COLL:"<<mac->pt_collsignal_coll<<"; ST_COLLsignal_COLL:"<<mac->st_collsignal_coll <<" avg Attempts:"<< (double)(mac->pt_suc+mac->pt_coll)/mac->pt_suc ;
//            cout<<endl;
//        }
//        cout<<"\t\tDelay_avg: "<< (double) mac->delay_sum/(mac->pt_suc+ mac->st_suc) /pow(10,6) <<" Delay_max: "<< (double)mac->delay_max /pow(10,6) ;
//        cout<<endl;
    }
    
//    cout<<"SUC:"<< s_suc+p_suc<<"; P_SUC:"<< p_suc<<"; S_SUC:"<<s_suc<<"; FD ratio:"<<(double)s_suc/p_suc<< "; Coll ratio:"<< (double)coll/ (coll+p_suc) <<endl;
    
//    cout<<"Throughput:"<< ((double)s_suc+p_suc)/ENDTIME*1500<<endl;
//    cout<<"FD ratio:"<<(double)s_suc/p_suc<<endl;
//    cout<< "Fairness:"<< avg_fair/( ENDTIME/ Interval)<<endl;
//    cout<< "AP Delay:"<< avg_ap_delay/( ENDTIME/ Interval)<<endl;
//    cout<< "Client Delay:"<< avg_client_delay/( ENDTIME/ Interval)<<endl<<endl ;
    
    

    
    return 0;
}
