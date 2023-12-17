  #set(BINARY ${CMAKE_PROJECT_NAME}_test)
  #set(TEST_SOURCES "gtest.cpp")
  #set(SOURCES ${TEST_SOURCES})
  #add_executable(${BINARY} ${TEST_SOURCES})
  #add_test(NAME ${BINARY} COMMAND ${BINARY})
  #target_link_libraries(${BINARY} PUBLIC gtest)

  add_executable(main "main.cpp")
  #target_link_libraries(main PUBLIC NAG::dco_cpp)
  #target_compile_definitions(main PUBLIC DCO_AUTO_SUPPORT)


  #add_executable(benchmark1 benchmark.cpp)
  #target_compile_options(benchmark1 PRIVATE -march=native)
  #target_link_libraries(benchmark1 GBench::GBench ${CMAKE_THREAD_LIBS_INIT} NAG::dco_cpp)
  #target_compile_definitions(benchmark1 PUBLIC DCO_AUTO_SUPPORT)
  #add_test(benchmark benchmark)

