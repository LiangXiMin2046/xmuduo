# 简介
muduo 是一个基于 Reactor 模式的现代 C++ 网络库，作者[@陈硕](https://github.com/chenshuo)。它采用非阻塞 IO 模型，基于事件驱动和回调，原生支持多核多线程，适合编写 Linux 服务端多线程网络应用程序。muduo网络库的核心代码只有数千行，在网络编程技术学习的进阶阶段，muduo是一个非常值得学习的开源库。目前我也是刚刚开始学习这个网络库的源码，希望将这个学习过程记录下来。这个网络库的源码已经发布在GitHub上，可以点击[这里](https://github.com/chenshuo/muduo)阅读。
目前Github上这份源码已经被作者用c++11重写，我学习的这个版本是没有使用c++11版本的。二者大同小异，核心思想是没有变化的，为了表示区别，添加了一个x。
代码详细说明在我的csdn博客[liangximin2046](https://blog.csdn.net/MoonWisher_liang)

# muduo学习基础
在你学习muduo源码之前，一定要确保具备以下基础
```
c/c++编程基础
操作系统与计算机网络知识
linux操作系统编程与网络编程，至少明白并发式echo server的写法
```
