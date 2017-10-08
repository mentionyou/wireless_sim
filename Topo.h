//
//  Topo.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2017-10-7.
//  Copyright © 2017年 段荣昌. All rights reserved.
//

#ifndef Topo_h
#define Topo_h

class Topo
{
public:
    Topo();
    ~Topo(){};
    bool dist(int,int);
    void start();
    void set_topo();
};


#endif /* Topo_h */
