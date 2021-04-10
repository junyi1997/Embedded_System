#include "thread.h"

void
Thread::setThreadID(int ID)
{
	_ID = ID;
}


void
Thread::setThreadCore(int core)
{ 
	setCore = core;
}


void
Thread::setThreadMatrixSize(int matrix_size)
{
	_matrixSize = matrix_size;
}	


/*
 * Set up the affinity mask for the thread.
 *
 * @ Part1 Recommendation:
 * Implement the function to pinned current thread
 * to core with index CPU_NUM.
 *
 */
void
Thread::setUpCPUAffinityMask(int cpu_num)
{
	/*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
    // Pined the thread to core.將線程固定到核心。
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
}


void
Thread::setStartCalculatePoint(int start_calculate_point)
{
    startCalculatePoint = start_calculate_point;
}


void
Thread::setEndCalculatePoint(int end_calculate_point)
{
    endCalculatePoint = end_calculate_point;
}


void
Thread::setSchedulingPolicy(int policy)
{
    _schedulingPolicy = policy;
}


void 
Thread::setCheck (Check* tmp_check)
{
    check = tmp_check;
}


/*
 * Set up the _utilization of the thread depend on the size of matrix.
 * Let matrix pointer (singleResult, multiResult and Matrix) point 
 * to the matrix whcih create by System.
 *
 */
void
Thread::initialThread(float** single_result, float** multi_result, float** input_matrix)
{
	_utilization = float(_matrixSize / float(UTILIZATION_DIVIDER));	

    startCalculatePoint = 0;
    endCalculatePoint = _matrixSize;

    singleResult = single_result;
    multiResult = multi_result;
    matrix = input_matrix;
}


/*
 * Matrix multiplication operation and store the result into
 * singleResult which used to check the corennectness of multiResult 
 * by compare singleResult with multiResult.
 *
 */
void
Thread::singleMatrixMultiplication()
{	
	/* Print Thread information */
	core = sched_getcpu();
	PID = syscall(SYS_gettid);
	printInformation();

	/* Multiplication */
	for (int i = 0 ; i < _matrixSize; i++) {
		for (int j = 0 ; j < _matrixSize; j++) {
			singleResult[i][j] = 0;
			for (int k = 0 ; k < _matrixSize; k++) {
				singleResult[i][j] += matrix[i][k]*matrix[k][j];
			}	
		}
	}
}


/*
 * Matrix multiplication operation and store the result into
 * multieResult. 
 *
 * 1. For all part, pinned the thread to coresspond core dependent
 *    on variable setCore. 
 *
 * 2. For all part, using the system call the check is thread migrat
 *    to others core or not.
 *
 * 3. For Part3, call the member function setUpScheduler to set the 
 *    scheduler for current thread, and print out the Core0 current
 *    executed thread id to observie the different between FIFO and RR.
 *
 * @ Part1 Recommendation:
 * Implementation of pinned the current thread onto specify core.
 * Could be accomplish by call function SetUpCPUAffinityMask or other methods.
 * Implementation of detected the thread is migrate or not. 
 *
 * @ Part3 Recommendation:
 * Implementation of detected the Core0 is encounter the context
 * switch or not. If there is context swich then print out it was
 * switch from thread # to thread #.
 *
 *
 *矩陣乘法運算並將結果存儲到
  * multieResult。
  *
  * 1.對於所有部分，將線程固定到coresspond依賴於core
  * 在變量setCore上。
  *
  * 2.對於所有部分，使用系統調用檢查的是線程migrat
  *對他人的核心與否。
  *
  * 3.對於Part3，調用成員函數setUpScheduler來設置
  *當前線程的調度程序，並打印出當前的Core0
  *執行線程ID以觀察FIFO和RR之間的差異。
  *
  * @第1部分建議：
  *將當前線程固定在指定內核上的實現。
  *可以通過調用功能SetUpCPUAffinityMask或其他方法來完成。
  *實現檢測到的線程是否遷移。
  *
  * @第3部分建議：
  *執行檢測到的Core0遇到上下文
  *是否切換。 如果有上下文切換，則將其打印出來
  *從線程＃切換到線程＃。 
 */
void*
Thread::matrixMultiplication(void* args)
{
    Thread *obj = (Thread*)args;

#if (PART == 3)
    obj->setUpScheduler();
#endif

	/*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
    // Set up the affinity mask 設置親和力掩碼
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

    /* matrix multiplication */
	for (int i = obj->startCalculatePoint; i < obj->endCalculatePoint; i++) {
		for (int j = 0 ; j < obj->_matrixSize; j++) {
			obj->multiResult[i][j] = 0;
			for (int k = 0 ; k < obj->_matrixSize; k++) {
				obj->multiResult[i][j] += obj->matrix[i][k] * obj->matrix[k][j];
            }	

	        /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
            // Observe the thread migration 觀察線程遷移
	        /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

		}
#if (PART == 3)
	    /*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
        // Obaserve the execute thread on core-0
	    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
#endif
	}

	pthread_mutex_lock( &count_Mutex );
    obj->check->checkCorrectness();
	pthread_mutex_unlock( &count_Mutex );
    return 0;
}


/*
 * Print out the tread information.
 *
 */
void
Thread::printInformation()
{
    std::cout << "Thread ID : " << _ID ;
    std::cout << "\tPID : " << PID;
    std::cout << "\tCore : " << core;
#if (PART != 1)
    std::cout << "\tUtilization : " << _utilization;
    std::cout << "\tMatrixSize : " << _matrixSize;	
#endif
    std::cout << std::endl;
}


/* 
 * Set up the scheduler for current thread.
 *
 * @ Part3 Recommendation:
 * Using function sched_setscheduler to set up the scheduling
 * policy, or other method.
 *
 */
void
Thread::setUpScheduler()
{
	/*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
    // Set up the scheduler for current thread 為當前線程設置調度程序
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
}
