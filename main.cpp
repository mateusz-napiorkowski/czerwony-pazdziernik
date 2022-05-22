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
#define MSG_SIZE 1
#define changel_1 1
#define changel_2 2

using namespace std;

typedef struct mess {
  int position = 'L';
  int channel = 0;
  int status = 0;
  int T = 0;
  int rank = -1;
} mess;

int sizeOfMess(){
  return sizeof(mess)/sizeof(int);
}

struct mess_processTO {
  int rank;
  int T;
  int status;
};

// struct mess_tz {
//   int rank;
// };

// struct mess_channel_in {
//   int rank;
// };

// struct mess_channel_out {
//   int rank;
// };

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


    
  //   // MPI_Recv(msg, MSG_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // while(!isCompleted){
    //   MPI_Irecv(&recv_of_process, MSG_SIZE, message, MPI_ANY_SOURCE,
    //           1, MPI_COMM_WORLD, &request);
    //   MPI_Irecv(&recv_of_process, MSG_SIZE, message, MPI_ANY_SOURCE,
    //           2, MPI_COMM_WORLD, &request);
            // printf("recv -> rank : %d orzymal od %d ", rank, process_mess.rank);
      // if(recv_of_process.)
    
  
  

  // obj.set_value(process);

  // TAGS: =================
  // request : 0
  // response yes : 1;
  
  

  void sendRequestToAll(general_process_struct* process){
    mess process_mess;
    process_mess.channel = process->channel;
    process_mess.position = process->position;
    process_mess.rank = process->rank;
    process_mess.status = process->status;
    process_mess.T = process->T;
    for(int i=0; i<2; i++){
      if(process->rank != i){
        cout<<"rank : "<<process->rank<<" is sending request to "<< i << endl;
        MPI_Send(&process_mess, sizeOfMess(), MPI_INT, i, 0, MPI_COMM_WORLD);
      }
      
    }

  }

  returnedMess getMessage(){
    mess recv_mess;
    returnedMess messageToReturn;
    MPI_Status mpi_status;

    MPI_Recv(&recv_mess, sizeOfMess() , MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &mpi_status);

    cout<<"message recv from : "<<recv_mess.rank << endl;
    messageToReturn.message = recv_mess;
    messageToReturn.message_status = mpi_status;
    cout<<"mpi tag : "<<mpi_status.MPI_TAG <<endl;
    return messageToReturn;
  }



  void communicationThread(general_process_struct* process){
    cout<<"rank : "<<process->rank << " in communication thread"<<endl;
    while(true){
      returnedMess recv_message = getMessage();
      if(recv_message.message_status.MPI_TAG == 0){

      }
      if(recv_message.message_status.MPI_TAG == 1){
        
      }

    }


    
  };

  void mainThread(general_process_struct* process){
    // cout<<"In main thread"<<endl;

    char savedPosition = 'c';

    while(true){

      if(process->position == 'L'){
          if(process->position != savedPosition){
            savedPosition = process->position;
            cout<<"rank : "<<process->rank<<" is in position : "<<process->position<<endl;
            sleep(5);
            cout<<"rank : "<<process->rank<<" is going out from "<<process->position<<endl;
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
            sendRequestToAll(process);

          
            // cout<<"rank : "<<process->rank<<" is going out from "<<process->position<<endl;
          }
      }
      if( process->position == 'K'){
        if(process->position != savedPosition){
            cout<<"rank : "<<process->rank<<" is in position : "<<process->position<<endl;
            sleep(5);

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
