
#include "Simulator.h"
#include "Event.h"
#include "Node.h"
#include "Common.h"
#include <math.h>



void Simulator::run()
{
    u_seconds endtime= ENDTIME;
    while(!this->sim_queue.empty())
    {
        Event tmp_event=this->sim_queue.top();
//        extern Node Nodelist[size_of_Nodelist];
//        if(Nodelist[1].MAClayer.to_T_coll==1 || Nodelist[2].MAClayer.to_T_coll==1 || Nodelist[3].MAClayer.to_T_coll==1 )
//        {
//            cout<< Nodelist[1].MAClayer.to_T_coll << Nodelist[2].MAClayer.to_T_coll<<Nodelist[3].MAClayer.to_T_coll<<endl;
//        }
        if(tmp_event.t >= endtime)
            break;
        this->sim_queue.pop();
        this->sch(tmp_event);
        cout<<endl;
    }
}

void Simulator::add(const Event &event)
{
    this->sim_queue.push(event);
}

//we don't delete events in simulator direcetly, So simulator should check before making event executive
//check this event should or should not be execute?

bool check_available(const Event &event)
{
    extern Node Nodelist[size_of_Nodelist];
    Node* node= &Nodelist[event.nodeid];
    if(node->next_sending_event == event)  //include send & end
        return true;
    return false;
};

// Simulator handle different type of event
// We treat end and collision as a kind of sending case.

void Simulator::sch(const Event & event)
{
    extern Node Nodelist[size_of_Nodelist];
    
    if (event.type==Sending_data)
    {
        if (!check_available(event)) {
            return;
        }
        
        Node* sending_node= &Nodelist[event.nodeid];
        DATA on_channel_data = sending_node->send_data(event);;
        
        for (int i=1;i<size_of_Nodelist;i++){
            if(i!= event.nodeid){
                Node* receiving_node= &Nodelist[i];
                receiving_node->receive_data(event, on_channel_data);
            }
        }
    }
    
    else if (event.type==Sending_data_end)
    {
        if (!check_available(event)) {
            return;
        }
        
        Node* sending_node= &Nodelist[event.nodeid];
        DATA on_channel_data=sending_node->send_data_end(event);
        
        for (int i=1;i<size_of_Nodelist;i++){
            if(i!= event.nodeid){
                Node* receiving_node= &Nodelist[i];
                receiving_node->receive_data_end(event, on_channel_data);
            }
        }
    }
    
    else if(event.type==Sending_data_collision)
    {
        Node* sending_node= &Nodelist[event.nodeid];
        
        if(sending_node->MAClayer.state==MAC_IDLE){
            return;
        }
        
        sending_node->send_data_collision(event);
        for (int i=1;i<size_of_Nodelist;i++){
            if(i!= event.nodeid){
                Node* receiving_node= &Nodelist[i];
                receiving_node->receive_data_collision(event);
            }
        }
    }
    
    else if(event.type== Sending_ack)
    {
        if (!check_available(event)) {
            return;
        }
        
        Node* sending_node= &Nodelist[event.nodeid];
        ACK on_channel_ack= sending_node->send_ack(event);
        
        for (int i=1;i<size_of_Nodelist;i++)
        {
            if(i!= event.nodeid)
            {
                Node* receiving_node= &Nodelist[i];
                receiving_node->receive_ack(event,on_channel_ack);
            }
        }
    }
    
    else if(event.type== Sending_ack_end)
    {
        if (!check_available(event)) {
            return;
        }
        
        Node* sending_node= &Nodelist[event.nodeid];
        ACK on_channel_ack= sending_node->send_ack_end(event);
        
        for (int i=1;i<size_of_Nodelist;i++)
        {
            if(i!= event.nodeid)
            {
                Node* receiving_node= &Nodelist[i];
                receiving_node->receive_ack_end(event,on_channel_ack);
            }
        }
    }
    
    else if(event.type==Sending_ack_collision)
    {
        Node* sending_node= &Nodelist[event.nodeid];
        if(sending_node->MAClayer.state==MAC_IDLE){
            return;
        }

        sending_node->send_ack_collision(event);
        for (int i=1;i<size_of_Nodelist;i++)
        {
            if(i!= event.nodeid)
            {
                Node* receiving_node= &Nodelist[i];
                receiving_node->receive_ack_collision(event);
            }
        }
    }
    
    
    else if(event.type==Inner_node)
    {
        Node* node= &Nodelist[event.nodeid];
        node->generate_data(event);
    }
    else
    {
        cout<<"Simulator::undefined type of event"<<endl;
        exit(-1);
    }
    
};


