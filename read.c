#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

struct shmem {
  int num;
  int endflag;
};
struct shmem *share;
int shm_id;

char shm_name[] = "shared_memory_dz_7";

void safe_exit() {
  printf("Safely exiting\n");
  if (share->endflag) {
    shm_unlink(shm_name);
    printf("Shared memory removed\n");
  } else {
    share->endflag = 1;
  }
  exit(0);
}

int main() {
  signal(SIGINT, safe_exit);
  shm_id = shm_open(shm_name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
  ftruncate(shm_id, sizeof(struct shmem));
  if (shm_id == -1) {
    perror("shmget()");
    exit(1);
  }

  share = (struct shmem *) mmap(0, sizeof(struct shmem), PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0);
  if (share == NULL) {
    perror("shmat()");
    exit(2);
  }

  while (1) {
    sleep(1);
    if (share->endflag) {
      safe_exit();
    }
    printf("%d\n", share->num);
  }

  return 0;
}
