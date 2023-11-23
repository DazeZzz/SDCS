#include <httplib.h>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>

#include "absl/flags/flag.h"
#include "client_rpc.h"
#include "server_rpc.h"

static constexpr int NOT_FOUND = 404;
static constexpr uint NUM_OF_SERVER = 3;
static const std::string PORT_FOR_RPC = ":3000";
static const std::unordered_map<int, std::string> IP_OF_SERVERS = {
    {0, "172.18.0.2"}, {1, "172.18.0.3"}, {2, "172.18.0.4"}};

ABSL_FLAG(uint16_t, port, 3000, "Server port for the service");

std::string GetLocalIP();

int main() {

  const std::string local_ip = GetLocalIP();
  std::shared_ptr database{std::make_shared<DatabaseGuard>()};

  httplib::Server svr;
  std::thread t(RunServer, absl::GetFlag(FLAGS_port), database);

  svr.Get(R"((/.*))", [&](const httplib::Request &req, httplib::Response &res) {
    std::optional value = database->Find(req.path);
    if (value) {
      res.set_content(*value, "text/plain");
      return;
    }
    for (const auto &ip : IP_OF_SERVERS) {
      if (ip.second == local_ip) {
        continue;
      }
      RPCClient cli(grpc::CreateChannel(ip.second + PORT_FOR_RPC,
                                        grpc::InsecureChannelCredentials()));
      value = cli.RPCGet(req.path);
      if (value) {
        res.set_content(*value, "text/plain");
        return;
      }
    }
    res.status = 404;
  });

  svr.Post("/", [&](const httplib::Request &req, httplib::Response &res) {
    std::string::size_type first_pos = req.body.find('"') + 1;
    std::string key =
        "/" +
        req.body.substr(first_pos, req.body.find('"', first_pos) - first_pos);

    if (database->Find(key)) {
      database->Add(key, req.body);
      std::cout << local_ip << " POST-m: " << req.body << std::endl;
      return;
    }

    for (const auto &ip : IP_OF_SERVERS) {
      if (ip.second == local_ip) {
        continue;
      }
      RPCClient cli(grpc::CreateChannel(ip.second + PORT_FOR_RPC,
                                        grpc::InsecureChannelCredentials()));
      if (cli.RPCGet(key)) {
        cli.RPCPost(key, req.body);
        std::cout << ip.second << " POST-m: " << req.body << std::endl;
        return;
      }
    }

    uint64_t next_server = database->GetNextServer() % NUM_OF_SERVER;
    std::string ip_to_post = IP_OF_SERVERS.find(next_server)->second;
    if (ip_to_post == local_ip) {
      database->Add(key, req.body);
      std::cout << local_ip << " POST: " << req.body << std::endl;
    } else {
      RPCClient cli(grpc::CreateChannel(ip_to_post + PORT_FOR_RPC,
                                        grpc::InsecureChannelCredentials()));
      cli.RPCPost(key, req.body);
      std::cout << ip_to_post << " POST: " << req.body << std::endl;
    }
    database->AddNextServer();
  });

  svr.Delete(R"((/.*))", [&](const httplib::Request &req,
                             httplib::Response &res) {
    int delete_num = database->Remove(req.path);
    if (delete_num == 0) {
      for (const auto &ip : IP_OF_SERVERS) {
        if (ip.second == local_ip) {
          continue;
        }
        RPCClient cli(grpc::CreateChannel(ip.second + PORT_FOR_RPC,
                                          grpc::InsecureChannelCredentials()));
        delete_num = cli.RPCDelete(req.path);
        if (delete_num == 1) {
          std::cout << ip.second << " DELETE: " << req.path << std::endl;
          break;
        }
      }
    } else {
      std::cout << local_ip << " DELETE: " << req.path << std::endl;
    }
    if (delete_num == 0) {
      std::cout << "DELETE nothing: " << req.path << std::endl;
    }
    res.set_content(std::to_string(delete_num), "text/plain");
  });

  svr.listen("0.0.0.0", 2000);
}

std::string GetLocalIP() {
  char hostname[128];
  gethostname(hostname, sizeof(hostname));
  struct hostent *hent;
  hent = gethostbyname(hostname);
  std::string local_ip =
      inet_ntoa(*(reinterpret_cast<struct in_addr *>(hent->h_addr)));
  std::cout << "Local IP is obtained: " << local_ip << std::endl;
  return local_ip;
}