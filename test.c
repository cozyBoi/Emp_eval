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

int main(){
	int i, j;
	pid_t pid, pid2, pid3;
	struct timeval s, e, eps;
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

	//gettimeofday(&s, NULL);	
	for(i = 0; i < 1024; i++){
		space[i] = malloc(maxNum);
	}
	
	for(i = 0; i < 1024; i++){
		for(j = 0; j < maxNum; j++){
			((char*)(space[i]))[j] = 'a';
		}
	}
	srand(time(0));
	char fileName[10];
//	itoa((long int)getpid(), fileName, 5);
	fileName[0] = 'a' + (long int)getpid() % 5;
	fileName[1] = 0;
	fp = fopen(fileName, "w+");
	//gettimeofday(&s, NULL);
	for(i = 0; i < 10; i++){
		for(j = 0; j < maxNum; j++){
			if(j % pageNum == 0){
				gettimeofday(&s, NULL);
				//((char*)(space[i]))[rand()%(pageNum) * 1024] = 'b' + (i + j) % 128;
				((char*)(space[i]))[j] = 'b' + (i + j) % 128;

				gettimeofday(&e, NULL);
				timersub(&e, &s, &eps);
				fprintf(fp, "%ld elapse time : %ld.%08ld\n", (long int)getpid(), (long int)eps.tv_sec, (long int)eps.tv_usec);
			}
		}
	}

	//gettimeofday(&e, NULL);
	//timersub(&e, &s, &eps);
	//fprintf(fp, "%ld elapse time : %ld.%08ld\n", (long int)getpid(), (long int)eps.tv_sec, (long int)eps.tv_usec);
	for(i = 0; i < 10; i++){
		free(space[i]);
	}
	//gettimeofday(&e, NULL);
	//timersub(&e, &s, &eps);	
	//printf("elapse time : %ld.%06ld\n", (long int)eps.tv_sec, (long int)eps.tv_usec);
	if(pid != 0){
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
	}
	return 0;
}
