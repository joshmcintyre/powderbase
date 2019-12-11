cmake_minimum_required(VERSION 3.9.3)

project(powderbase_api)

set(HEADERS
	DB.h
)

file(
	GLOB SOURCES 
	*.cpp
)

add_library(powderbase_api
	STATIC
	${HEADERS}
	${SOURCES}
)