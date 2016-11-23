#include "main.h"

void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {   
        c++;
    }   
}

void *pthreadMutexTest()
{
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numIterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_mutex_lock(&count_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_mutex_unlock(&count_mutex);    
	
    }   


}

void *pthreadSpinTest(){

	int i;
	int j;
	int k;
	
	int localCount = 0;
    for(i=0;i<numIterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++){  /*How much work is done outside the CS*/
			localCount++;
		}

		pthread_spin_lock(&count_spin_lock);
		for(k=0;k<workInsideCS;k++){  /*How much work is done inside the CS*/
			c++;
		}
		pthread_spin_unlock(&count_spin_lock); 
	
    } 
}

void *mySpinTest(){

	int i;
	int j;
	int k;
	
	int localCount = 0;
    for(i=0;i<numIterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++){  /*How much work is done outside the CS*/
			localCount++;
		}

		my_spinlock_lockTAS(&count_my_spin_lock);
		// my_spinlock_lockTAS(&count_my_spin_lock);
		for(k=0;k<workInsideCS;k++){  /*How much work is done inside the CS*/
			c++;
		}
		my_spinlock_unlock(&count_my_spin_lock); 
	
    } 
}

void *mySpinTTASTest(){

	int i;
	int j;
	int k;
	
	int localCount = 0;
    for(i=0;i<numIterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++){  /*How much work is done outside the CS*/
			localCount++;
		}

		my_spinlock_lockTTAS(&count_my_spin_lock);
		for(k=0;k<workInsideCS;k++){  /*How much work is done inside the CS*/
			c++;
		}
		my_spinlock_unlock(&count_my_spin_lock); 
	
    } 
}

void *myMutexTest(){

	int i;
	int j;
	int k;
	
	int localCount = 0;
    for(i=0;i<numIterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++){  /*How much work is done outside the CS*/
			localCount++;
		}

		my_mutex_lock(&count_my_mutex);
		for(k=0;k<workInsideCS;k++){  /*How much work is done inside the CS*/
			c++;
		}
		my_mutex_unlock(&count_my_mutex); 
	
    } 
}

void *myQueueTest(){

	int i;
	int j;
	int k;
	
	int localCount = 0;
	printf("entering test");
    for(i=0;i<numIterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++){  /*How much work is done outside the CS*/
			localCount++;
		}

		my_queuelock_lock(&count_my_queuelock);
		for(k=0;k<workInsideCS;k++){  /*How much work is done inside the CS*/
			c++;
		}
		my_queuelock_unlock(&count_my_queuelock); 
    } 
}

int runTest(int testID)
{

	/*You must create all data structures and threads for you experiments*/
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;
	printf("Begin runTest()\n");
	if (testID == 0 || testID == 1 ) /*Pthread Mutex*/{	
		c=0;
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++){
		
			if( rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL)) ){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++){ //Wait for all threads to finish
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run Pthread (Mutex) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Mutex time(ms): %llu\n",result/1000000);

	}

	if(testID == 0 || testID == 2){ /*Pthread Spinlock*/
		printf("testID = 2\n");
		pthread_spin_init(&count_spin_lock, PTHREAD_PROCESS_PRIVATE);
		c=0;
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++){
		
			if( rt=(pthread_create( threads+i, NULL, &pthreadSpinTest, NULL)) ){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++){ //Wait for all threads to finish
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run Pthread (Spin) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Spin time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 3){ /*MySpinlockTAS*/

		printf("testID = 3\n");
		my_spinlock_init(&count_my_spin_lock);
		c=0;
		printf("Initialized\n");
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++){
		
			if( rt=(pthread_create( threads+i, NULL, &mySpinTest, NULL)) ){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++){ //Wait for all threads to finish
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run Spinlock TAS  Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Spin time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 4){ /*MySpinlockTTAS*/
	
		printf("testID = 4\n");
		my_spinlock_init(&count_my_spin_lock);
		c=0;
		printf("Initialized\n");
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++){
		
			if( rt=(pthread_create( threads+i, NULL, &mySpinTTASTest, NULL)) ){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++){ //Wait for all threads to finish
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run Spin TTAS Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Spin time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 5){ /*MyMutexLockTAS*/
	
		printf("testID = 5\n");
		my_mutex_init(&count_my_mutex);
		c=0;
		printf("Initialized\n");
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++){
		
			if( rt=(pthread_create( threads+i, NULL, &myMutexTest, NULL)) ){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++){ //Wait for all threads to finish
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run My Mutex Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Spin time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 6){ /*MyQueuelock*/
	
		printf("testID = 6\n");
		my_queuelock_init(&count_my_queuelock);
		c=0;
		printf("Initialized count_my_queuelock to new serving %lld, next_ticket %lld\n", count_my_queuelock.now_serving, count_my_queuelock.next_ticket);
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++){
			if( rt=(pthread_create(threads+i, NULL, &myMutexTest, NULL)) ){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++){ //Wait for all threads to finish
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run my queuelock Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Spin time(ms): %llu\n",result/1000000);
	}

	return 0;
}

int testAndSetExample(){
	volatile long test = 0; //Test is set to 0
	printf("Test before atomic OP:%d\n",test);
	tas(&test);
	printf("Test after atomic OP:%d\n",test);
}

int processInput(int argc, char *argv[]){

/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/
	int i = 0;

	numThreads=4;
	numIterations=1000000;
	testID=0;
	workOutsideCS=0;
	workInsideCS=1;

	for(i = 1; i < argc && i+1 < argc; i+=2){
		if (!strcmp(argv[i], "-t")){
			numThreads=atoi(argv[i+1]);
			printf("Set numThreads to %d\n", numThreads);
		}
		if (!strcmp(argv[i], "-i")){
			numIterations=atoi(argv[i+1]);
			printf("Set numIterations to %d\n", numIterations);
		}
		if (!strcmp(argv[i], "-o")){
			workOutsideCS=atoi(argv[i+1]);
			printf("Set workOutsideCS to %d\n", workOutsideCS);
		}
		if (!strcmp(argv[i], "-c")){
			workInsideCS=atoi(argv[i+1]);
			printf("Set workInsideCS to %d\n", workInsideCS);
		}
		if (!strcmp(argv[i], "-d")){
			testID =atoi(argv[i+1]);
			printf("Set testID to %d\n", testID);
		}
	}

	return 0;
}


int main(int argc, char *argv[])
{


	printf("Usage of: %s -t #threads -i #Iterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");	
	srand(time(NULL));
	// testAndSetExample(); //Uncomment this line to see how to use TAS
	
	processInput(argc,argv);
	runTest(testID);
	return 0;

}
