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
#include "Common.h"
#include <queue>


//MAC layer type, as well as data/busytone packet type

#define MAC_IDLE 0

#define BFD_PT 11
#define BFD_ST 12
#define DBFD_PT 21 //simple——T
#define DBFD_ST 22
#define DBFD_SR 23
#define SBFD_PT 31
#define SBFD_ST 32
#define SBFD_PR 33
#define BUSYTONE 41
#define MAC_BUSY 100


#define MODE_HD  111
#define MODE_FD  222



//#define MAC_OPEN_TO_CHANGE 4

using namespace std;


class MAC
{
public:
    MAC();
    ~MAC(){};
    void* node;
    vector <DATA> m_queue;
    vector<DATA>::iterator iter;
    address m_adrress;
    int state;
    int mode; // 0 IDLE; 1 HD;2 FD
    address peer; // the node, expeceted to receive DATA from
    address dst; // the node,  expeceted to receive ACK from
    
    int backoff_count;
    int num_bour;
    int neighbour[size_of_Nodelist][4];
    //[0], neighbour node id;
    //[1], neighbour node traffic
    //[2], does the node have data for me?
    //[3], does the node have data to send to another node, in order to build FD?
    
    
    
    void freeze(int);
    bool freeze_flag;   //// a flag indicate whether the mac freeze backoff count or not
    bool busytone_flag; //// a flag indicate whether the node sends busytone or not
    
    bool to_sim_coll;
    bool to_send_coll; //// a flag to indicate whether to send data/ack coll.
    bool to_send_ack;  //// a flag to indicate when to send ack. '1' : sends ack after send data end. '0' :sends busytone after send
    bool to_busy;      //// a flag to set a node into MAC_BUSY mode.
    bool to_cwfix;     //// a flag to indicate cw not increase when collide.
    
public:
    void mac_generate_send_data_event();
    void mac_generate_send_data_event(u_seconds);
    void mac_generate_send_data_end_event(u_seconds);
    void mac_generate_send_data_collision_event(u_seconds);
    void mac_generate_send_ack_event(u_seconds);
    void mac_generate_send_ack_end_event(u_seconds);
    void mac_generate_send_ack_collision_event(u_seconds);
    void mac_generate_inner_node_event();
    void mac_generate_send_busytone_event(u_seconds);
    void mac_generate_send_busytone_end_event(u_seconds);
    
public:
    bool have_data(address);
    void mac_generate_data();
    void mac_pop_data();
    void set_mac_busy();    
    
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
    void sim_trans();
    DATA mac_send_busytone();
    void mac_receive_busytone(const DATA&);
    DATA mac_send_busytone_end();
    void mac_receive_busytone_end(const DATA&);
    
    DATA get_data();
    ACK  get_ack();
//    bool moredata(address);
    int up_traffic;
    int down_traffic;
public:
    int pt_coll;//base on sense or, base on trigger signal?
    int st_coll;
    int pt_fd_op;
    int pt_fd_suc;
    int pt_suc;
    int st_suc;
    
    u_seconds delay_sum;
    int delay_count;
    u_seconds delay_max;
    
    
private:
    int CWmin=CWMIN;
    int CWmax=CWMAX;
    int CWfd=CWFD;
    int CW;
};

class random_number
{
public:
    random_number();
    ~random_number(){};
    int ran();
};

#endif /* MAC_h */
