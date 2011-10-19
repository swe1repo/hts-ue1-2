CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(HTS-UE1 CXX)

ADD_DEFINITIONS(-O0 -Werror -Wall -std=c++0x -DVERBOSE)

FILE(GLOB SERVER_SOURCES "src/*")
FILE(GLOB MSG_SOURCES "src/messages/*")
FILE(GLOB CLIENT_SOURCES "src/client/*")
FILE(GLOB RSP_SOURCES "src/client/responses/*")

ADD_EXECUTABLE(Server ${SERVER_SOURCES} ${MSG_SOURCES})
ADD_EXECUTABLE(Client ${CLIENT_SOURCES} ${MSG_SOURCES} ${RSP_SOURCES}
		       src/Logging.h src/MessageParser.h src/MessageParser.cpp src/NetworkException.h src/readline.h src/readline.cpp)

SET_TARGET_PROPERTIES(Server PROPERTIES DEFINE_SYMBOL VERBOSE)

TARGET_LINK_LIBRARIES(Server boost_filesystem)
TARGET_LINK_LIBRARIES(Server boost_thread)

TARGET_LINK_LIBRARIES(Client boost_thread)