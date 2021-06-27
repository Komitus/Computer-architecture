#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
  
  char command[100];
  system("ps aux | awk 'FNR == 2 {print; exit}'");
  
  kill(1,9);
  system("ps aux | awk 'FNR == 2 {print; exit}'");
  int correct = kill(1,9);
  printf("\n Executed? %d \n", correct);
}

//ja mam oprocz inita systemd
