#pragma once

#include <cstddef>

namespace esphome::siedle_in_home_bus {

template<typename T, size_t N> class FixedQueue {
 public:
  bool push(const T &item) {
    size_t next_head = (this->head_ + 1) % N;
    if (next_head == this->tail_)
      return false;
    this->buffer_[this->head_] = item;
    this->head_ = next_head;
    return true;
  }

  bool pop() {
    if (this->empty())
      return false;
    this->tail_ = (this->tail_ + 1) % N;
    return true;
  }

  T &front() { return this->buffer_[this->tail_]; }

  bool empty() const { return this->head_ == this->tail_; }

  void clear() { this->head_ = this->tail_ = 0; }

 private:
  T buffer_[N];
  uint8_t head_{0};
  uint8_t tail_{0};
};

}  // namespace esphome::siedle_in_home_bus
