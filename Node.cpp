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
    this->MAClayer.node=this;
    this->PHYlayer.node=this;
};

//start

void Node::start(u_seconds t)
{
    this->current_t=t;
    MAC* mac= &(this->MAClayer);
    if(mac->up_traffic!=0)
        mac->mac_generate_inner_node_event();
    
};

//////////  generate_events

void Node::generate_event(u_seconds t,int type)
{
    extern int unique_id;
    extern Simulator sim;
    Event event;
    
    event.nodeid = this->nodeid;
    event.t=current_t+t;
    event.type= type;
    event.uid=unique_id;
    unique_id++;
    sim.add(event);
    if(type < Inner_node_event && type !=Sending_data_collision_event)
        this->next_sending_event=event;
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
//        if(mac->to_send_coll== true)
//        {
//            if(TEST)
//                cout<<"Node::Node"<<this->nodeid<<" has not send coll signal yet"<<endl;
//            return;
//        }
        mac->state=MAC_IDLE;
        if(!mac->m_queue.empty())
        {
            mac->mac_generate_send_data_event();
        }
        else
        {
            mac->freeze_flag=0;
        }
        if(TEST)
            cout<<"Node::Node"<<nodeid<<" Free!"<<endl;
    }
}


void Node::node_pt_send_check(const Event& event) // send data check
{
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    if (mac->state != PT)
        return;
    if (phy->phy_pt_send_check() != true)
    {
        mac->PT_coll++;
        mac->set_mac_busy();
        mac->mac_generate_send_data_collision_event(0);
        if(TEST)
        cout<<"NODE::node"<<this->nodeid<<" pt_send_check coll "<<endl;
    }
    else
    {
        if(TEST)
        cout<<"NODE::node"<<this->nodeid<<" pt_send_check OK "<<endl;
    }
    
}

void Node::node_pt_receive_check(const Event& event) //receive data check
{
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    if (!(mac->state == HD_PT || mac->state==BFD_PT || mac->state==DBFD_PT || mac->state==PT))
        return;
    int node_id = (int)(mac->dst-(1<<10));
    if (phy->phy_pt_receive_check(node_id) != true)
    {
        if (mac->state == HD_PT)
            mac->HD_pt_coll++;
        else if (mac->state==BFD_PT)
            mac->BFD_pt_coll++;
        else if( mac->state==DBFD_PT)
            mac->DBFD_pt_coll++;
        mac->set_mac_busy();
        mac->mac_generate_send_data_collision_event(0);
        if(TEST)
        cout<<"NODE::node"<<this->nodeid<<" pt_recv_check coll "<<endl;
    }
    else
    {
        if(TEST)
        cout<<"NODE::node"<<this->nodeid<<" pt_recv_check OK "<<endl;
    }
}



void Node::node_st_send_check(const Event& event) // send data check
{
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    if ( !(mac->state == BFD_ST || mac->state ==DBFD_ST || mac->state==HD_PR))
        return;
    int node_id = (int)(mac->peer-(1<<10));
    if (phy->phy_st_send_check(node_id) != true)
    {
        if(mac->state == BFD_ST)
            mac->BFD_st_coll++;
        else if(mac->state == DBFD_PT)
            mac->DBFD_st_coll++;
        else if (mac->state==HD_PR)
            mac->HD_pr_coll++;
        mac->set_mac_busy();
        mac->mac_generate_send_data_collision_event(0);
        if(TEST)
        cout<<"NODE::node"<<this->nodeid<<" st_send_check coll "<<endl;
    }
    else
    {
        if(TEST)
        cout<<"NODE::node"<<this->nodeid<<" st_send_check OK "<<endl;
    }
}


void Node::node_st_receive_ack_check(const Event& event) // receive ack check
{
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    if (mac->state !=DBFD_ST)
        return;
    int node_id = (int)(mac->dst-(1<<10));
    if (phy->phy_pt_receive_check(node_id) != true)
    {
        mac->DBFD_st_coll++;
        if(TEST)
        cout<<"NODE::node"<<this->nodeid<<" st_recv_ack_check coll "<<endl;
        return;
    }
    else
    {
        if(TEST)
        cout<<"NODE::node"<<this->nodeid<<" st_recv_ack_check OK "<<endl;
    }
}




/////////////// Handle events

/////////////// event.type==Inner_node

void Node::generate_data(const Event& event)
{
    this->current_t=event.t;
    MAC* mac=& (this->MAClayer);
    //    cout<<"Node::Node"<<nodeid<<" gen a DATA at "<<current_t<<endl;
    mac->mac_generate_data(this->target_node);
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
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a DATA at "<<event.t<<"us to Node"<<data.receiver-(1<<10) <<endl;
    //    if(mac->to_sim_coll==1) // same time transmission collision
    //        mac->sim_trans();
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
    if(event.t == this->next_sending_event.t){
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
    
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a DATA at "<<event.t<<"s from Node"<<data.sender-(1<<10) <<endl;
//    if(mac->state==MAC_BUSY && this->next_sending_event.t !=0)
//    {
////        if(this->next_sending_event.type== Sending_data_collision_event || this->next_sending_event.type== Sending_ack_collision_event)
//        if(this->next_sending_event.type== Sending_data_collision_event)
//            return;
//        cout<<"Node::Node"<<this->nodeid<<" should not send when it is busy"<<endl;
//        cout<< this->next_sending_event.t << "::" <<this->next_sending_event.type<<endl;
//        exit(-1);
//    }
    return;
};

///////////////  event.type==Sending_data_end

DATA Node::send_data_end(const Event & event)
{
    MAC* mac =& (this->MAClayer);
    PHY* phy =& (this->PHYlayer);
    this->current_t=event.t;
    
    phy->phy_send_end();
    mac->mac_send_data_end();
    DATA data=mac->get_data();
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a DATA END at "<<event.t<<"us to Node"<<data.receiver-(1<<10) <<endl;
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
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a DATA END at "<<event.t<<"s from Node"<<data.sender-(1<<10) <<endl;
    
    this->node_free();
    return;
};

///////////// event.type==Sending_data_collision

void Node::send_data_collision (const Event & event)
{
    
    //    if(event!= this->next_sending_event ){
    //        if (! ( this->next_sending_event.t <= event.t && this->next_sending_event.type==Sending_ack_collision_event ))
    //        {
    //            cout<<"Node::send_data_collision error1"<<endl;
    //            exit(-1);
    //        }
    //    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    if(mac->state != MAC_BUSY){
        cout<<"Node::send_data_collision error2"<<endl;
        exit(-1);
    }
    if (phy->tx_state==PHY_IDLE)
    {
        cout<<"Node::send data collision error"<<endl;
        exit(-1);
    }
    
    phy->phy_send_end();
    mac->mac_send_data_collision();
    if(TEST)
        cout<<"Node::Node"<<this->nodeid <<"("<<mac->state<<")" <<" sends a DATA Collision at "<< current_t <<"us"<<endl;
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
    if(TEST)
        cout<<"Node::Node"<<this->nodeid<<"("<<mac->state<<")"<<" receives a DATA collision at "<< current_t<<"s"<<endl;
    this->node_free();
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
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a ACK at "<<event.t<<"s to Node"<<ack.receiver-(1<<10) <<endl;
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
    if(event.t == this->next_sending_event.t){
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
    
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a ACK at "<<event.t<<"s to Node"<<ack.receiver-(1<<10) <<endl;
    
//    if(mac->state==MAC_BUSY && this->next_sending_event.t !=0)
//    {
//        if(this->next_sending_event.type== Sending_data_collision_event || this->next_sending_event.type== Sending_ack_collision_event)
//            return;
//        cout<<"Node::Node"<<this->nodeid<<" should not send when it is busy"<<endl;
//        cout<< this->next_sending_event.t << "::" <<this->next_sending_event.type<<endl;
//        exit(-1);
//    }
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
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a ACK END at "<<event.t<<"s to Node"<<ack.receiver-(1<<10) <<endl;
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
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a ACK END at "<<event.t<<"s to Node"<<ack.receiver-(1<<10) <<endl;
    this->node_free();
}


///// event.type==Sending_ack_collision
//void Node::send_ack_collision(const Event & event)
//{
//    if(event!=this->next_sending_event){
//        cout<<"Node::send_ack_collision error"<<endl;
//        exit(-1);
//    }
//    
//    if(event!=this->next_sending_event ){
//        if (! ( this->next_sending_event.t <= event.t && this->next_sending_event.type==Sending_data_collision_event ))
//        {
//            cout<<"Node::send_data_collision error1"<<endl;
//            exit(-1);
//        }
//    }
//    
//    MAC* mac=& (this->MAClayer);
//    PHY* phy=& (this->PHYlayer);
//    this->current_t=event.t;
//    
//    phy->phy_send_end();
//    mac->mac_send_ack_collision();
//    if(TEST)
//        cout<<"Node:: Node"<<this->nodeid<<"("<<mac->state<<")" <<" sends a ACK Collision at "<< current_t <<"s"<<endl;
//    this->node_free();
//}

//void Node::receive_ack_collision(const Event & event)
//{
//    if(! dist(nodeid, event.nodeid)) // true < 5
//        return;
//    if(event.t < current_t){
//        cout<<"Node::receive_ack_collision error"<<endl;
//        exit(-1);
//    }
//    MAC* mac=& (this->MAClayer);
//    PHY* phy=& (this->PHYlayer);
//    this->current_t=event.t;
//    
//    phy->phy_receive_end(event);
//    mac->mac_receive_ack_collision(event);
//    if(TEST)
//        cout<<"Node:: Node"<<this->nodeid <<"("<<mac->state<<")"<<" receives a ACK Collision at "<< current_t <<"s"<<endl;
//    this->node_free();
//}


DATA Node::send_busytone(const Event & event)
{
    if(event!=this->next_sending_event){
        cout<<"Node::send_busytone error"<<endl;
        exit(-1);
    }
    
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    DATA data=mac->mac_send_busytone();
    phy->phy_send();
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a busytone at "<<event.t<<"us" <<endl;
    return data;
};


void Node::receive_busytone(const Event & event, const DATA & data)
{
    if(! dist(nodeid, event.nodeid)) // true < 5
        return;
    if(event.t < current_t){
        cout<<"Node::receive_busytone error"<<endl;
        exit(-1);
    }
    MAC* mac=& (this->MAClayer);
    PHY* phy=& (this->PHYlayer);
    this->current_t=event.t;
    
    ////sim trans coll
    if(event.t == this->next_sending_event.t){
        phy->phy_receive(event);
        return;
    }
    
    ////freeze
    if(mac->state==MAC_IDLE){
        mac->freeze( ceil((double) (next_sending_event.t - current_t)/SLOT));
        this->next_sending_event.t=0;
    }
    
    phy->phy_receive(event);
    mac->mac_receive_busytone(data);
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a BUSYTONE at "<<event.t<<"s from Node"<<data.sender-(1<<10) <<endl;
    return;
};


DATA Node::send_busytone_end(const Event & event)
{
    if(event!=this->next_sending_event){
        cout<<"Node::send_busytone_end error"<<endl;
        exit(-1);
    }
    
    MAC* mac =& (this->MAClayer);
    PHY* phy =& (this->PHYlayer);
    this->current_t=event.t;
    phy->phy_send_end();
    DATA data=mac->mac_send_busytone_end();
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" sends a busytone END at "<<event.t<<"us" <<endl;
    return data;
};


void Node::receive_busytone_end(const Event & event,const DATA & data)
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
    mac->mac_receive_busytone_end(data);
    if(TEST)
        cout<<"Node::Node"<< this->nodeid <<"("<<mac->state<<")" <<" receives a BUSYTONE END at "<<event.t<<"s from Node"<<data.sender-(1<<10) <<endl;
    this->node_free();
    return;
};



