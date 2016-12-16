//
//  MAC.cpp
//  Wireless Simulator
//
//  Created by 段荣昌 on 2016/11/4.
//  Copyright © 2016年 段荣昌. All rights reserved.
//


#include "MAC.h"
#include "Event.h"
#include "Node.h"
#include "Simulator.h"
#include "Common.h"
#include <iostream>
#include <math.h>



using namespace std;


////////MAC
int random(int CW)
{
    extern random_number ran_generator;
    return ran_generator.ran()%CW;
}

u_seconds randomExponential(double lambda) // lambda packets generated in 1000000
{
    double pV = 0.0;
    extern random_number ran_generator;
    while(true)
    {
        pV = (double)ran_generator.ran()/(double)RAND_MAX;
        if (pV != 1)
        {
            break;
        }
    }
    pV = pow(10,6)*(-1.0/lambda)*log(1-pV);
    return pV;
}

MAC::MAC()
{
    this->state=MAC_IDLE;
    this->m_queue.reserve(150);//set the max length of queue;
    while (!this->m_queue.empty()) {
        this->m_queue.clear();
    }
    this->CW=32;
    this->backoff_count=0;
    this->m_adrress=0;
    this->node=NULL;
    this->iter=m_queue.begin();
    this->peer=0;
    this->dst=0;
    this->freeze_flag=0;
}

/////////generate events

void MAC::mac_generate_send_data_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_data_event(t);
};

void MAC::mac_generate_send_data_event()
{
    Node *node= (Node*) this->node;
    node->generate_sending_data_event(DIFS+random(CW)*SLOT);
};


void MAC::mac_generate_send_data_collision_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_data_collision_event(t);
};


void MAC::mac_generate_send_ack_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_ack_collision_event(t);
}

void MAC::mac_generate_send_ack_collision_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_data_collision_event(t);
}


void MAC::mac_generate_inner_node_event()
{
    Node *node= (Node*) this->node;
    node->generate_inner_node_event(randomExponential(PACKETS_IN_1_SECONDS));
}

////////// handle


//inner node

void MAC::mac_generate_data()
{
    if(m_queue.size()==150)
    {
        cout<<"MAC::queue is full,drop packet"<<endl;
        this->mac_generate_inner_node_event();
        return;
    }
    
    DATA tmp;
    extern random_number ran_generator;
    address dst= (ran_generator.ran() % (size_of_Nodelist-2)) + (1<<10)+1;//Size of Nodelist
    if(dst >= this->m_adrress)
        dst++;
    tmp.source=this->m_adrress;
    tmp.destination=dst;
    tmp.peer=0;
    tmp.payload=1280;
    tmp.type=0;
    if(m_queue.empty() && this->state==MAC_IDLE)
    {
        mac_generate_send_data_event(random(CW)*SLOT);
    }
    //    this->iter=m_queue.begin();
    this->m_queue.push_back(tmp);
    this->mac_generate_inner_node_event();
}

//send data
void MAC::mac_send_data()
{
    if(this->state==MAC_IDLE)
    {
        // to be added
        this->state= BFD_PT; // BFD only now
        this->iter=this->m_queue.begin();
        this->dst = (*iter).destination;
        this->peer =(*iter).destination;
        // add the peer and type for a data packet.
        (*iter).type=BFD_PT;
        (*iter).peer=(*iter).destination;
    }
    
    //to be decide
}

void MAC::mac_receive_data(const DATA& data)
{
    if(this->state==MAC_IDLE)
    {
        if(this->m_adrress==data.destination)
        {
            if(data.type==BFD_PT)
            {
                this->state=BFD_ST;
                this->peer=data.source;
                this->dst=data.source;
                
                if(this->have_data(this->dst))
                {
                    this->mac_generate_send_data_event(HEADER/RATE);
                }
            }
            //            else if(data.type==DBFD_PT)
            //            {
            //                this->state=DBFD_ST;
            //                this->peer=data.source;
            //                this->dst=0; // to be decide
            //                this->mac_generate_send_data_event(HEADER/RATE);
            //                cout<<"MAC::to be decided"<<endl;
            //            }
            //            else if(data.type==DBFD_ST)
            //            {
            //                this->state=DBFD_SR;
            //                this->peer=data.source;
            //                this->dst=0; // to be decide
            //            }
            //            else if(data.type==SBFD_PT)
            //            {
            //                this->state=SBFD_PR;
            //                this->peer=data.source;
            //                this->dst=0;
            //                this->mac_generate_send_data_event(HEADER/RATE);
            //                cout<<"MAC::to be decided"<<endl;
            //            }
            //            else
            //            {
            //                cout<<"MAC::error"<<endl;
            //            }
        }
        else if(this->m_adrress==data.peer)
        {
            if(data.type==SBFD_PT)
            {
                this->state=SBFD_ST;
                this->peer=data.source;
                this->dst=data.source;
                this->mac_generate_send_data_event(HEADER/RATE);
                cout<<"MAC::to be decided"<<endl;
            }
        }
        else
        {
            //            Node *tmp=(Node*) this->node;
            //            cout<<"MAC::Node"<<tmp->nodeid <<" receive data to mac busy"<<endl;
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
        }
    }
    if ((this->state==BFD_PT) || (this->state==BFD_ST) )
    {
        if(mac_is_collision(data))
        {
            if((*iter).destination== this->peer)
            {
                cout<<" possible "<<endl;
                extern int collided;
                collided++;
            }
            
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
            this->mac_generate_send_data_collision_event(5);
            
            
            Node* tmpnode= (Node*) node;
            cout<<"MAC::Node"<<tmpnode->nodeid<< " send collision "<<endl;
        }
    }
    else if(this->state==MAC_BUSY)
    {
        if((data.type==BFD_PT || data.type==BFD_ST) && (data.destination==this->m_adrress|| data.peer== this->m_adrress))
        {
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
            this->mac_generate_send_data_collision_event(5);
        }
    }
    
    
    
    //    if(this->state != MAC_BUSY &&  mac_is_collision(data))
    //    {
    //
    //
    //
    //
    //        this->state=MAC_BUSY;
    //        this->peer=0;
    //        this->dst=0;
    //        this->mac_generate_send_data_collision_event(0);
    //        return;
    //    }
    
    //
    //
    //
    //            //try_send_back();
    //
    ///////find and get the element to build fd transmissions.
    //            if(!this->m_queue.empty())
    //            {
    //                //find_send_back();
    //                this->mac_generate_send_data_event(HEADER);//decoding phy header
    //                extern int success;
    //                success++;
    //            }
    //
    //            else
    //            {
    //                cout<<"cannot send back because of no data in queue"<<endl;
    //                extern int failed;
    //                failed++;
    //            }
    ////            this->mac_generate_data();
    ////
    ////            mac generate a sending back event. //+ decoding mac header (45);
    //        }
    
}

bool MAC::have_data(address dst)
{
    extern int fd;
    this->iter= this->m_queue.begin();
    if(m_queue.empty())
    {
        return false;
    }
    if( (*iter).destination==dst)
    {
        cout<<"the first packet is to send to build fd transmissions"<<endl;
        fd+=2;
        return true;
    }
    iter++;
    for(;iter < m_queue.end();iter++)
    {
        if( (*iter).destination==dst )
        {
            cout<<"other packet is to send to build fd transmissions"<<endl;
            fd+=2;
            return true;
        }
    }
    this->iter=this->m_queue.begin();
    return false;
}

bool MAC::mac_is_collision(const DATA & data)
{
    if(data.source == this->peer)
        return false;
    else
        return true;
}

//send data end

void MAC::mac_send_data_end()
{
    //    Node* tmpnode = (Node*) this->node;
    //    cout<<"MAC::Node"<<tmpnode->nodeid <<" send a DATA END"<<endl;
}

void MAC::mac_receive_data_end(const DATA & data)
{
    if(data.destination==this->m_adrress)
    {
        if(data.source==this->peer)
        {
            extern int success;
            success++;
            //this->mac_generate_send_ack_event(0);
        }
        else
        {
            extern int failed;
            cout<<"why failed"<<endl;
            failed++;
            //this->mac_generate_send_ack_collision_event(0);
        }
    }
}

void MAC::mac_pop_data()
{
    this->m_queue.erase(this->iter);
    iter=m_queue.begin();
}

//send data collision

void MAC::mac_send_data_collision()
{
    
    //cout<<"mac_send_collision"<<endl;
    // double CW
    this->state=MAC_BUSY;
    this->iter=m_queue.begin();
    this->peer=0;
    this->dst=0;
    
}

void MAC::mac_receive_data_collision(const Event & event)
{
    //cout<<"receive collision"<<endl;
    if(event.nodeid==(int)this->peer-(1<<10))
    {
        if(this->state==BFD_PT || this->state==BFD_ST)// T
        {
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
            this->mac_generate_send_data_collision_event(0);
            extern int collided;
            collided++;
            Node* tmpnode= (Node*) node;
            cout<<"MAC::Node"<<tmpnode->nodeid <<" recieve and send collision"<<endl;
        }
    }
}


DATA MAC::get_data()
{
    DATA data;
    data=*(this->iter);
    data.type=this->state;
    return data;
}

void MAC::freeze(int count)
{
    this->backoff_count=count;
}


////////random_number
random_number::random_number()
{
    srand((unsigned int)time(NULL));
};


int random_number::ran()
{
    return rand();
};
