#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 

int frame[16][4];

int Mem[32];


FILE *write;
FILE *read;


int temp;
int page_Size;

int offset = 0;
int full = 0;

unsigned int bit;

pthread_mutex_t mutex;

// Tracking assigned thread 

typedef struct __thread_info {
    char file[100];
    int thread_ID;
} thread_info;

void *threads(void *arg)
{
    thread_info *a = (thread_info *) arg;
    FILE *temp;
    
   
    int VMS; // virtual meamory size
    char rw[1];
    
    
    char Reg[3];
    int addr;
    temp = fopen(a->file, "r");
    
    char ch;
    int line_Count = 0;  //Counting lines in the file 
    while((ch = fgetc(temp)) != EOF)
    {
        if(ch=='\n')
            line_Count++;
    }
    fclose(temp);
    //Reads File
    temp = fopen(a->file, "r");
    fscanf(temp, "%d", &VMS);
   
   //Page variables 
    int page_Num;
    int openPage = -1;
    
    
    
    bool found = 1;
    unsigned int frame_Num;
   
 // end of setup
    for(int j = 0; j < line_Count; j++) 
    {
        fscanf(temp, "%s", rw);
        fscanf(temp, "%s", Reg);
        fscanf(temp, "%d", &addr);
        fprintf(write, "P%d OPERATION: %c %s 0x%08x\n", a->thread_ID,rw[0], Reg, addr);
        pthread_mutex_lock(&mutex);
        int page_Num = addr / page_Size;
        int x = 0;
       
        if(openPage != page_Num)
        {
            fprintf(write, "P%d: page %d not resident in memory\n", a->thread_ID, page_Num);
            openPage = page_Num; //Page number changed to Time to change frames
            

//Check frames 

 //Case its Full
            if (found == 0){
                fprintf(write, "P%d: evicting process %d, page %d from frame %d\n", a->thread_ID, frame[full][2], frame[full][3], full);
                frame_Num = full;
                full = (full + 1) % 16;
            }
            fprintf(write, "P%d: new translation from page %d to frame %d\n", a->thread_ID, openPage, frame_Num);
        }
        //Page did not change
        else{
            fprintf(write, "P%d: valid translation from page %d to frame %d\n", a->thread_ID, openPage, frame_Num);
        }
        
            while(1){ //case its not used
            
            if(frame[x][1] == 0) { 
                    frame_Num = x;
                    frame[x][1] = 1;
                    frame[x][2] = a->thread_ID;
                    frame[x][3] = page_Num;
                    fprintf(write, "P%d: using free frame %d\n", a->thread_ID, frame[x][0]);
                    break;
                }
                x++;
                if(x == 16)
                {
                    found = 0;
                    break;
                }
            }
            
        
        int PA = frame_Num << offset; //Getting physical Address
        
        PA += bit & addr;
        
        pthread_mutex_unlock(&mutex);
        
        fprintf(write, "P%d: translated VA 0x%08x to PA 0x%08x\n", a->thread_ID, addr, PA);
        fprintf(write, "P%d: %s = 0x%08x (mem at virtual addr 0x%08x)\n", a->thread_ID, Reg, Mem[bit & addr], addr);
    }
    fclose(temp);
}
int main(int argc, char *argv[]){
    
    int j;
    assert(argc == 4);
    read = fopen(argv[1], "r");
    write = fopen(argv[2], "w");
    
    int random = atoi(argv[3]);
    
    srand(random);
    //frame initialization
    for(j = 0; j < 16; j++)
    {
        frame[j][0] = j;
        
        frame[j][1] = 0; //case not used
    }
    for(j = 0; j < 32; j++) //initialization random memory address
    {
        Mem[j] = rand();
    }
  
  //Reading file
   
    int processes;
    
    int mem_Size;

    fscanf(read, "%d", &mem_Size);
    fscanf(read, "%d", &page_Size);
   
    temp = page_Size;
    while(temp != 1) {   //get Num bits
        temp /= 2;
        offset++;
    }
    for(j = 0; j < offset; j++){
        bit *= 2;
        bit += 1;
    }
    fscanf(read, "%d", &processes);
//creating threads   
    thread_info a[processes];
    pthread_t b[processes];
    char process[10][100];
    
    for(j = 0; j < processes; j++) {
        fscanf(read, "%s", process[j]);
        a[j].thread_ID = j;
        strcpy(a[j].file,process[j]);
        fprintf(write, "Process %d started\n", j);
        pthread_create(&b[j], NULL, threads, &a[j]);
    }
    for(j = 0; j < processes; j++){
        pthread_join(b[j], NULL);
        fprintf(write, "Process %d completed\n", a[j].thread_ID);
       
        for(int x = 0; x < 16; x++) { //assign frames to unused
        
            if(frame[x][2] == a[j].thread_ID)
            {
                frame[x][1] = 0;
            }
        }
    }
    fprintf(write, "Main: program completed");
    fclose(read);
    return 0;
}
