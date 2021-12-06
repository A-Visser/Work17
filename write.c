#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#define KEY 34553

int main(){
  int semd, shmd, file;
  int * data;
  semd = semget(KEY, 0, 0);
  struct sembuf us;
  us.sem_op = 0;
  us.sem_num = 0;
  us.sem_flg = 0;
  semop(semd, &us, 1);
  shmd = shmget(KEY, sizeof(int), 0);
  if (shmd == -1) {
    printf("error %d: %s\n", errno, strerror(errno));
  }
  else {
    us.sem_op = -1;
    semop(semd, &us, 1);
    data = shmat(shmd, 0, 0);
    char * last_line = calloc(*data+1, sizeof(char));
    file = open("foo.txt", O_RDONLY);
    lseek(file, *data, SEEK_END);
    read(file, last_line, *data);
    close(file);
    last_line[*data+1] = '\0';
    printf("Last line: %s\n", last_line);
    free(last_line);
    printf("Type into story:\n");
    char * line = calloc(100, sizeof(char));
    fgets(line, 100, stdin);
    *data = strlen(line)+1;
    char * new_line = calloc(*data, sizeof(char));
    strncpy(new_line, line, strlen(line));
    new_line[strlen(line)] = 10;
    file = open("foo.txt", O_WRONLY);
    write(file, new_line, *data);
    close(file);
    free(line);
    free(new_line);
    shmdt(data);
    us.sem_op = 1;
    semop(semd, &us, 1);
  }
}
