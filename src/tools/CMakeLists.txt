cmake_minimum_required(VERSION 3.9.3)

project(powderbase_file_viewer)
include_directories(
	"../api/"
)
set(SOURCES
	file_viewer.cpp
)
add_executable(
	powderbase_file_viewer ${SOURCES}
)
target_link_libraries(powderbase_file_viewer powderbase_api)


project(powderbase_perf)
include_directories(
	"../api/"
)
set(SOURCES
	perf.cpp
)
add_executable(
	powderbase_perf ${SOURCES}
)
target_link_libraries(powderbase_perf powderbase_api)


project(powderbase_sample)
include_directories(
	"../api/"
)
set(SOURCES
	sample.cpp
)
add_executable(
	powderbase_sample ${SOURCES}
)
target_link_libraries(powderbase_sample powderbase_api)

project(powderbase_test)
include_directories(
	"../api/"
)
set(SOURCES
	test.cpp
)
add_executable(
	powderbase_test ${SOURCES}
)
target_link_libraries(powderbase_test powderbase_api)