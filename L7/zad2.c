#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int  size;
    bool** matrix1;
    bool** matrix2;
    bool** result;
    int calculatedRows;
} matrixes_struct;

matrixes_struct initializeMatrixes(int size){

    bool** matrix1 = malloc(size*sizeof(bool*));
    bool** matrix2 = malloc(size*sizeof(bool*));
    bool** result = malloc(size*sizeof(bool*));
    if (matrix1==NULL || matrix2==NULL || result==NULL) exit(1);
    srand(time(0));

    for(int i=0; i<size; i++){

        matrix1[i]=malloc(size*sizeof(bool));
        matrix2[i]=malloc(size*sizeof(bool));
        result[i]=malloc(size*sizeof(bool));
        if (matrix1[i]==NULL || matrix2[i]==NULL || result[i]==NULL) exit(1);

        for (int j=0; j<size; j++) {
            matrix1[i][j] = rand()%2;
            matrix2[i][j] = rand()%2;
        }
    }

    matrixes_struct returned = {size: size, matrix1: matrix1, matrix2: matrix2, result: result, calculatedRows: 0};
    return returned;

}
void* multiply(void* arg){

    //printf("Thread have started");
    matrixes_struct* structure = (matrixes_struct*)arg;

    while(structure->calculatedRows < structure->size){

        //we lock numer of row we want to calculate in thread
        pthread_mutex_lock(&mutex);
        int i = structure->calculatedRows++;
        pthread_mutex_unlock(&mutex);
        //printf("%d  ", i);
        
        for(int j=0; j<structure->size; j++){
            int sum = 0;
            for(int sumRows = 0; sumRows < structure->size; sumRows++){
                //| - or  & - binary and
                sum = sum | (structure->matrix1[i][sumRows] & structure->matrix2[sumRows][j]);
                //so when one & is 1 the whole expression will be 1
                if(sum) break;
            }
            structure->result[i][j]=sum;
        }
    }
}
void printMatrixes(matrixes_struct* structure){

    printf("First matrix\n");
    for(int i=0; i<structure->size; i++){

        for(int j=0; j<structure->size; j++){
            printf("%d ", structure->matrix1[i][j]);
        }
        printf("\n");
    }
    printf("\nSecond matrix\n");
    for(int i=0; i<structure->size; i++){

        for(int j=0; j<structure->size; j++){
            printf("%d ", structure->matrix2[i][j]);
        }
        printf("\n");
    }
    printf("\nResult matrix\n");
    for(int i=0; i<structure->size; i++){

        for(int j=0; j<structure->size; j++){
            printf("%d ", structure->result[i][j]);
        }
        printf("\n");
    }
    
}


int main(int argc, char *argv[])
{   
    //when compiling add "gcc -pthread"
    if (argc != 3){
        printf("Too few arguments\n");
        exit(1);
    }
    int matrixSize = atoi(argv[1]);
    if (matrixSize<=0){
        printf("Invalid matrix size\n");
        exit(1);
    }
    int numbOfThreads= atoi(argv[2]);
    if (numbOfThreads<=0){
        printf("Invalid number of threads\n");
        exit(1);
    }
    
    matrixes_struct matrixes = initializeMatrixes(matrixSize);
    pthread_t threadsId[numbOfThreads];
    
    for(int i=0; i<numbOfThreads; i++){
        pthread_create(threadsId+i,NULL, multiply, (void*)&matrixes);
    }
    for (int i=0; i<numbOfThreads; i++) {
        pthread_join(threadsId[i], NULL);
    }
    
    //printMatrixes(&matrixes);
    return 0;
}