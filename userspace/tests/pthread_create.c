#include "pthread.h"
#include "stdio.h"

//-----------------------------------------------------------------------------

void* thread(void *arg)
{
  printf("2. Thread: %p\n", arg);

  return NULL;
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  printf("1. Main\n");

  pthread_t tid;
  pthread_create(&tid, NULL, &thread, NULL);

  while(1);
  return 0;
}
