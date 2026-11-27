add_executable(ee_tests
    tests_main.cpp
    test_smart_ptr.cpp
    test_eitem_tree.cpp
    test_parent_destruction.cpp
    test_remove_range.cpp
    test_matrix_scale.cpp
    test_string_util.cpp
    test_default_init.cpp)
target_link_libraries(ee_tests PRIVATE LibCommon)
target_include_directories(ee_tests PRIVATE "${PROJECT_SOURCE_DIR}/LibCommon")

if(MSVC)
    target_compile_options(ee_tests PRIVATE /utf-8 /W3)
    target_compile_definitions(ee_tests PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

set_target_properties(ee_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_test(NAME ee_tests COMMAND ee_tests)
