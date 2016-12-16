//
//  Node.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include "Node.h"
#include "Common.h"
#include <iostream>

using namespace std;

Node::Node()
{
    this->nodeid=0;
    this->current_t=0;
    this->x=0;
    this->y=0;
    this->power=120;
    this->MAClayer.node=this;
    this->PHYlayer.node=this;
    this->node_state=Node_IDLE;
};

//start

void Node::start(u_seconds t)
{
    this->current_t=t;
    this->generate_data();
};

///////////// just for a testing case
//
//
//void Node::generate_a_case()
//{
//    MAC* mac= &(this->MAClayer);
//    mac->mac_generate_data();
////    this->generate_sending_data_event(0);
//};


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


/////////////// Handle events

void Node::generate_data()
{
    MAC* mac=& (this->MAClayer);
    mac->mac_generate_data();
}

/////////////// event.type==Inner_node

void Node::generate_data(const Event& event)
{
    this->current_t=event.t;
    MAC* mac=& (this->MAClayer);
    mac->mac_generate_data();
    cout<<"Node::Node"<< this->nodeid<<" generates pakcets at "<< event.t<<"s"<<endl;
}

//////////////// event.type==Sending_data

DATA Node::send_data(const Event & event)
{
    if(event!=this->next_sending_event && event.t < current_t)
    {
        cout<<"Node::error"<<endl;
        exit(-1);
    }
    
    if (this->node_state == Node_COLL) // same time transmission collision
    {
        cout<<"Node::node"<< this->nodeid <<" Collided"<<endl;
        this->generate_sending_data_collision_event(5); //
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    
//    if(mac->state==MAC_BUSY || mac->state ==DBFD_SR || mac->state== SBFD_PR )
//    {
//        cout<<"Node"<<this->nodeid<<" "<< mac->state <<" should not send data here"<<endl;
//        exit(-1);
//    }
    
    this->current_t=event.t;
    phy->phy_send();
    mac->mac_send_data();
    
    DATA data=mac->get_data();
    this->generate_sending_data_end_event((data.payload+HEADER)/RATE);
    
    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a DATA at "<<event.t<<"s to Node"<<data.destination-(1<<10) <<endl;
    
    return data;
};

void Node::receive_data(const Event & event, const DATA & data)
{
    if(event.t < current_t)
    {
        cout<<"Node::wrong"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    
    
    if(mac->state == MAC_IDLE && event.t == this->next_sending_event.t)
    {
            //cout<<"Node::node"<<this->nodeid<<" Collided"<<endl;
            this->node_state=Node_COLL;
            phy->phy_receive(event);
            return;
    }
    
    
    if(mac->state == MAC_IDLE)
    {
//      cout<<"Node::Node"<<this->nodeid<<" receive data and freeze"<<endl;
//      not consider freeze.
        this->next_sending_event.t=0;
    } /// may change the location in future.
    this->current_t=event.t;
    
    phy->phy_receive(event);
    mac->mac_receive_data(data);

    
    if(mac->state==MAC_BUSY && this->next_sending_event.t !=0)
    {
        //cout<< send collided<<endl;
        //cout<<"Node::Node"<<this->nodeid<<" error"<<endl;
    }
    
    return;
};

///////////////  event.type==Sending_data_end

DATA Node::send_data_end(const Event & event)
{

    
    if(event!=this->next_sending_event)
    {
        cout<<"wrong"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    
    this->current_t=event.t;
    
    
    phy->phy_send_end();
    
    DATA data=mac->get_data();
    mac->mac_send_data_end();
    mac->mac_pop_data();
    // would change for ack in future.
    
    
    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a DATA END at "<<event.t<<"s to Node"<<data.destination-(1<<10) <<endl;
    
    if( phy->tx_state==PHY_IDLE && phy->rx_state==PHY_IDLE)
    {
        this->node_state=Node_IDLE;
        mac->state=MAC_IDLE;
        if(!mac->m_queue.empty())
        {
            mac->mac_generate_send_data_event();
        }
    }
    return data;
};

void Node::receive_data_end(const Event & event,const DATA & data)
{
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    
    this->current_t=event.t;
    phy->phy_receive_end(event);
    mac->mac_receive_data_end(data);
    
    cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a DATA END at "<<event.t<<"s to Node"<<data.destination-(1<<10) <<endl;
    
    if(phy->tx_state==PHY_IDLE && phy->rx_state==PHY_IDLE )
    {
        this->node_state=Node_IDLE;
        mac->state=MAC_IDLE;
        if(!mac->m_queue.empty())
        {
            mac->mac_generate_send_data_event();
        }
    }
};

///////////// event.type==Sending_data_collision

void Node::send_data_collision(const Event & event)
{
    
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    
    if(mac->state==MAC_IDLE)
    {
        return;
    }
    
    this->current_t=event.t;
    this->next_sending_event.t=0;
    
    phy->phy_send_end();
    mac->mac_send_data_collision();
    
    cout<<"Node:: Node"<<this->nodeid <<" sends collision at "<< current_t <<"s"<<endl;
    
    if(phy->tx_state==PHY_IDLE && phy->rx_state==PHY_IDLE)
    {
        this->node_state=Node_IDLE;
        mac->state=MAC_IDLE;
        if(!mac->m_queue.empty())
        {
            mac->mac_generate_send_data_event();
        }
    }
}

void Node::receive_data_collision(const Event & event)
{
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    
    this->current_t=event.t;
    
    phy->phy_receive_end(event);
    mac->mac_receive_data_collision(event);
    
//    cout<<"Node:: Node"<<this->nodeid <<" receives collision at "<< current_t<<"s"<<endl;
    
    if(phy->tx_state==PHY_IDLE && phy->rx_state==PHY_IDLE)
    {
        this->node_state=Node_IDLE;
        mac->state=MAC_IDLE;
        if(!mac->m_queue.empty())
        {
            mac->mac_generate_send_data_event();
        }
    }
    
}

