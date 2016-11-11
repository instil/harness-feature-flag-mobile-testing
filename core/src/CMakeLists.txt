set(JNI_SRCS)

add_library(surge ${LIBRARY_TYPE}
  Surge.h
  Surge.cc

  RtspClient.h
  RtspClient.cc
  IRtspClientDelegate.h

  InterleavedRtspTransport.h
  InterleavedRtspTransport.cc
  UdpTransport.h
  UdpTransport.cc

  Transport.h
  ISocketHandlerDelegate.h
  ITransportInterface.h

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

  BitstreamReader.h
  BitstreamReader.cc
  BitstreamWriter.h
  BitstreamWriter.cc

  Depacketizer.h

  H264Depacketizer.h
  H264Depacketizer.cc

  MP4VDepacketizer.h
  MP4VDepacketizer.cc

  MJPEGDepacketizer.h
  MJPEGDepacketizer.cc

  MimeTypes.h
  ILoggingDelegate.h
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

  # RxCpp
  ${RX_CPP_SRCS}
)

target_link_libraries(surge uv)
