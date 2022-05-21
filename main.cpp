#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <future>
#include<utility>
#include<unistd.h>
#define K 2
#define MSG_SIZE 1
#define changel_1 1
#define changel_2 2

using namespace std;

typedef struct mess {
  char position = 'L';
  int channel = 0;
  int status = 0;
  int T = 0;
  int rank = -1;
} mess;

struct mess_processTO {
  int rank;
  int T;
  int status;
};

struct mess_tz {
  int rank;
};

struct mess_channel_in {
  int rank;
};

struct mess_channel_out {
  int rank;
};

struct general_process_struct { 
  int rank;
  char position;
  int channel;
  int status;
  int T;
  int responseCounter;
  vector<mess_channel_in> kryt_tab;
  vector<mess_processTO> TO;
  vector<mess_tz> TZ;
};

void recv_thread(promise<general_process_struct> &obj, general_process_struct process){
  cout<< "inside thread with rank --> "<< process.rank << " position : " << process.position << endl;

    MPI_Request request;

    const int nitems=5;
    int          blocklengths[5] = {1,1,1,1,1};
    MPI_Datatype types[5] = {MPI_CHAR, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype message;
    MPI_Aint     offsets[5];

    offsets[0] = offsetof(mess, position);
    offsets[1] = offsetof(mess, channel);
    offsets[2] = offsetof(mess, status);
    offsets[3] = offsetof(mess, T);
    offsets[4] = offsetof(mess, rank);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &message);
    MPI_Type_commit(&message);

    

    mess recv_of_process;
    int rank_response;

    bool isCompleted = false;
  //   // MPI_Recv(msg, MSG_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    while(!isCompleted){
      MPI_Irecv(&recv_of_process, MSG_SIZE, message, MPI_ANY_SOURCE,
              1, MPI_COMM_WORLD, &request);
      MPI_Irecv(&recv_of_process, MSG_SIZE, message, MPI_ANY_SOURCE,
              2, MPI_COMM_WORLD, &request);
            // printf("recv -> rank : %d orzymal od %d ", rank, process_mess.rank);
      // if(recv_of_process.)
    }
  
  

  // obj.set_value(process);

}

int main(int argc, char **argv) {
  
  int rank, size;
  char position = 'L';
  int channel = 0;
  int status = 0;
  int T = 0;
  int responseCounter = 0;
  vector<mess_channel_in> kryt_tab;
  vector<mess_processTO> TO;
  vector<mess_tz> TZ;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int namelen;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(processor_name, &namelen);

  MPI_Request request;

  const int nitems=5;
    int          blocklengths[5] = {1,1,1,1,1};
    MPI_Datatype types[5] = {MPI_CHAR, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype message;
    MPI_Aint     offsets[5];

    offsets[0] = offsetof(mess, position);
    offsets[1] = offsetof(mess, channel);
    offsets[2] = offsetof(mess, status);
    offsets[3] = offsetof(mess, T);
    offsets[4] = offsetof(mess, rank);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &message);
    MPI_Type_commit(&message);

    mess process_mess;
    process_mess.position = 'L';
    process_mess.channel = 0;
    process_mess.status = 0;
    process_mess.T = 0;
    process_mess.rank = rank;

    mess recv_mess;

    bool isPositionChanged = 1;


    general_process_struct process_struct;
    process_struct.rank = rank;
    process_struct.position = 'L';
    process_struct.channel = 0;
    process_struct.status = 0;
    process_struct.T = 0;
    process_struct.responseCounter = 0;
    

    promise<general_process_struct> obj;
    future<general_process_struct> futureObj = obj.get_future();

    // 1: mess
    // 2: zgoda
    // 3: mess chanel in
    // 4: mess channel out
    

while(true){


        if(position == 'L'){

          MPI_Irecv(&process_mess, MSG_SIZE, message, MPI_ANY_SOURCE,
              1, MPI_COMM_WORLD, &request);
            // printf("recv -> rank : %d orzymal od %d ", rank, process_mess.rank);
          if(process_mess.position == 'W'){
              cout<< "otrzymalem wiadomosc od : " << process_mess.rank << " position -->  W"<<endl; 
          }

          if(isPositionChanged){
            // recv_response.join();
            // general_process_struct res;
            // res = futureObj.get();
            cout << "rank : " << rank << " position : " << position << endl;

            srand(time(0));
            sleep(rand() % 6 + 2);
            position = 'W';
          } 
        };

        if(position == 'W'){


          if(isPositionChanged){
             cout << "rank" << rank << " position : " << position << endl;
            channel = rand()% K + 1;
            cout<<"rank : "<<rank<<" got a channel number : " << channel <<endl;

            process_mess.channel = channel;
            process_mess.position = position;

            for(int i=0; i<3; i++){
              if(rank != i){
                MPI_Send(&process_mess, MSG_SIZE, message, i, 1, MPI_COMM_WORLD);
              }
              
            }
            thread recv_response(recv_thread, ref(obj), process_struct);
              recv_response.join();


            isPositionChanged = false;
          }

          // MPI_Irecv(&recv_mess, MSG_SIZE, message, MPI_ANY_SOURCE,
          //     2, MPI_COMM_WORLD, &request);
        };

        if(position == 'K'){
          
        };


      };

  




  


  // std::cout << "Jestem " << rank << " z " << size << " na " << processor_name
  //           << std::endl;
  MPI_Finalize();
}
