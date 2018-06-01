//
//  Common.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/29.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include "Node.h"


#ifndef  Common_h
#define  Common_h


#define  TEST 0
//debug记录
#define  STATISTIC 1
//统计数据记录
#define  TOPO_MODE    0
// Number of WLANS
#define  ST_CW_FIX 0
// ST 是否重置退避值，1重制，0不重制
#define  AP_queue_manage 0
//未完整实现，保持0


#define  size_of_Nodelist (2*25+2+1)// 4 equal to 3 node evaluated,(1AP+2Node)
#define  num_AP 2
#define  num_client 25


#define  SLOTS_TO_HANDLE_COLLISION 5
#define  QUEUE_SIZE 50
#define  CWMIN 64
#define  CWMAX 512
#define  CWFD  512

#define  ENDTIME pow(10,8)

#define  SLOT 9
#define  RATE 24/// 24b in 1 us= 24Mbps
#define  DIFS 34 
#define  SIFS 16
#define  PHY_HEADER (4*13) // 802.11a  4us each symbol, 12+1 symbol for PHY header
#define  MAC_HEADER (30*8/RATE)
#define  INTERFERENCE 1000

#define  AP_BACKOFF_PATTERN 1 //BEB 0 //BASE ON QUEUE LENGTH 1
#define  CLIENT_BACKOFF_PATTERN 1
#define  FUMAC 0
#define  AVOID_UNNESSARY_TRANSMISSION 0
#define  ADHOC 9
#define  WLAN  99

#endif /* common_h */
