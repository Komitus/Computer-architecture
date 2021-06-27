#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void sighandle(int sig) {
  printf("%d handled\n", sig);
}

int main (void) {

  for (int i=0; i<=64; i++) {     // w manualu kill -l mamy 65 pozycjy, 0 to exit
  
    if (signal(i, sighandle) == SIG_ERR) {
    
      printf("cannot handle signal %d\n", i);
      char command[100];
      sprintf(command,"kill -l %d", i); 
      system(command);
    }
  }
}

/*If you are a normal user, you can kill your own processes, but not those that belong to other users. 
When a process performs input/output operations (such as reading from or writing to disks), 
it is said to be in a state of “uninterruptible sleep”. You can’t kill a process while it is in this state.
Również nie można zabijać procesów zombie, ponieważ jest to tylko wpis na liście wszystkich procesów 
i nie ma z nim powiązanego rzeczywistego procesu.


SIGKILL - zabija bez czekania na wykonanie instruckji po zamknieciu
SIGTERM - bardziej łągodny, czeka aż program wykona "ostatnie życzenie"
*/
