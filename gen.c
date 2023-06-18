#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
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
  int num;

  srand(time(NULL));

  shm_id = shm_open(shm_name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
  ftruncate(shm_id, sizeof(struct shmem));

  printf("shm_id = %d\n", shm_id);
  if (shm_id < 0) {
    perror("shm_open()");
    exit(1);
  }

  /* подключение сегмента к адресному пространству процесса */
  share = (struct shmem *) mmap(0, sizeof(struct shmem), PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0);
  if (share == NULL) {
    perror("mmap()");
    exit(2);
  }
  printf("share = %p\n", share);

  while (1) {
    num = random() % 1000;
    share->num = num;
    printf("write a random number %d\n", num);
    sleep(1);
    if (share->endflag) {
      safe_exit();
    }
  }
  return 0;
}
