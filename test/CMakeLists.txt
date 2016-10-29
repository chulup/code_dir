set(TEST_SOURCES 
    test_prefix_tree.cpp 
    test_vendor_tree.cpp 
    test_rates.cpp
    test_codename_tree.cpp
    test_code_directory.cpp
)

set(SPEED_TEST_SRC
    test_speed.cpp
)

#include_directories(${GTEST_INCLUDE_DIRS})

add_executable(tests ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries(tests implementation ${GTEST_BOTH_LIBRARIES})
add_test(tests tests)

add_executable(speed_test ${SPEED_TEST_SRC})
target_compile_features(speed_test PUBLIC cxx_digit_separators)
target_link_libraries(speed_test implementation ${GTEST_BOTH_LIBRARIES})
add_test(speed_test speed_test)

