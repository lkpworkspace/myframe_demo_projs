set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../)

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui)
find_package(glm)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../landlord.proto
)

set(LANDLORD_CLIENT_SRCS
    LandlordClient.cpp
    LandlordHallC.cpp
    LandlordRoomC.cpp
    PlayerC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${LANDLORD_SRCS}
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
)

add_library(landlord_client SHARED ${LANDLORD_CLIENT_SRCS})
target_link_libraries(landlord_client GL glfw GLEW glm ${PROTOBUF_LIBRARIES})
