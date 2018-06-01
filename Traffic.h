//
//  Traffic.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2017-8-15.
//  Copyright © 2017年 段荣昌. All rights reserved.
//

#ifndef Traffic_h
#define Traffic_h



#include "Node.h"

#define BASE_TRAFFIC 200

//#define PACKETS_IN_1_SECONDS  5
class Traffic
{
public:
    Traffic();
    ~Traffic(){};
    void start();
//    void start(int,int);
    void set_traffic();
//    void set_traffic(int,int);
};



#endif /* Traffic_h */
