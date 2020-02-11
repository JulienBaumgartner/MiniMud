cmake_minimum_required(VERSION 3.14)

project(miniMUD)

if(WIN32)
	add_compile_options("/std:c++latest")
	add_compile_options("/D _WIN32_WINNT=0x0601")
endif(WIN32)
set(CMAKE_CXX_STANDARD 17)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

add_library(protobuf_mud_lib
	${CMAKE_CURRENT_SOURCE_DIR}/protobuf_mud_lib/mud_lib.pb.cc
	${CMAKE_CURRENT_SOURCE_DIR}/protobuf_mud_lib/mud_lib.pb.h
	${CMAKE_CURRENT_SOURCE_DIR}/protobuf_mud_lib/mud_lib.grpc.pb.cc
	${CMAKE_CURRENT_SOURCE_DIR}/protobuf_mud_lib/mud_lib.grpc.pb.h
)

target_link_libraries(protobuf_mud_lib
  PRIVATE
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
	cryptopp-static
)

add_library(mud_lib
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/helper.h
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/keyboard.h
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/keyboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/hash.h
	${CMAKE_CURRENT_SOURCE_DIR}/mud_lib/hash.cpp
)

target_link_libraries(mud_lib
  PUBLIC
	protobuf_mud_lib
  PRIVATE 
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
	cryptopp-static
) 


add_executable(server
	${CMAKE_CURRENT_SOURCE_DIR}/server/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/game.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/game.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server/session.h
	${CMAKE_CURRENT_SOURCE_DIR}/server/session.cpp
)

target_link_libraries(server
  PUBLIC
	protobuf_mud_lib
	mud_lib
  PRIVATE 
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
	cryptopp-static
) 

add_executable(clientCmd
	${CMAKE_CURRENT_SOURCE_DIR}/clientCmd/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/clientCmd/client.h
	${CMAKE_CURRENT_SOURCE_DIR}/clientCmd/client.cpp
)

target_link_libraries(clientCmd
  PUBLIC
	protobuf_mud_lib
	mud_lib
  PRIVATE 
	protobuf::libprotobuf
	gRPC::gpr
	gRPC::grpc
	gRPC::grpc++
	cryptopp-static
)