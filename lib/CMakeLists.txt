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
	SGM_Specification_lib STATIC ${SRC_FILES} ${INCLUDE_FILES}
)

target_include_directories(SGM_Specification_lib PUBLIC ${INCLUDE_DIR})
target_compile_features(SGM_Specification_lib PUBLIC cxx_std_17)