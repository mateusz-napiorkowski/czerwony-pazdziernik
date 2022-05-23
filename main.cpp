#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <vector>
#include<mutex>
#include <thread>
#include<chrono>
#include <future>
#include<utility>
#include<unistd.h>
#define K 2
#define PROCESS_COUNT 4
#define MSG_SIZE 1

int channels[K] = {1, 2};


const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");

using namespace std;

mutex rc_mutex;

typedef struct mess {
  int position;
  int channel;
  int status;
  int T;
  int rank;
} mess;

int sizeOfMess(){
  return sizeof(mess)/sizeof(int);
}

struct mess_processTO {
  int rank;
  int T;
  int status;
};

struct general_process_struct { 
  int rank;
  char position;
  int channel;
  int status;
  int T;
  int responseCounter;
  vector<int> kryt_tab;
  vector<int> TO;
  vector<int> TZ;
};

typedef struct returnedMess {
  mess message;
  MPI_Status message_status;
} returnedMess;

  // TAGS: =================
  // request : 0
  // response yes : 1;
  // notification MESSAGE_KANAL_IN: 2;
  // notification MESSAGE_KANAL_OUT: 3;
  
  void incrementResponseCounter(general_process_struct* process){
    lock_guard<mutex> lock(rc_mutex);
    process->responseCounter++;
    // cout<<"rank : "<<process->rank<<" increment RESPONSE COUNTER to "<<process->responseCounter<<endl;
  }

  void clearResponseCounter(general_process_struct* process){
    lock_guard<mutex> lock(rc_mutex);
    process->responseCounter = 0;
    // cout<<"rank : "<<process->rank<<" RESPONSE COUNTER equals to "<<process->responseCounter<<endl;
  }

  void sendRequestToAll(general_process_struct* process, int status){
    mess process_mess;
    MPI_Request request;
    process_mess.channel = process->channel;
    process_mess.position = process->position;
    process_mess.rank = process->rank;
    process_mess.status = process->status;
    process_mess.T = process->T;
    for(int i=0; i<PROCESS_COUNT; i++){
      if(process->rank != i){
        // cout<<"rank : "<<process->rank<<" is sending request to "<< i <<" with tag "<<status<<endl;
        sleep(1);
        MPI_Isend(&process_mess, sizeOfMess(), MPI_INT, i, status, MPI_COMM_WORLD, &request);
      }
      
    }

  }

  void sendConfirmationAsReponse(general_process_struct* process, int dest_rank){
    mess process_mess;
    process_mess.channel = process->channel;
    process_mess.position = process->position;
    process_mess.rank = process->rank;
    process_mess.status = process->status;
    process_mess.T = process->T;
      MPI_Send(&process_mess, sizeOfMess(), MPI_INT, dest_rank, 1, MPI_COMM_WORLD);
  }

  returnedMess getMessage(general_process_struct* process){
    mess recv_mess;
    returnedMess messageToReturn;
    MPI_Status mpi_status;

    MPI_Recv(&recv_mess, sizeOfMess() , MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &mpi_status);

    messageToReturn.message = recv_mess;
    messageToReturn.message_status = mpi_status;
    return messageToReturn;
  }

  void exitCriticalSection(general_process_struct* process){
    sendRequestToAll(process, 3);
    channels[process->channel-1]++;
    // sendRequestToAll(process, 1);
    for(const auto& r: process->TO) {
      cout<<"[ TO ] ==> rank : "<<process->rank<<" send tag 1 to "<<r<<endl;
      sendConfirmationAsReponse(process, r);
    }
    process->TO.clear();
    cout<<red<<"RANK : "<<process->rank<<" GOING OUT FROM : "<<process->channel<<reset<<endl;
    sleep(1);
    process->status = !process->status;
    process->channel = 0;
    process->position = 'L';
    process->responseCounter = 0;

  }

  void communicationThread(general_process_struct* process){
    while(true){
      returnedMess recv_message = getMessage(process);
      if(process->position == 'L'){
        if(recv_message.message_status.MPI_TAG == 0){
          sendConfirmationAsReponse(process, recv_message.message.rank);
        }
      }
      if(process->position == 'W') {
        if(recv_message.message_status.MPI_TAG == 0){
          if(recv_message.message.position == 'W'){
            if(recv_message.message.channel == process->channel){
              if(recv_message.message.T < process->T){
                  // cout<<"rank : "<< process->rank<<" [ confirmation ] --> " <<recv_message.message.rank<<endl;
                  sendConfirmationAsReponse(process, recv_message.message.rank);
              }else if(recv_message.message.T == process->T && recv_message.message.rank < process->rank){
                  // cout<<"rank : "<< process->rank<<" [ confirmation ] --> " <<recv_message.message.rank<<endl;
                  sendConfirmationAsReponse(process, recv_message.message.rank);
              }else{
              
                process->TO.push_back(recv_message.message.rank);
              }
            }
            if(recv_message.message.channel != process->channel){
              // cout<<"rank : "<< process->rank<<" [ confirmation ] --> " <<recv_message.message.rank<<endl;
              sendConfirmationAsReponse(process, recv_message.message.rank);
            }
          }

        };
        if(recv_message.message_status.MPI_TAG == 1){
            incrementResponseCounter(process);
            sleep(1);
        };
        if(recv_message.message_status.MPI_TAG == 2){
            
            process->kryt_tab[recv_message.message.rank] = recv_message.message.channel;
            channels[recv_message.message.channel-1]--;
        };
        if(recv_message.message_status.MPI_TAG == 3){
            process->kryt_tab[recv_message.message.rank] = 0;
            channels[recv_message.message.channel-1]++;
        };
      }
      if(process->position == 'K') {
        if(recv_message.message.position == 'W') {
          if(recv_message.message.channel != process->channel) {
            // cout<<"rank : "<< process->rank<<" [ confirmation ] --> " <<recv_message.message.rank<<endl;

            sendConfirmationAsReponse(process, recv_message.message.rank);
          } 
          if(recv_message.message.channel == process->channel){
            if(recv_message.message.status == process->status){
              if(channels[process->channel-1] > 0){
                // cout<<"rank : "<< process->rank<<" [ confirmation ] --> " <<recv_message.message.rank<<endl;
                sendConfirmationAsReponse(process, recv_message.message.rank);
              }else{
                process->TO.push_back(recv_message.message.rank);
              }
            }else{
              process->TO.push_back(recv_message.message.rank);
            }
          }
        }
        if(recv_message.message.position == 'K') {
          if(recv_message.message_status.MPI_TAG == 3){
            if(recv_message.message.channel == process->channel) {
              // exitCriticalSection(process);
            } else {
              
              process->kryt_tab[recv_message.message.rank] = 0;
              channels[recv_message.message.channel-1]++;
            }
          }
          if(recv_message.message_status.MPI_TAG == 2){
            process->kryt_tab[recv_message.message.rank] = recv_message.message.channel;
            channels[recv_message.message.channel-1]--;
          };
        }
      }

    }

  

    
  };

  

  void mainThread(general_process_struct* process){
    char savedPosition = 'c';

    while(true){

      if(process->position == 'L'){
          if(process->position != savedPosition){
            savedPosition = process->position;
            cout<<"rank : "<<process->rank<<" is in position : "<<process->position<<" with status : " << process->status <<endl;
            sleep(rand()%4+2);
            process->position = 'W';
          }
          
      }
      if( process->position == 'W'){
        if(process->position != savedPosition){
            savedPosition = process->position;
            sleep(rand()%3+1);
            process->channel = (((rand()%2+1) + process->rank)%K)+1;
            cout<<"rank : "<<process->rank<<" is in position : "<<process->position <<" and chose channel " << process->channel<<endl;
            sleep(1);
            sendRequestToAll(process, 0);

          }

          if(process->responseCounter == PROCESS_COUNT-1 && channels[process->channel -1] > 0){
            cout<<"\033[3;43;30m"<<"RANK : "<<process->rank<< " IS GOING TO CRITICAL SECTION to CHANNEL "<<process->channel<<"\033[0m\t\t"<<endl;
            sleep(1);
            channels[process->channel-1]--;
            process->position = 'K';
          }
      }
      if( process->position == 'K'){
        if(process->position != savedPosition){
            savedPosition = process->position;
            process->kryt_tab[process->rank] = process->channel;
            sendRequestToAll(process, 2); 
            if(channels[process->channel-1] > 0) {
              for(const auto& r: process->TO) {
                // cout<<"[ TO ] ==> rank : "<<process->rank<<" send tag 1 to "<<r<<endl;
                sendConfirmationAsReponse(process, r);
              }
              process->TO.clear();
            } 
            clearResponseCounter(process);
            cout<<"rank : "<<process->rank<<" is in position : "<<process->position<<endl;   
            sleep(rand()%10 + 2);
            // sendRequestToAll(process, 3);
            if(process->position == 'K') {
              exitCriticalSection(process);
            }    
          }
      }
    }


  };




int main(int argc, char **argv) {
  
  int rank, size;
  char position = 'L';
  int channel = 0;
  int status = 0;
  int T = 0;
  int responseCounter = 0;
  vector<int> kryt_tab;
  vector<int> TO;
  vector<int> TZ;

  for(int i=0; i<PROCESS_COUNT; i++){
    kryt_tab.push_back(0);
  }
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int namelen;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(processor_name, &namelen);

  general_process_struct process_struct;
  process_struct.rank = rank;
  process_struct.position = position;
  process_struct.channel = channel;
  process_struct.status = status;
  process_struct.T = T;
  process_struct.responseCounter = responseCounter;
  process_struct.kryt_tab = kryt_tab;
  process_struct.TO = TO;
  process_struct.TZ = TZ;   

  thread ct(communicationThread, &process_struct);

  thread mt(mainThread, &process_struct);

  ct.join();
  mt.join();




  MPI_Finalize();
}
