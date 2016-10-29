set(CD_SOURCES 
    code_directory.cpp
    codename_tree.cpp
)
set(CD_HEADERS 
    code_directory.h
    codename_tree.h
    codename.h
    prefix_tree.h
    rate.h
    types.h
    vendor_tree.h
    visit_stats.h
)

add_library(implementation ${CD_SOURCES} ${CD_HEADERS})
target_link_libraries(implementation
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
target_compile_definitions(implementation PUBLIC BOOST_LOG_DYN_LINK)
target_compile_features(implementation PUBLIC cxx_range_for)

add_executable(code-directory main.cpp)
target_link_libraries(code-directory
    implementation
)
