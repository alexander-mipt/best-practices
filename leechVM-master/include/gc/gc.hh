#ifndef __INCLUDE_GC_GC_HH__
#define __INCLUDE_GC_GC_HH__

#include <cstdint>
#include <stdexcept>
#include <sys/mman.h>
#include <type_traits>
#include <utility>

namespace leech::gc {

template <std::uintptr_t StartAddr, std::size_t Size> class Region final {
  template <class T> struct Allocator final {
    using value_type = T;

    Allocator() = default;

    template <class U> Allocator([[maybe_unused]] const Allocator<U> &other) {}

    [[nodiscard]] auto allocate(std::size_t size) {
      return Region::allocateInstance<T>(size);
    }

    void deallocate([[maybe_unused]] T *ptr,
                    [[maybe_unused]] std::size_t size) noexcept {}
  };

  static inline auto curOffset = 0;

  [[nodiscard]] static auto getStartPtr() {
    return reinterpret_cast<void *>(StartAddr);
  }

public:
  Region() = delete;

  template <class T> static auto getAllocator() { return Allocator<T>(); }

  static void reset() { curOffset = 0; }

  [[nodiscard]] static auto allocateRaw(std::size_t num_of_bytes) {
    auto newOffset = curOffset + num_of_bytes;

    if (newOffset > Size)
      throw std::runtime_error{"Out of memory"};

    return getStartPtr() + std::exchange(curOffset, newOffset);
  }

  template <class T>
  [[nodiscard]] static auto allocateInstance(std::size_t num_elems = 1) {
    return reinterpret_cast<T *>(allocateRaw(num_elems * sizeof(T)));
  }
};

class MemoryManager final {
  static inline constexpr std::size_t kBlockSizeMB = 32;
  static inline constexpr std::size_t kBlockSize = kBlockSizeMB << 20U;
  static inline constexpr std::uintptr_t kStartAddr = 0xE000000;

  class MMapWrapper final {
    std::size_t size_{};
    void *buf_ = nullptr;

  public:
    struct MMapFail : public std::runtime_error {
      explicit MMapFail(const char *err) : std::runtime_error(err) {}
    };

    explicit MMapWrapper(std::size_t reg_size, std::uintptr_t start_addr = 0)
        : size_(reg_size), buf_(mmap(reinterpret_cast<void *>(start_addr),
                                     size_, PROT_READ | PROT_WRITE,
                                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) {
      if (buf_ == MAP_FAILED)
        throw MMapFail("mmap() allocation failed");
    }

    MMapWrapper(const MMapWrapper &) = delete;
    MMapWrapper &operator=(const MMapWrapper &) = delete;

    MMapWrapper(MMapWrapper &&) = default;
    MMapWrapper &operator=(MMapWrapper &&) = default;

    [[nodiscard]] auto get() const { return buf_; }

    template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
    [[nodiscard]] auto as() const {
      return reinterpret_cast<T>(buf_);
    }
    [[nodiscard]] auto size() const { return size_; }

    ~MMapWrapper() { munmap(buf_, size_); }
  } block_{kBlockSize, kStartAddr};

public:
  MemoryManager() {
    if (block_.as<decltype(kStartAddr)>() != kStartAddr)
      throw std::logic_error{"Trying to create more than one mem block"};
  }

private:
};
} // namespace leech::gc

#endif // __INCLUDE_GC_GC_HH__
