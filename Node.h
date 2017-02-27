//
//  Node.h
//  Wireless Simulator
//
//  Created by 段荣昌 on 16/10/18.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include "Event.h"
#include "PHY.h"
#include "MAC.h"
#include "Packet.h"
#include "Simulator.h"

#ifndef Node_h
#define Node_h


#define Node_IDLE 0
#define Node_SEND 1
#define Node_RECV 2
#define Node_FD   3
#define Node_BUSY 4
#define Node_COLL 5

class Node
{
public:
    Node();
    ~Node(){};
    
    int nodeid;//node id
    int x,y;// location
    int power;
    u_seconds current_t;
    
    MAC  MAClayer;
    PHY  PHYlayer;
    Event next_sending_event;
    int node_state;
    
    
    
public:
    void start(u_seconds t);//start at time(t)
    
    // generate_events;
    void generate_a_case();
    void generate_sending_data_event(u_seconds t); // here, t is the offest to the current_t
    void generate_sending_data_end_event(u_seconds t);
    void generate_sending_data_collision_event(u_seconds t);
    void generate_sending_ack_event(u_seconds t);
    void generate_sending_ack_end_event(u_seconds t);
    void generate_sending_ack_collision_event(u_seconds t);
    void generate_inner_node_event(u_seconds);
       
    //handle_events;
    
    DATA send_data(const Event &);
    void receive_data(const Event&, const DATA& );
    
    DATA send_data_end(const Event &);
    void receive_data_end(const Event &, const DATA &);
    
    void send_data_collision(const Event &);
    void receive_data_collision(const Event &);
    
    ACK send_ack(const Event &);
    void receive_ack(const Event&, const ACK & );
    
    ACK send_ack_end(const Event &);
    void receive_ack_end(const Event &, const ACK &);
    
    void send_ack_collision(const Event &);
    void receive_ack_collision(const Event &);
    
    void generate_data(const Event&);
    void generate_data();

};


#endif /* Node_h */
