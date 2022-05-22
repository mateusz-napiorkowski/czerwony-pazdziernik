#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include<unistd.h>
#define K 4
#define MSG_SIZE 1

using namespace std;

typedef struct mess {
  int messType = 1;
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
  MPI_Status s;
  int messType = 1;
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


  const int nitems=6;
    int          blocklengths[6] = {1,1,1,1,1,1};
    MPI_Datatype types[6] = {MPI_INT, MPI_CHAR, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype message;
    MPI_Aint     offsets[6];

    offsets[0] = offsetof(mess, messType);
    offsets[1] = offsetof(mess, position);
    offsets[2] = offsetof(mess, channel);
    offsets[3] = offsetof(mess, status);
    offsets[4] = offsetof(mess, T);
    offsets[5] = offsetof(mess, rank);
    

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &message);
    MPI_Type_commit(&message);

    mess process_mess;
    process_mess.messType = 1;
    process_mess.position = 'L';
    process_mess.channel = 0;
    process_mess.status = 0;
    process_mess.T = 0;
    process_mess.rank = rank;

    mess recv_mess;

    bool isPositionChanged = 0;
    
    // int zgoda = 0;
    // int process_to_add = -1;
    // int process_to_remove = -1;
    // int ready;
//tagi
//1: mess
//2: zgoda
//3: mess_kanal_in
//4: mess_kanal_out


  while(true){
        if(position == 'L'){         
            srand(time(0));
            sleep(rand() % 8 + 2);
            // MPI_Irecv(&recv_mess, MSG_SIZE, message, MPI_ANY_SOURCE,
            //   1, MPI_COMM_WORLD, &request1);
            // MPI_Test(&request1, &ready, MPI_STATUS_IGNORE);
            // printf("%d %d\n", rank, ready);
            // if(ready) {
            //   if(recv_mess.rank != -1 && recv_mess.position == 'W') {
            //     printf("recv -> rank : %d orzymal od %d w sekcji %c\n", rank, recv_mess.rank, process_mess.position);
            //     zgoda = 1;
            //     MPI_Send(&zgoda, 1, MPI_INT, recv_mess.rank, 2, MPI_COMM_WORLD );
            //     // zgoda = 0;
            //     recv_mess.rank = -1;
            //   }
            // }
            position = 'W';
            isPositionChanged = true;
        };

        if(position == 'W'){

          if(isPositionChanged){
            channel = rand()% K + 1;

            process_mess.messType = 1;
            process_mess.channel = channel;
            process_mess.position = position;
            printf("GOT HERE\n");
            for(int i=0; i<=3; i++){
              if(rank != i){
                MPI_Send(&process_mess, MSG_SIZE, message, i, 1, MPI_COMM_WORLD);
              }
            }
            isPositionChanged = false;
          }

          while(responseCounter != K-1) { 
            MPI_Recv(&recv_mess, MSG_SIZE, message, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &s);
            //TODO synchronizuj czas
            if(recv_mess.messType == 1) {
              printf("%d got mess type %d from %d\n", rank, recv_mess.messType, recv_mess.rank);
              // if(recv_mess.position == 'W') {
                if(recv_mess.channel == channel) {
                  if(recv_mess.T < T || (recv_mess.T == T && recv_mess.rank < rank)) {
                    process_mess.messType = 2;
                    MPI_Send(&process_mess, MSG_SIZE, message, recv_mess.rank, 1, MPI_COMM_WORLD);
                  } else {
                    //dodaj proces do TO
                  }
                } else {
                  process_mess.messType = 2;
                  MPI_Send(&process_mess, MSG_SIZE, message, recv_mess.rank, 1, MPI_COMM_WORLD);
                }
              // }
            } else if(recv_mess.messType == 2) {
              responseCounter++;
              printf("Rank: %d, responses: %d\n", rank, responseCounter);
            } else if(recv_mess.messType == 3) {
              // dodaj proces do tablicy procesów sekcji krytycznej
            } else if(recv_mess.messType == 4) {
              // usun proces z tablicy procesów sekcji krytycznej
            }
          }
//           while(responseCounter != K-1) {
//             printf("%d IN A LOOP\n", rank);
//             // //TODO zsynchronizuj czas

//             // //czekanie na odpowiedzi/zgody od procesów
//             // if(!pierwsza_zgoda) {
//             //   MPI_Irecv(&zgoda, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &request2);
              
//             // }
//             // MPI_Test(&request2, &ready, MPI_STATUS_IGNORE);
//             // if(ready) {
//             //     pierwsza_zgoda = 1;
//             //     ready = 0;
//             //     if(zgoda == 1) {
//             //       cout << "EEE" << endl;
//             //       responseCounter++;
//             //       cout << responseCounter << endl;
//             //       //printf("recv -> rank : %d orzymal od %d w sekcji %c\n", rank, recv_mess.rank, process_mess.position);
//             //       zgoda = 0;
//             //     }
//             //     if(responseCounter != K-1) {
//             //       MPI_Irecv(&zgoda, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &request);
//             }
//             // else
//             //     printf("message not received yet.\n");
            
//             //czekanie na pytania od procesów
//             //TODO zsynchronizuj czas
//             // MPI_Irecv(&recv_mess, MSG_SIZE, message, MPI_ANY_SOURCE,
//             //   1, MPI_COMM_WORLD, &request);
//             // if(recv_mess.rank != -1) {
//             //   if(recv_mess.position == 'W') {
//             //     if(recv_mess.channel == channel) {
//             //       if(recv_mess.T < T || (recv_mess.T == T && recv_mess.rank < rank)) {
//             //         zgoda = 1;
//             //         MPI_Send(&zgoda, 1, MPI_INT, recv_mess.rank, 2, MPI_COMM_WORLD );
//             //         zgoda = 0;
//             //       } else {
//             //         //TODO dodaj proces do TO
//             //       }  
//             //     } else {
//             //       zgoda = 1;
//             //       MPI_Send(&zgoda, 1, MPI_INT, recv_mess.rank, 2, MPI_COMM_WORLD );
//             //       zgoda = 0;
//             //     }
//             //   }
//             //   recv_mess.rank = -1;
//             // }
//             // MPI_Irecv(&process_to_add, 1, MPI_INT, MPI_ANY_SOURCE,
//             //     3, MPI_COMM_WORLD, &request);
//             // if(process_to_add != -1) {
//             //   //TODO dodaj proces do tablicy procesow sekcji krytycznej
//             //   process_to_add = -1;
//             // }
//             // MPI_Irecv(&process_to_remove, 1, MPI_INT, MPI_ANY_SOURCE,
//             //     4, MPI_COMM_WORLD, &request);
//             // if(process_to_remove != -1) {
//             //   //TODO usun proces z tablicy procesow sekcji krytycznej
//             //   process_to_remove = -1;
//             // }

//           }
//           position = K;
//         };

//         if(position == 'K'){
          
//         };

        }
  };

  




  


  std::cout << "Jestem " << rank << " z " << size << " na " << processor_name
            << std::endl;
  MPI_Finalize();
}
