//
//  Node.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include "Node.h"
#include "Common.h"
#include "math.h"
#include "Topo.h"
#include <iostream>
#include <cstring>


using namespace std;

Node::Node()
{
    this->nodeid=0;
    this->current_t=0;
    this->x=0;
    this->y=0;
    this->node_type=0;
    this->MAClayer.node=this;
    this->PHYlayer.node=this;
    this->num_assocaition_node=0;
    this->num_interferation_node=0;
    memset( this->assocaition_node,0,sizeof(this->assocaition_node));
    memset( this->interferation_node,0,sizeof(this->interferation_node));
    this->assocaition_AP_id=0;
};

//start

void Node::start(u_seconds t)
{
    this->current_t=t;
    MAC* mac=& (this->MAClayer);
    mac->mac_generate_inner_node_event();

};

//////////  generate_events

void Node::generate_sending_data_event(u_seconds t)
{
    extern int unique_id;
    extern Simulator sim;
    Event event;
    
    event.nodeid = this->nodeid;
    event.t=current_t+t;
    event.type=Sending_data;
    event.uid=unique_id;
    unique_id++;
    this->next_sending_event=event;
    sim.add(event);
};

void Node::generate_sending_data_end_event(u_seconds t)
{
    extern int unique_id;
    extern Simulator sim;
    Event event;
    
    event.nodeid = this->nodeid;
    event.t=current_t+t;
    event.type=Sending_data_end;
    event.uid=unique_id;
    unique_id++;
    this->next_sending_event=event;
    sim.add(event);
}

void Node::generate_sending_data_collision_event(u_seconds t)
{
    extern int unique_id;
    extern Simulator sim;
    Event event;
    
    event.nodeid = this->nodeid;
    event.t= this->current_t+ t;
    event.type=Sending_data_collision;
    event.uid=unique_id;
    unique_id++;
    this->next_sending_event=event;
    sim.add(event);
}

void Node::generate_sending_ack_event(u_seconds t)
{
    extern int unique_id;
    extern Simulator sim;
    Event event;
    
    event.nodeid = this->nodeid;
    event.t=current_t+t;
    event.type=Sending_ack;
    event.uid=unique_id;
    unique_id++;
    this->next_sending_event=event;
    sim.add(event);
}

void Node::generate_sending_ack_end_event(u_seconds t)
{
    extern int unique_id;
    extern Simulator sim;
    Event event;
    
    event.nodeid = this->nodeid;
    event.t=current_t+t;
    event.type=Sending_ack_end;
    event.uid=unique_id;
    unique_id++;
    this->next_sending_event=event;
    sim.add(event);
}

void Node::generate_sending_ack_collision_event(u_seconds t)
{
    extern int unique_id;
    extern Simulator sim;
    Event event;
    
    event.nodeid = this->nodeid;
    event.t=current_t+t;
    event.type=Sending_ack_collision;
    event.uid=unique_id;
    unique_id++;
    this->next_sending_event=event;
    sim.add(event);
}

void Node::generate_inner_node_event(u_seconds t)
{
    extern int unique_id;
    extern Simulator sim;
    Event event;
    
    event.nodeid = this->nodeid;
    event.t=current_t+t;
    event.type=Inner_node;
    event.uid=unique_id;
    unique_id++;
    sim.add(event);
}

/////////////// tmp functions

bool dist(int a, int b)
{
    Topo* topo;
    return topo->dist(a,b);
}


void Node::node_free()
{
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    
    if(phy->tx_state==PHY_IDLE && phy->rx_state==PHY_IDLE && mac->state==MAC_BUSY)
    {
        if(mac->to_send_coll== true)
        {
//            cout<<"Node::Node"<<this->nodeid<<" has not send coll signal yet"<<endl;
            return;
        }
        mac->state=MAC_IDLE;
        if(!mac->m_queue.empty())
        {
            mac->mac_generate_send_data_event();
        }
        else
        {
            mac->to_freeze_flag=0;
        }
//        cout<<"Node::Node"<<nodeid<<" Free!"<<endl;
    }
}




/////////////// Handle events

/////////////// event.type==Inner_node

void Node::generate_data(const Event& event)
{
    this->current_t=event.t;
    MAC* mac=& (this->MAClayer);
//    cout<<"Node::Node"<<nodeid<<" gen a DATA at "<<current_t<<endl;
    mac->mac_generate_data();
}

//////////////// event.type==Sending_data

DATA Node::send_data(const Event & event)
{
    if(event!=this->next_sending_event){
        cout<<"Node::send_data error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    phy->phy_send();
    mac->mac_send_data();
    DATA data=mac->get_data();
//    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a DATA at "<<event.t<<"us to Node"<<data.destination-(1<<10) <<endl;
    
    if(mac->to_sim_coll==1) // same time transmission collision
        mac->sim_trans();
    return data;
};

void Node::receive_data(const Event & event, const DATA & data)
{
    if(! dist(nodeid, event.nodeid)) // true < 5
        return;
    if(event.t < current_t){
        cout<<"Node::receive_date error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    ////sim trans coll
    if(mac->state == MAC_IDLE && event.t == this->next_sending_event.t)
    {
        mac->to_sim_coll=1;
        phy->phy_receive(event);
        return;
    }
    else if( mac->to_sim_coll==1)
    {
        phy->phy_receive(event);
        return;
    }
    
    ////freeze
    if(mac->state==MAC_IDLE){
        mac->freeze( ceil((double) (next_sending_event.t - current_t)/SLOT));
        this->next_sending_event.t=0;
    }
    
    phy->phy_receive(event);
    mac->mac_receive_data(data);
//    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" Receive a DATA at "<<event.t<<"s from Node"<<data.source-(1<<10) <<endl;
    
    ////double check
    if(mac->state==MAC_BUSY && this->next_sending_event.t !=0)
    {
        if(this->next_sending_event.type== Sending_data_collision || this->next_sending_event.type== Sending_ack_collision)
            return;
        cout<<"Node::Node"<<this->nodeid<<" should not send when it is busy"<<endl;
        cout<< this->next_sending_event.t << "::" <<this->next_sending_event.type<<endl;
        exit(-1);
    }
    
    return;
};

///////////////  event.type==Sending_data_end

DATA Node::send_data_end(const Event & event)
{
    if(event!=this->next_sending_event){
        cout<<"Node::send_data_end error"<<endl;
        exit(-1);
    }
    
    MAC* mac =& (this->MAClayer);
    PHY* phy =& (this->PHYlayer);
    this->current_t=event.t;
    
    phy->phy_send_end();
    mac->mac_send_data_end();
    DATA data=mac->get_data();
    
//    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a DATA END at "<<event.t<<"us to Node"<<data.destination-(1<<10) <<endl;
    return data;
};

void Node::receive_data_end(const Event & event,const DATA & data)
{
    if(! dist(nodeid, event.nodeid)) // true < 5
        return;
    if(event.t < current_t){
        cout<<"Node::receive_date_end error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    
    phy->phy_receive_end(event);
    mac->mac_receive_data_end(data);
    
//    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a DATA END at "<<event.t<<"s from Node"<<data.source-(1<<10) <<endl;
    
    this->node_free();
    return;
};

///////////// event.type==Sending_data_collision

void Node::send_data_collision(const Event & event)
{
    
    if(event!=this->next_sending_event ){
        if (! ( this->next_sending_event.t <= event.t && this->next_sending_event.type==Sending_ack_collision ))
        {
            cout<<"Node::send_data_collision error1"<<endl;
            exit(-1);
        }
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    if(mac->state != MAC_BUSY){
        cout<<"Node::send_data_collision error2"<<endl;
        exit(-1);
    }
    
    phy->phy_send_end();
    mac->mac_send_data_collision();
//    cout<<"Node::Node"<<this->nodeid <<"("<<mac->state<<")" <<" sends a DATA Collision at "<< current_t <<"us"<<endl;
    
    this->node_free();
}

void Node::receive_data_collision(const Event & event)
{
    if(! dist(nodeid, event.nodeid)) // true < 5
        return;
    if(event.t < current_t){
        cout<<"Node::receive_date_collision error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    phy->phy_receive_end(event);
    mac->mac_receive_data_collision(event);
//    cout<<"Node::Node"<<this->nodeid<<"("<<mac->state<<")"<<" receives a DATA collision at "<< current_t<<"s"<<endl;
    this->node_free();
    
    ////double check
    if(mac->state==MAC_BUSY && this->next_sending_event.t !=0)
    {
        if(this->next_sending_event.type== Sending_data_collision || this->next_sending_event.type== Sending_ack_collision)
            return;
        cout<<"Node::Node"<<this->nodeid<<" should not send when it is busy"<<endl;
        cout<< this->next_sending_event.t << "::" <<this->next_sending_event.type<<endl;
        exit(-1);
    }
    
}

///// event.type==Sending_ack

ACK Node::send_ack(const Event & event)
{
    if(event!=this->next_sending_event){
        cout<<"Node::send_ack error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;

    phy->phy_send();
    mac->mac_send_ack();
    ACK ack=mac->get_ack();
    
//    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a ACK at "<<event.t<<"s to Node"<<ack.destination-(1<<10) <<endl;
    return ack;
}


void Node::receive_ack(const Event& event, const ACK & ack)
{
    if(! dist(nodeid, event.nodeid)) // true < 5
        return;
    if(event.t < current_t){
        cout<<"Node::receive_ack error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    //sending coll
    if(mac->state == MAC_IDLE && event.t == this->next_sending_event.t){
        mac->to_sim_coll=1;
        phy->phy_receive(event);
        return;
    }
    else if( mac->to_sim_coll==1)
    {
        phy->phy_receive(event);
        return;
    }
    
    //freeze
    if(mac->state==MAC_IDLE){
        mac->freeze( ceil((double) (next_sending_event.t - current_t)/SLOT));
        this->next_sending_event.t=0;
    }
    
    phy->phy_receive(event);
    mac->mac_receive_ack(ack);
    
//    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a ACK at "<<event.t<<"s to Node"<<ack.destination-(1<<10) <<endl;
    
    if(mac->state==MAC_BUSY && this->next_sending_event.t !=0)
    {
        if(this->next_sending_event.type== Sending_data_collision || this->next_sending_event.type== Sending_ack_collision)
            return;
        cout<<"Node::Node"<<this->nodeid<<" should not send when it is busy"<<endl;
        cout<< this->next_sending_event.t << "::" <<this->next_sending_event.type<<endl;
        exit(-1);
    }

    
    return;
}

///// event.type==Sending_ack_end
ACK Node::send_ack_end(const Event & event)
{
    if(event!=this->next_sending_event){
        cout<<"Node::send_ack_end error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    ACK ack=mac->get_ack();
    phy->phy_send_end();
    mac->mac_send_ack_end();
//    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a ACK END at "<<event.t<<"s to Node"<<ack.destination-(1<<10) <<endl;
    this->node_free();
    return ack;
}

void Node::receive_ack_end(const Event & event, const ACK& ack)
{
    if(! dist(nodeid, event.nodeid)) // true < 5
        return;
    if(event.t < current_t){
        cout<<"Node::receive_ack_end error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    phy->phy_receive_end(event);
    mac->mac_receive_ack_end(ack);
//    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a ACK END at "<<event.t<<"s to Node"<<ack.destination-(1<<10) <<endl;
    this->node_free();
}


///// event.type==Sending_ack_collision
void Node::send_ack_collision(const Event & event)
{
//    if(event!=this->next_sending_event){
//        cout<<"Node::send_ack_collision error"<<endl;
//        exit(-1);
//    }
    
    if(event!=this->next_sending_event ){
        if (! ( this->next_sending_event.t <= event.t && this->next_sending_event.type==Sending_data_collision ))
        {
            cout<<"Node::send_data_collision error1"<<endl;
            exit(-1);
        }
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    phy->phy_send_end();
    mac->mac_send_ack_collision();
//    cout<<"Node:: Node"<<this->nodeid<<"("<<mac->state<<")" <<" sends a ACK Collision at "<< current_t <<"s"<<endl;
    this->node_free();
}

void Node::receive_ack_collision(const Event & event)
{
    if(! dist(nodeid, event.nodeid)) // true < 5
        return;
    if(event.t < current_t){
        cout<<"Node::receive_ack_collision error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    phy->phy_receive_end(event);
    mac->mac_receive_ack_collision(event);
//    cout<<"Node:: Node"<<this->nodeid <<"("<<mac->state<<")"<<" receives a ACK Collision at "<< current_t <<"s"<<endl;
    this->node_free();
}

