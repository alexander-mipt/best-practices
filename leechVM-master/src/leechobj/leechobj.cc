#include "leechobj/leechobj.hh"

namespace leech {
bool LeechObj::compare(LeechObj *obj, CmpOp) const {
  std::ostringstream ss;
  ss << "Comparing of " << toUnderlying(type_) << " and "
     << toUnderlying(obj->type_) << " is not supported yet.";
  throw std::invalid_argument(ss.str());
}
pLeechObj LeechObj::add(LeechObj *obj) const {
  std::ostringstream ss;
  ss << "Adding of " << toUnderlying(type_) << " and "
     << toUnderlying(obj->type_) << " is not supported yet.";
  throw std::invalid_argument(ss.str());
}

pLeechObj LeechObj::sub(LeechObj *obj) const {
  std::ostringstream ss;
  ss << "Substracting of " << toUnderlying(type_) << " and "
     << toUnderlying(obj->type_) << " is not supported yet.";
  throw std::invalid_argument(ss.str());
}

pLeechObj LeechObj::div(LeechObj *obj) const {
  std::ostringstream ss;
  ss << "Division of " << toUnderlying(type_) << " and "
     << toUnderlying(obj->type_) << " is not supported yet.";
  throw std::invalid_argument(ss.str());
}

pLeechObj LeechObj::subscript(LeechObj *) const {
  std::ostringstream ss;
  ss << "Subscription of " << toUnderlying(type_) << " is not supported yet.";
  throw std::invalid_argument(ss.str());
}
} // namespace leech
