cmake_minimum_required(VERSION 3.18)

project(cpp_event_framework)

include(GNUInstallDirs REQUIRED)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "dirk@ziegelmeier.net")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++20 event framework")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-header-filter=.*;-extra-arg=-Wno-unknown-warning-option;-checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*,-bugprone*,-clang-analyzer-cplusplus*,-cert*,-cppcoreguidelines*,-readability*,-modernize*,-portability*,-performance*")

# find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
# if(CMAKE_CXX_CPPCHECK)
# 	list(APPEND CMAKE_CXX_CPPCHECK "--enable=all")
# 	list(APPEND CMAKE_CXX_CPPCHECK "-I${CMAKE_CURRENT_SOURCE_DIR}/include")
# endif()

# add_custom_target(check ALL cppcheck --enable=all --project=${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
#     DEPENDS ${CMAKE_PROJECT_NAME}
#     COMMENT cppcheck
# )

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(${CMAKE_PROJECT_NAME}
    examples/activeobject/Fsm.cxx
    examples/activeobject/FsmImpl.cxx

    examples/interface/Fsm.cxx
    examples/interface/FsmImpl.cxx

    examples/pimpl/Fsm.cxx
    examples/pimpl/FsmImpl.cxx

    examples/plain/FsmImpl.cxx

    examples/signals/Fsm.cxx
    examples/signals/FsmImpl.cxx

    test/ActiveObjectFramework_unittest.cxx
    test/Statemachine_unittest.cxx
    test/Events_unittest.cxx
    test/main.cxx
)

target_link_options(${CMAKE_PROJECT_NAME}
    PRIVATE
    -Xlinker -Map=map.txt
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC
    include
)

target_compile_options(${CMAKE_PROJECT_NAME}
    PRIVATE
    -std=c++20

    -Werror
    -pedantic
    -Wall
    -Wextra

    -Wconversion
    -Wsign-conversion
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
)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc)
    doxygen_add_docs(${CMAKE_PROJECT_NAME}_docs
        include
        ALL
    )
endif()

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY doc/html/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/
)

include(CPack)

