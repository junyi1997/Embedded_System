#include "system.h"

pthread_mutex_t System::ioMutex;
/*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
pthread_barrier_t System::barr;
/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
pthread_spinlock_t System::lock;
/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

/**
 * Set up the threadSet dependent on the inputfile.
 *
 */
System::System ()
{
    init ();
    setUpMatrix (); // Inital singleResult, multiResult, matrix, and inpoutMatrix

    threadSet = new Thread* [PROGRAM_NUM];
    check = new Check [PROGRAM_NUM];

    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++) {

	    threadSet [prog_index] = new Thread [THREAD_NUM];
        check [prog_index].initialCheck (prog_index,
                                         singleResult [prog_index], 
                                         multiResult [prog_index],
                                         MATRIX_SIZE);

    }

    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++) {

        for (int thread_index = 0; thread_index < THREAD_NUM; thread_index++) {

	    	threadSet [prog_index][thread_index].initialThread (prog_index,
                                                                thread_index,
                                                                MATRIX_SIZE,
                                                                singleResult [prog_index],
                                                                multiResult [prog_index],
                                                                matrix [prog_index], 
                                                                inputMatrix [prog_index],
                                                                &sharedSum [prog_index]);

            threadSet [prog_index][thread_index].setThreadCore (thread_index);

            threadSet [prog_index][thread_index].setStartCalculatePoint (thread_index * MATRIX_SIZE / THREAD_NUM);
            threadSet [prog_index][thread_index].setEndCalculatePoint ((thread_index + 1) * MATRIX_SIZE / THREAD_NUM);

            threadSet [prog_index][thread_index].setUpIOMutex (&System::ioMutex);
            /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
            threadSet[prog_index][thread_index].setUpBarr (&System::barr);
            /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
            /*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
            threadSet[prog_index][thread_index].setUpSpinlock(&System::lock);
            /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
            
        }

    }

}


/**
 * Set up 2-D matrix for singleResult, multiResult,
 * matrix, and inputMatrix as the size of 
 * matrix_size * matrix_size. Also inclue a initial
 * value of inputMatrix.
 *
 */
void
System::setUpMatrix ()
{
	singleResult  = new int** [PROGRAM_NUM];
	multiResult   = new int** [PROGRAM_NUM];
	matrix        = new int** [PROGRAM_NUM];
	inputMatrix   = new int** [PROGRAM_NUM];

    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++) {

	    singleResult [prog_index]  = new int* [MATRIX_SIZE];
	    multiResult [prog_index]   = new int* [MATRIX_SIZE];
	    matrix [prog_index]        = new int* [MATRIX_SIZE];
	    inputMatrix [prog_index]   = new int* [MATRIX_SIZE];

        for (int i = 0; i < MATRIX_SIZE; i++) {
        	singleResult [prog_index][i] = new int [MATRIX_SIZE];
        	multiResult [prog_index][i]  = new int [MATRIX_SIZE];
        	matrix [prog_index][i]       = new int [MATRIX_SIZE];
        	inputMatrix [prog_index][i]  = new int [MATRIX_SIZE];
        
        	for (int j = 0; j < MATRIX_SIZE; j++)
        		inputMatrix [prog_index][i][j] = rand () % 2;
        }

    }
    

}


void
System::init ()
{

    std::cout << "\n========================System Info===========================" << std::endl; 
    std::cout << "Protect Shared Resource: ";
#if PROTECT_SHARED_RESOURCE == MUTEX
    std::cout << "Mutex" << std::endl;
#else
    std::cout << "Spinlock" << std::endl;
#endif

    std::cout << "Synchronize: ";
#if SYNCHRONIZE == BARRIER
    std::cout << "Barrier" << std::endl;
#else
    std::cout << "Semaphore" << std::endl;
#endif

    sharedSum = new int [PROGRAM_NUM];

	/*~~~~~~~~~Your code(PART1&PART3)~~~~~~~~*/
    pthread_barrier_init(&System::barr, NULL, 4);
#if (PART == 3) //part3 code
    pthread_spin_init(&System::lock, PTHREAD_PROCESS_PRIVATE);
#endif
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
}


/**
 * Execute the single thread matrix multiplication and 
 * store the result at singleResult.
 *
 */
void
System::singleCoreMatrixMulti ()
{
    std::cout << "\n===========Start Single Thread Matrix Multiplication==========" << std::endl; 

    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++)
        threadSet [prog_index][0].resetMatrix (); // copy the data from inputMatrix to matrix

    setStartTime ();
    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++)
	    threadSet [prog_index][0].singleMatrixMultiplication ();
    setEndTime ();

    std::cout << "Single-thread spend time : " << timeUse << std::endl;
}


/**
 * Creating multi-thread for matrix multiplication.
 *
 */
void
System::multiCoreMatrixMulti ()
{
    std::cout << "\n===========Start Multi-Thread Matrix Multiplication===========" << std::endl; 
    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++)
        threadSet [prog_index][0].resetMatrix (); // copy the data from inputMatrix to matrix

    setStartTime ();

    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++) {

	    for (int thread_index = 0; thread_index < THREAD_NUM; thread_index++) {

	    	pthread_create (&(threadSet [prog_index][thread_index].pthreadThread),
                            NULL,
                            (THREADFUNCPTR)&Thread::matrixMultiplication,
                            &threadSet [prog_index][thread_index]);
        }

    }

    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++) {

	    for (int thread_index = 0; thread_index < THREAD_NUM; thread_index++) 
	    	pthread_join (threadSet [prog_index][thread_index].pthreadThread, NULL);

    }

    setEndTime ();

    std::cout << "Multi-thread spend time : " << timeUse << std::endl;

    std::cout << "\n============================Result============================" << std::endl; 
    for (int prog_index = 0; prog_index < PROGRAM_NUM; prog_index++) {
        check [prog_index].compareResult ();
    }

}


/**
 * Set the current time as start time.
 *
 */
void
System::setStartTime ()
{
	gettimeofday (&start, NULL);
}


/**
 * Set the current time as end time. Calculate the time 
 * interval between start time and end time.
 *
 */
void
System::setEndTime ()
{
	gettimeofday (&end, NULL);
	timeUse = (end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec)/1000000.0;
}
