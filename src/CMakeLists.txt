add_subdirectory(database)
add_subdirectory(rpc)

add_executable(SDCS main.cpp)

target_link_libraries(SDCS 
  PRIVATE 
  database
  rpc_impl)