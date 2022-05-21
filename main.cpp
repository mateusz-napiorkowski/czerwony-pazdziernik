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
    
    int zgoda = 0;
    int process_to_add = -1;
    int process_to_remove = -1;
//tagi
//1: mess
//2: zgoda
//3: mess_kanal_in
//4: mess_kanal_out

while(true){

        if(position == 'L'){         
            srand(time(0));
            sleep(rand() % 3 + 1);
            cout<<"process rank : "<< rank << " in position " << position << endl;
            MPI_Irecv(&recv_mess, MSG_SIZE, message, MPI_ANY_SOURCE,
              1, MPI_COMM_WORLD, &request);
            if(recv_mess.rank != -1 && recv_mess.position == 'W') {
              printf("recv -> rank : %d orzymal od %d w sekcji %c\n", rank, recv_mess.rank, process_mess.position);
              zgoda = 1;
              MPI_Send(&zgoda, 1, MPI_INT, recv_mess.rank, 2, MPI_COMM_WORLD );
              zgoda = 0;
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
            cout<<"process rank : "<< rank << " in position " << position << endl;

            for(int i=0; i<3; i++){
              if(rank != i){
                MPI_Send(&process_mess, MSG_SIZE, message, i, 1, MPI_COMM_WORLD);
                cout<<"[ "<< rank << " ] send to [ " << i << " ]"<< endl;
              }
            }
            isPositionChanged = false;
          }



          while(responseCounter != K-1) {
            //TODO zsynchronizuj czas

            //czekanie na odpowiedzi/zgody od procesów
            // MPI_Irecv(&zgoda, 1, MPI_INT, MPI_ANY_SOURCE,
            //     2, MPI_COMM_WORLD, &request);
            // if(zgoda == 1) {
            //   responseCounter++;
            //   cout<< "proces rank : "<<rank<<" got agreement from process "<< endl;
            //   cout<< "counter " << responseCounter<<endl;
            //   zgoda = 0;
            // }
            //czekanie na pytania od procesów
            //TODO zsynchronizuj czas
            MPI_Irecv(&recv_mess, MSG_SIZE, message, MPI_ANY_SOURCE,
              1, MPI_COMM_WORLD, &request);
            if(recv_mess.rank != -1) {
              if(recv_mess.position == 'W') {
                if(recv_mess.channel == channel) {
                  if(recv_mess.T < T || (recv_mess.T == T && recv_mess.rank < rank)) {
                    zgoda = 1;
                    MPI_Send(&zgoda, 1, MPI_INT, recv_mess.rank, 2, MPI_COMM_WORLD );
                    zgoda = 0;
                  } else {
                    //TODO dodaj proces do TO
                  }  
                } else {
                  zgoda = 1;
                  MPI_Send(&zgoda, 1, MPI_INT, recv_mess.rank, 2, MPI_COMM_WORLD );
                  zgoda = 0;
                }
              }
              recv_mess.rank = -1;
            }
            // MPI_Irecv(&process_to_add, 1, MPI_INT, MPI_ANY_SOURCE,
            //     3, MPI_COMM_WORLD, &request);
            // if(process_to_add != -1) {
            //   //TODO dodaj proces do tablicy procesow sekcji krytycznej
            //   process_to_add = -1;
            // }
            // MPI_Irecv(&process_to_remove, 1, MPI_INT, MPI_ANY_SOURCE,
            //     4, MPI_COMM_WORLD, &request);
            // if(process_to_remove != -1) {
            //   //TODO usun proces z tablicy procesow sekcji krytycznej
            //   process_to_remove = -1;
            // }

          }
          // position = K;



        };

        if(position == 'K'){
          
        };


      };

  




  


  std::cout << "Jestem " << rank << " z " << size << " na " << processor_name
            << std::endl;
  MPI_Finalize();
}
