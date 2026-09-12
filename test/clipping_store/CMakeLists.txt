add_executable(ClippingStoreTest
  ClippingStoreTest.cpp
  ${REPO_ROOT}/src/ClippingStore.cpp
  ${REPO_ROOT}/lib/Utf8/Utf8.cpp
)
target_include_directories(ClippingStoreTest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${REPO_ROOT}/test/shims
  ${REPO_ROOT}/src
  ${REPO_ROOT}/lib/Serialization
  ${REPO_ROOT}/lib/Utf8
)
target_link_libraries(ClippingStoreTest PRIVATE crosspoint_test_common GTest::gtest_main)
gtest_discover_tests(ClippingStoreTest)

add_executable(GlobalBookmarkIndexTest GlobalBookmarkIndexTest.cpp ${REPO_ROOT}/src/GlobalBookmarkIndex.cpp)
target_include_directories(GlobalBookmarkIndexTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${REPO_ROOT}/test/shims ${REPO_ROOT}/src)
target_link_libraries(GlobalBookmarkIndexTest PRIVATE crosspoint_test_common GTest::gtest_main)
gtest_discover_tests(GlobalBookmarkIndexTest)
