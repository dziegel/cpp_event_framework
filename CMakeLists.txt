cmake_minimum_required(VERSION 3.18)

project(cpp_event_framework)

add_executable(cpp_event_framework
    examples/SimpleStatemachineExample.cxx

    test/Statemachine_unittest.cxx
    test/Events_unittest.cxx
    test/main.cxx
)

target_link_options(cpp_event_framework
    PRIVATE
    -Xlinker -Map=map.txt
)

target_include_directories(cpp_event_framework
    PUBLIC
    include
)

target_compile_options(cpp_event_framework
    PRIVATE
    -std=c++17

    -Werror
    -pedantic
    -Wall
    -Wextra

    -Wcast-qual
    -Wwrite-strings
    -Wtrampolines
    -Wvla
    -Wshadow
    -Wmissing-include-dirs
    -Wfloat-equal
    -Wundef
    -Wcast-align

# Gnu specific warnings
#   for all versions of GCC
    $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>

#   for GCC version >= 7
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,7>>:-Walloc-zero>
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,7>>:-Wduplicated-branches>

#   for GCC version >= 8
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,8>>:-Wcast-align=strict>

    # for GCC version >= 10
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wextra-semi>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wcatch-value>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wzero-as-null-pointer-constant>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wstrict-null-sentinel>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wvirtual-inheritance>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wmismatched-tags>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wctor-dtor-privacy>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wsuggest-override>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wold-style-cast>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,10>>:-Wmultiple-inheritance>
)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc)
    doxygen_add_docs(cpp_event_framework_docs
        include/cpp_event_framework
        ALL
    )
endif()
