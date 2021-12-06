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
  printf("Do you want to remove or create?:\n");
  char * option = calloc(10, sizeof(char));
  fgets(option, 10, stdin);
  int semd, shmd, file;
  int * data;
  if(strcmp(option, "remove")>0){
    shmd = shmget(KEY, sizeof(int), 0);
    if (shmd == -1) {
      printf("error %d: %s\n", errno, strerror(errno));
    } else {
      shmctl(shmd, IPC_RMID, 0);
    }
    semd = semget(KEY, 1, 0);
    if (semd == -1) {
      printf("error %d: %s\n", errno, strerror(errno));
    } else {
      semctl(semd, IPC_RMID, 0);
    }
    char * file = "foo.txt";
    execvp("cat", &file);
  } else if (strcmp(option, "create")>0){
    file = open("foo.txt", O_CREAT|O_TRUNC, 0664);
    close(file);
    shmd = shmget(KEY, sizeof(int), IPC_CREAT | 0600);
    if (shmd == -1) {
      printf("error %d: %s\n", errno, strerror(errno));
    }
    else {
      data = shmat(shmd, 0, 0);
      *data = 0;
      shmdt(data);
  }
    semd = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
    if (semd == -1) {
      printf("error %d: %s\n", errno, strerror(errno));
    }
    else {
      union semun us;
      us.val = 1;
      semctl(semd, 0, SETVAL, us);
  }
  } else {
    printf("use valid command\n");
  }
  free(option);
  return 0;
}
