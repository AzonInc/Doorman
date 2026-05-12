#pragma once

namespace esphome::tc_bus
{
    template <typename T, size_t N> class FixedQueue
    {
        public:
            bool push(const T &item) {
                size_t next_head = (head_ + 1) % N;
                if (next_head == tail_) {
                    // Queue full
                    return false;
                }
                buffer_[head_] = item;
                head_ = next_head;
                return true;
            }

            bool pop() {
                if (empty()) return false;
                tail_ = (tail_ + 1) % N;
                return true;
            }
 
            T &front() {
                return buffer_[tail_];
            }

            bool empty() const {
                return head_ == tail_;
            }

            void clear() {
                head_ = tail_ = 0;
            }

        private:
            T buffer_[N];
            uint8_t head_ = 0;
            uint8_t tail_ = 0;
    };

}