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



using namespace std;


////////MAC
int random(int window)
{
    extern random_number ran_generator;
    return ran_generator.ran() % window;
}

u_seconds randomExponential(double lambda) // lambda packets generated in 1000000
{
    double pV = 0.0;
    extern random_number ran_generator;
    while(true)
    {
        pV = (double)ran_generator.ran()/(double)RAND_MAX;
        if (pV != 1)
            break;
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
    this->to_send_ack=0;
    this->to_busy=0;
    
    this->to_T_coll=0;
    
    this->num_bour=0;
    memset(this->neighbour, size_of_Nodelist, sizeof(int)*3);
}

/////////generate events

void MAC::mac_generate_send_data_event(u_seconds t)  /// start based on FD transmission
{
    Node *node= (Node*) this->node;
    node->generate_sending_data_event(t);
};

void MAC::mac_generate_send_data_event()   /// start based on backoff
{
    Node *node= (Node*) this->node;
    if(freeze_flag==1)
    {
        node->generate_sending_data_event(DIFS+backoff_count*SLOT);
        freeze_flag=0;
        if(this->state != MAC_IDLE)  //////// questions?
        {
            cout<<"MAC::generate_send_event_error"<<endl;
            exit(-1);
        }
        cout<<"MAC::Node"<<node->nodeid<<" restarts a backoff, from "<< backoff_count <<endl;
    }
    else
    {
        this->backoff_count=random(CW);
        cout<<"MAC::Node"<<node->nodeid<<" starts a new backoff, base on "<< this->CW <<" and current backoff is "<< this->backoff_count <<endl;
        node->generate_sending_data_event(DIFS+backoff_count*SLOT);
        return;
    }
};

void MAC::mac_generate_send_data_collision_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_data_collision_event(t);
};

void MAC::mac_generate_send_ack_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_ack_event(t);
}

void MAC::mac_generate_send_ack_collision_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_ack_collision_event(t);
}

void MAC::mac_generate_inner_node_event()
{
    Node *node= (Node*) this->node;
    node->generate_inner_node_event(randomExponential(PACKETS_IN_1_SECONDS));
}

////////// handle



///////////inner node

void MAC::mac_generate_data()
{
    if(m_queue.size()==150)
    {
        cout<<"MAC::queue is full,drop packet"<<endl;
        this->mac_generate_inner_node_event();
        return;
    }
    
    extern random_number ran_generator;
    DATA tmp_data;
    int tmp_index;

    
    tmp_index=  (ran_generator.ran()% (this->num_bour) ) + 1;
    address dst= (1<<10)+this->neighbour[tmp_index][0];
    
    tmp_data.source=this->m_adrress;
    tmp_data.destination=dst;
    tmp_data.peer=0;
    tmp_data.payload=1280;
    tmp_data.type=0;
    
    if(m_queue.empty() && this->state==MAC_IDLE)
    {
        mac_generate_send_data_event(random(CW)*SLOT);
    }
    
    this->m_queue.push_back(tmp_data);     // may cause error in future.?????
    this->mac_generate_inner_node_event();
}

//////////send data
void MAC::mac_send_data()
{
    if(this->state==MAC_IDLE)
    {
        /////// to added other modes
        
        this->state = BFD_PT; // BFD only now
        this->iter  = this->m_queue.begin();
        this->dst   = (*iter).destination;
        this->peer  = (*iter).destination;
        
        // add the peer and type for a data packet.
        (*iter).type=BFD_PT;
        (*iter).peer=(*iter).destination;
    }
    else if(this->state==BFD_PT || this->state==BFD_ST)
    {
        if (this->state==BFD_ST)
                return;
        else
        {
            cout<<"MAC::send data error"<<endl;
            exit(2);
        }
    }
    
    else
    {
        cout<<"MAC::send data error"<<endl;
        exit(-1);
    }
}

void MAC::mac_receive_data(const DATA& data)
{
    Node* node=(Node*) this->node;
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
            else
            {
                cout<<"MAC::receive data error"<<endl;
                exit(-1);
            }
        }
        
        //        else if(this->m_adrress==data.peer)
        //        {
        //            if(data.type==SBFD_PT)
        //            {
        //                this->state=SBFD_ST;
        //                this->peer=data.source;
        //                this->dst=data.source;
        //                this->mac_generate_send_data_event(HEADER/RATE);
        //                cout<<"MAC::to be decided"<<endl;
        //                exit(-1);
        //            }
        //            else
        //            {
        //                cout<<"MAC::error "<<endl;
        //                exit(-1);
        //            }
        //        }
        
        else
        {
            //            Node *tmp=(Node*) this->node;
            //            cout<<"MAC::Node"<<tmp->nodeid <<" receive data to mac busy"<<endl;
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
        }
    }
    
    else if(this->state==MAC_BUSY)
    {
        if((data.type==BFD_PT || data.type==BFD_ST) && (data.destination==this->m_adrress|| data.peer== this->m_adrress))
        {
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
            this->mac_generate_send_data_collision_event(0);
        }
    }
    
    else if( (this->state==BFD_PT) || (this->state==BFD_ST) )
    {
        if(! (this->peer==data.source && this->m_adrress== data.destination))
        {
            if((*iter).destination == this->peer)  //// if BFD_ST sends, it is true, and if BFD_ST doesn't , it is not. &
                                                    //// BFD_PT is always true
            {
                extern int T_coll;
                T_coll++;
                Node* node=(Node*)this->node;
                cout<<"T_COLL:"<<T_coll<<" NODEID:"<<node->nodeid <<endl;
            }
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
            node->next_sending_event.t=0;
            this->mac_generate_send_data_collision_event(0);
        }
    }
    
}

bool MAC::have_data(address dst)
{
    extern int fd_op;
    this->iter= this->m_queue.begin();
    if(m_queue.empty())
    {
        return false;
    }
    if( (*iter).destination==dst)
    {
        cout<<"the first packet is to send to build fd transmissions"<<endl;
        fd_op+=1;
        this->to_busy=0;
        return true;
    }
    
    iter++;
    for(;iter < m_queue.end();iter++)
    {
        if( (*iter).destination==dst )
        {
            cout<<"other packet is to send to build fd transmissions"<<endl;
            fd_op+=1;
            this->to_busy=0;
            return true;
        }
    }
    
    this->iter=this->m_queue.begin();
    this->to_busy=1;
    return false;
}


void MAC::freeze(int count)
{
    if(freeze_flag==0 && (this->state==MAC_IDLE) && !(this-> m_queue).empty())
    {
        freeze_flag=1;
        if(backoff_count!=0)
        {
            if(backoff_count >= count)
                backoff_count=count;
        }
        else
        {
            backoff_count = count;
        }
        cout<<"MAC::Node"<<this->m_adrress-(1<<10)<<" backoff count freeze to "<<backoff_count <<endl;
    }
}

DATA MAC::get_data()
{
    DATA data;
    data=*(this->iter);
    data.type=this->state;
    return data;
}

///////////send data end

void MAC::mac_send_data_end()
{
    if(this->state==MAC_IDLE)
    {
        cout<<"MAC::error"<<endl;
        exit(-1-1);
    }
    
    else if(this->state==MAC_BUSY)
    {
       // Node* tmp=(Node*)this->node;
        cout<<"MAC::error"<<endl;
        exit(-1-2);
    }
    
    else if(this->state==BFD_PT || this->state ==BFD_ST)
    {
        if(this->to_send_ack==1)
        {
            this->mac_generate_send_ack_event(0);
            this->to_send_ack=0;
        }
    }
    else
    {
        cout<<"MAC::error"<<endl;
        exit(-1-3);
    }
}

void MAC::mac_receive_data_end(const DATA & data)
{
    Node* node= (Node*) this->node;
    PHY* phy = &(node->PHYlayer);
    
    if(this->state==MAC_IDLE)
    {
        cout<<"error"<<endl;
        exit(-1-4);
    }
    
    else if(this->state==MAC_BUSY)
    {
        if(data.destination==this->m_adrress)
        {
            this->mac_generate_send_ack_collision_event(0);
        }
    }
    
    else if(this->state==BFD_PT || this->state ==BFD_ST)
    {
        if(data.destination==this->m_adrress)
        {
            if(phy->tx_state==PHY_IDLE && phy->rx_state==PHY_IDLE)
                this->mac_generate_send_ack_event(0);
            else
            {
                this->to_send_ack=1;
            }
        }
        else
        {
            cout<<"MAC::error"<<endl;
            exit(-1-5);
        }
    }
    
    
    //    if(data.destination==this->m_adrress)
    //    {
    //        if(data.source==this->peer)
    //        {
    ////            extern int success;
    ////            success++;
    //
    //            this->mac_generate_send_ack_event(0);
    //        }
    //        else
    //        {
    ////            extern int failed;
    ////            cout<<"why failed"<<endl;
    ////            failed++;
    //            this->mac_generate_send_ack_collision_event(0);
    //        }
    //    }
}

void MAC::mac_pop_data()
{
    this->m_queue.erase(this->iter);
    iter=m_queue.begin();
}

//////////////send data collision

void MAC::mac_send_data_collision()
{
    if(this->state != MAC_BUSY && this->to_T_coll==1)
    {
        this->state=MAC_BUSY;
        this->peer=0;
        this->dst=0;
        extern int T_coll;
        T_coll++;
        Node* tmpnode=(Node*) this->node;
        
        tmpnode->next_sending_event.t=0;
        cout<<"T_COLL:"<<T_coll<<" NODEID:"<<tmpnode->nodeid<<endl;
        this->to_T_coll=0;
    }
    
    if(this->state!=MAC_BUSY)
    {
        cout<<"MAC::error"<<endl;
        exit(-1-6);
    }
    
    if(CW<CWmax)
    {
        CW=2*CW;
        cout<< "MAC::Node"<< (m_adrress-(1<<10)) <<", CW is " << CW << endl;
    }
    else
    {
        CW=CWmax;
        cout<< "MAC::Node"<< (m_adrress-(1<<10)) <<", CW reaches CWmax " << CW << endl;
    }
    this->to_T_coll=0;
    this->state=MAC_BUSY;
    this->iter=m_queue.begin();
    this->peer=0;
    this->dst=0;
}

void MAC::mac_receive_data_collision(const Event & event)
{
    if(this->state==MAC_IDLE)
        return;
    else if(this->state==MAC_BUSY)
        return;
    else if ((this->state==BFD_PT || this->state==BFD_ST))
    {
        if(this->to_T_coll==1)
        {
            Node* tmpnode= (Node*) node;
            cout<<"NODE"<<tmpnode->nodeid<<" WAIT!!"<<endl;
            return;
        }
        if(event.nodeid==(int)this->peer-(1<<10))
        {
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
            this->mac_generate_send_data_collision_event(0);
            
            if(this->to_T_coll != 1)
            {
                extern int R_coll;
                R_coll++;
                Node* tmpnode= (Node*) node;
                cout<<"MAC::Node"<<tmpnode->nodeid <<" recieve and will send collision"<<endl;
                cout<<"R_COLL:"<<R_coll<<" Node:"<< tmpnode->nodeid<<endl;
            }
        }
    }
    else
    {
        exit(-1);
    }
    
}
//////////////send ack
void MAC::mac_send_ack()
{
    if(this->state==MAC_IDLE|| this->state==MAC_BUSY)
    {
        cout<<"error"<<endl;
        exit(-1-7);
    }
    else if(this->state==BFD_PT || this->state== BFD_ST)
    {
        return;
    }
    
}

void MAC::mac_receive_ack(const ACK & ack)
{
    if(this->state==MAC_IDLE){
        this->state=MAC_BUSY;
        this->peer=0;
        this->dst=0;
        return;
    }
    else if(this->state==MAC_BUSY)
    {
        return;
    }
    else if(this->state==BFD_ST|| this->state==BFD_PT)
    {
        if(this->m_adrress==ack.destination)
            return;
        else
        {
            //            if((*iter).destination == this->peer)
            //            {
            //                extern int collided;
            //                collided++;
            //            }
            
            Node* tmpnode= (Node*) node;
            this->state=MAC_BUSY;
            this->peer=0;
            this->dst=0;
            this->mac_generate_send_data_collision_event(0);
            tmpnode->next_sending_event.t=0;
            
            cout<<"MAC::Node"<<tmpnode->nodeid<< " send collision "<<endl;
            
            extern int T_coll;
            T_coll++;
        }
    }
    return;
}

ACK MAC::get_ack()
{
    ACK ack;
    ack.destination=this->peer;
    return ack;
}

//////////////send ack
void MAC::mac_send_ack_end()
{
    if(this->state==MAC_IDLE)
    {
        cout<<"MAC::send_ack_end error"<<endl;
        exit(-1-8);
    }
    else if(this->state==MAC_BUSY)
    {
        cout<<"MAC::send_ack_end error"<<endl;
        exit(-1-9);
    }
    
    else if(this->state==BFD_PT || this->state ==BFD_ST)
    {
        if(this->to_busy==1)   /////// currently BFD == DBFD
        {
            this->state=MAC_BUSY;
            this->to_busy=0;
            this->to_send_ack=0;
        }
    }
    else
    {
        cout<<"MAC::send_ack_end error"<<endl;
        exit(-1-10);
    }
}

void MAC::mac_receive_ack_end(const ACK & ack)
{
    Node* node= (Node*) this->node;
    PHY* phy = &(node->PHYlayer);
    
    if(this->state==MAC_IDLE || this->state==MAC_BUSY){
        return;
    }
    else if(this->state==BFD_ST || this->state==BFD_PT)
    {
        if (ack.destination==this->m_adrress)
        {
            extern int success;
            success++;
            cout<<"SUCCESS:"<<success<<endl;
            CW=CWmin;
            cout<< "MAC::Node"<< (m_adrress-(1<<10)) <<", CW back to " << CW << endl;
            
            if(this->state==BFD_ST)
            {
                extern int fd_suc;
                fd_suc++;
                cout<<"FD SUC:"<<fd_suc<<endl;
            }
            
            if(phy->tx_state == PHY_IDLE && phy->rx_state==PHY_IDLE)
                this->state=MAC_BUSY;
            else
            {
                this->to_busy=1;
            }
        }
        else
        {
            cout<<"MAC::receive_ack_end error"<<endl;
            exit(-1);
        }
    }
}


void MAC::mac_send_ack_collision()
{
    if(this->state != MAC_BUSY)
    {
        cout<<"MAC::send_ack_collision error"<<endl;
        exit(-1-11);
    }
}

void MAC::mac_receive_ack_collision(const Event & event)
{
    if(this->state==MAC_IDLE ||this->state==MAC_BUSY){
        return;
    }
    else if(this->state==BFD_PT || this->state==BFD_ST)
    {
        if (event.nodeid == this->dst) {
            extern int failed;
            failed++;
            this->state=MAC_BUSY;
        }
        else
            return;
    }
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



////////////////   state motivated




