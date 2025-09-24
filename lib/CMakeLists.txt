#	SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#	SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

file(
	GLOB_RECURSE INCLUDE_FILES CONFIGURE_DEPENDS
	${INCLUDE_DIR}/*.hpp
)

file(
	GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_library(
	How2use_lib STATIC ${SRC_FILES} ${INCLUDE_FILES}
)

target_include_directories(How2use_lib PUBLIC ${INCLUDE_DIR})

target_compile_options(
	How2use_lib PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:
		/permissive-; /W4; /WX;
	>
)

target_compile_definitions(
	How2use_lib PRIVATE
	_CRT_SECURE_NO_WARNINGS
)