//
//  MAC.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//

#include "Common.h"
#include "MAC.h"
#include "Event.h"
#include "Node.h"
#include "Simulator.h"
#include <iostream>
#include <math.h>

#define  Fake_node (256)
#define  Fake_node_address (256 +(1<<10))


using namespace std;


////////MAC
int random(int window)
{
    extern random_number ran_generator;
    return ran_generator.ran() % window;
}

u_seconds randomExponential(double lambda) // lambda packets generated in 1000 000 000us
{
    double pV = 0.0;
    extern random_number ran_generator;
    while(true)
    {
        pV = (double)ran_generator.ran()/(double)RAND_MAX;
        if (pV != 1)
            break;
    }
    pV = pow(10,9)*(-1.0/lambda)*log(1-pV);
    return pV;
}


MAC::MAC()
{
    this->state=MAC_IDLE;
    while (!this->m_queue.empty()) {
        this->m_queue.clear();
    }
    this->CW=CWmin;
    this->backoff_count=0;
    this->m_adrress=0;
    this->node=NULL;
    this->iter=m_queue.begin();
    this->peer=0; // temperory used in DBFD mode
    this->dst=0; // used in HD, BFD , DBFD
    
    this->freeze_flag=0;
    this->to_send_ack=0;
    this->to_busy=0;
    this->to_sim_coll=0;
    this->to_send_coll=0;
    this->m_queue.reserve(QUEUE_SIZE); //set the max length of queue;
    
    this->HD_pt_suc=0;
    this->HD_pt_coll=0;
    this->HD_pr_coll =0;
    
    this->BFD_pt_coll=0;
    this->BFD_pt_suc=0;
    this-> BFD_st_coll=0;
    this->BFD_st_suc=0;
    
    this->DBFD_pt_coll=0;
    this->DBFD_pt_suc=0;
    this->DBFD_st_coll=0;
    this->DBFD_st_suc=0;
    this->DBFD_sr_coll=0;
    //    this->delay_max=0;
    //    this->delay_sum=0;
    //    this->delay_count=0;
}

/////////generate events


void MAC::mac_generate_send_data_event(u_seconds t)  /// start based on FD transmission
{
    Node *node= (Node*) this->node;
    node->generate_event(t,Sending_data_event);
};


void MAC::mac_generate_send_data_event()   /// start based on backoff
{
    Node *node= (Node*) this->node;
    if(freeze_flag==1)
    {
        if(this->state != MAC_IDLE)  //////// questions?
        {
            cout<<"MAC::generate_send_event_error"<<endl;
            exit(-1);
        }
        node->generate_event (DIFS + backoff_count*SLOT, Sending_data_event);
        freeze_flag=0;
        if (TEST)
            cout<<"MAC::Node"<<node->nodeid<<" restarts a backoff, from "<< backoff_count <<endl;
        return;
    }
    else if (freeze_flag==0)
    {
        this->backoff_count=random(CW);
        node->generate_event(DIFS+backoff_count*SLOT,Sending_data_event);
        return;
    }
    else
    {
        cout<<"MAC::freeze flag error"<<endl;
        exit(-1);
    }
};

void MAC::mac_generate_send_data_end_event(u_seconds t)  /// start based on FD transmission
{
    Node *node= (Node*) this->node;
    node->generate_event(t,Sending_data_end_event);
}

void MAC::mac_generate_send_data_collision_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_event(t,Sending_data_collision_event);
    //    cout<< "MAC::Node"<< node->nodeid<<" gen a send data collision event at "<< node-> current_t<<"s" <<endl;
};

void MAC::mac_generate_send_ack_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_event(t,Sending_ack_event);
}

void MAC::mac_generate_send_ack_end_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_event(t,Sending_ack_end_event);
}

void MAC::mac_generate_send_ack_collision_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_event(t, Sending_ack_collision_event);
    //    cout<< "MAC::Node"<< node->nodeid<<" gen a send ack collision event at"<< node-> current_t <<endl;
}

void MAC::mac_generate_send_busytone_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_event(t,Sending_busytone_event);
    //    cout<< "MAC::Node"<< node->nodeid<<" gen a send busytone event at "<< node-> current_t<<"s" <<endl;
};

void MAC::mac_generate_send_busytone_end_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_event(t,Sending_busytone_end_event);
    //    cout<< "MAC::Node"<< node->nodeid<<" gen a send busytone event at "<< node-> current_t<<"s" <<endl;
};

void MAC::mac_generate_inner_node_event()
{
    Node *node= (Node*) this->node;
    node->generate_event(randomExponential(this->up_traffic), Inner_node_event);
    
}



////////// handle

void MAC::set_mac_busy()
{
    this->state=MAC_BUSY;
    this->peer=0;
    this->dst=0;
    
    this->busytone_flag=false;
    this->to_sim_coll=false;
    this->to_busy=false;
    this->to_send_ack=false;
    
    this->iter=m_queue.begin();
    Node* node= (Node*) this->node;
    if(this->to_send_coll != true && node->next_sending_event.type != Sending_ack_collision_event )
        node->next_sending_event.t=0;
}



///////////inner node




bool MAC::have_data(address target)
{
    if( m_queue.empty() )
        return false;
    this->iter= this->m_queue.begin();
    for(;iter != m_queue.end();iter++)
    {
        if( (*iter).receiver==target ){
            return true;
        }
    }
    this->iter=this->m_queue.begin();
    return false;
}


void MAC::freeze(int count)
{
    if(freeze_flag==0 && (this->state==MAC_IDLE) && !(this-> m_queue).empty())
    {
        freeze_flag=1;
        if(backoff_count!=0){
            if(backoff_count > count)
                backoff_count=count;
        }
        else {
            backoff_count = count;
        }
        //        cout<<"MAC::Node"<<this->m_adrress-(1<<10)<<" backoff count freeze to "<<backoff_count <<endl;
    }
}


void MAC::mac_pop_data()
{
    //    DATA data = this->get_data();
    //    Node* node=(Node*)this->node;
    //    u_seconds delay=node->current_t-data.time_stamp;
    //    this->delay_sum +=delay;
    //    this->delay_count++;
    //    if (this->delay_max< delay)
    //    this->delay_max=delay;
    //    if(this->m_adrress <= num_AP+(1<<10))
    //    {
    //    this->neighbour[data.destination-(1<<10)][4] -- ;
    //    }
    this->m_queue.erase(this->iter);
    if(!this->m_queue.empty())
        iter=m_queue.begin();
}

void MAC::mac_push_data(DATA data)
{
    extern int overall_suc,overall_drop;
    extern int route_table[size_of_Nodelist][size_of_Nodelist];
    if (data.destination == this->m_adrress)
    {
        overall_suc++;
        if (TEST)
            cout<<"MAC::Node"<<this->m_adrress-(1<<10)<<"("<<this->state<<")"<<"receive a data from Node "<<data.source-(1<<10) <<endl;
        return;
    }
    else if(m_queue.size()==QUEUE_SIZE)
    {
        overall_drop++;
        if (TEST)
            cout<<"MAC::Node"<<this->m_adrress-(1<<10)<<"("<<this->state<<")"<<" queue is full,drop packet"<<endl;
        return;
    }
    data.type=0;
    data.sender=this->m_adrress;
    data.receiver=route_table[this->m_adrress-(1<<10)][data.destination-(1<<10)];
    
    if(m_queue.empty())
    {
        this->m_queue.push_back(data);
        this->iter=m_queue.begin();
        if(this->state == MAC_IDLE)
            mac_generate_send_data_event();
    }
    else
        this->m_queue.push_back(data);
    
}




DATA MAC::get_data()
{
    DATA data;
    data=*(this->iter);
    data.type=this->state;
    return data;
}

ACK MAC::get_ack()
{
    ACK ack;
    ack.receiver=this->peer;
    return ack;
}

//////////send data

//bool MAC::moredata(address dst)
//{
////    if( m_queue.empty() )
////        return false;
//    auto it = this->m_queue.begin();
//    it++;
//    for(;it < m_queue.end();it++)
//    {
//        if( (*it).destination==dst ){
//            return true;
//        }
//    }
//    return false;
//}



void MAC::mac_generate_data(int target)
{
    if(m_queue.size()==QUEUE_SIZE)
    {
        if (TEST)
            cout<<"MAC::Node"<<this->m_adrress-(1<<10)<<"("<<this->state<<")"<<" queue is full,drop packet"<<endl;
        this->mac_generate_inner_node_event();
        return;
    }
    
    extern random_number ran_generator;
    DATA tmp_data;
    //    address tmp_dst;
    Node* node=(Node*) this->node;
    
    
    //    tmp_dst=target+(1<<10);
    
    // generate frame for several nodes
    //    {
    //        int tmp_index=ran_generator.ran()% this->up_traffic;
    //        int i=1; // index in neighbour
    //        while (1)
    //        {   int index=this->neighbour[i][0];
    //            tmp_index -= this->neighbour[index][1];
    //            if (tmp_index<0)
    //            break;
    //            i++;
    //            if(i>this->num_bour)
    //            {
    //                cout<<"MAC::generate traffic error"<<endl;
    //                exit(-1);
    //            }
    //        }
    //        tmp_dst=this->neighbour[i][0]+(1<<10);
    //    }
    
    
    
    extern int route_table[size_of_Nodelist][size_of_Nodelist];
    tmp_data.source=this->m_adrress;
    tmp_data.destination=target+(1<<10);
    tmp_data.sender=this->m_adrress;
    tmp_data.receiver= route_table[node->nodeid][target];
    tmp_data.time_stamp=node->current_t;
    tmp_data.type=0;
    //    this->neighbour[tmp_dst-(1<<10)][4]++;
    
    if(m_queue.empty())
    {
        this->m_queue.push_back(tmp_data);
        this->iter=m_queue.begin();
        if(this->state == MAC_IDLE)
            mac_generate_send_data_event();
    }
    else
    {
        this->m_queue.push_back(tmp_data);     // may cause error in future.?????
    }
    if(this->up_traffic!=0)
        this->mac_generate_inner_node_event();
}



void MAC::mac_send_data()
{
    if(this->state==MAC_IDLE)
    {
        if(this->m_queue.empty())
        {
            cout<<"MAC::send data error1"<<endl;
            exit(-1);
        }
        
        this->state = PT;
        this->iter  = this->m_queue.begin();
        this->dst   = (*iter).receiver;
        this->peer = (*iter).receiver;
        
        (*iter).type = this->state;
        this->mac_generate_send_data_end_event( DATA_time );
        return;
    }
    else if (this->state == HD_PT)
    {
        cout<<"MAC::send data error2"<<endl;
        exit(-1);
    }
    
    else if(this->state==BFD_PT || this->state== BFD_ST)
    {
        if (this->state == BFD_PT)
        {
            cout<<"MAC::send data error3"<<endl;
            exit(-2);
        }
        this->mac_generate_send_data_end_event( DATA_time );
        return;
    }
    
    else if(this->state==DBFD_PT || this->state== DBFD_ST)
    {
        if (this->state == DBFD_PT)
        {
            cout<<"MAC::send data error4"<<endl;
            exit(-2);
        }
        this->mac_generate_send_data_end_event( DATA_time );
        return;
    }
    else
    {
        cout<<"MAC::send data error5"<<endl;
        exit(-2);
    }
    
}

void MAC::mac_send_data_end()
{
    if(this->state==MAC_IDLE || this->state==MAC_BUSY || this->state == PT)
    {
        cout<<"MAC::send_data error1 Node"<< this->m_adrress-(1<<10)<<endl;
        exit(-1);
    }
    else if (this->state==HD_PT)
    {
        return;
    }
    else if(this->state==BFD_PT || this->state ==BFD_ST)
    {
        this->state==BFD_ST ? this->mac_generate_send_ack_event(SIFS): this->mac_generate_send_busytone_event(0);
        return;
    }
    else if(this->state==DBFD_PT || this->state == DBFD_ST)
    {
        this->state== DBFD_ST ? this->mac_generate_send_ack_event(SIFS): this->mac_generate_send_busytone_event(0);
        return;
    }
    else
    {
        cout<<"MAC:send data end error2"<<endl;
        exit(-1);
    }
}



DATA MAC::mac_send_busytone()
{
    if(this->busytone_flag != 0)
    {
        cout<<"MAC::mac send busytone error1"<<endl;
        exit(-1);
    }
    
    if(this->state == HD_PR || this->state==BFD_PT || this-> state ==DBFD_PT)
    {
        DATA data;
        data.source=this->m_adrress;
        data.sender=this->m_adrress;
        data.receiver=Fake_node+(1<<10);
        data.destination=Fake_node+(1<<10);
        data.type=BUSYTONE;
        this->busytone_flag=1;
        return data;
    }
    else
    {
        cout<<"MAC::send busytone error"<<endl;
        exit(-1);
    }
}


DATA MAC::mac_send_busytone_end()
{
    if(this->busytone_flag!=1)
    {
        cout<<"MAC::mac send busytone end error"<<endl;
        exit(-1);
    }
    if(this->state == HD_PR || this->state==BFD_PT || this-> state ==DBFD_PT)
    {
        DATA data;
        data.source=this->m_adrress;
        data.sender=this->m_adrress;
        data.receiver=Fake_node+(1<<10);
        data.destination=Fake_node+(1<<10);
        data.type=BUSYTONE;
        this->busytone_flag=0;
        
        if( this->state== HD_PR)
            this->mac_generate_send_ack_event(SIFS);
        else if (this->state == BFD_PT)
            this->mac_generate_send_ack_event(SIFS);
        else if (this->state == DBFD_PT)
            ;
        return data;
    }
    else
    {
        cout<<"MAC::send busytone error"<<endl;
        exit(-1);
    }
}

void MAC::mac_send_data_collision()
{
    if(this->state != MAC_BUSY)
    {
        cout<<"MAC::send_data_collision error"<<endl;
        exit(-7);
    }
    if(this->to_cwfix !=1)
    {
        CW<CWmax? CW=2*CW: CW=CWmax ;
        this->to_cwfix=1;
    }
    this->to_send_coll=0;
    this->set_mac_busy();
    return;
}

void MAC::mac_send_ack()
{
    ACK ack;
    if (this->state == HD_PR || this->state == BFD_PT || this->state == BFD_ST || this->state == DBFD_ST || this->state == DBFD_SR)
    {
        this->mac_generate_send_ack_end_event(ACK_time);// size of ack
        return;
    }
    else
    {
        cout<<"MAC::mac_send_ack error"<<endl;
        exit(-1);
    }
}

void MAC::mac_send_ack_end()
{
    Node* node= (Node*) this->node;
    PHY*  phy= &(node->PHYlayer);
    
    if (this->state == HD_PR || this->state == BFD_PT || this->state == BFD_ST || this->state == DBFD_ST || this->state == DBFD_SR)
    {
        if(phy->tx_state == PHY_IDLE && phy->rx_state==PHY_IDLE)
            this->set_mac_busy();
        return;
    }
    else
    {
        cout<<"MAC::mac_send_ack_end error"<<endl;
        exit(-1);
    }
}

void MAC::mac_send_ack_collision()
{
    if(this->state != MAC_BUSY){
        cout<<"MAC::send_ack_collision error"<<endl;
        exit(-1);
    }
    this->to_send_coll=0;
    this->set_mac_busy();
    
}

void MAC::sim_trans()
{
    if(this->to_send_coll!=0)
    {
        cout<<"MAC::sim trans error"<<endl;
        exit(-1);
    }
    if(this->to_sim_coll==1 && this->to_send_coll==0)
    {
        this->to_sim_coll=0;
        this->to_send_coll=1;
        this->set_mac_busy();
        this->pt_coll++;
        this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
        
    }
}


void MAC::mac_receive_data(const DATA& data)
{
    if(this->to_send_coll == 1){
        return;
    }
    if(this->state==MAC_IDLE)
    {
        if(this->m_adrress == data.receiver && data.type == PT)
        {
            if (this->have_data(data.sender))
            {
                this->state = BFD_ST;
                this->dst= data.sender;
                this->peer=data.sender;
                this->mac_generate_send_data_event(PHY_HEADER+MAC_HEADER);
                return;
            }
            else if (this->m_queue.empty() != true)
            {
                this->state =DBFD_ST;
                this->dst = (*iter).receiver;
                this->peer = data.sender;
                this->mac_generate_send_data_event(PHY_HEADER+MAC_HEADER);
                return;
            }
            else if (this->m_queue.empty() ==true )
            {
                this->state = HD_PR;
                this->dst = data.sender;
                this->peer = data.sender;
                this->mac_generate_send_busytone_event(PHY_HEADER+MAC_HEADER);
                return;
            }
        }
        else if (this->m_adrress == data.receiver && data.type == DBFD_ST)
        {
            this->state = DBFD_SR;
            this->dst = data.sender;
            this->peer = data.sender;
            return;
        }
        else
        {
            this->set_mac_busy();
        }
    }
    
    else if(this->state==MAC_BUSY)
    {
        if(this->to_send_coll==1)
            return;
        if ( data.receiver!=this->m_adrress)
            return;
        else if(data.type==PT && data.receiver==this->m_adrress)
        {
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(PHY_HEADER+MAC_HEADER);
        }
        else if((data.type==DBFD_ST) && (data.receiver==this->m_adrress))
        {
            return;
        }
        else if((data.type==BFD_ST) && (data.receiver==this->m_adrress))
        {
            cout<<"MAC::receive data error4"<<endl;
            exit(-1);
        }
        else
        {
            cout<<"MAC::receive data error4"<<endl;
            exit(-1);
        }
    }
    
    else if (this->state==PT)
    {
        if( data.type == PT)
        {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(PHY_HEADER+MAC_HEADER);
        }
        else if( data.type == BFD_ST && data.receiver == this->m_adrress && data.sender == this->dst)
        {
            this->state= BFD_PT;
        }
        else if( data.type == DBFD_ST && data.sender == this->dst)
        {
            this->state= DBFD_PT;
        }
        else if(data.sender != this->dst )
        {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(PHY_HEADER+MAC_HEADER);
        }
        else
        {
            cout<<"MAC::receive data error4"<<endl;
            exit(-1);
        }
    }
    
    else
    {
        this->set_mac_busy();
        this->to_send_coll=1;
        this->mac_generate_send_data_collision_event(PHY_HEADER+MAC_HEADER);
    }
}


///////////send data end

void MAC::mac_receive_busytone(const DATA& data)
{
    if(this->state==MAC_IDLE)
    {
        this->set_mac_busy();
    }
    else if(this->state==MAC_BUSY)
    {
        ;
    }
    if(this->state==PT)
    {
        if (data.sender==this->dst) {
            this->state=HD_PT;
        }
        else
        {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0);
        }
    }
    else if( this->state == BFD_ST || this->state == BFD_PT)
    {
        if (this->state==BFD_PT) {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0);
        }
        else if (this->state == BFD_ST && data.sender == this->dst)
        {
            ;
        }
        else if (this->state == BFD_ST && data.sender != this->dst)
        {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0);
        }
        else
        {
            cout<<"receive busytone error1"<<endl;
            exit(-1);
        }
    }
    
    else if( this->state == DBFD_ST || this->state == DBFD_PT || this->state== DBFD_SR)
    {
        if (this->state==DBFD_PT) {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0);
            return;
        }
        else if (this->state == DBFD_ST && data.sender == this->peer)
        {
            return;
        }
        else if (this->state == DBFD_ST && data.sender != this->peer)
        {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0);
            return;
        }
        else if (this->state ==DBFD_SR)
        {
            this->set_mac_busy();
            return;
        }
        else
        {
            cout<<"receive busytone error2"<<endl;
            exit(-1);
        }
    }
}

void MAC::mac_receive_data_end(const DATA & data)
{
    Node* node= (Node*) this->node;
    
    
    if(this->state == MAC_IDLE)
    {
        cout<<"MAC::Node"<<node->nodeid<<" mac_receive_data_end error"<<endl;
        exit(-1-4);
    }
    else if(this->state==MAC_BUSY)
    {
        if(data.receiver==this->m_adrress)
        {
            if(this->to_send_coll==0)
            {
                this->to_send_coll=1;
                this->mac_generate_send_ack_collision_event(SIFS);
            }
        }
    }
    else if(this->state==PT)
    {
        cout<<"MAC::Node"<<node->nodeid<<" mac_receive_data_end error"<<endl;
        exit(-1-4);
    }
    
    else if (this->state == HD_PT || this->state ==HD_PR)
    {
        if (data.receiver != this->m_adrress)
        {
            cout<<"MAC::Node"<<node->nodeid<<" mac_receive_data_end error"<<endl;
            exit(-1-4);
        }
        else if (this->state ==HD_PR)
        {
            this->mac_generate_send_busytone_end_event(0) ;
            this->mac_push_data(data);
        }
    }
    
    else if(this->state == BFD_PT || this->state == BFD_ST)
    {
        if (data.receiver != this->m_adrress)
        {
            cout<<"MAC::Node"<<node->nodeid<<" mac_receive_data_end error"<<endl;
            exit(-1-4);
        }
        else if (this->state==BFD_PT)
        {
            this->mac_generate_send_busytone_end_event(0);
            this->mac_push_data(data);
        }
        else if (this->state ==BFD_ST)
        {
            this->mac_push_data(data);
        }
    }
    
    else if(this->state == DBFD_PT || this->state == DBFD_ST || this->state ==DBFD_SR)
    {
        if(this->state == DBFD_PT)
        {
            if (data.sender != this->dst)
            {
                cout<<"MAC::Node"<<node->nodeid<<" mac_receive_data_end error"<<endl;
                exit(-1-4);
            }
            this->mac_generate_send_busytone_end_event(0);
            this->mac_push_data(data);
            return;
        }
        else if (this->state == DBFD_ST)
        {
            if (data.sender==this->peer)
                this->mac_push_data(data);
            else
            {
                cout<<"MAC::Node"<<node->nodeid<<" mac_receive_data_end error"<<endl;
                exit(-1-4);
            }
        }
        else if (this->state== DBFD_SR)
        {
            this->mac_push_data(data);// may generate ACK
        }
    }
    
}


void MAC::mac_receive_busytone_end(const DATA & data)
{
    Node* node= (Node*) this->node;
    
    if(this->state == MAC_IDLE)
    {
        cout<<"MAC::Node"<<node->nodeid<<" mac_receive_busytone_end error"<<endl;
        exit(-1);
    }
    else if(this->state==MAC_BUSY)
    {
        return;
    }
    else if ( this->state== HD_PT  && data.sender == this->dst)
    {
        return;
    }
    else if(this->state ==  BFD_ST && data.sender == this->dst)
    {
        return;
    }
    else if (this->state == DBFD_ST && data.sender ==this->peer)
    {
        return;
    }
    else if (this->state == DBFD_SR)
    {
        return;
    }
    else
    {
        cout<<"MAC::Node"<<node->nodeid<<" mac_receive_busytone_end error"<<endl;
        exit(-1);
    }
}


//////////////send data collision



void MAC::mac_receive_data_collision(const Event & event)
{
    if(this->to_send_coll==1){
        return;
    }
    
    if(this->state==MAC_IDLE)
        return;
    else if(this->state==MAC_BUSY)
        return;
    else if (this->state==PT)
    {
        if(event.nodeid + (1<<10) == this->dst)
        {
            this->pt_coll++;
            this->pt_collsignal_coll++;
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0 * SLOT);
        }
        return;
        
    }
    
    else if ((this->state==BFD_PT || this->state==BFD_ST))
    {
        if(event.nodeid==(int)this->peer-(1<<10))
        {
            if(this->to_send_coll != 1){
                if(this->state==BFD_PT)
                {
                    this->pt_coll++;
                    this->pt_collsignal_coll++;
                }
                else if(this->state==BFD_ST)
                {
                    if(ST_CW_FIX)
                        this->to_cwfix=1;
                    this->st_coll++;
                    //                    this->st_collsignal_coll++;
                }
                this->set_mac_busy();
                this->to_send_coll=1;
                this->mac_generate_send_data_collision_event(0 * SLOT);
            }
        }
        return;
    }
    else if ((this->state==DBFD_PT || this->state==DBFD_ST || this->state == DBFD_SR))
    {
        if(event.nodeid==(int)this->dst-(1<<10) && this->state==DBFD_PT && this->to_send_coll != 1)
        {
            this->pt_coll++;
            this->pt_collsignal_coll++;
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0 * SLOT);
            return;
        }
        else if(event.nodeid==(int)this->peer-(1<<10) && this->state==DBFD_ST && this->to_send_coll != 1)
        {
            this->pt_coll++;
            this->pt_collsignal_coll++;
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0 * SLOT);
            return;
        }
        else if(event.nodeid==(int)this->peer-(1<<10) && this->state==DBFD_SR)
        {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0 * SLOT);
            return;
        }
        
    }
    
    else if ((this->state==HD_PT || this->state==HD_PR))
    {
        if(event.nodeid==(int)this->dst-(1<<10))
        {
            if(this->to_send_coll != 1){
                if(this->state==HD_PT)
                {
                    this->pt_coll++;
                    this->pt_collsignal_coll++;
                }
                else if(this->state==HD_PR)
                {
                    if(ST_CW_FIX)
                        this->to_cwfix=1;
                    this->st_coll++;
                    //                    this->st_collsignal_coll++;
                }
                this->set_mac_busy();
                this->to_send_coll=1;
                this->mac_generate_send_data_collision_event(0 * SLOT);
            }
        }
        return;
    }
    
    else
    {
        cout<<"MAC::receive_data_collision error"<<endl;
        exit(-1);
    }
    
}
//////////////send ack


void MAC::mac_receive_ack(const ACK & ack)
{
    if(this->state==MAC_IDLE){
        this->set_mac_busy();
        return;
    }
    else if(this->state==MAC_BUSY)
        return;
    else if (this->state==PT)
    {
        this->pt_coll++;
        this->set_mac_busy();
        this->to_send_coll=1;
        this->mac_generate_send_data_collision_event(PHY_HEADER+ MAC_HEADER);
        return;
    }
    else if (this->state==HD_PT ||this->state ==HD_PR)
    {
        if( this->state==HD_PT && this->m_adrress==ack.receiver)
        {
            return;
        }
        else
        {
            if(this->state==HD_PT){
                this->pt_coll++;
            }
            else if (this->state==HD_PR){
                if(ST_CW_FIX)
                    this->to_cwfix=1;
                this->st_coll++;
            }
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
            return;
        }
    }
    
    else if(this->state==BFD_ST|| this->state==BFD_PT)
    {
        if(this->m_adrress==ack.receiver)
        {
            return;
        }
        else
        {
            if(this->state==BFD_PT){
                this->pt_coll++;
            }
            else if (this->state==BFD_ST){
                if(ST_CW_FIX)
                    this->to_cwfix=1;
                this->st_coll++;
            }
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
            return;
        }
    }
    
    else if(this->state==DBFD_ST|| this->state==DBFD_PT)
    {
        if(this->m_adrress==ack.receiver)
        {
            return;
        }
        else
        {
            if(this->state==DBFD_PT){
                this->pt_coll++;
            }
            else if (this->state==DBFD_ST){
                if(ST_CW_FIX)
                    this->to_cwfix=1;
                this->st_coll++;
            }
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
            return;
        }
    }
    else if(this->state==DBFD_SR)
    {
        if(this->m_adrress==ack.receiver)
        {
            cout<<"MAC:: mac_receive_ack error1"<<endl;
            exit(-1);
        }
        else
        {
            this->set_mac_busy();
            return;
        }
    }
    
    
    
    return;
}


void MAC::mac_receive_ack_end(const ACK & ack)
{
    Node* node= (Node*) this->node;
    PHY*  phy=  &(node->PHYlayer);
    if(this->state==MAC_IDLE || this->state == PT){
        cout<<"MAC::Node"<<node->nodeid<<" mac_receive_ack_end error0"<<endl;
        exit(-1);
    }
    else if (this->state==MAC_BUSY)
    {
        return;
    }
    
    
    else if(this->state==HD_PT || this->state == HD_PR)
    {
        if (ack.receiver == this->m_adrress)
        {
            state==BFD_ST? this->st_suc++ : this->pt_suc++ ;
            this->CW=CWmin;
            this->mac_pop_data();
            if(phy->tx_state == PHY_IDLE && phy->rx_state==PHY_IDLE)
                this->set_mac_busy();
        }
        else
        {
            cout<<"MAC::Node"<<node->nodeid<<" mac_receive_ack_end error1"<<endl;
            exit(-1);
        }
    }
    
    else if(this->state==BFD_PT || this->state == BFD_ST)
    {
        if (ack.receiver == this->m_adrress)
        {
            state==BFD_ST? this->st_suc++ : this->pt_suc++ ;
            this->CW=CWmin;
            this->mac_pop_data();
            if(phy->tx_state == PHY_IDLE && phy->rx_state==PHY_IDLE)
                this->set_mac_busy();
        }
        else
        {
            cout<<"MAC::Node"<<node->nodeid<<" mac_receive_ack_end error2"<<endl;
            exit(-1);
        }
    }
    
    else if(this->state==DBFD_ST || this->state==DBFD_PT)
    {
        if (ack.receiver == this->m_adrress)
        {
            state==BFD_ST? this->st_suc++ : this->pt_suc++ ;
            this->CW=CWmin;
            this->mac_pop_data();
            if(phy->tx_state == PHY_IDLE && phy->rx_state==PHY_IDLE)
                this->set_mac_busy();
        }
        else
        {
            cout<<"MAC::Node"<<node->nodeid<<" mac_receive_ack_end error3"<<endl;
            exit(-1);
        }
    }
    else if (this->state == DBFD_SR )
    {
        cout<<"MAC::Node"<<node->nodeid<<" mac_receive_ack_end error4"<<endl;
        exit(-1);
    }
    
}

void MAC::mac_receive_ack_collision(const Event & event)
{
    if(this->state==MAC_IDLE || this->state==MAC_BUSY){
        return;
    }
    else if (this-> state == PT)
    {
        if (event.nodeid+ (1<<10) == this->dst )
        {
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(0 * SLOT);
        }
    }
    else if (this->state == HD_PT || this-> state == HD_PR)
    {
        if (event.nodeid == this-> dst)
            this->set_mac_busy();
        else
            return ;
    }
    else if (this->state == BFD_PT || this->state ==BFD_ST)
    {
        if (event.nodeid == this-> dst)
            this->set_mac_busy();
        else
            return ;
    }
    else if (this->state == DBFD_PT || this->state == DBFD_ST)
    {
        if (event.nodeid == this-> dst)
            this->set_mac_busy();
        else
            return ;
    }
    else
    {
        cout<<"MAC::receive ack collision error"<<endl;
        exit(-1);
    }
}

// need more

////////random_number
random_number::random_number()
{
    srand((unsigned int)time(NULL));
};

int random_number::ran()
{
    return rand();
};



////////////////   state motivated




