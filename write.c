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
  struct sembuf sb;
  shmd = shmget(KEY, sizeof(int), 0);
  if (shmd == -1) {
    printf("error %d: %s\n", errno, strerror(errno));
  }
  else {
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;
    sb.sem_num = 0;
    semop(semd, &sb, 1);
    data = shmat(shmd, 0, 0);
    char * last_line = calloc(*data, sizeof(char));
    file = open("foo.txt", O_RDONLY);
    lseek(file, -*data, SEEK_END);
    read(file, last_line, *data);
    close(file);
    printf("Last line: %s\n", last_line);
    free(last_line);
    printf("Type into story:\n");
    char * line = calloc(100, sizeof(char));
    fgets(line, 100, stdin);
    *data = strlen(line);
    char * new_line = calloc(*data, sizeof(char));
    strncpy(new_line, line, strlen(line));
    file = open("foo.txt", O_WRONLY | O_APPEND);
    write(file, new_line, *data);
    close(file);
    free(line);
    free(new_line);
    shmdt(data);
    sb.sem_op = 1;
    semop(semd, &sb, 1);
  }
}
