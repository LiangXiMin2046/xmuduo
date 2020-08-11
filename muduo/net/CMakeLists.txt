set(net_SRCS
  Acceptor.cc
  Buffer.cc
  Channel.cc
  Connector.cc
  EventLoop.cc
  EventLoopThread.cc
  EventLoopThreadPool.cc
  InetAddress.cc
  Poller.cc
  poller/DefaultPoller.cc
  poller/PollPoller.cc
  Socket.cc
  SocketsOps.cc
  TcpConnection.cc
  TcpClient.cc
  TcpServer.cc
  Timer.cc
  TimerQueue.cc
  )

add_library(muduo_net ${net_SRCS})
link_directories(/xmuduo/muduo/base/build)
target_link_libraries(muduo_net muduo_base pthread)

install(TARGETS muduo_net DESTINATION lib)
set(HEADERS
  Accptor.h
  Buffer.h
  Channel.h
  Connector.h
  Endian.h
  EventLoop.h
  EventLoopThead.h
  EventLoopThreadPool.h
  InetAddress.h
  TcpConnection.h
  TcpServer.h
  TcpClient.h
  TimerId.h
  )
install(FILES ${HEADERS} DESTINATION include/muduo/net)

add_subdirectory(http)
add_subdirectory(inspect)
#
if(NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
#
#if(PROTOBUF_FOUND)
#  add_subdirectory(protorpc)
#else()
#  add_subdirectory(protorpc EXCLUDE_FROM_ALL)
#endif()

