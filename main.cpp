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
#include "Event.h"
#include "Simulator.h"
#include "Node.h"
#include "Packet.h"
#include "MAC.h"
#include <math.h>

using namespace std;

Node Nodelist[size_of_Nodelist]; // 0 is not used
Simulator sim;
random_number ran_generator;
int unique_id;
int success,failed,T_coll,R_coll,fd_op,fd_suc;

bool dist_(int a, int b) // nodeid a,b
{
    extern Node Nodelist[size_of_Nodelist];
    Node* na= &(Nodelist[a]);
    Node* nb= &(Nodelist[b]);
    if( (na->x-nb->x)*(na->x-nb->x) + (na->y-nb->y)*(na->y-nb->y) < 5)
        return true;
    return false;
}

int main(int argc, const char * argv[]) {
    
//    freopen("outa.txt","w",stdout);
    
    unique_id=1;
    address tmp= (1<<10);
    //int loc[11][2]={{0,0},{1,-2},{1,-1},{1,0},{1,1},{1,2},{-1,-2},{-1,-1},{-1,0},{-1,1},{-1,2}};
    
    
    for(int i=1;i<size_of_Nodelist;i++)
    {
        Nodelist[i].nodeid=i;
        Nodelist[i].MAClayer.m_adrress=tmp+i;
        Nodelist[i].x=0;
        Nodelist[i].y=0;
//        Nodelist[i].x=loc[i][0];
//        Nodelist[i].y=loc[i][1];
    }
    
    for (int i=1; i<size_of_Nodelist;i++)
    {
        for (int j=1; j<size_of_Nodelist;j++)
        {
            if(i==j)
                continue;
            if(dist_(i,j))
            {
                Nodelist[i].MAClayer.num_bour++;
                int k=Nodelist[i].MAClayer.num_bour;
                Nodelist[i].MAClayer.neighbour[k][0]=j;
            }
        }
        Nodelist[i].start(0);
    }
    
    
    success=0;
    failed=0;
    T_coll=0;
    R_coll=0;
    fd_op=0;
    fd_suc=0;
    
    cout<<"Number of Nodes:"<<size_of_Nodelist-1<<"\nData arrival rate:"<<PACKETS_IN_1_SECONDS<<"\nCWmin:"<<CWMIN <<endl;
    cout<<"Queue size:"<< QUEUE_SIZE<<endl;
    
    sim.run();
    
    ///T_COLL and R_COLL have same troubles.
    
    cout<<"Success:"<<success<<"; Failed:"<<failed<<"; T collided:"<<T_coll<< "; R collided:"<<R_coll<<"; FD_OP:"<<fd_op<<"; FD_SUC:"<<fd_suc<<endl;
    cout<<"FD ratio:"<< ((double)fd_suc*2/(failed+success)) <<endl;
    cout<<"collided ratio:"<<((double)(T_coll+R_coll)/(failed+success+T_coll +R_coll))<<endl;
    
    cout<<"\n"<<success<<" "<<((double)fd_suc*2/(failed+success))<<" "<< ((double)(T_coll+R_coll)/(failed+success+T_coll +R_coll)) <<endl<<endl;
    
    return 0;
}
