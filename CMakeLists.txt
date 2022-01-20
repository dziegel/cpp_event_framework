project(cpp_event_framework)

add_executable(cpp_event_framework
    test/Statemachine_unittest.cxx
)

target_include_directories(cpp_event_framework
    PUBLIC
    include
)

target_compile_options(cpp_event_framework
    PRIVATE
    -std=c++17
)
