#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <thread>
#define K 3
#define MSG_SIZE 1

// test push
using namespace std;

struct mess {
  char position = 'L';
  int channel = 0;
  int status = 0;
  int T = 0;
  int rank = -1;
};

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

// struct 
// void communication_thread(){

//       while(true){

//         if(position == "L"){

//             printf("p: %d , pos: %c", rank, position);

//         };

//         if(position == "W"){

//         };

//         if(position == "K"){
          
//         };


//       };

//   }

//   void critical_section_thread(){

//   };


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

  mess process_mess;
  process_mess.position = 'L';
  process_mess.channel = 0;
  process_mess.status = 0;
  process_mess.T = 0;
  process_mess.rank = rank;


  //losowanie kanału
  channel = rand()% K + 1;

while(true){

        if(position == 'L'){
            printf("p: %d , pos: %c", rank, position);
        };

        if(position == 'W'){
          for(int i=0; i <3; i++){
              if(rank != i){
                MPI_Send( process_mess, MSG_SIZE, MPI_ANY_SOURCE, i, 2, MPI_COMM_WORLD);
              }
            }

        };

        if(position == 'K'){
          
        };


      };

  




  


  std::cout << "Jestem " << rank << " z " << size << " na " << processor_name
            << std::endl;
  MPI_Finalize();
}
