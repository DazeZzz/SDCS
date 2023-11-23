#include "database_guard.h"
#include <optional>
#include <string>
#include <thread>

std::optional<std::string> DatabaseGuard::Find(const std::string &key) {
  std::shared_lock lk{map_latch_};
  auto it = database_.find(key);
  if (it != database_.end()) {
    return it->second;
  }
  return std::nullopt;
}

void DatabaseGuard::Add(const std::string &key, const std::string &value) {
  std::scoped_lock lk{map_latch_};
  database_.insert_or_assign(key, value);
}

int32_t DatabaseGuard::Remove(const std::string &key) {
  std::scoped_lock lk{map_latch_};
  return database_.erase(key);
}

void DatabaseGuard::Print(const std::string &key) {
  std::shared_lock lk{map_latch_};
  std::cout << database_.at(key) << std::endl;
}

void DatabaseGuard::AddNextServer() { next_server_++; }

uint64_t DatabaseGuard::GetNextServer() { return next_server_; }