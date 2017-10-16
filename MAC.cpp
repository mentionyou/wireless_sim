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
    this->to_sim_coll=0;
    this->to_send_coll=0;
//    this->queue is full,drop packet=0;
    this->num_bour=0;
    this->m_queue.reserve(QUEUE_SIZE);//set the max length of queue;
    this->success=0;
    memset(this->neighbour, 0, sizeof(int)*4*size_of_Nodelist);
    
    this->pt_suc=0;
    this->pt_coll=0;
    this->st_suc=0;
    this->st_coll=0;
    this->pt_fd_op=0;
    this->pt_fd_suc=0;
//    this->st_fd_suc=0;
    this->delay_max=0;
    this->delay_sum=0;
    this->delay_count=0;
}

/////////generate events


void MAC::mac_generate_send_data_event(u_seconds t)  /// start based on FD transmission
{
    Node *node= (Node*) this->node;
    node->generate_sending_data_event(t);
};

int AP_cw(int queuelength)
{
    if(queuelength < (QUEUE_SIZE)/2)
    {
        return 1024;
    }
    else if(queuelength < (QUEUE_SIZE*3)/4)
    {
        return 128;
    }
    else if(queuelength < (QUEUE_SIZE*9)/10)
        return 64;
    else
        return 64;
}

int client_cw(int queuelength)
{
    if(queuelength <= (QUEUE_SIZE)/2)
    {
        return 1024;
    }
    else if(queuelength <= (QUEUE_SIZE*3)/4)
    {
        return 256;
    }
    else
        return 128;
}



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
        node->generate_sending_data_event(DIFS+backoff_count*SLOT);
        freeze_flag=0;
//        cout<<"MAC::Node"<<node->nodeid<<" restarts a backoff, from "<< backoff_count <<endl;
        return;
    }
    else if (freeze_flag==0)
    {
        if(this->m_adrress<=num_AP+(1<<10))
        {
            if(AP_BACKOFF_PATTERN==0)
                this->backoff_count=random(CW);
            else if (AP_BACKOFF_PATTERN==1)
                this->backoff_count= random( AP_cw( (int) this->m_queue.size() ) );
        }
        else
        {
            if(CLIENT_BACKOFF_PATTERN==0)
                this->backoff_count=random(CW);
            else if(CLIENT_BACKOFF_PATTERN==1)
                this->backoff_count= random(  client_cw( (int) this->m_queue.size() ) );
        
        }
//        if(this->m_adrress-(1<<10)==9 )
//        {
//            cout<<"MAC::Node9 gen a backoff from "<<backoff_count<<endl;
//        }
        node->generate_sending_data_event(DIFS+backoff_count*SLOT);
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
    node->generate_sending_data_end_event(t);
}

void MAC::mac_generate_send_data_collision_event(u_seconds t)
{
    Node *node= (Node*) this->node;
//    cout<< "MAC::Node"<< node->nodeid<<" gen a send data collision event at "<< node-> current_t<<"s" <<endl;
    node->generate_sending_data_collision_event(t);
};

void MAC::mac_generate_send_ack_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_ack_event(t);
}

void MAC::mac_generate_send_ack_end_event(u_seconds t)
{
    Node *node= (Node*) this->node;
    node->generate_sending_ack_end_event(t);
}

void MAC::mac_generate_send_ack_collision_event(u_seconds t)
{
    Node *node= (Node*) this->node;
//    cout<< "MAC::Node"<< node->nodeid<<" gen a send ack collision event at"<< node-> current_t <<endl;
    node->generate_sending_ack_collision_event(t);
}

void MAC::mac_generate_inner_node_event()
{
    Node *node= (Node*) this->node;
    node->generate_inner_node_event(randomExponential(this->up_traffic));
    
}

////////// handle

void MAC::set_mac_busy()
{
    this->state=MAC_BUSY;
    this->peer=0;
    this->dst=0;
    this->to_sim_coll=0;
    this->to_busy=0;
    this->to_send_ack=0;
    this->iter=m_queue.begin();
    Node* node=(Node*) this->node;
    if(this->to_send_coll!=true && node->next_sending_event.type!=Sending_ack_collision )
        node->next_sending_event.t=0;
}

///////////inner node

void MAC::mac_generate_data()
{
    if(m_queue.size()==QUEUE_SIZE)
    {
//        cout<<"MAC::Node"<<this->m_adrress-(1<<10)<<"("<<this->state<<")"<<" queue is full,drop packet"<<endl;
        this->mac_generate_inner_node_event();
        return;
    }
    extern random_number ran_generator;
    extern Node Nodelist[size_of_Nodelist];
    DATA tmp_data;
    address tmp_dst;
    Node* node=(Node*) this->node;

    if(this->m_adrress > num_AP+(1<<10))
    {
        tmp_dst=this->neighbour[1][0]+(1<<10);
    }
    else
    {
        int tmp_index=ran_generator.ran()% this->up_traffic;
        int i=1; // index in neighbour
        while (1)
        {
            tmp_index -= this->neighbour[i][1];
            if (tmp_index<0)
                break;
            i++;
            if(i>this->num_bour)
            {
                cout<<"MAC::generate traffic error"<<endl;
                exit(-1);
            }
        }
        tmp_dst=this->neighbour[i][0]+(1<<10);
    }
    
    
    tmp_data.source=this->m_adrress;
    tmp_data.destination=tmp_dst;
    tmp_data.time=node->current_t;
    
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
    this->mac_generate_inner_node_event();
}


//////////send data

bool MAC::moredata(address dst)
{
//    if( m_queue.empty() )
//        return false;
    auto it = this->m_queue.begin();
    it++;
    for(;it < m_queue.end();it++)
    {
        if( (*it).destination==dst ){
            return true;
        }
    }
    return false;
}

void MAC::mac_send_data()
{
    if(this->state==MAC_IDLE)
    {
        /////// to added other modes
        if(this->m_queue.empty())
        {
            cout<<"MAC::send data error"<<endl;
            exit(-1);
        }
        
        
        this->state = BFD_PT; // BFD only now
        this->iter  = this->m_queue.begin();
        this->dst   = (*iter).destination;
        this->peer  = (*iter).destination;
        
        // add the peer and type for a data packet.
        (*iter).type = BFD_PT;
        (*iter).peer = (*iter).destination;
//        (*iter).flag_moredata= this->moredata(dst);
    }
    
    else if(this->state==BFD_PT || this->state==BFD_ST)
    {
        if (this->state != BFD_ST)
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
    
    this->mac_generate_send_data_end_event( PHY_HEADER + MAC_HEADER+ ((*iter).payload /RATE) );
    
}

void MAC::sim_trans()
{
    if(this->to_sim_coll==1 && this->to_send_coll==0)
    {
        this->to_sim_coll=0;
        this->set_mac_busy();
        if(this->to_send_coll!=0)
        {
            cout<<"MAC::sim trans error"<<endl;
            exit(-1);
        }
        this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
        this->to_send_coll=1;
        this->pt_coll++;
    }
}


void recordmoredata(MAC* const mac, const DATA data)
{
    for(int i=0;i<size_of_Nodelist;i++){
        if(data.source == (mac->neighbour[i][0])){
//            mac->neighbour[i][1]=data.flag_moredata;
            return;
        }
    }
}

void MAC::mac_receive_data(const DATA& data)
{
    if(this->to_send_coll == 1){
//        Node* tmpnode= (Node*) node;
//        cout<<"MAC::Node"<<tmpnode->nodeid<<" WAIT!!"<<endl;
        return;
    }
    
    if(this->state==MAC_IDLE)
    {
        if(this->m_adrress == data.destination)
        {
            if(data.type == BFD_PT)
            {
                this->state = BFD_ST;
                this->peer  = data.source;
                if(this->have_data(data.source)){
                    {
                        extern Node Nodelist[size_of_Nodelist];
                        Node* tmpnode=&( Nodelist[(data.source-(1<<10))]);
                        tmpnode->MAClayer.pt_fd_op++; // a chance to choose fd, or hd
                    }
                    this->mac_generate_send_data_event(PHY_HEADER+MAC_HEADER);
                    this->dst = data.source;
                }
                if(AVOID_UNNESSARY_TRANSMISSION==1 && this->m_adrress == 1+ (1<<10) && this->have_data(data.source) == false && AP_cw(this->m_queue.size()<=16) )
                {
                    this->set_mac_busy();
                    if(this->to_send_coll==false)
                    {
                        this->to_send_coll=1;
                        
                    }
                    else
                    {
                        cout<<"MAC:: to_send_coll error in receive data"<<endl;
                    }
                    this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
                }
                recordmoredata(this, data);
            }
            else
            {
                cout<<"MAC::receive data error"<<endl;
                exit(-1);
            }
        }
        else
            this->set_mac_busy();
    }
    
    else if(this->state==MAC_BUSY)
    {
        if(this->to_send_coll==1)
            return;
        if((data.type==BFD_PT || data.type==BFD_ST) && (data.destination==this->m_adrress|| data.peer== this->m_adrress))
        {
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
        }
    }
    
    else if( (this->state==BFD_PT) || (this->state==BFD_ST) )
    {
        
        if(! (this->peer == data.source && this->m_adrress == data.destination))
        {
            if(this->state==BFD_PT)
            {
                this->to_send_coll=1;
                this->pt_coll++;
                this->set_mac_busy();
                this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
                return;
            }
            else if ( this->state==BFD_ST && (!this->m_queue.empty() && ( (*iter).destination == this->peer )))
                //BFD_ST maynot build fd transmission with PT
            {
                this->st_coll++;
                this->set_mac_busy();
                this->to_send_coll=1;
                this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
                return;
            }
            else
            {
                this->set_mac_busy();
                this->to_send_coll=1;
                this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
                this->to_cwfix=1;
                return;
            }
        }
        else if (this->state==BFD_PT && data.type == BFD_PT)
        // for simutalous transmission
        {
            this->pt_coll++;
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
            return;
        }
    }
}

bool MAC::have_data(address dst)
{
    if( m_queue.empty() )
        return false;
    this->iter= this->m_queue.begin();
    for(;iter != m_queue.end();iter++)
    {
        if( (*iter).destination==dst ){
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
            if(backoff_count >= count)
                backoff_count=count;
        }
        else {
            backoff_count = count;
        }
        //cout<<"MAC::Node"<<this->m_adrress-(1<<10)<<" backoff count freeze to "<<backoff_count <<endl;
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
    if(this->state==MAC_IDLE || this->state==MAC_BUSY)
    {
        cout<<"MAC::send_data error"<<endl;
        exit(-1-1);
    }
    
    else if(this->state==BFD_PT || this->state ==BFD_ST)
    {
        if(this->to_send_ack == 1)
        {
            this->to_send_ack = 0;
            this->mac_generate_send_ack_event(0);
        }
    }
    else
    {
        cout<<"MAC::send_data error"<<endl;
        exit(-1-3);
    }
}

void MAC::mac_receive_data_end(const DATA & data)
{
    Node* node= (Node*) this->node;
    PHY* phy = &(node->PHYlayer);
    
    if(this->state == MAC_IDLE)
    {
        cout<<"MAC::Node"<<node->nodeid<<" mac_receive_data_end error"<<endl;
        exit(-1-4);
    }
    else if(this->state==MAC_BUSY)
    {
        if(data.destination==this->m_adrress)
        {
            this->to_send_coll=1;
            this->mac_generate_send_ack_collision_event(0);
        }
    }
    else if(this->state == BFD_PT || this->state == BFD_ST)
    {
        if(data.destination == this->m_adrress)
        {
            if(phy->tx_state==PHY_IDLE && phy->rx_state==PHY_IDLE)
                this->mac_generate_send_ack_event(0);
            else {
                this->to_send_ack = 1;
            }
            if (this->state==BFD_ST)
            {
//                this->st_fd_suc++;
                ;
                
            }
            
        }
        else
        {
            cout<<"MAC::receive_data_end error"<<endl;
            exit(-1-5);
        }
    }

}



//////////////send data collision

void MAC::mac_send_data_collision()
{
    if(this->state != MAC_BUSY)
    {
        cout<<"MAC::send_data_collision error"<<endl;
        exit(-1-6);
    }
    if(this->to_cwfix!=1)
    {
        if(CW<CWmax){
            CW=2*CW;
            //cout<< "MAC::Node"<< (m_adrress-(1<<10)) <<", CW is " << CW << endl;
            this->to_cwfix=1;
        }
        else{
            CW=CWmax;
            //cout<< "MAC::Node"<< (m_adrress-(1<<10)) <<", CW reaches CWmax " << CW << endl;
            this->to_cwfix=1;
        }
    }
    this->to_send_coll=0;
    this->set_mac_busy();
}

void MAC::mac_receive_data_collision(const Event & event)
{
    if(this->to_send_coll==1){
//        Node* tmpnode= (Node*) node;
//        cout<<"MAC::Node"<<tmpnode->nodeid<<" WAIT!!"<<endl;
        return;
    }
    
    if(this->state==MAC_IDLE)
        return;
    else if(this->state==MAC_BUSY)
        return;
    else if ((this->state==BFD_PT || this->state==BFD_ST))
    {
        if(event.nodeid==(int)this->peer-(1<<10))
        {
            if(this->to_send_coll != 1){
                this->set_mac_busy();
                this->to_send_coll=1;
//                this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
                this->mac_generate_send_data_collision_event(0 * SLOT);
            }
        }
    }
    else
    {
        cout<<"MAC::receive_data_collision error"<<endl;
        exit(-1);
    }
    
}
//////////////send ack
void MAC::mac_send_ack()
{
    if(this->state==MAC_IDLE || this->state==MAC_BUSY)
    {
        cout<<"MAC::mac_send_ack error"<<endl;
        exit(-1-7);
    }
    ACK ack;
    this->mac_generate_send_ack_end_event( PHY_HEADER+ack.payload /RATE);// size of ack
}

void MAC::mac_receive_ack(const ACK & ack)
{
    if(this->state==MAC_IDLE){
        this->set_mac_busy();
        return;
    }
    else if(this->state==MAC_BUSY)
        return;
    else if(this->state==BFD_ST|| this->state==BFD_PT)
    {
        if(this->m_adrress==ack.destination)
            return;
        else
        {
            if(this->state==BFD_PT)
            {
                this->pt_coll++;
                this->set_mac_busy();
                this->to_send_coll=1;
                this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
                return;
            }
            else if (this->state==BFD_ST && (!this->m_queue.empty() && (*iter).destination == this->peer ))
                //BFD_ST maynot build fd transmission with PT
            {

                this->st_coll++;
                //Node* node=(Node*)this->node;
                //cout<<"ST ack T_COLL:"<<T_coll<<" NODEID:"<<node->nodeid <<endl;
                this->set_mac_busy();
                this->to_send_coll=1;
                this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
                return;
            }
            this->set_mac_busy();
            this->to_send_coll=1;
            this->mac_generate_send_data_collision_event(SLOTS_TO_HANDLE_COLLISION * SLOT);
            this->to_cwfix=1;
            return;
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
    Node* node= (Node*) this->node;
    PHY*  phy= &(node->PHYlayer);
    if(this->state == MAC_IDLE || this->state==MAC_BUSY)
    {
        cout<<"MAC::send_ack_end error"<<endl;
        exit(-1-8);
    }
    
    else if(this->state==BFD_PT || this->state ==BFD_ST)
    {
        if (this->state==BFD_PT) {
            this->pt_fd_suc++;
            ;
        }
        if(phy->tx_state == PHY_IDLE && phy->rx_state==PHY_IDLE)
            this->set_mac_busy();
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
    PHY*  phy= &(node->PHYlayer);
    if(this->state==MAC_IDLE || this->state==MAC_BUSY){
        return;
    }
    else if(this->state==BFD_ST || this->state==BFD_PT)
    {
        if (ack.destination == this->m_adrress)
        {
            if(state==BFD_ST)
                this->st_suc++;
            else if(state==BFD_PT)
                this->pt_suc++;
            
            this->CW=CWmin;
//            more complex cw control would add in future.
//            cout<< "MAC::Node"<< (m_adrress-(1<<10)) <<", CW back to " << CW << endl;
            
            this->mac_pop_data();
            
            
            if(this->state==BFD_ST)
            {
                ;
//                extern int fd_suc;
//                fd_suc++;
                //cout<<"FD SUC:"<<fd_suc<<endl;
            }
            
            if(phy->tx_state == PHY_IDLE && phy->rx_state==PHY_IDLE)
                this->set_mac_busy();
        }
        else
        {
            cout<<"MAC::receive_ack_end error"<<endl;
            exit(-1);
        }
    }
    
}

void MAC::mac_pop_data()
{
    DATA data = this->get_data();
    Node* node=(Node*)this->node;
    u_seconds delay=node->current_t-data.time;
    this->delay_sum +=delay;
    this->delay_count++;
    if (this->delay_max< delay)
        this->delay_max=delay;
    this->m_queue.erase(this->iter);
    if(!this->m_queue.empty())
        iter=m_queue.begin();
}

void MAC::mac_send_ack_collision()
{
    if(this->state != MAC_BUSY){
        cout<<"MAC::send_ack_collision error"<<endl;
        exit(-1);
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
            this->set_mac_busy();
        }
        else
            return;
    }
} // need more

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




