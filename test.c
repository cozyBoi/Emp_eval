#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

/*
* TODO :
* Make 3 process
* Alloc space up to maximum amount of malloc (16GB)
* Measure local / remote access time
*/

//const int maxNum = 16 * (1<<30);
//const long long maxNum = 15 * (1<<30);
long long maxNum = 16 * 1024 * 1024;
const int pageNum = 4 * (1<<10);

enum { NS_PER_SECOND = 1000000000 };

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

int main(){
	int i, j;
	pid_t pid, pid2, pid3;
	FILE*fp;
	pid = fork();
	if(pid == -1) exit(0); //error

	if(pid != 0) {
		//parent
		pid2 = fork();

		if(pid2 != 0){
			pid3 = fork();
			if(pid3 != 0){
				fork();
			}
		}
	}

	void*space[1024];

	for(i = 0; i < 1024; i++){
		space[i] = malloc(maxNum);
	}
	
	for(i = 0; i < 1024; i++){
		for(j = 0; j < maxNum; j++){
			((char*)(space[i]))[j] = 'a';
		}
	}
    //make 16GB space per each process

	srand(time(0));
	char fileName[10] = {'e', 'v', 'a', 'l'};

	fileName[4] = '0' + (long int)getpid() % 5;
	fileName[5] = 0;
	fp = fopen(fileName, "w+");
    //open eval file

    struct timespec start, finish, delta;
	//for(i = 0; i < 1024; i+=100){
	for(i = 0; i < 20; i++){
		for(j = 0; j < maxNum; j++){
			if(j % pageNum == 0){
                clock_gettime(CLOCK_REALTIME, &start);
				((char*)(space[i]))[j] = 'b' + (i + j) % 128;
                clock_gettime(CLOCK_REALTIME, &finish);
                sub_timespec(start, finish, &delta);
	            fprintf(fp, "%.8X : %ld.%.9ld\n", i*maxNum + j, delta.tv_sec, delta.tv_nsec);
			}
		}
	}
	//fprintf(fp, "%ld.%.9ld\n", delta.tv_sec, delta.tv_nsec);

	for(i = 0; i < 1024; i++){
		free(space[i]);
	}

	if(pid != 0){
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
	}
	return 0;
}
