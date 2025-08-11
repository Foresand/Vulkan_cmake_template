cmake_minimum_required(VERSION 3.20)
project(LearnVK)

# set parameters
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan_3party)
set(VK_SDK_PATH $ENV{VK_SDK_PATH})

# add GLMsubdirectory
add_subdirectory(${THIRDPARTY_DIR}/glm-master)

# add excutable object
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(LearnVK ${SOURCE_FILES})

# define GLEW_STATIC macro
target_compile_definitions(LearnVK PRIVATE GLEW_STATIC)

# include directories
target_include_directories(LearnVK PRIVATE
    include
    ${VK_SDK_PATH}/Include
    ${THIRDPARTY_DIR}/include
)

# 链接目录设置
target_link_directories(LearnVK PRIVATE ${THIRDPARTY_DIR}/lib)

# 关键链接配置
target_link_libraries(LearnVK PRIVATE
    glm::glm
    ${THIRDPARTY_DIR}/lib/libglfw3.a  # 或libglfw3dll.a如果使用动态库
    ${THIRDPARTY_DIR}/lib/glew32s.lib
    ${VK_SDK_PATH}/Lib/vulkan-1.lib
    opengl32  # Windows OpenGL库
    gdi32     # Windows GDI库
)