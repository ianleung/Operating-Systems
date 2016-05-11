#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

//=========================================================================
// Shared Variables
//=========================================================================
static int TERMINATE = -1;
static int TIMING = -2;

struct timeval tv;

int N,B,P,C,in,out;
pthread_t* thread_producer;
pthread_t* thread_consumer;
int* buffer;

double t1,t2;

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t not_empty_sem;
sem_t not_full_sem;

//=========================================================================
// Helper Functions
//=========================================================================
//initialize: initalize queues, thread arrays, and semaphores
void initialize ()
{
	buffer = malloc(sizeof(pthread_t)*B);
	thread_producer = malloc(sizeof(pthread_t)*P);
	thread_consumer = malloc(sizeof(pthread_t)*C);
	in = 0;
	out = 0;
	sem_init (&not_empty_sem, 0, 0);
	sem_init (&not_full_sem, 0, B);
}

// get_time: Retrieves Times
double get_time(){
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec/1000000.0;
}

// is_integer: checks if double is an integer
int is_integer(double n) {
	return n - floor(n) == 0.0;
}

// push_buffer: pushes a item into circular buffer
void push_buffer(int number){
	buffer[in]= number;
	in = (in + 1)%B;
}

// push_buffer: removes item from circular buffer
int pop_buffer(){
	int number = buffer[out];
	out = (out + 1)%B;
	return number;
}

//=========================================================================
// Producer / Consumer Threaded
//=========================================================================
void* producer (void* arg)
{
	int i;
	int id = (intptr_t)arg; 

	for (i = 0 ; i < N; i++){
		//filter by thread param
		if (i%P != id) continue; 

		//wait for not full, push to buffer, signal not empty
		sem_wait (&not_full_sem);
		pthread_mutex_lock (&buffer_mutex);
		push_buffer(i);
		pthread_mutex_unlock (&buffer_mutex);
		sem_post (&not_empty_sem);
	}
}

void* consumer (void* arg)
{
	int number;
	int id = (intptr_t)arg; 
	while (1) {
		//wait for not empty, retrieve from buffer, signal not full
		sem_wait (&not_empty_sem);
		pthread_mutex_lock (&buffer_mutex); 
		number = pop_buffer();
		pthread_mutex_unlock (&buffer_mutex);   
		sem_post(&not_full_sem);

		//check for time
		if (number== TIMING) {t2 = get_time();}
		
		//check for termination
		if (number<= TERMINATE) {return NULL;}

		//process number
		double squared = sqrt(number);     
		if (is_integer(squared))
			printf ("%d %d %d\n", id , number, (int)floor(squared));
	}
}


int main(int argc, char* argv[] )
{
	//Handle arguments
	if ( argc !=5 ) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}
	N = atoi(argv[1]);
	B = atoi(argv[2]);
	P = atoi(argv[3]);
	C = atoi(argv[4]);

	initialize();

	// Initialize timing
	t1 = get_time();

	int i;
	// Create producer and consumers
	for(i = 0; i<P; i++)
		pthread_create (&thread_producer[i], NULL, producer, (void *)(intptr_t)i);
	for(i = 0; i<C; i++)
		pthread_create (&thread_consumer[i], NULL, consumer, (void *)(intptr_t)i); 

	//Join with terminated producers
	for(i = 0; i<P; i++)
		pthread_join (thread_producer[i], NULL); 


	//Mark consumers for termination, first one will save time aswell
  int number;
	for(i = 0; i<C; i++){
    number = i? TERMINATE:TIMING;
		sem_wait (&not_full_sem);
		pthread_mutex_lock (&buffer_mutex);
		push_buffer(number);
		pthread_mutex_unlock (&buffer_mutex);
		sem_post (&not_empty_sem);
	}
  
	//Join with terminated consumer
	for(i = 0; i<C; i++)
		pthread_join (thread_consumer[i], NULL);

  printf("System execution time: %f seconds\n", (t2-t1));

	free(buffer);
	return 0;
}