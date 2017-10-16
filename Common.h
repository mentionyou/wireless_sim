//
//  Common.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/29.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include "Node.h"


#ifndef Common_h
#define Common_h


#define size_of_Nodelist (71)// 4 equal to 3 node evaluated,(1AP+2Node)
#define num_AP 5
#define num_client 13

#define SLOTS_TO_HANDLE_COLLISION 5
#define QUEUE_SIZE 10
#define CWMIN 64
#define CWMAX 1024
#define CWFD  16

#define ENDTIME pow(10,7)

#define  SLOT 9
#define  RATE 24
/// 24b in 1 us= 24Mbps
#define  DIFS 34
//#define  SIFS 10
#define  PHY_HEADER (4*13)
// 802.11a  4us each symbol, 12+1 symbol for PHY header
#define  MAC_HEADER (30*8/RATE)
#define  INTERFERENCE 500

//BEB 0
//BASE ON QUEUE LENGTH 1
#define  AP_BACKOFF_PATTERN 0
#define  CLIENT_BACKOFF_PATTERN 0
#define  AVOID_UNNESSARY_TRANSMISSION 0
#define ADHOC 9
#define WLAN  99
//#define TOPO WLAN
//#define APmulti 20


#endif /* common_h */
