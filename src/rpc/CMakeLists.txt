add_subdirectory(proto)

add_library(rpc_impl 
  client_rpc.cpp
  server_rpc.cpp)

target_link_libraries(rpc_impl 
  PUBLIC
  sdcs_grpc_proto
)