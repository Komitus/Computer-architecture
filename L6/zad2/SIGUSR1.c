#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h> //opcje do waitpid


void handler(int sig) {
  printf("%d handled by child\n", sig);
}

int main (void) {
  signal(SIGUSR1, handler);
	
	//int *status;
  pid_t pid = fork();
  if (pid == 0) {
    while(1);
  } 
  else {
    // rodzic
    for(int i=0; i<10; i++) {
      sleep(1);
      kill(pid, SIGUSR1);
    }
    printf("finshed sending signals\n");
    //kill(pid,9);
    pid_t waited = waitpid(pid, NULL, 0);   
    //czekamy na proces dziecko 
    //int value =  WIFEXITED(*status);
    //kill(pid,9);
    //printf("Value of our sended signal: %d", value); ale sigusr nie terminuje naszego procesu
       
  }
  
}
