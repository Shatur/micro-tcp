cmake_minimum_required(VERSION 2.8)

project(micro-tcp)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE ${SOURCE}
    main.cpp
    server.cpp
    client.cpp
    )
set(HEADERS ${HEADERS}
   server.h
   client.h
)

add_executable(${PROJECT_NAME} ${SOURCE} ${HEADERS})

target_link_libraries(${PROJECT_NAME} stdc++fs)
