cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

project(dummy NONE)
  if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_CXX_COMPILER clang++)
  endif()
set(PROJECT_NAME easy-gl)
project(${PROJECT_NAME})

set(PROJECTS_DIR_NAME projects)
include_directories(external)
include_directories(include)
include_directories(base)

# Compile mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO, "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g -pg -O3")
set(CMAKE_CXX_FLAGS_RELEASE, "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O4 -flto")
set(CMAKE_CXX_FLAGS_MINSIZEREL, "${CMAKE_CXX_FLAGS_MINSIZEREL} -DNDEBUG -Oz")


find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
    message(FATAL_ERROR "Could not find opengl!")
else()
    include_directories(${OPENGL_LIBRARIES})
endif()

find_package(GLEW REQUIRED)
if (NOT GLEW_FOUND)
    message(FATAL_ERROR "Could not find glew!")
else()
    include_directories(${GLEW_INCLUDE_DIR})
endif()

find_package(GLFW REQUIRED)
if (NOT GLFW_FOUND)
    message(FATAL_ERROR "Could not find glfw!")
else()
    include_directories(${GLFW_INCLUDE_DIR})
endif()

find_package(ASSIMP REQUIRED)
if (NOT ASSIMP_FOUND)
    message(FATAL_ERROR "Could not find assimp!")
else()
    include_directories(${ASSIMP_INCLUDE_DIR})
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS chrono program_options system filesystem thread date_time regex serialization REQUIRED)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost!")
else()
    message("@@ Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message("@@ Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Set preprocessor defines
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")

# Warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wundef")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-macros -Wno-gnu-anonymous-struct -Wno-nested-anon-types") # for GLM
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reserved-id-macro -Wno-old-style-cast") # for GLEW
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation -Wno-documentation-unknown-command")    # for GLFW
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant -Wno-sign-conversion -Wno-disabled-macro-expansion -Wno-undefined-func-template -Wno-deprecated-dynamic-exception-spec -Wno-undef") # for Boost
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-signed-enum-bitfield -Wno-shorten-64-to-32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables -Wno-covered-switch-default")
endif()

add_definitions(-std=c++17)
file(GLOB SOURCE *.cpp base/*.cpp)

# Function for building
function(build TARGET_NAME)
    # Main
    file(GLOB SOURCE *.cpp base/*.cpp ${PROJECTS_DIR_NAME}/${TARGET_NAME}/*.cpp)
    set(MAIN_CPP ${TARGET_NAME}/${TARGET_NAME}/main.cpp)
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECTS_DIR_NAME}/${TARGET_NAME}/main.cpp)
        set(MAIN_CPP ${PROJECTS_DIR_NAME}/${TARGET_NAME}/main.cpp)
    endif()

    add_executable(${TARGET_NAME}
        ${MAIN_CPP}
        ${SOURCE}
    )
    target_link_libraries(${TARGET_NAME} 
        ${OPENGL_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${GLEW_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${ASSIMP_LIBRARY}
        ${Boost_LIBRARIES}
    )
endfunction(build)


# Build All
function(buildAll)
    foreach(TARGET ${TARGETS})
        build(${TARGET})
    endforeach(TARGET)
endfunction(buildAll)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")
set(TARGETS
    hello
    bezier
    #particles
)
buildAll()
