set(JNI_SRCS)

file(GLOB surge_sources *.cc)
file(GLOB surge_headers *.h)

add_library(surge ${LIBRARY_TYPE} ${surge_sources} ${surge_headers})

target_link_libraries(surge uv ${OpenSSL})
