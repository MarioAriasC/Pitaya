set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

add_executable(Boost_Tests_run
        parser_tests.cpp
        lexer_tests.cpp)
target_link_libraries(Boost_Tests_run ${Boost_LIBRARIES})
target_link_libraries(Boost_Tests_run Pitaya_lib)