#ifndef __INCLUDE_LEECHOBJ_LEECHOBJ_HH__
#define __INCLUDE_LEECHOBJ_LEECHOBJ_HH__

#include <algorithm>
#include <istream>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "common/common.hh"
#include "common/opcodes.hh"

namespace leech {

class LeechObj;
using pLeechObj = std::shared_ptr<LeechObj>;

class LeechObj : public ISerializable {
  std::size_t size_{};
  ValueType type_{};

public:
  LeechObj(std::size_t size, ValueType type) : size_(size), type_(type) {}

  void serialize(std::ostream &ost) const override final {
    serializeTypeNSize(ost);
    serializeVal(ost);
  }

  virtual void print() const = 0;

  virtual pLeechObj clone() const = 0;
  [[noreturn]] virtual bool compare(LeechObj *obj, CmpOp op) const;
  [[noreturn]] virtual pLeechObj add(LeechObj *obj) const;
  [[noreturn]] virtual pLeechObj sub(LeechObj *obj) const;
  [[noreturn]] virtual pLeechObj div(LeechObj *obj) const;
  [[noreturn]] virtual pLeechObj subscript(LeechObj *obj) const;

  auto getType() const { return type_; }

protected:
  void serializeTypeNSize(std::ostream &ost) const {
    serializeNum(ost, toUnderlying(type_));
    if (type_ != ValueType::None)
      serializeNum(ost, size_);
  }

  auto getSize() const { return size_; }

private:
  virtual void serializeVal(std::ostream &) const = 0;
};

inline pLeechObj deserializeObj(std::istream &ist);

class NoneObj final : public LeechObj {
public:
  NoneObj() : LeechObj(0, ValueType::None) {}

  void print() const override { std::cout << "None" << std::endl; }

  pLeechObj clone() const override { return std::make_unique<NoneObj>(); }

private:
  void serializeVal(std::ostream &) const override {}
};

template <typename T> class NumberObj final : public LeechObj {
  static_assert(NumberLeech_v<T>);
  T value_{};

public:
  explicit NumberObj(T value)
      : LeechObj(sizeof(T), typeToValueType<T>()), value_(value) {}

  void print() const override { std::cout << value_; }

  static pLeechObj deserialize(std::istream &ist) {
    deserializeNum<uint64_t>(ist);
    auto val = deserializeNum<T>(ist);
    return std::make_unique<NumberObj>(val);
  }

  pLeechObj clone() const override {
    return std::make_unique<NumberObj>(value_);
  }

  pLeechObj div(LeechObj *obj) const override {
    if (obj->getType() != getType())
      throw std::invalid_argument("Can't divide");

    auto *pObj = dynamic_cast<NumberObj<T> *>(obj);
    if (pObj == nullptr)
      throw std::runtime_error("Dynamic cast failed");
    return std::make_shared<NumberObj<Float>>(static_cast<Float>(value_) /
                                              static_cast<Float>(pObj->value_));
  }

  bool compare(LeechObj *obj, CmpOp op) const override {
    if (obj->getType() != getType() || getType() != ValueType::Integer)
      throw std::invalid_argument("Can't compare");

    auto *pObj = dynamic_cast<NumberObj<T> *>(obj);

    if (pObj == nullptr)
      throw std::runtime_error("Dynamic cast failed");

    switch (op) {
    case CmpOp::LE:
      return value_ < pObj->value_;
    case CmpOp::LEQ:
      return value_ <= pObj->value_;
    case CmpOp::EQ:
      return value_ == pObj->value_;
    case CmpOp::NEQ:
      return value_ != pObj->value_;
    case CmpOp::GR:
      return value_ > pObj->value_;
    case CmpOp::GREQ:
      return value_ >= pObj->value_;

    default:
      throw std::runtime_error("Uknown cmp op type");
    }
  }

  pLeechObj sub(LeechObj *obj) const override {
    auto pobj = dynamic_cast<NumberObj *>(obj);
    if (nullptr == pobj)
      throw std::runtime_error("Dynamic cast failed");
    return std::make_unique<NumberObj>(value_ - pobj->value_);
  }

  pLeechObj add(LeechObj *obj) const override {
    auto pobj = dynamic_cast<NumberObj *>(obj);
    if (nullptr == pobj)
      throw std::runtime_error("Dynamic cast failed");
    return std::make_unique<NumberObj>(pobj->value_ + value_);
  }

  auto getVal() const { return value_; }

private:
  void serializeVal(std::ostream &ost) const override {
    serializeNum(ost, value_);
  }
};

using IntObj = NumberObj<Integer>;
using FloatObj = NumberObj<Float>;

class StringObj final : public LeechObj {
  std::string string_;

public:
  explicit StringObj(std::string_view string)
      : LeechObj(string.size(), ValueType::String), string_(string) {}

  void print() const override { std::cout << '"' << string_ << '"'; }

  pLeechObj clone() const override {
    return std::make_unique<StringObj>(string_);
  }

  static pLeechObj deserialize(std::istream &ist) {
    auto strlen = deserializeNum<uint64_t>(ist);
    auto str = deserializeString(ist, strlen);
    return std::make_unique<StringObj>(str);
  }

private:
  void serializeVal(std::ostream &ost) const override {
    for (auto sym : string_)
      serializeNum(ost, sym);
  }
};

class ClassObj final : public LeechObj {

  std::map<std::string, pLeechObj> fields{};
  std::set<std::string> methods{};

public:
  explicit ClassObj() : LeechObj(sizeof(ClassObj), ValueType::Class) {}

  ClassObj(ClassObj &&) = default;
  ClassObj(const ClassObj &) = default;
  ClassObj &operator=(ClassObj &) = default;
  ClassObj &operator=(ClassObj &&) = default;
  ~ClassObj() = default;

  void print() const override {
    std::cout << "~~~Class dump:~~~" << std::endl;
    std::cout << "~~~Fields~~~" << std::endl;
    for (auto &&[key, value] : fields) {
      std::cout << "key: " << key << " = ";
      value->print();
      std::cout << std::endl;
    }
    std::cout << "~~~Methods~~~" << std::endl;
    for (auto &&met : methods) {
      std::cout << met << std::endl;
    }
    std::cout << "~~~~~~~~~~~~~" << std::endl;
  }

  void updateField(std::string_view name, pLeechObj obj) {
    fields.insert_or_assign(std::string(name), obj);
  }

  void registerMethod(std::string_view name) {
    auto res = methods.insert(std::string(name));
    if (!res.second) // name already exist
      throw std::logic_error("Trying to register method 2nd time");
  }

  void checkMethod(std::string_view name) {
    auto It = methods.find(std::string(name));
    if (It == methods.end()) {
      auto msg = std::string("There is no method named ") += std::string(name);
      throw std::logic_error(msg.c_str());
    }
  }

  pLeechObj getField(std::string_view name) const {
    auto It = fields.find(std::string(name));
    if (It == fields.end()) {
      auto msg = std::string("Invalid field name: ") += std::string(name);
      throw std::runtime_error(msg);
    } else
      return It->second;
  }

  std::size_t getNumFields() { return fields.size(); }
  std::size_t getNumMethods() { return methods.size(); }

  void serializeVal([[maybe_unused]] std::ostream &ost) const override {
    throw std::runtime_error("Hey buddy, I think you've got the wrong door, "
                             "the leather club's two blocks down. ");
  }

  pLeechObj clone() const override { return std::make_shared<ClassObj>(*this); }

  // TODO :  deserialize
};

using Tuple = std::vector<pLeechObj>;

class TupleObj final : public LeechObj {
  Tuple tuple_;

public:
  template <class InpIt>
  TupleObj(InpIt begin, InpIt end)
      : LeechObj(static_cast<std::size_t>(std::distance(begin, end)),
                 ValueType::Tuple),
        tuple_(getSize()) {
    std::move(begin, end, tuple_.begin());
  }

  template <class Cont>
  explicit TupleObj(Cont &&cont) : TupleObj(cont.begin(), cont.end()) {}

  void print() const override {
    std::cout << '(';
    for (auto &&elem : tuple_) {
      elem->print();
      std::cout << ',';
    }
    std::cout << ')';
  }

  pLeechObj subscript(LeechObj *pobj) const override {
    if (pobj->getType() != ValueType::Integer)
      throw std::invalid_argument("Incorrect subscription index");

    auto *numObj = dynamic_cast<IntObj *>(pobj);

    return tuple_.at(static_cast<std::size_t>(numObj->getVal()));
  }

  pLeechObj clone() const override {
    Tuple res;
    res.reserve(tuple_.size());

    for (auto &&elem : tuple_)
      res.push_back(elem->clone());

    return std::make_unique<TupleObj>(std::move(res));
  }

  static pLeechObj deserialize(std::istream &ist) {
    auto len = deserializeNum<uint64_t>(ist);
    Tuple tuple{};
    for (uint64_t i = 0; i < len; ++i)
      tuple.push_back(deserializeObj(ist));

    return std::make_unique<TupleObj>(std::move(tuple));
  }

private:
  void serializeVal(std::ostream &ost) const override {
    for (auto &&ptr : tuple_)
      ptr->serialize(ost);
  }
};

inline pLeechObj deserializeObj(std::istream &ist) {
  auto cstTyVal = deserializeNum<std::underlying_type_t<ValueType>>(ist);
  auto cstTy = static_cast<ValueType>(cstTyVal);

  switch (cstTy) {
  case ValueType::Integer:
    return IntObj::deserialize(ist);
  case ValueType::Float:
    return FloatObj::deserialize(ist);
  case ValueType::String:
    return StringObj::deserialize(ist);
  case ValueType::Tuple:
    return TupleObj::deserialize(ist);
  default:
    return nullptr;
  }
}

} // namespace leech

#endif // __INCLUDE_LEECHOBJ_LEECHOBJ_HH__
