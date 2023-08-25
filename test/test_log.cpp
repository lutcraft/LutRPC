
#include <pthread.h>
#include "code/comm/log.h"

void *fun(void *)
{

  int i = 20;
  while (i--)
  {
    DEBUGLOG("debug this is thread in %s - %d", "fun\n", i);
    INFOLOG("info this is thread in %s - %d", "fun\n", i);
  }

  return NULL;
}

int main()
{

  lutrpc::Logger::InitGlobalLogger();

  pthread_t thread;
  // 启动辅线程，测试日志打印
  pthread_create(&thread, NULL, &fun, NULL);

  int i = 20;
  // 主线程中，测试日志打印
  while (i--)
  {
    DEBUGLOG("test debug log %s - %d\n", "main", i);
    INFOLOG("test info log %s- %d\n", "main", i);
  }
  // 主线程等待辅线程返回，再结束，防止辅线程被误杀
  pthread_join(thread, NULL);
  return 0;
}