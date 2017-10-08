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
    if( (na->x-nb->x)*(na->x-nb->x) + (na->y-nb->y)*(na->y-nb->y) < INTERFERENCE)
        return true;
    return false;
}

void Topo::set_topo()
{
    extern Node Nodelist[size_of_Nodelist];
    int AP[5][2]={{1,1},{3,3},{5,5},{1,5},{5,1}};
    
    //int loc[11][2]={{0,0},{1,-2},{1,-1},{1,0},{1,1},{1,2},{-1,-2},{-1,-1},{-1,0},{-1,1},{-1,2}};
    //    for(int i=1;i<size_of_Nodelist;i++)
    //    {
    //        //        Nodelist[i].x=loc[i][0];
    //        //        Nodelist[i].y=loc[i][1];
    //    }
    
    for (int i=1; i<size_of_Nodelist;i++)
    {
        for (int j=1; j<size_of_Nodelist;j++)
        {
            if(i==j)
                continue;
            if(dist(i,j))
            {
                Nodelist[i].MAClayer.num_bour++;
                int k=Nodelist[i].MAClayer.num_bour;
                Nodelist[i].MAClayer.neighbour[k][0]=j;
            }
        }
    }
}
