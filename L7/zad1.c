#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define BUFSIZE 1024

char* intToStr(int value, int base) {

    char* result = malloc(BUFSIZE*sizeof(char));
    if (result == NULL) exit(1);
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        //here we use integral dividing and we subtract e.g 123 - (123/10)*10 = 123 - 12*10 = 3
        //we have array going two sides beceause we can have negative values
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-'; //on *ptr we put '-' and then increment value
    *ptr-- = '\0';      //we put \0 on *ptr then decrement 

    //reversing output from 321- to -123
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1; //here we remember values from begining by puting it at the end counting from left
        *ptr1++ = tmp_char; //here we replace values from begining with values from end
    }
   return result;
}

//we operate on small letters
int strToInt(char* str, int base) {
  // works for integer bases up to 16
  int result=0;
  char isnegative = *str=='-';
  if (isnegative) str++;

  while (*str != '\0') {
    if (*str >= '0' && *str <= '9') {      
      result = result*base + (*str - '0');        //Horner Schema
    } else if (*str >= 'a' && *str <= 'f') {
      result = result*base + (*str - 'a'+10);
    }
    str++;
  }

  if (isnegative) result = -result;

  return result;
}

void myPrintf(char* template, ...) {

  void* nextarg = (void*)&template + sizeof(template);
  while(*template!='\0'){
    
    if(*template=='%'){
      template++;

      switch(*template){

        case 'd':
           myPrintf(intToStr(*(int*)nextarg, 10));
           nextarg+=sizeof(int);
           break;
        case 'x':
          myPrintf(intToStr(*(int*)nextarg, 16));
          nextarg+=sizeof(int);
          break;
        case 'b':
          myPrintf(intToStr(*(int*)nextarg, 2));
          nextarg+=sizeof(int);
          break;
        case 's':
          myPrintf(*(char**)nextarg);
          nextarg+=sizeof(char*);
          break;
      }
    }
    else{
      write(1,template,sizeof(char));
    }
    template++;

  }
  
}

void myScanf(char* template, ...) {
  char* nextarg = (char*)&template + sizeof(template);
  

  while (*template != '\0') {
    if (*template == '%') {
      
      char* tempstr = malloc(BUFSIZE*sizeof(char));
      if (tempstr == NULL) exit(1);

     
      int  i=-1;
      char* tmp = malloc(sizeof(char));
      if (tmp == NULL) exit(1);
      //reading input sign by sign
      do {
        i++;
        read(0, tmp, sizeof(char));
        tempstr[i] = *tmp;
        //printf("%s \n", tmp);
      } while (*tmp!='\n' && *tmp!=*(template+2)); //template+2 is our separator e.g %d,%d - comma is separator
      free(tmp);
        
      tempstr[i]='\0';
      
      template++;
      switch(*template) {
        case 'd': {
          int* result = (int*)(*(int*)nextarg);

          *result = strToInt(tempstr, 10);

          nextarg += sizeof(int*);
          break;
        }
        case 'b': {
          int* result = (int*)(*(int*)nextarg);

          *result = strToInt(tempstr, 2);

          nextarg += sizeof(int*);
          break;
        }
        case 'x': {
          int* result = (int*)(*(int*)nextarg);

          *result = strToInt(tempstr, 16);

          nextarg += sizeof(int*);
          break;
        }
        case 's': {
          char* result = (char*)(*(int*) nextarg);

          int i;
          for (i=0; tempstr[i] != '\0'; i++) {
            result[i]=tempstr[i];
          }
          result[i]='\0';

          nextarg += sizeof(char*);
          break;
        }

      }
      free(tempstr);
    } else {
        //do nothing
    }
    template++;
   
  }

}





int main(){

    /*
    myPrintf("-123(10): %d \n", -123);
    myPrintf("126(2): %b \n", 126);
    myPrintf("255(16): %x \n", 255);
    myPrintf("16(16): %x \n", 15);
    */
    int a,b;
    char word[6];
    /*
    myScanf("%d,%d", &a, &b);  //here we have comma as separator @see template+2
    myPrintf("%d %d \n", a, b);
    myScanf("%s", word);
    myPrintf("%s \n", word);
    myScanf("%d %s", &a, word);
    myPrintf("%d  %s \n", a, word);
    */

    myScanf("%x", &a);
    myPrintf("%d \n", a);
    myScanf("%b", &b);
    myPrintf("%d \n", b);
    
    
    
    
    return 0;
}