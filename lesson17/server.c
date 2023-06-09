#include "comm.h"
#include <sys/wait.h>
int main(){

  umask(0);
  if(mkfifo(MY_FIFO, 0666) < 0){
    perror("mkfifo");
    return 1;
  }
  //只需要文件操作即可
  int fd = open(MY_FIFO, O_RDONLY);
  if(fd < 0){
    perror("open");
    return 2;
  }
  //业务逻辑，可以进行对应的读写了
  while(1){
    char buffer[64] = {0};
    ssize_t s = read(fd, buffer, sizeof(buffer)-1);
    if(s > 0){
      //success
      buffer[s] = 0;
      if(strcmp(buffer, "show") == 0){

        if(fork() == 0){
          execl("/usr/bin/ls", "ls", "-l", NULL);
          exit(1);
        }
      waitpid(-1, NULL, 0);
      }    
      else if(strcmp(buffer, "run") == 0){
        if(fork() == 0){
          execl("/usr/bin/sl", "sl", NULL);
        } 
        waitpid(-1, NULL, 0);
      }
      else{
        printf("client# %s\n", buffer);
      }
    }
    else if( s == 0){
      //peer close
      printf("client quit...\n");
      break;
    }else{
      //error
      perror("read");
      break;
    }
  }
  close(fd);
  return 0;
}
