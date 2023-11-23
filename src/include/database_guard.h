#pragma once

#include <atomic>
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <iostream>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

class DatabaseGuard {
public:
  std::optional<std::string> Find(const std::string &key);

  void Add(const std::string &key, const std::string &value);

  int32_t Remove(const std::string &key);

  void Print(const std::string &key);

  void AddNextServer();

  uint64_t GetNextServer();

private:
  std::unordered_map<std::string, std::string> database_;
  std::atomic_uint64_t next_server_ = 0;
  std::shared_mutex map_latch_;
};