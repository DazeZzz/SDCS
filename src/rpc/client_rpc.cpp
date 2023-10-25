#include "client_rpc.h"

std::optional<std::string> RPCClient::RPCGet(const std::string &key) {
  Key request;
  request.set_key(key);
  Value reply;
  ClientContext context;
  Status status = stub_->RPCGet(&context, request, &reply);
  if (status.ok()) {
    if (reply.has_value()) {
      return reply.value();
    }
    return std::nullopt;
  }
  std::cout << status.error_code() << ": " << status.error_message()
            << std::endl;
  return "RPC failed";
}

std::string RPCClient::RPCPost(const std::string &key,
                               const std::string &value) {
  Pair request;
  request.set_key(key);
  request.set_value(value);
  Flag reply;
  ClientContext context;
  Status status = stub_->RPCPost(&context, request, &reply);
  if (status.ok()) {
    return "RPC successed";
  }
  std::cout << status.error_code() << ": " << status.error_message()
            << std::endl;
  return "RPC failed";
}

int32_t RPCClient::RPCDelete(const std::string &key) {
  Key request;
  request.set_key(key);
  Flag reply;
  ClientContext context;
  Status status = stub_->RPCDelete(&context, request, &reply);
  if (status.ok()) {
    return reply.flag();
  }
  std::cout << status.error_code() << ": " << status.error_message()
            << std::endl;
  return -1;
}

std::string RPCClient::RPCAddNextSever() {
  Flag request;
  Flag reply;
  ClientContext context;
  Status status = stub_->RPCAddNextSever(&context, request, &reply);
  if (status.ok()) {
    return "RPC successed";
  }
  std::cout << status.error_code() << ": " << status.error_message()
            << std::endl;
  return "RPC failed";
}