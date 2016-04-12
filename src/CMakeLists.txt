set(JNI_SRCS)

if(FOR_ANDROID)
  MESSAGE(STATUS "Will Build JNI Bindings: " ${BUILD_JNI_WRAPPER})
  
  if(BUILD_JNI_WRAPPER)
    set(JNI_SRCS
      ./wrappers/java/interface_wrap.cc
      ./wrappers/java/interface_wrap.h
      ./wrappers/java/RtspClientProxy.h
      ./wrappers/java/RtspClientProxy.cc
      )
  endif()

  # this will cause crashes on android - swig generates old style pointer code for DIRECTORS
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -Wunused-function")
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
    SocketHandlerDelegate.h

    RtspCommand.h
    RtspCommandFactory.h
    RtspCommandFactory.cc
    Describe.h
    Setup.h
    Pause.h
    Play.h
    Options.h
    Teardown.h

    Response.h
    RtspResponse.h
    DescribeResponse.h
    SetupResponse.h

    SessionDescription.h
    SessionDescriptionV0.h
    SessionDescriptionV0.cc
    SessionDescriptionFactory.h
    SessionDescriptionFactory.cc

    ResponseFactory.h

    RtpPacket.h
    RtpPacket.cc

    H264Depacetizer.h
    H264Depacetizer.cc

    MP4VDepacketizer.h
    MP4VDepacketizer.cc
    
    MJPEGDepacketizer.h
    MJPEGDepacketizer.cc

    Transport.h
    ITransportInterface.h

    MimeTypes.h
    LoggingDelegate.h
    DelegateInterface.h
    Constants.h
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
