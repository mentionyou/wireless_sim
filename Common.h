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


#define  TEST 0 //debug记录
#define  STATISTIC 1 //统计数据记录
#define  TOPO_MODE    0 // Number of WLANS
#define  ST_CW_FIX 0 // ST 是否重置退避值，1重制，0不重制
#define  AP_queue_manage 0 //未完整实现，保持0


#define  size_of_Nodelist (10+1)// 4 equal to 3 node evaluated,(1AP+2Node)
//#define  num_AP 2
//#define  num_client 5


#define  SLOTS_TO_HANDLE_COLLISION 1
#define  QUEUE_SIZE 50
#define  CWMIN 32
#define  CWMAX 1024
//#define  CWFD  512

#define  ENDTIME pow(10,7)

//802.11g 24mbps
//96 bit per symbol, 4 us
//24b in 1 us= 24Mbps

#define  SLOT 9
#define  RATE 24
#define  DIFS 28
#define  SIFS 10

#define  PHY_HEADER 20 // fixed in 802.11 g
#define  MAC_HEADER ( 12 ) // 30B=240bits -> 3symbol -> 12 us
#define  PHY_Extension 6
// (not used) 802.11a  4us each symbol, 12+1 symbol for PHY header

#define  DATA_time (20 + 128*4 + 6)  //  PHY_HEADER+(Payload  1500B + MAC header + CRC 4B) /rate -> 128symbols  + PHY_Extension
#define  ACK_time (20+2*4 +6) //   PHY_Header+(14B=112bits -> 2symbol) + PHY_Extension



#define  INTERFERENCE 2000

#define  AP_BACKOFF_PATTERN 1 //BEB 0 //BASE ON QUEUE LENGTH 1
#define  CLIENT_BACKOFF_PATTERN 1
#define  FUMAC 0
#define  AVOID_UNNESSARY_TRANSMISSION 0
#define  ADHOC 9
#define  WLAN  99

#endif /* common_h */
