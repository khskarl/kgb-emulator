cmake_minimum_required(VERSION 3.0.2)
project(gameboy-emulator)


# build options
set(TARGET_OS "Linux" CACHE STRING "Specifies your target OS for build config")
set(TARGET_CPU "native" CACHE STRING "Specifies the target CPU architecture for -march option")

option(ASAN OFF)
option(USAN OFF)
option(ENABLE_LTO OFF)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wshadow\
	-Wstrict-aliasing")

set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -Werror -fstrict-aliasing \
	-march=${TARGET_CPU} -fomit-frame-pointer -ffunction-sections \
	-fdata-sections")

set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -DDEBUG -O1 -g3")


if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, defaulted to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()

if (${USAN} OR ${ASAN} AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	message(FATAL_ERROR "ASAN and USAN cannot be used on ${CMAKE_BUILD_TYPE} build type")
endif()

# "Release" full optimization , no debug info.
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")

	if(ENABLE_LTO)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
		set(LINK_FLAGS "${LINK_FLAGS} -flto")
	endif()

	set(CMAKE_EXE_LINKER_FLAGS "${LINK_FLAGS}")
	set(CMAKE_SHARED_LINKER_FLAGS "${LINK_FLAGS}")
	set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${LINK_FLAGS}")
	set(CMAKE_STATIC_LIBRARY_LINK_CXX_FLAGS "${LINK_FLAGS}")
# "Debug" full debug information, no optimization, asserts enabled
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	if (${ASAN})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	endif()

	if (${USAN})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
	endif()
endif()

# gbx src directory
set(SRC_DIR "./src")
set(3RD_DIR "./3rdparty")
# include src directory
include_directories("${SRC_DIR}" "${3RD_DIR}")


# build
file(GLOB SRC_FILES "${SRC_DIR}/*.cpp" "${SRC_DIR}/context/*.cpp" "${3RD_DIR}/imgui/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} -lsfml-system -lsfml-window -lsfml-graphics -lGL)
