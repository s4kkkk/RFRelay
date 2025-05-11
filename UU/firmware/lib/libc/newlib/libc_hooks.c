/**
 * @file 
 * @brief Заглушки для системных вызовов newlib
 *
 * @author Stanislav Timoshko <s4kkkk@mail.ru>
 */

#include <errno.h>
#include <unistd.h>
#undef errno
extern int errno;
#include <sys/stat.h>
#include <stm32f1xx.h>

char *__env[1] = { 0 };
char **environ = __env;

__attribute__((noreturn))
void stub() 
{
        while(1);
}

void _exit(int outcode) 
{
    stub();
}

int _close(int file) 
{
  return -1;
}

int _execve(char *name, char **argv, char **env) 
{
  errno = ENOMEM;
  return -1;
}

int _fork(void) 
{
  errno = EAGAIN;
  return -1;
}

int _fstat(int file, struct stat *st) 
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _getpid(void) 
{
  return 1;
}

int _isatty(int file) 
{
  return 1;
}

int _kill(int pid, int sig) 
{
  errno = EINVAL;
  return -1;
}

int _link(char *old, char *neww) 
{
  errno = EMLINK;
  return -1;
}

int _lseek(int file, int ptr, int dir) 
{
  return 0;
}

int _open(const char *name, int flags, int mode) 
{
  return -1;
}

// чтение из файла. USART..?

int _read(int file, char *ptr, int len) 
{ 
  return 0;
}

void * _sbrk(ptrdiff_t __incr) 
{
  extern uint8_t _end;
  static uint8_t *current_heap_end = &_end;
  
  if((uint32_t)(current_heap_end + __incr) <  __get_MSP()) {
    uint8_t *previous_heap_end = current_heap_end;
    current_heap_end += __incr;
    return (void *) previous_heap_end;
  }

  else {
    errno = ENOMEM;
    return ((void *) -1);
  }
}

int _stat(const char *file, struct stat *st) 
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _times(void *buf) 
{
  return -1;
}

int _unlink(char *name) 
{
  errno = ENOENT;
  return -1; 
}

int _wait(int *status) 
{
  errno = ECHILD;
  return -1;
}

// запись в файл. USART..?

int _write(int file, char *ptr, int len) 
{
  return 0;
}
