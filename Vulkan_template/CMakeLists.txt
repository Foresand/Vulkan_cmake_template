cmake_minimum_required(VERSION 3.20)
project(LearnVK)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan_3party)
set(VK_SDK_PATH $ENV{VK_SDK_PATH})

#GLFW
find_package(glfw3 REQUIRED)
# GLM
add_subdirectory(${THIRDPARTY_DIR}/glm-master)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(LearnVK ${SOURCE_FILES})

include_directories(${CMAKE_SOURCE_DIR}/Tools/include)
# add_subdirectory(src)
# add_subdirectory(${CMAKE_SOURCE_DIR}/Tools)

add_library(MyLib STATIC
    ${THIRDPARTY_DIR}/lib
    Tools/source/func.cpp
    Tools/source/stdVkTemplate.cpp
)
target_include_directories(MyLib PUBLIC 
    # include
    ${VK_SDK_PATH}/Include
    # ${GLFW_INCLUDE_DIR}
    ${THIRDPARTY_DIR}/include
    ${CMAKE_SOURCE_DIR}/Tools/include
)

target_compile_definitions(LearnVK PRIVATE GLEW_STATIC)

target_include_directories(LearnVK PRIVATE
    # include
    ${VK_SDK_PATH}/Include
    ${THIRDPARTY_DIR}/include
)

target_link_directories(LearnVK PRIVATE ${THIRDPARTY_DIR}/lib)

target_link_libraries(LearnVK PRIVATE
    glm::glm
    MyLib
    ${THIRDPARTY_DIR}/lib/libglfw3dll.a  # 或libglfw3dll.a如果使用动态库
    ${THIRDPARTY_DIR}/lib/glew32s.lib
    ${VK_SDK_PATH}/Lib/vulkan-1.lib
    opengl32  # Windows OpenGL库
    gdi32     # Windows GDI库
)