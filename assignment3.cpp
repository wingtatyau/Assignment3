#include <iostream>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <queue>

using namespace std;

#define NUM_ROW     9
#define NUM_COL     4

// Data types
struct seat{
    int row;
    int col;
};

struct queueItem{
    int row;
    int col;
    char luggage;
};

// Global variables
seat seatAvailable;
queueItem tag;
queue<queueItem> boxQueue;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
void *passengers();
void automatic_ticketing_machine(int[]);
void enqueue(queueItem);
queueItem dequeue();

void *passengers(void *passengerId){

    cout << "Passenger " << *(int *)passengerId << " created\n";
    sleep(rand() % 120 + 1);        // wait 0 to 2 minutes
    cout << "Passenger " << *(int *)passengerId << " arrived\n";

    int position[2] = {0};
    automatic_ticketing_machine(position);

    cout << "Passenger " <<*(int *)passengerId  << " Seat No.:(" << position[0] << ", " << position[1] << ")\n";

    sleep(rand() % 60 + 1);         // wait 0 to 1 minute

    cout << "Passenger " <<*(int *)passengerId  << " got on the coach\n";

    pthread_exit(NULL);
}

void automatic_ticketing_machine(int position[2]){

    pthread_mutex_lock(&mutex);
    cout << "locked\n";
    sleep(1);

    if(seatAvailable.col > 3){

        position[0] = ++seatAvailable.row;

        sleep(1);

        seatAvailable.col = 1;

        position[1] = seatAvailable.col;

    } else {

        position[0] = seatAvailable.row;

        sleep(1);

        position[1] = seatAvailable.col++;

    }

    cout << "unlock\n";
    pthread_mutex_unlock(&mutex);

}

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
    int passengerThreadId[num_of_passenger];

    seatAvailable.row = 1;
    seatAvailable.col = 1;

    srand(time(NULL));                  // seed for a new psedorandom integer

    for(i=0; i<NUM_ROW; i++){
        for (j=0; j<NUM_COL; j++) {
            //cout << "Number of passengers: " << num_of_passenger << endl;
            //cout << "Passenger on coach: " << passenger_on_coach << endl;
            if(num_of_passenger <= passenger_on_coach){
                luggage[i][j] = 'E';
            } else{
                //cout << "Passenger [" << i << "] [" << j << "]\n" ;
		passengerThreadId[4*i + j] = 4*i + j;
		cout << "Passenger thread ID: " << passengerThreadId[4*i + j] << endl;
                rc = pthread_create(&passenger[i][j], NULL, passengers, (void *)&passengerThreadId[4*i+j]);

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
