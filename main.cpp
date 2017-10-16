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
#include <math.h>

using namespace std;

Node Nodelist[size_of_Nodelist]; // 0 is not used
Simulator sim;
random_number ran_generator;
int unique_id;
int t_queue;

int main(int argc, const char * argv[]) {
    
    unique_id=1;
    t_queue=0;// t_queue is used to trace the length of queue
    
//    int traffic_input= atoi(argv[1]);
//    int node_input=atoi(argv[2]);
//    cout<<"Traffic:"<<traffic_input<<" Nodes:"<<node_input<<endl;
//    cout<<" Nodes:"<<node_input<<endl;
    
    Topo* topo;
    topo->start();
    Traffic* traffic;
    traffic->start();

    
//    traffic->start(traffic_input,node_input+2);


    
    cout<<"Number of Nodes:"<<size_of_Nodelist-1<<endl;
    cout<<"Data arrival rate:"<<BASE_TRAFFIC <<endl;
    cout<<"CWmin:"<<CWMIN<<" CWmax:"<<CWMAX <<endl;
    cout<<"Queue size:"<< QUEUE_SIZE<<endl;
    cout<<"SLOTS:"<<SLOTS_TO_HANDLE_COLLISION<<endl;
    if(AP_BACKOFF_PATTERN==0)
        cout<<"Normal backoff pattern";
    else if(AP_BACKOFF_PATTERN ==1)
        cout<<"Dynamic queue backoff pattern";
    cout<<"\n";

    sim.run();
    
//    
//        success=0;
//        failed=0;
//        T_coll=0;
//        R_coll=0;
//        fd_op=0;
//        fd_suc=0;
//    
//    /T_COLL and R_COLL have same troubles.
//    
//    cout<<"Success:"<<success<<"; Failed:"<<failed<<"; T collided:"<<T_coll<< "; R collided:"<<R_coll<<"; FD_OP:"<<fd_op<<"; FD_SUC:"<<fd_suc<<endl;
//    cout<<"FD ratio:"<< ((double)fd_suc*2/(failed+success)) <<endl;
//    cout<<"collided ratio:"<<((double)(T_coll+R_coll)/(failed+success+T_coll +R_coll))<<endl;
//    
//    cout<<"\n"<<success<<" "<<((double)fd_suc*2/(failed+success))<<" "<< ((double)(T_coll+R_coll)/(failed+success+T_coll +R_coll)) <<endl<<endl;
    
    int suc=0;
    int coll=0;
    int p_suc=0;
    int s_suc=0;
    
    cout<<endl;
    for(int i=1;i<size_of_Nodelist;i++)
    {
        MAC* mac= & (Nodelist[i].MAClayer);
        suc+=mac->st_suc+mac->pt_suc;
        p_suc+=mac->pt_suc;
        s_suc+=mac->st_suc;
        coll+=mac->pt_coll;
        
       cout<<"Node"<<i<<":"<< mac->pt_suc+ mac->st_suc <<" PT_SUC:"<<mac->pt_suc<< "; PT_FD_SUC:"<<mac->pt_fd_suc <<"; PT_OP:"<<mac->pt_fd_op <<"; ST_SUC:"<<mac->st_suc <<" PT_COLL:"<<mac->pt_coll<<" Delay_sum:"<<mac->delay_sum;//<<"; ST_COLL:"<< mac->st_coll;
        cout<<endl;
        cout<<"\t\tDelay_avg: "<< (double) mac->delay_sum/(mac->pt_suc+ mac->st_suc) /pow(10,6) <<" Delay_max: "<< (double)mac->delay_max /pow(10,6) ;
        cout<<endl;
    }
    
    cout<<"SUC:"<< s_suc+p_suc<<"; P_SUC:"<< p_suc<<"; S_SUC:"<<s_suc<<"; FD ratio:"<<2*(double)s_suc/suc<< "; Coll ratio:"<< (double)coll/ (coll+p_suc) <<endl;
    
    DATA data;
    cout<<"Throughput:"<< ((double)s_suc+p_suc)/ENDTIME*data.payload<<endl;
    cout<<"FD ratio:"<<2*(double)s_suc/suc<<endl;
    
//    int tmp_suc=0;
//    int tmp_fd=0;
//    for(int i=2;i<node_input+2;i++)
//    {
//        MAC* mac= & (Nodelist[i].MAClayer);
//        tmp_suc+=mac->pt_suc+mac->st_fd_recv+mac->st_suc+mac->pt_fd_suc;
//        tmp_fd +=mac->pt_fd_suc+mac->st_suc;
//    }
//    cout<<"Throughput of low rate clients:"<< ((double)tmp_suc)/ENDTIME*data.payload/(node_input)<<endl;
//    cout<< "FD ratio of low rate clients:"<< 2*(double)tmp_fd/tmp_suc<<endl;
//    
//    tmp_suc=0;
//    tmp_fd=0;
//    for(int i=node_input+2;i<size_of_Nodelist;i++)
//    {
//        MAC* mac= & (Nodelist[i].MAClayer);
//        tmp_suc+=mac->pt_suc+mac->st_fd_recv+mac->st_suc+mac->pt_fd_suc;
//        tmp_fd +=mac->pt_fd_suc+mac->st_suc;
//    }
//    cout<<"Throughput of high rate clients:"<< ((double)tmp_suc)/ENDTIME*data.payload/(size_of_Nodelist-node_input-2)<<endl;
//    cout<< "FD ratio of high rate clients:"<< 2*(double)tmp_fd/tmp_suc<<endl;
    
    
    return 0;
}
