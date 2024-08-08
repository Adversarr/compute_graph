// The MIT License (MIT)
// Copyright © 2024 Adversarr
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <any>
#include <deque>
#include <unordered_map>

#include "intern/config.hpp"

namespace compute_graph {

using ctx_frame = std::unordered_map<std::string, std::any>;
using stacked_frames = std::deque<ctx_frame>;

class Context {
public:
  Context() noexcept;
  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;
  Context(Context&&) noexcept = default;
  Context& operator=(Context&&) = delete;

  // Stack related.
  void push_stack() noexcept;
  void push_stack(ctx_frame frame) noexcept;
  void pop_stack() noexcept;
  size_t stack_size() const noexcept;
  ctx_frame const& top() const noexcept;
  ctx_frame &top() noexcept;
  stacked_frames& frames() noexcept;
  stacked_frames const& frames() const noexcept;

  // Frame related.
  // Put a value in the current frame.
  auto insert(std::string_view key, std::any const& value);
  template <typename T, typename... Args> auto emplace(std::string_view key, Args&&... args);

  // Get a value from the current frame.
  std::any const& get(std::string const& key) const CG_NOEXCEPT;
  std::any const& get_top(std::string const& key) const CG_NOEXCEPT;

  bool has(std::string const& key) const noexcept;
  bool has_top(std::string const& key) const noexcept;

private:
  stacked_frames frames_;
};

CG_STRONG_INLINE Context::Context() noexcept : frames_{} { frames_.emplace_back(); }

CG_STRONG_INLINE void Context::push_stack() noexcept { frames_.emplace_back(); }

CG_STRONG_INLINE void Context::push_stack(ctx_frame frame) noexcept {
  frames_.emplace_back(std::move(frame));
}

CG_STRONG_INLINE void Context::pop_stack() noexcept {
  if (!frames_.empty()) frames_.pop_back();
}

CG_STRONG_INLINE size_t Context::stack_size() const noexcept { return frames_.size(); }

CG_STRONG_INLINE ctx_frame const& Context::top() const noexcept { return frames_.back(); }

CG_STRONG_INLINE ctx_frame& Context::top() noexcept { return frames_.back(); }

CG_STRONG_INLINE stacked_frames& Context::frames() noexcept { return frames_; }

CG_STRONG_INLINE stacked_frames const& Context::frames() const noexcept { return frames_; }

CG_STRONG_INLINE auto Context::insert(std::string_view key, std::any const& value) {
  return top().emplace(key, value);
}

CG_STRONG_INLINE std::any const& Context::get(std::string const& key) const CG_NOEXCEPT {
  for (auto it = frames_.crbegin(); it != frames_.crend(); ++it) {
    if (auto const& it2 = it->find(key); it2 != it->end()) return it2->second;
  }
  CG_THROW(std::out_of_range, "Key not found in context: " + std::string(key));
  CG_UNREACHABLE();
}

inline std::any const& Context::get_top(std::string const& key) const CG_NOEXCEPT {
  if (auto const& it = top().find(key); it != top().end()) return it->second;
  CG_THROW(std::out_of_range, "Key not found in context: " + std::string(key));
  CG_UNREACHABLE();
}

CG_STRONG_INLINE bool Context::has(std::string const& key) const noexcept{
  for (auto it = frames_.crbegin(); it != frames_.crend(); ++it) {
    if (auto const& it2 = it->find(key); it2 != it->end()) return true;
  }
  return false;
}

CG_STRONG_INLINE bool Context::has_top(std::string const& key) const noexcept{
  auto const& it = top().find(key);
  return it != top().end();
}

template <typename T, typename... Args>
CG_STRONG_INLINE auto Context::emplace(std::string_view key, Args&&... args) {
  return top().emplace(key, std::make_any<T>(std::forward<Args>(args)...));
}

}  // namespace compute_graph