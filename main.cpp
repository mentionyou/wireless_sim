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
    
    unique_id=1;
    address tmp= (1<<10);
    int loc[11][2]={{0,0},{1,-2},{1,-1},{1,0},{1,1},{1,2},{-1,-2},{-1,-1},{-1,0},{-1,1},{-1,2}};
    
    for(int i=1;i<size_of_Nodelist;i++)
    {
        Nodelist[i].nodeid=i;
        Nodelist[i].MAClayer.m_adrress=tmp+i;
        Nodelist[i].x=loc[i][0];
        Nodelist[i].y=loc[i][1];
    }
    
    for (int i=1; i<size_of_Nodelist;i++)
    {
        for (int j=1; j<size_of_Nodelist;j++)
        {
            if(i==j)
                continue;
            if(dist_(i,j))
            {
                Nodelist[i].num_bour++;
                int k=Nodelist[i].num_bour;
                Nodelist[i].neighbour[k]=j;
            }
        }
        Nodelist[i].start(0);
    }
    
//    for (int i=1; i<size_of_Nodelist;i++)
//    {
//        cout<< Nodelist[i].num_bour<<" ";
//        cout<< Nodelist[i].neighbour[Nodelist[i].num_bour]<<endl ;
//    }
//    
    
    
    success=0;
    failed=0;
    T_coll=0;
    R_coll=0;
    fd_op=0;
    fd_suc=0;
    
    
    //    Event event;
    //    for(int i=1;i<size_of_Nodelist;i++)
    //    {
    //        cout<<i<<":";
    //        for(int j=1;j<size_of_Nodelist;j++)
    //        {
    //            cout<<dist_(i,j);
    //        }
    //        cout<<endl;
    //    }
    
    sim.run();
    
    cout<<"Success:"<<success<<"; Failed:"<<failed<<"; T collided:"<<T_coll<< "; R collided:"<<R_coll<<"; FD_OP:"<<fd_op<<"; FD_SUC:"<<fd_suc<<endl;
    cout<<"FD ratio:"<< ((double)fd_suc*2/(failed+success)) <<endl;
    cout<<"collided ratio:"<<((double)(T_coll+R_coll)/(failed+success+T_coll +R_coll))<<endl;
    return 0;
}
