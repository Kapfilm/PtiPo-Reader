#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>

// At most eight nested references; no growing container on the ESP32 heap.
// Anchor strings are bounded by FootnoteEntry's 191-byte href limit.
class FootnoteHistory {
 public:
  struct Position {
    int spineIndex = 0;
    int pageNumber = 0;
    int pageCount = 0;
    std::optional<uint16_t> paragraph;
    std::string previewAnchor;
    std::string noteAnchor;
  };
  static constexpr int CAPACITY = 8;
  bool empty() const { return count_ == 0; }
  bool full() const { return count_ == CAPACITY; }
  int size() const { return count_; }
  const Position& root() const { return positions_[0]; }
  bool push(Position position) {
    if (full()) return false;
    positions_[count_++] = std::move(position);
    return true;
  }
  std::optional<Position> pop() {
    if (empty()) return std::nullopt;
    Position result = std::move(positions_[--count_]);
    positions_[count_] = {};
    return result;
  }
  void keepRoot() {
    while (count_ > 1) pop();
  }

 private:
  std::array<Position, CAPACITY> positions_{};
  int count_ = 0;
};
