set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../landlord.proto
)

add_library(landlord_server SHARED LandlordService.cpp LandlordHall.cpp LandlordRoom.cpp Player.cpp ${LANDLORD_SRCS} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(landlord_server ${PROTOBUF_LIBRARIES})

