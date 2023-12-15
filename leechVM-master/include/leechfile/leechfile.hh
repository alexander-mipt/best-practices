#ifndef __INCLUDE_LEECHFILE_LEECHFILE_HH__
#define __INCLUDE_LEECHFILE_LEECHFILE_HH__

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/common.hh"
#include "leechobj/leechobj.hh"

namespace leech {

struct FuncMeta final : public ISerializable {
  FuncAddr addr{};
  uint64_t argNum{};
  std::vector<std::shared_ptr<LeechObj>> cstPool{};
  std::vector<std::string> names{};

  FuncMeta() = default;

  FuncMeta(const FuncMeta &fm);
  FuncMeta &operator=(const FuncMeta &fm);

  FuncMeta(FuncMeta &&fm) = default;
  FuncMeta &operator=(FuncMeta &&fm) = default;

  ~FuncMeta() = default;

  void serialize(std::ostream &ost) const override;
  static std::pair<std::string, FuncMeta> deserialize(std::istream &ist);
};

struct Meta final : public ISerializable {
  std::unordered_map<std::string, FuncMeta> funcs{};

  Meta() = default;
  Meta(std::unordered_map<std::string, FuncMeta> &&funcs_);
  Meta(const std::unordered_map<std::string, FuncMeta> &funcs_);

  void serialize(std::ostream &ost) const override;
  static Meta deserialize(std::istream &ist);
};

struct LeechFile final : public ISerializable {
  constexpr static std::string_view theLEECH = "theLEECH";

  Meta meta{};
  std::vector<Instruction> code{};

  LeechFile() = default;
  LeechFile(Meta &&meta_, std::vector<Instruction> &&code_);
  void serialize(std::ostream &ost) const override;
  static LeechFile deserialize(std::istream &ist);
  void dump2LeechFormat(std::ostream &ost);
};

} // namespace leech

#endif // __INCLUDE_LEECHFILE_LEECHFILE_HH__
