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
    int tmp=(na->x-nb->x)*(na->x-nb->x) + (na->y-nb->y)*(na->y-nb->y);
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
    
    int AP[5][2]={{0,0},{40,0},{999,999},{999,-999},{-999,-999}};
    int client[13][2]={{0,0},{-10,10},{-10,-10},{10,-10},{10,10},{-20,20},{-20,0},{-20,-20}, {0,-20},{20,-20},{20,0},{20,20},{0,20}};
    
    for (int i=1;i<=num_AP;i++)
    {
        Nodelist[i].x=AP[i-1][0];
        Nodelist[i].y=AP[i-1][1];
        Nodelist[i].node_type=1;
        for(int j=1;j<=num_client;j++)
        {
            int tmp=num_AP+num_client*(i-1)+j;
            Nodelist[tmp].x=AP[i-1][0]+client[j-1][0];
            Nodelist[tmp].y=AP[i-1][1]+client[j-1][1];
            Nodelist[tmp].node_type=2;
            Nodelist[tmp].assocaition_node[i]=true;
            Nodelist[tmp].num_assocaition_node=1;
            Nodelist[tmp].assocaition_AP_id=i;
            Nodelist[tmp].MAClayer.num_bour=1;
            Nodelist[tmp].MAClayer.neighbour[1][0]=i;
            
            
            Nodelist[i].assocaition_node[tmp]=true;
            Nodelist[i].num_assocaition_node++;
            Nodelist[i].MAClayer.num_bour++;
            int tmp_val=Nodelist[i].MAClayer.num_bour;
            Nodelist[i].MAClayer.neighbour[tmp_val][0]=tmp;
        }
    }
    
    
    for (int i=1; i<size_of_Nodelist;i++)
    {
        for (int j=1; j<size_of_Nodelist;j++)
        {
            if(i==j)
                continue;
            if(dist(i,j))
            {
                Nodelist[i].num_interferation_node++;
                Nodelist[i].interferation_node[j]=true;
            }
        }
    }

}
