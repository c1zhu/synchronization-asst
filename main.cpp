
//
// Example from: http://www.amparo.net/ce155/sem-ex.c
// 
// Adapted using some code from Downey's book on semaphores
//
// Compilation:
//
//       g++ main.cpp -lpthread -o main -lm
// or 
//      make
//
// This code does not work on Mac OS X. It should work fine on any Linux system. 
//
// 

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include <iostream>
using namespace std;

/*
 This wrapper class for semaphore.h functions is from:
 http://stackoverflow.com/questions/2899604/using-sem-t-in-a-qt-project
 */
class Semaphore {
public:
    // Constructor
    Semaphore(int initialValue)
    {
        sem_init(&mSemaphore, 0, initialValue);
    }
    // Destructor
    ~Semaphore()
    {
        sem_destroy(&mSemaphore); /* destroy semaphore */
    }
    
    // wait
    void wait()
    {
        sem_wait(&mSemaphore);
    }
    // signal
    void signal()
    {
        sem_post(&mSemaphore);
    }
    
    
private:
    sem_t mSemaphore;
};




/* global vars */
int customers = 0;
int n = 4;
int writers;

/* semaphores are declared global so they can be accessed
 in main() and in thread routine. */
Semaphore Mutex(1);
Semaphore Customer(1);
Semaphore Barber(0);
Semaphore customerDone(0);
Semaphore barberDone(0);



/*
    Producer function 
*/
void *customer ( void *threadID )
{
    // Thread number 
    int x = (long)threadID;

    while( 1 )
    {
        sleep(3); // Slow the thread down a bit so we can see what is going on
        Mutex.wait();
            if (customers == n){
                Mutex.signal();
                printf("The barber shop is full. The customer has to leave. \n");
            }
            customers += 1;
        Mutex.signal();

        Customer.signal();
        Barber.wait();
        //getHaircut
            printf("Customer %d is getting haircut \n", x);
                fflush(stdout);

        customerDone.signal();
        barberDone.wait();

        Mutex.wait();
            customers -= 1;
        Mutex.signal();
    }

}

/*
    Consumer function 
*/
void *barber ( void *threadID )
{
    // Thread number 
    //int x = (long)threadID;
    
    while( 1 )
    {
        Customer.wait();
        Barber.signal();

            printf("Barber is giving haircut \n");
                fflush(stdout);

        customerDone.wait();
        barberDone.signal();

        sleep(5);   // Slow the thread down a bit so we can see what is going on
    }

}


int main(int argc, char **argv )
{
    pthread_t customerThread[ 4 ];
    pthread_t barberThread[ 3 ];

    // Create the customers 
    for( long c = 0; c < 4; c++ )
    {
        int customerthread = pthread_create ( &customerThread[ c ], NULL, 
                                  customer, (void *) (c+1) );
        if (customerthread) {
            printf("ERROR creating producer thread # %d; \
                    return code from pthread_create() is %d\n", c, customerthread);
            exit(-1);
        }
    }

    // Create the barber 
    int barberthread = pthread_create ( &barberThread[ 3 ], NULL, 
                                  barber, (void *) (3) );
        
	for( long b = 0; b < 3; b++ )
    {
        int customerthread = pthread_create ( &customerThread[ b ], NULL, 
                                  customer, (void *) (b+1) );
        if (barberthread) {
            printf("ERROR creating consumer thread # %d; \
                    return code from pthread_create() is %d\n", b, barberthread);
            exit(-1);

        }
    }
      
    printf("Main: program completed. Exiting.\n");


    // To allow other threads to continue execution, the main thread 
    // should terminate by calling pthread_exit() rather than exit(3). 
    pthread_exit(NULL); 
}


 /* main() */


