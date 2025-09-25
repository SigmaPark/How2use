#	SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#	SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========

if(MSVC)
	add_definitions(-D_WINDOWS_SOLUTION_)

	add_compile_options(/Zc:__cplusplus)
	add_definitions(-DUNICODE)
	add_definitions(-D_UNICODE)

endif()

if(UNIX)
	add_definitions(-D_POSIX_SOLUTION_)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

file(
	GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_executable(How2use_guide ${SRC_FILES})

target_link_libraries(How2use_guide PRIVATE How2use_lib)

target_compile_options(
	How2use_guide PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:
		/permissive-; /W4; /WX;
	>
)