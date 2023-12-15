#include <algorithm>

#include "leechfile/leechfile.hh"

namespace leech {

/**
 * FuncMeta definitions
 */

FuncMeta::FuncMeta(const FuncMeta &fm) : addr(fm.addr), names(fm.names) {
  std::transform(fm.cstPool.begin(), fm.cstPool.end(),
                 std::back_inserter(cstPool),
                 [](const auto &cst) { return cst->clone(); });
}

FuncMeta &FuncMeta::operator=(const FuncMeta &fm) {
  addr = fm.addr;
  names = fm.names;
  std::transform(fm.cstPool.begin(), fm.cstPool.end(),
                 std::back_inserter(cstPool),
                 [](const auto &cst) { return cst->clone(); });
  return *this;
}

void FuncMeta::serialize(std::ostream &ost) const {
  /* Write function address */
  serializeNum<FuncAddr>(ost, addr);

  /* Write function argument num */
  serializeNum<uint64_t>(ost, argNum);

  /* Write constant pool */
  auto cstNum = cstPool.size();
  serializeNum<std::uint64_t>(ost, cstNum);
  for (const auto &cst : cstPool)
    cst->serialize(ost);

  /* Write names */
  auto nameNum = names.size();
  serializeNum<std::uint64_t>(ost, nameNum);
  for (const auto &name : names)
    serializeString(ost, name);
}

std::pair<std::string, FuncMeta> FuncMeta::deserialize(std::istream &ist) {
  auto nameLen = deserializeNum<uint64_t>(ist);
  auto name = deserializeString(ist, nameLen);

  auto addr = deserializeNum<FuncAddr>(ist);
  auto argNum = deserializeNum<uint64_t>(ist);

  auto cstNum = deserializeNum<uint64_t>(ist);
  std::vector<pLeechObj> cstPool{};
  for (uint64_t i = 0; i < cstNum; ++i)
    cstPool.push_back(deserializeObj(ist));

  auto nameNum = deserializeNum<uint64_t>(ist);
  std::vector<std::string> names{};
  for (uint64_t i = 0; i < nameNum; ++i) {
    auto len = deserializeNum<uint64_t>(ist);
    names.push_back(deserializeString(ist, len));
  }

  FuncMeta fm{};
  fm.addr = addr;
  fm.argNum = argNum;
  fm.cstPool = std::move(cstPool);
  fm.names = std::move(names);

  return {name, std::move(fm)};
}

/**
 * Meta definitions
 */

Meta::Meta(std::unordered_map<std::string, FuncMeta> &&funcs_)
    : funcs(funcs_) {}

Meta::Meta(const std::unordered_map<std::string, FuncMeta> &funcs_)
    : funcs(funcs_) {}

void Meta::serialize(std::ostream &ost) const {
  /* Write function number */
  auto funcNum = funcs.size();
  serializeNum<std::uint64_t>(ost, funcNum);

  /* Write functions meta */
  for (auto &&[name, fm] : funcs) {
    serializeString(ost, name);
    fm.serialize(ost);
  }
}

Meta Meta::deserialize(std::istream &ist) {
  auto fnum = deserializeNum<uint64_t>(ist);
  std::unordered_map<std::string, FuncMeta> funcs{};
  for (uint64_t i = 0; i < fnum; ++i) {
    auto &&[name, meta] = FuncMeta::deserialize(ist);
    funcs[name] = meta;
  }

  return {funcs};
}

/**
 * LeechFile definitions
 */

LeechFile::LeechFile(Meta &&meta_, std::vector<Instruction> &&code_)
    : meta(meta_), code(code_) {}

void LeechFile::serialize(std::ostream &ost) const {
  /* Write magic */
  auto magic = reinterpret_cast<const std::uint64_t *>(theLEECH.data());
  serializeNum<uint64_t>(ost, *magic);

  /* Write meta */
  meta.serialize(ost);

  /* Write code */
  serializeNum<uint64_t>(ost, code.size());
  for (const auto &inst : code)
    inst.serialize(ost);
}

LeechFile LeechFile::deserialize(std::istream &ist) {
  auto magic = deserializeString(ist, theLEECH.size());
  if (magic != theLEECH)
    throw std::runtime_error{"Wrong magic number value"};

  auto meta = Meta::deserialize(ist);

  auto instNum = deserializeNum<uint64_t>(ist);
  std::vector<Instruction> code{};
  for (uint64_t i = 0; i < instNum; ++i)
    code.push_back(Instruction::deserialize(ist));

  return {std::move(meta), std::move(code)};
}

void LeechFile::dump2LeechFormat(std::ostream &ost) {
  constexpr char blockOffset[] = "    ";
  constexpr char dataOffset[] = "        ";
  for (const auto &[name, fmeta] : meta.funcs) {
    ost << ".func " << name << "(" << fmeta.addr << ")" << std::endl;
    ost << blockOffset << ".cpool" << std::endl;
    for (std::size_t i = 0; i < fmeta.cstPool.size(); ++i) {
      ost << dataOffset << i << ": ";
      fmeta.cstPool[i]->print();
      ost << std::endl;
    }
    ost << blockOffset << ".names" << std::endl;
    for (std::size_t i = 0; i < fmeta.names.size(); ++i) {
      ost << dataOffset << i << ": " << fmeta.names[i] << std::endl;
    }
  }
  ost << ".code" << std::endl;
  for (std::size_t i = 0; i < code.size(); ++i) {
    ost << blockOffset << i << " "
        << OpcodeConv::toName(code[i].getOpcode()).value() << " "
        << static_cast<int>(code[i].getArg()) << std::endl;
  }
}

} // namespace leech
