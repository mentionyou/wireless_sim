//
//  Statistics.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2017-8-15.
//  Copyright © 2017年 段荣昌. All rights reserved.
//

#ifndef Statistics_h
#define Statistics_h

#include "math.h"
#include "Common.h"
#include "Event.h"

#define Interval (pow(10,6))


class Statistics
{
public:
    u_seconds interval= Interval;
    void record();
};



#endif /* Statistics_h */
