#include <iostream>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>

using namespace std;

#define NUM_ROW     9
#define NUM_COL     4

void *setLuggage(void *seat_num){

    sleep(rand()%120+1);            // wait 0 to 2 minutes

    // 20% of the passengers carry a luggage (marking the sheet)
    *(char *)seat_num = (rand()%101 <= 20)?'Y':'N';

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){

    pthread_t passenger[NUM_ROW][NUM_COL];

    int rc, i, j;
    int passenger_on_coach = 0;
    int num_of_passenger = atoi(argv[1]);
    //cout << num_of_passenger << endl;
    char luggage[NUM_ROW][NUM_COL];     // luggage record sheet

    srand(time(NULL));                  // seed for a new psedorandom integer

    for(i=0; i<NUM_ROW; i++){
        for (j=0; j<NUM_COL; j++) {
            //cout << "Number of passengers: " << num_of_passenger << endl;
            //cout << "Passenger on coach: " << passenger_on_coach << endl;
            if(num_of_passenger <= passenger_on_coach){
                luggage[i][j] = 'E';
            } else{
                //cout << "Passenger [" << i << "] [" << j << "]\n" ;
                rc = pthread_create(&passenger[i][j], NULL, setLuggage, (void *)&luggage[i][j]);

                if(rc){
                    cout << "Error: unable to create thread\n";
                    exit(-1);
                }
            }
            passenger_on_coach++;

        }
    }

    // master thread waiting for each worker-thread
    // i.e. driver waiting each passenger to submit his/her luggage record sheet
    for(i=0; i<NUM_ROW; i++){
        for (j=0; j<NUM_COL; j++) {
            if(num_of_passenger){
                rc = pthread_join(passenger[i][j], NULL);

                if(rc){
                    cout << "Error: unable to join, " << rc << endl;
                    exit(-1);
                }

                num_of_passenger--;
            }
        }
    }

    int num_of_luggage = 0;

    // display the luggage record sheet
    for(i=0; i<NUM_ROW; i++){
        for(j=0; j<NUM_COL; j++){
            cout << luggage[i][j];
            // count how many passengers carry lugguage
            if (luggage[i][j] == 'Y'){
                num_of_luggage++;
            }
        }
        cout << endl;
    }

    cout << num_of_luggage << " passenger";
    // singular or plural
    if(num_of_luggage > 1){
        cout << "s";
    }
    cout << " carry luggage.\n";

    // cout << "main(): program exiting" << endl;
    pthread_exit(NULL);
}
