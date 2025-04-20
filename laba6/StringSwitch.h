#include <cassert>
#include <cstring>
#include <optional>
#include <string_view>

template<typename T, typename R = T>
class StringSwitch {
  const std::string_view str_;
 
  std::optional<T> result_;
 
public:
  explicit StringSwitch(std::string_view S): str_(S), result_() { }
 
  StringSwitch(const StringSwitch &) = delete;
 
  void operator=(const StringSwitch &) = delete;
  void operator=(StringSwitch &&other) = delete;
 
  StringSwitch(StringSwitch &&other): str_(other.str_), result_(std::move(other.result_)) { }
 
  ~StringSwitch() = default;
 
  StringSwitch &Case(std::string_view S, T Value) {
    if (!result_ && str_ == S) {
      result_ = std::move(Value);
    }
    return *this;
  }

  [[nodiscard]] R Default(T Value) {
    if (result_)
      return std::move(*result_);
    return Value;
  }
 
  [[nodiscard]] operator R() {
    assert(result_ && "Fell off the end of a string-switch");
    return std::move(*result_);
  }
};
 