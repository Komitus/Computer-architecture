#include <stdio.h>
#include <string.h>

int main(void)

{

 // char[] word1 = {'H', 'e', 'l', 'l', 'o', ',', 'w', 'o', '\0'}
  char word[] = "Hello, World!";
  unsigned int size = strlen(word);
  
  for(int i=0; i<size; i++)
  {	
  	 char letter = word[i];
  	 if(i<8) printf("\033[%dm %c", 30+i, letter);
  	 else printf("\033[%dm %c", 82+i, letter);
 	 
 	 
  }
  printf("\n");
  
  char answer[] = "Yes, we can do this!";
  unsigned int size2 = strlen(answer);
  int color_changer=10;
  for(int i=0; i<size2; i++)
  {
  	char letter = answer[i];
  	printf("\033[38;5;%dm %c", color_changer, letter); 
  	color_changer+=10; 
  
  }
  
  /*
  	\033[38;2;<r>;<g>;<b>m     #Select RGB foreground color
	\033[48;2;<r>;<g>;<b>m     #Select RGB background color

  */
  
  
  return (0);

}
