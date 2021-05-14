// 9893, Росихин Александр, 
// Дисциплина ОС,
// Лабораторная работа №6.

/*
 № 1
*/

#include <iostream>
#include <unistd.h> // для sleep()
#include <semaphore.h>
#include <fcntl.h> // для O_CREAT
#include <sys/mman.h>
#include <sys/stat.h>

int flag = 0;
sem_t* semRead;
sem_t* semWrite;
int  shm;
void* localAddr;

#define SEM_WRITE "/rosikhinwrite"
#define SEM_READ "/rosikhinread"
#define SHM "SHM"

void* thread(void* flag)
{
 int r;
 
 while (*((int*)flag) == 0)
 {
  sem_wait(semWrite);
  srand(time(0));
  r = rand() % 10;
  std:: cout << "Cлучайное число: " << r << std::endl;
  
  *((int*)localAddr) = r;
  
  sem_post(semWrite);
  
  usleep(50000);
 }

 return 0;
}

int main()
{
 pthread_t id;

 shm = shm_open(SHM, O_RDWR | O_CREAT, 0777);
 ftruncate(shm, sizeof(int));
 
 localAddr = mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm, 0);
 
 semWrite = sem_open(SEM_WRITE, O_CREAT, 0644, 100);
 semRead = sem_open(SEM_READ, O_CREAT, 0644, 100);

 pthread_create(&id, NULL, thread, &flag); 

 getchar();
 flag = 1;
 pthread_join(id, NULL);  
 
 sem_close(semRead);
 sem_unlink(SEM_READ);
  
 sem_close(semWrite);
 sem_unlink(SEM_WRITE);
 
 munmap(localAddr, sizeof(int));
 
 //close(shm);
 shm_unlink(SHM);
 
 std::cout << "\nЗавершение работы программы.\n";
 
 

 return 0;
}
