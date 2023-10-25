#include "sever_rpc.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <iostream>
#include <optional>
#include <string>

Status SDCSServiceImpl::RPCGet(ServerContext *context, const Key *request,
                               Value *reply) {
  std::cout << "RPC GET from other sever" << std::endl;
  std::optional value = database_->Find(request->key());
  if (value) {
    reply->set_value(*value);
  }
  return Status::OK;
}

Status SDCSServiceImpl::RPCPost(ServerContext *context, const Pair *request,
                                Flag *reply) {
  std::cout << "RPC POST from other sever" << std::endl;
  database_->Add(request->key(), request->value());
  return Status::OK;
}

Status SDCSServiceImpl::RPCDelete(ServerContext *context, const Key *request,
                                  Flag *reply) {
  std::cout << "RPC DELETE from other sever" << std::endl;
  int32_t num = database_->Remove(request->key());
  reply->set_flag(num);
  return Status::OK;
}

Status SDCSServiceImpl::RPCAddNextSever(ServerContext *context,
                                        const Flag *request, Flag *reply) {
  std::cout << "RPC AddNextSever from other sever" << std::endl;
  database_->AddNextSever();
  return Status::OK;
}

void RunServer(uint16_t port, const std::shared_ptr<DatabaseGuard> &database) {
  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  SDCSServiceImpl service{database};

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "RPC server is listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}