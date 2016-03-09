if(FOR_ANDROID)
  MESSAGE(STATUS "Will Build JNI Bindings: " ${BUILD_JNI_WRAPPER})
  
  if(BUILD_JNI_WRAPPER)
    set(JNI_SRCS
      ./wrappers/java/interface_wrap.cpp
      ./wrappers/java/interface_wrap.h
      )
  endif()

  # this will cause crashes on android - swig generates old style pointer code for DIRECTORS
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -Wunused-function")
else()
  set(JNI_SRCS)
endif()

MESSAGE(STATUS "Will Build JNI Jar archive: " ${BUILD_JAR})
if(BUILD_JAR)
  FILE(GLOB source "./wrappers/java/*.java")
  add_jar(Surge ${source})
endif()

if(BUILD_JAR STREQUAL OFF)
  add_library(surge ${LIBRARY_TYPE}

    Surge.h
    Surge.cc
    
    RtspClient.h
    RtspClient.cc

    SocketHandler.h
    SocketHandler.cc

    RtspCommand.h
    RtspCommandFactory.h
    Describe.h
    Setup.h
    Options.h

    Response.h
    RtspResponse.h
    DescribeResponse.h
    SetupResponse.h

    SessionDescription.h
    SessionDescriptionFactory.h
    SessionDescriptionFactory.cc

    RtpPacket.h
    RtpPacket.cc

    H264Depacetizer.h
    H264Depacetizer.cc

    BaseWaitableEvent.h
    BasicFDEvent.h
    DateTime.cc
    DateTime.h
    FireableEvent.cc
    FireableEvent.h
    Logging.h
    Mutex.cc
    Mutex.h
    MutexLocker.h
    StoppableThread.h
    Thread.cc
    Thread.h
    WaitableEvent.h
    WaitableEvents.cc
    Base64.cc
    Base64.h
    Url.h
    Helpers.h
    Helpers.cc
    DataEventQueue.h
    ErrorDispatcher.h
    ErrorDispatcher.cc

    # Jni
    ${JNI_SRCS}
  )

endif()
