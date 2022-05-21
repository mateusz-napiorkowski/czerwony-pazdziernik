#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include<unistd.h>
#define K 3
#define MSG_SIZE 1

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

    bool isPositionChanged = 0;

while(true){

        if(position == 'L'){         
            srand(time(0));
            sleep(rand() % 8 + 2);
            MPI_Irecv(&recv_mess, MSG_SIZE, message, MPI_ANY_SOURCE,
              2, MPI_COMM_WORLD, &request);
            if(recv_mess.rank != -1) {
              printf("recv -> rank : %d orzymal od %d w sekcji %c\n", rank, recv_mess.rank, process_mess.position);
              recv_mess.rank = -1;
            }
            position = 'W';
            isPositionChanged = true;
        };

        if(position == 'W'){

          if(isPositionChanged){
            channel = rand()% K + 1;

            process_mess.channel = channel;
            process_mess.position = position;

            for(int i=0; i<3; i++){
              if(rank != i){
                MPI_Send(&process_mess, MSG_SIZE, message, i, 2, MPI_COMM_WORLD);
              }
            }
            isPositionChanged = false;
          }

          MPI_Irecv(&recv_mess, MSG_SIZE, message, MPI_ANY_SOURCE,
              2, MPI_COMM_WORLD, &request);
              if(recv_mess.rank != -1) {
                responseCounter++;
                printf("recv -> rank : %d orzymal od %d w sekcji %c\n", rank, recv_mess.rank, process_mess.position);
                recv_mess.rank = -1;
              }

          
        };

        if(position == 'K'){
          
        };


      };

  




  


  std::cout << "Jestem " << rank << " z " << size << " na " << processor_name
            << std::endl;
  MPI_Finalize();
}
