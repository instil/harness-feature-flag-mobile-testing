if(FOR_ANDROID)
  MESSAGE(STATUS "Will Build JNI Bindings: " ${BUILD_JNI_WRAPPER})
  if(BUILD_JNI_WRAPPER)
    set(JNI_SRCS
      ./jni/interface_wrap.cpp
      ./jni/interface_wrap.h
      )
  endif()
  
  MESSAGE(STATUS "Will Build Xamarin android bindings: " ${BUILD_XAMARIN_ANDROID_WRAPPER})
  if(BUILD_XAMARIN_ANDROID_WRAPPER)
    set(XAMARIN_ANDROID_SRCS
      ./xamarin-android/interface_wrap.cpp
      ./xamarin-android/interface_wrap.h
      )
  endif()

  # this will cause crashes on android - swig generates old style pointer code for DIRECTORS
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -Wunused-function")
else()
  set(XAMARIN_ANDROID_SRCS)
  set(JNI_SRCS)
endif()

MESSAGE(STATUS "Will Build JNI Jar archive: " ${BUILD_JAR})
if(BUILD_JAR)
  FILE(GLOB source "./jni/*.java")
  add_jar(VideoEdgeRtsp ${source})
endif()

file(GLOB SURGE_UTIL_SRCS
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
  Sleep.cc
  Sleep.h
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
  )

if(BUILD_JAR STREQUAL OFF)
  add_library(surge ${LIBRARY_TYPE}

    RtspClient.h
    RtspClient.cc

    SocketHandler.h
    SocketHandler.cc

    RtspCommand.h
    RtspCommandFactory.h
    Describe.h

    Response.h
    
    # surge util
    ${SURGE_UTIL_SRCS}

    # Jni
    ${JNI_SRCS}
    
    # xamarin-android
    ${XAMARIN_ANDROID_SRCS}

    # xamarin-ios
    ${XAMARIN_IOS_SRCS}
  )

endif()
