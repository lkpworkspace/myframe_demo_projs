cmake_minimum_required(VERSION 2.8)

include_directories(${PROJECT_SOURCE_DIR}/myframe/myframe)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/landlord_common)
set(LANDLORD_SRCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/landlord_common/Poker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/landlord_common/PlayPoker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/landlord_common/LandlordUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/landlord_common/IPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/landlord_common/ILandlordRoom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/landlord_common/ILandlordHall.cpp
)

add_subdirectory(landlord_server)
add_subdirectory(landlord_client)

