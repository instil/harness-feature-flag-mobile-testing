include_directories(
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/test"
)

add_executable(testrunner
  testrunner.cc

  ScratchTest.cc
  UrlTests.cc
  RtspResponseTests.cc
  SessionDescriptionV0Tests.cc
  DescribeResponseTests.cc
  SetupResponseTests.cc

  TestUtil.cc
  TestUtil.h
  NaluParser.h
)

target_link_libraries(testrunner
  surge
  gtest
  gmock
)

add_test(NAME testrunner
  COMMAND testrunner
)
