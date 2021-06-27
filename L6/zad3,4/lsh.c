#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define TOKEN_BUFFER_SIZE 8
#define READL_BUFFER_SIZE 1024
#define MAX_ARRAY_LENGTH   10

void  siginthandle(int signal) {}
pid_t* pidsK = NULL;
char* read_line() {
  int bufsize = READL_BUFFER_SIZE;
  int pos = 0;
  char* buffer = malloc(sizeof(char)*bufsize);
  char c;
  
  if (!buffer) {
    fprintf(stderr, "lsh: memory error\n");
    exit(EXIT_FAILURE);
  }


  while (1) {
    
    c = getc(stdin);

    if (c == EOF) {
    	printf("Exiting with killing processes\n");
		  int j = 0;
			while(pidsK!=NULL){
				kill(pidsK[j],9); 
				j++;
		    
      }
      free(pidsK);
      exit(0);
    }
    else if (c == '\n') {
      buffer[pos] = '\0';
      return buffer;
    } 
    else buffer[pos] = c;

    pos++;

    if (pos > bufsize) {
      bufsize += READL_BUFFER_SIZE;
      buffer = realloc(buffer, bufsize*sizeof(char)); //realokujemy pamieć, gdy wyjdziemy poza wartość bufora
      if (!buffer) {
        fprintf(stderr, "lsh: memory error\n");
        exit(1);
      }
    }
  }
}

char** tokenize_line(char* line) {

  int bufsize = TOKEN_BUFFER_SIZE ;
  if(line[0] == '\0') return NULL;
  
  char** tokens = malloc(TOKEN_BUFFER_SIZE *sizeof(char*)); //wskaźnik do tablicy naszych wskaznikow do "tokenow"
  char* token;
  if (!tokens) {
    fprintf(stderr, "lsh: memory error\n");
    exit(EXIT_FAILURE);
  }
  int iterator = 0;

  
  token = strtok(line, " ");
  while (token != NULL) {
    tokens[iterator] = token;
    iterator++;

    if (iterator > bufsize) {
      bufsize += READL_BUFFER_SIZE;
      tokens = realloc(tokens, bufsize*sizeof(char));
      if (!tokens) {
        fprintf(stderr, "lsh: memory error\n");
        exit(1);
      }
    }

    token = strtok(NULL, " "); 
  }

  tokens[iterator]  = NULL; // chcemy wiedzieć, gdzie zakonczy sie nasza tablica

  return tokens;
}

int check_for_bg_exec(char** args) {
  for (int i=0; args[i] != NULL; i++) {
    if (args[i][0] == '&') {
      args[i] = NULL;
      return 1;
    }
  }
  return 0;
}

void redirect (char** args) {
  for (int i=0; args[i] != NULL; i++) {
    if (strcmp(args[i], ">") == 0) {
      // redirect  stdout
      args[i] = NULL;
      freopen(args[i+1], "w+", stdout);
    } else if (strcmp(args[i], "2>") == 0) {
      // redirect stderr
      args[i] = NULL;
      freopen(args[i+1], "w+", stderr);
    } else  if (strcmp(args[i], "<") == 0) {
      // redirect stdin
      args[i] = NULL;
      freopen(args[i+1], "r", stdin);
    }
  }
}



int execute(char** args) {
	
	if (strcmp(args[0], "exit") == 0) {
    printf("Exiting with killing processes\n");
    int j = 0;
  	while(pidsK!=NULL){
  		kill(pidsK[j],9); 
  		j++;
  	}
  	free(pidsK);
    exit(0);
  }
  if (strcmp(args[0], "cd") == 0) {
    chdir(args[1]);
    return 1;
  }
  
  int bg_exec = check_for_bg_exec(args);
  char*** commands = malloc(MAX_ARRAY_LENGTH*sizeof(char**)); 
  if (!commands) {
    fprintf(stderr, "memory error\n");
    exit(1);
  }

  int command_no = 0;
  commands[command_no] = malloc(MAX_ARRAY_LENGTH*sizeof(char*));
  if (!commands[command_no]) {
    fprintf(stderr, "memory error\n");
    exit(1);
  }

  int i=0;
   
  int command_new_pos=0;
  while(args[i]!=NULL) {
    if (strcmp(args[i], "|") != 0) {
      commands[command_no][command_new_pos] = args[i];
      command_new_pos++;
    } else {
      commands[command_no][command_new_pos] = NULL;
      command_no++;
      command_new_pos = 0;
      commands[command_no] = malloc(MAX_ARRAY_LENGTH*sizeof(char*));
      if (!commands[command_no]) {
        fprintf(stderr, "memory error\n");
        exit(1);
      }
    }
    i++;
  }
  commands[command_no][i] = NULL;

  // inicjalizacja deskryptorow, robimy to przed forkiem bo inaczej stworzymy dwa osobne dla dziecak i rodzica !!!
  int pipes[command_no][2];
  for (int j=0; j<command_no; j++) {
    if (pipe(pipes[j]) < 0) {
      perror("lsh: piping error");
    }
  }


  pidsK = malloc((command_no+1)*sizeof(int));
  pid_t pids[command_no+1];  //bo command_no zaczyna sie od 0
  //robimy forka dla każdego procesu i załączamy deskryptory
  for (int j=0; j<=command_no; j++) {
    if ((pids[j] = fork()) == 0) {
      // jesli proces jest rodzicem
      if (j == 0) {
        //j to nasze pierwsze polecenie to zalaczamy fd 1 do nowego fd o numerze 1
        //dup2() tworzy nam kopie podanego fd z nazwą podaną po przecinku
        if (dup2(pipes[j][1], 1) == -1) {
          perror("lsh: dup2 error,only output");
          exit(1);
        }
      } 
      
      else if (j == command_no) {
        //j jest to nasze ostatnie polecenie to zalaczamy fd 0 do nowego fd o numerze 0
        if (dup2(pipes[j-1][0], 0) == -1) {
          perror("lsh: dup2 error, only input");
          exit(1);
        }
      } 
      //jeśli proces jest rodzicem ale nie jest ostatni
      else {
        // załaczamy input i output
        
        if (dup2(pipes[j-1][0], 0) == -1) {
          perror("lsh: dup2 error, both");
          exit(1);
        }
        if (dup2(pipes[j][1], 1) == -1) {
          perror("lsh: dup2 error,both");
          exit(1);
        }
      }
      // zamykamy fds dla forkow
      for (int k=0; k<command_no; k++) {
      	
        close(pipes[k][0]);
        close(pipes[k][1]);
      }
      
      // wykonujemy komendy

      redirect(commands[j]);
			if (j==command_no && bg_exec) {
        freopen("/dev/null", "w+", stdout);
      }

      if (execvp(commands[j][0], commands[j]) == -1) {
        perror("lsh");
        exit(1);
      }
			
      free(commands[j]);
      exit(0);
    }
		else if(pids[j]!=0){
				int tmp = 0;
			  pidsK[tmp] = pids[j];
			  //printf("%d", pidsK[tmp]);
			  
		}
    free(commands[j]);
  }

  //skończyły się wykonywać nasze forki
  //musimy zamknąć fds
  for (int j=0; j<command_no; j++) {
    close(pipes[j][0]);
    close(pipes[j][1]);
  }
	
	
  // rodzic (nasz lsh musi czekać na dzieci), tak zabijamy procesy dzieci
  int j;
  if (!bg_exec) { 
		for(j=0; j<=command_no; j++) {
		  if (pids[j] > 0) {
		    waitpid(pids[j], NULL, 0);  // parametr 0 mówi nam, że czekamy na procesy w grupie
		  }
		}
	}
	// tu jesli nie bylo ampersanda to czekamy, jesto to ostatnia komenda, wiec jak bd tylko jedna z & to to   sie nie wykona
 /* if (!bg_exec) {  
    waitpid(pids[j],NULL,0);
  }
*/
  free(commands);

  return 1;
}






void lsh_loop(void) {
  char *line;
  char **args;
  int status;

  do {

    printf("> ");
    char c = getc(stdin);
    if (c =='\n') {
      continue;
    } else {
      ungetc(c, stdin);
    }
    line = read_line();
    args = tokenize_line(line);
    status = execute(args);

    free(line);
    free(args);
  } while (status);
}

int main() {
  //SIGCHLD Sygnał pułapki wskazujący na zakończenie procesu uruchomionego przez bieżący proces
  //SIG_IGN ignorujemy to ze proces dziecka sie zakonczył, bo to dla nas dobrze
  //gdy ktoś wywoła prerwanie z klawiatury SIGINT to siginthandler bierz czyli nic nie robi
  signal(SIGCHLD, SIG_IGN); 
  signal(SIGINT, siginthandle); 
  lsh_loop();
   
   return 0;
}
