#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  setuid(0);
  system("/bin/bash");
  return 0;
}

/*
sudo chown root:root a.out
sudo chmod 4755 a.out 
whoami
4755 - group and others cant write 
*/
