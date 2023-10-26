#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>

#include "database_guard.h"
#include "sdcs.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
// using helloworld::Greeter;
// using helloworld::HelloReply;
// using helloworld::HelloRequest;


// Logic and data behind the server's behavior.
class SDCSServiceImpl final : public SDCS::Service {
public:
  explicit SDCSServiceImpl(std::shared_ptr<DatabaseGuard> database)
      : database_(std::move(database)) {}

  Status RPCGet(ServerContext *context, const Key *request,
                Value *reply) override;

  Status RPCPost(ServerContext *context, const Pair *request,
                 Flag *reply) override;

  Status RPCDelete(ServerContext *context, const Key *request,
                   Flag *reply) override;

  Status RPCAddNextServer(ServerContext *context, const Flag *request,
                         Flag *reply) override;

private:
  std::shared_ptr<DatabaseGuard> database_;
};

void RunServer(uint16_t port, const std::shared_ptr<DatabaseGuard> &database);