# LutRPC
一个C++实现的简单的RPC原型




## 更新日志

### 2023/8/14
多个线程共用一个全局的logger，记录日志时，logger将logevent对象放入队列，需要打印日志时，将队列中的对象全部打印到终端。


### 2023/8/25
采用半同步/半异步架构实现多进程事件处理模型，主线程只负责监听socket，当事件发生后，将客户端socket交给从线程处理，由从线程进行socket的接收，数据读取，业务处理。

### 2023/10/24
完成反应堆模式框架，实现唤醒机制。


### 2023/10/31
完成定时器机制
### 2023/11/20
开始开发IO线程，此特性主要服务与以下场景：
    当主反应堆需要进行IO操作时，可以调用thread模块接口，创建IO线程完成读写，而不影响主反应堆的继续执行，实现IO流程异步，IO读写完成的后处理可以由IO线程自己继续完成，也可以注册到主反应堆上处理。
因为IO线程与主线程是分开调度，IO线程需要的创建需要与主线程同步，防止出现IO线程初始化没有结束，主线程已经对其进行使用的情况。
本特性开发没有使用C++11标准库中定义的<thread>模块，而使用了C-type的pthread模块。
