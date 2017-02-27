//
//  MAC.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#ifndef MAC_h
#define MAC_h


#include "Packet.h"
#include "Event.h"
#include "Packet.h"
#include <queue>


#define MAC_IDLE 0

#define BFD_PT 11
#define BFD_ST 12

#define DBFD_PT 21 //simple——T
#define DBFD_ST 22
#define DBFD_SR 23

#define SBFD_PT 31
#define SBFD_ST 32
#define SBFD_PR 33

#define MAC_BUSY 100



//#define MAC_OPEN_TO_CHANGE 4

using namespace std;

class MAC
{
public:
    MAC();
    ~MAC(){};
    
    vector <DATA> m_queue;
    vector<DATA>::iterator iter;
    address m_adrress;
    address peer; // the node, expeceted to receive DATA from
    address dst; // the node,  expeceted to receive ACK from
    
    void* node;
    
    int state;
    
    bool freeze_flag;
    int backoff_count;
    void freeze(int);
    
    bool to_send_ack;
    bool to_busy;
    bool to_T_coll;
    bool to_FD;
    
    
    void mac_generate_send_data_event(u_seconds);
    void mac_generate_send_data_event();
    void mac_generate_send_data_collision_event(u_seconds);
    void mac_generate_send_ack_event(u_seconds);
    void mac_generate_send_ack_collision_event(u_seconds);
    void mac_generate_inner_node_event();
    
    
    bool mac_is_collision(address);
    bool have_data(address);
    
    void mac_generate_data();
    void mac_pop_data();
    
    
    void mac_send_data(); // action is related with MAC state
    void mac_send_data_end();
    void mac_send_data_collision();
    void mac_send_ack();
    void mac_send_ack_end();
    void mac_send_ack_collision();
    
    
    void mac_receive_data(const DATA&);
    void mac_receive_data_end(const DATA&);
    void mac_receive_data_collision(const Event&);
    void mac_receive_ack(const ACK &);
    void mac_receive_ack_end(const ACK &);
    void mac_receive_ack_collision(const Event &);
    
    
    DATA get_data();
    ACK  get_ack();
   
    ///// state motivated
    
    
private:
    int CWmin=32;
    int CWmax=256;
    int CW;
};

class random_number
{
public:
    random_number();
    int ran();
};

#endif /* MAC_h */
