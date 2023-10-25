#pragma once

#include <grpcpp/grpcpp.h>
#include <optional>

#include "sdcs.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class RPCClient {
public:
  explicit RPCClient(const std::shared_ptr<Channel> &channel)
      : stub_(SDCS::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.

  std::optional<std::string> RPCGet(const std::string &key);

  std::string RPCPost(const std::string &key, const std::string &value);

  int32_t RPCDelete(const std::string &key);

  std::string RPCAddNextSever();

private:
  std::unique_ptr<SDCS::Stub> stub_;
};
