#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include<chrono>
#include <future>
#include<utility>
#include<unistd.h>
#define K 2
#define PROCESS_COUNT 3
#define MSG_SIZE 1

int array[K] = {1,2};

using namespace std;

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
  
  

  void sendRequestToAll(general_process_struct* process, int status){
    mess process_mess;
    process_mess.channel = process->channel;
    process_mess.position = process->position;
    process_mess.rank = process->rank;
    process_mess.status = process->status;
    process_mess.T = process->T;
    for(int i=0; i<PROCESS_COUNT; i++){
      if(process->rank != i){
        cout<<"rank : "<<process->rank<<" is sending request to "<< i << endl;
        MPI_Send(&process_mess, sizeOfMess(), MPI_INT, i, status, MPI_COMM_WORLD);
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

    cout<<"rank : "<<process->rank<<" got message recv from : "<<recv_mess.rank <<" with tag " << mpi_status.MPI_TAG<<endl;
    messageToReturn.message = recv_mess;
    messageToReturn.message_status = mpi_status;
    return messageToReturn;
  }



  void communicationThread(general_process_struct* process){
    while(true){
      returnedMess recv_message = getMessage(process);
      if(recv_message.message_status.MPI_TAG == 0){
        if(recv_message.message.position == 'W'){
          if(recv_message.message.channel == process->channel){
            if(recv_message.message.T < process->T){
                cout<<"rank : "<< process->rank<<" [ confirmation ] --> " <<recv_message.message.rank<<endl;
                sendConfirmationAsReponse(process, recv_message.message.rank);
            }else if(recv_message.message.T == process->T && recv_message.message.rank < process->rank){
                cout<<"rank : "<< process->rank<<" [ confirmation ] --> " <<recv_message.message.rank<<endl;
                sendConfirmationAsReponse(process, recv_message.message.rank);
            }else{
              cout<<"rank : "<< process->rank<<" [ push to TO array ] --> " <<recv_message.message.rank<<endl;
              process->TO.push_back(recv_message.message.rank);
            }
          }
          if(recv_message.message.channel != process->channel){
            cout<<"rank : "<< process->rank<<" [ confirmation ] --> " <<recv_message.message.rank<<endl;
            sendConfirmationAsReponse(process, recv_message.message.rank);
          }
        }

      };
      if(recv_message.message_status.MPI_TAG == 1){
          cout<<"rank : "<<process->rank<<"increment responseCounter"<<endl;
          process->responseCounter++;
      };
      if(recv_message.message_status.MPI_TAG == 2){
          cout<<"rank : "<<process->rank<<" add process "<< recv_message.message.rank<<" to kryt_tab"<<endl;
          process->kryt_tab[recv_message.message.rank] = recv_message.message.channel;
      };
      if(recv_message.message_status.MPI_TAG == 3){
          cout<<"rank : "<<process->rank<<" add process "<< recv_message.message.rank<<" to kryt_tab"<<endl;
          process->kryt_tab[recv_message.message.rank] = 0;
      };

    }


    
  };

  void mainThread(general_process_struct* process){
    char savedPosition = 'c';

    while(true){

      if(process->position == 'L'){
          if(process->position != savedPosition){
            savedPosition = process->position;
            cout<<"rank : "<<process->rank<<" is in position : "<<process->position<<endl;
            sleep(5);
            process->position = 'W';
          }
          
      }
      if( process->position == 'W'){
        if(process->position != savedPosition){
            savedPosition = process->position;
            process->channel = rand()%2 + 1;
            cout<<"rank : "<<process->rank<<" is in position : "<<process->position<<endl;
            cout<<"rank : "<<process->rank<<"choseed chaneel" << process->channel<<endl;
            sleep(5);
            sendRequestToAll(process, 0);


            // cout<<"rank : "<<process->rank<<" is going out from "<<process->position<<endl;
          }

          if(process->responseCounter == K-1){
            cout<<"==================================="<<endl;
            cout<<"RANK : "<<process->rank<< " CAN GO TO CRITICAL SECTION"<<endl;
            cout<<"==================================="<<endl;
            process->position = 'K';
          }
      }
      if( process->position == 'K'){
        if(process->position != savedPosition){
            cout<<"rank : "<<process->rank<<" is in position : "<<process->position<<endl;
            sleep(5);
            sendRequestToAll(process, 2);

            // cout<<"rank : "<<process->rank<<" is going out from "<<process->position<<endl;
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
