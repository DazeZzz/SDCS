include("${PROJECT_SOURCE_DIR}/cmake/find_Protobuf_and_gRPC.cmake")

get_filename_component(sdcs_proto "${PROJECT_SOURCE_DIR}/src/rpc/proto/sdcs.proto" ABSOLUTE)
get_filename_component(sdcs_proto_path "${sdcs_proto}" PATH)

set(sdcs_proto_srcs "${PROJECT_BINARY_DIR}/sdcs.pb.cc")
set(sdcs_proto_hdrs "${PROJECT_BINARY_DIR}/sdcs.pb.h")
set(sdcs_grpc_srcs "${PROJECT_BINARY_DIR}/sdcs.grpc.pb.cc")
set(sdcs_grpc_hdrs "${PROJECT_BINARY_DIR}/sdcs.grpc.pb.h")

add_custom_command(
      OUTPUT "${sdcs_proto_srcs}" "${sdcs_proto_hdrs}" "${sdcs_grpc_srcs}" "${sdcs_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${PROJECT_BINARY_DIR}"
        --cpp_out "${PROJECT_BINARY_DIR}"
        -I "${sdcs_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${sdcs_proto}"
      DEPENDS "${sdcs_proto}")

add_library(sdcs_grpc_proto
  ${sdcs_grpc_srcs}
  ${sdcs_grpc_hdrs}
  ${sdcs_proto_srcs}
  ${sdcs_proto_hdrs})

target_link_libraries(sdcs_grpc_proto 
  PUBLIC
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})