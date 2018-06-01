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
    double tmp=(na->x-nb->x)*(na->x-nb->x) + (na->y-nb->y)*(na->y-nb->y);
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
    
//    double AP[5][2]={{0,0},{40,0},{80,0},{999,-999},{-999,-999}};
    double AP[10][2]={{43.38,  88.128},{ 32.204,  48.402},{ 24.583, 5.095},{ 47.981, 72.109},{ 21.632,  63.981} ,{64.988, 45.839},{33.9,  59.241},{ 74.99,  87.502},{ 32.115,  98.192},{ 38.573, 51.198}};
    
    double client_x[150]={17.293,  -7.129,  -1.16 , -18.558,  -8.606, -19.743, -24.924,
        5.521,  15.352,  12.337, -14.745,   0.023,   1.621, -17.644,
        -0.274,   6.228,  -1.89 ,  -7.234, -17.704,   1.518,   3.39 ,
        12.659,   1.867,   9.752, -23.295,  19.399, -10.991,   9.221,
        7.223,   9.135,   0.383, -26.861,   0.982,   9.179,   6.928,
        3.814,  -3.648,  -7.42 ,   7.462,  -0.612,  -8.022,  -0.347,
        -21.993,  18.09 ,   0.987,  -0.525,  -1.813,  12.921, -13.183,
        -13.813,  -1.456,  -1.202, -17.714,  -7.782, -14.202,   3.635,
        -18.184,  19.9  ,  -0.   ,  -1.86 ,  -1.396,  -7.99 , -26.379,
        -2.431,  15.5  ,  -1.483, -20.7  ,  26.767, -12.844, -11.165,
        9.499, -11.197,  -4.241,  15.266,   7.808,  -3.352,   3.972,
        0.625,  10.808,   0.644,  19.411,   0.547,   0.484,  -4.084,
        -2.804,  -3.935,  11.251,   5.739, -28.828,   2.929,  -8.455,
        17.914,  12.998, -11.996,  23.24 ,  15.733,   7.352,   2.441,
        0.004,  14.421,  -4.697,   1.177,   2.839,  -6.997,   4.125,
        26.393,  -2.347,  -2.461,  13.706,  10.631, -13.317,  19.998,
        -0.902,   4.464,  -7.777,  15.891,  -7.216, -11.406, -13.869,
        2.06 , -17.616, -15.545, -28.131,   3.091,  27.242,  16.561,
        -19.839,  23.112,  19.859, -14.121,  -4.999,  -0.903, -21.899,
        -4.262,  24.429,  -9.62 , -12.246, -13.714, -16.457,  -5.877,
        10.675,  -1.694,  28.42 ,  21.403,  -2.951,   0.012,   2.479,
        20.281,   2.335, -23.572};
    double client_y[150]={1.11 ,   7.041,  26.659, -14.   ,   1.433,  -0.125,  14.34 ,
        -6.165,  -2.444, -17.892,  11.402,  -8.786, -30.167,  14.671,
        21.237, -22.618,  14.098, -19.985,   1.792, -23.861,  14.857,
        -25.393, -20.496,   5.74 ,  15.417,  -4.348,  13.113,  14.386,
        -3.36 ,  -1.086,  12.214,  -4.404,  25.614,  21.649,  -4.409,
        -19.661,   0.909,  -7.794,  16.118,  21.302,   4.162, -17.23 ,
        -6.857,  -8.958,  -1.529,   8.809,   1.59 ,  -9.508,   0.223,
        11.215, -16.977, -26.369,  -4.211,  10.214, -14.954,   2.729,
        -1.279, -10.647,  -7.48 ,  -3.357, -10.157,  20.78 ,   0.197,
        10.721,  -4.757,   6.104,   3.179,   7.888, -16.713,  -1.945,
        -6.798,   1.76 ,   7.943,   5.098,  14.906,  -6.492,   1.334,
        -6.374,   0.04 ,  18.615,   0.453,   0.574, -16.185,  24.562,
        17.062,  18.855,  -5.374,  12.489,   0.439,   2.888,  11.82 ,
        -1.532,  13.966,  12.496,   5.582,  -2.779,  19.443, -13.526,
        13.812, -27.686, -12.011,  16.296,  -1.341, -14.895,   0.092,
        -5.596,  -8.32 ,  16.093,   1.32 ,  28.237,  -7.876,  -0.591,
        -19.168,  23.295,  15.34 ,   1.264,  -4.869,   4.294,   3.25 ,
        16.493,  24.925,  -0.385,   1.656,  10.431,  -8.953,  -2.393,
        19.604,  -2.595,   1.679,   1.797,  -2.37 ,  14.825,   1.874,
        5.374,  -6.181,  -6.095,   8.517,  21.427,   3.131,  -0.99 ,
        -0.753,  21.689,   0.106, -15.005,   1.59 ,  20.258,  -2.902,
        -3.054,  12.8  ,  -6.534};
    
//    double client[25][2]=;
//    默认有25个用户，需要时进行更改。
//    
//    for (int i=-2;i<=2;i++)
//    {
//        for (int j=-2;j<=2;j++)
//        {
//            client[5*(i+2)+(j+2)][0]=j*10;
//            client[5*(i+2)+(j+2)][1]=i*10;
//        }
//    }
    
//    int client [100][2];
//    memset(client, 0, sizeof(int)*2*100);

    for (int i=1;i<=num_AP;i++)
    {
        Nodelist[i].x=AP[i-1][0];
        Nodelist[i].y=AP[i-1][1];
        Nodelist[i].node_type=1;
        for(int j=1;j<=num_client;j++)
        {
            int tmp=num_AP+num_client*(i-1)+j;
            Nodelist[tmp].x=AP[i-1][0]+client_x[(i)*25+j-1];
            Nodelist[tmp].y=AP[i-1][1]+client_y[(i)*25+j-1];

//            Nodelist[tmp].x=AP[i-1][0]+client[j-1][0];
//            Nodelist[tmp].y=AP[i-1][1]+client[j-1][1];
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
    
    
    for (int i=1+num_AP;i<size_of_Nodelist;i++)
    {
        if(!dist(i, Nodelist[i].assocaition_AP_id))
            cout<<"NODE"<<i<<" x:"<<Nodelist[i].x<<" y:"<<Nodelist[i].y<<endl;
    }
//    ;
//    for(int i=1;i<size_of_Nodelist;i++)
//    {
//        cout<<"NODE"<<i<<":"<<Nodelist[i].x<<" "<<Nodelist[i].y<<endl;
//        cout<<"INTERFER:"<< Nodelist[i].num_interferation_node<< " Asso:"<< Nodelist[i].num_assocaition_node<<endl;
//    }
//    ;
}
