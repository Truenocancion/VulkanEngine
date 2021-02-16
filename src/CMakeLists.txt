set(SRCS
"main.cpp" 
"engine.h"
"engine.cpp"
"viewport.h"
"viewport.cpp"
"render.h"
"render.cpp"
"dispatch.h"
"vertex.h"
"shaders/frag.spv"
"shaders/vert.spv"
)

add_executable(VulkanEngine WIN32 ${SRCS})

set_target_properties(VulkanEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

target_link_libraries(VulkanEngine sdl)

target_include_directories(VulkanEngine PRIVATE "$ENV{VULKAN_SDK}/Include/vulkan")