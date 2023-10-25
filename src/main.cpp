#include <httplib.h>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>

#include "absl/flags/flag.h"
#include "client_rpc.h"
#include "sever_rpc.h"

static constexpr int NOT_FOUND = 404;
static constexpr uint NUM_OF_SEVER = 3;
static const std::string PORT_FOR_RPC = ":3000";
static const std::unordered_map<int, std::string> IP_OF_SEVERS = {
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
      std::cout << "HTTP GET from local ip" << std::endl;
      return;
    }
    for (const auto &ip : IP_OF_SEVERS) {
      if (ip.second == local_ip) {
        continue;
      }
      RPCClient cli(grpc::CreateChannel(ip.second + PORT_FOR_RPC,
                                        grpc::InsecureChannelCredentials()));
      value = cli.RPCGet(req.path);
      if (value) {
        res.set_content(*value, "text/plain");
        std::cout << "HTTP GET is forwarded to the server: " << ip.second
                  << std::endl;
        return;
      }
    }
    res.status = 404;
  });

  svr.Post("/", [&](const httplib::Request &req, httplib::Response &res) {
    std::string key = "/" + req.body.substr(2, req.body.find(':') - 3);

    if (database->Find(key)) {
      database->Add(key, req.body);
      std::cout << "HTTP POST to modify existing data locally" << std::endl;
      return;
    }

    for (const auto &ip : IP_OF_SEVERS) {
      if (ip.second == local_ip) {
        continue;
      }
      RPCClient cli(grpc::CreateChannel(ip.second + PORT_FOR_RPC,
                                        grpc::InsecureChannelCredentials()));
      if (cli.RPCGet(key)) {
        cli.RPCPost(key, req.body);
        std::cout << "HTTP POST to modify existing data is forwarded to the "
                     "server: "
                  << ip.second << std::endl;
        return;
      }
    }

    uint64_t next_sever = database->GetNextSever() % NUM_OF_SEVER;
    std::string ip_to_post = IP_OF_SEVERS.find(next_sever)->second;
    if (ip_to_post == local_ip) {
      database->Add(key, req.body);
      std::cout << "HTTP POST to local ip" << std::endl;
    } else {
      RPCClient cli(grpc::CreateChannel(ip_to_post + PORT_FOR_RPC,
                                        grpc::InsecureChannelCredentials()));
      cli.RPCPost(key, req.body);
      std::cout << "HTTP POST is forwarded to the server: " << ip_to_post
                << std::endl;
    }
    database->AddNextSever();
  });

  svr.Delete(R"((/.*))", [&](const httplib::Request &req,
                             httplib::Response &res) {
    int delete_num = database->Remove(req.path);
    if (delete_num == 0) {
      for (const auto &ip : IP_OF_SEVERS) {
        if (ip.second == local_ip) {
          continue;
        }
        RPCClient cli(grpc::CreateChannel(ip.second + PORT_FOR_RPC,
                                          grpc::InsecureChannelCredentials()));
        delete_num = cli.RPCDelete(req.path);
        if (delete_num == 1) {
          std::cout << "HTTP POST is forwarded to the server: " << ip.second
                    << std::endl;
          break;
        }
      }
    } else {
      std::cout << "HTTP DELETE from local ip" << std::endl;
    }
    if (delete_num == 0) {
      std::cout << "HTTP DELETE nothing" << std::endl;
    }
    res.set_content(std::to_string(delete_num), "text/plain");
  });

  // svr.Get("/stop", [&](const Request &, Response &) { svr.stop(); });

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