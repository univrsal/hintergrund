cmake_minimum_required(VERSION 2.8)

project(hintergrund-cli)
add_executable(${PROJECT_NAME} "main.cpp"
    "rule.cpp"
    "rule_date.cpp"
    "rule_time_span.cpp"
    "rule_weekday.cpp"
    "rule_date_span.cpp"
    "rule_month_span.cpp"
    "rule_io.cpp"
    "rule_io_file.cpp"
    "image_library.cpp"
    "rule_set.cpp")
