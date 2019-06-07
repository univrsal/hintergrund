cmake_minimum_required(VERSION 2.8)

project(hintergrund-cli)
find_path(JANSSON_INCLUDE_DIRS jansson.h)
find_library(JANSSON_LIBRARIES jansson)

string(TIMESTAMP TODAY "%Y.%m.%d %H:%M")
add_definitions(-DTIMESTAMP="${TODAY}")

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_definitions(-DLINUX=1)
    add_definitions(-DUNIX=1)
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_executable(${PROJECT_NAME}
        main.cpp
        src/rules/rule.cpp
        src/rules/rule_date.cpp
        src/rules/rule_time_span.cpp
        src/rules/rule_weekday.cpp
        src/rules/rule_date_span.cpp
        src/rules/rule_month_span.cpp
        src/rules/rule_io.cpp
        src/rules/rule_io_file.cpp
        src/rules/rule_set.cpp
        src/rules/rule_io_stdin.cpp
        src/images/image_library.cpp
        src/images/image.cpp
        src/tagging/tag.cpp
        src/tagging/tagger.cpp
        src/util/util.cpp
        src/util/config.cpp
        src/hintergrund_cli.cpp
        src/util/argument_parser.cpp
        )

include_directories(${JANSSON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${JANSSON_LIBRARIES})
