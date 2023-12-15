#include <stdexcept>
#include <unordered_map>

#include "common/common.hh"
#include "executor/executor.hh"

namespace {
using namespace leech;

void execute_POP_TOP([[maybe_unused]] const Instruction &inst, State &state) {
  state.getCurFrame().pop();
}
void execute_ROT_TWO([[maybe_unused]] const Instruction &inst,
                     [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_ROT_THREE([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DUP_TOP([[maybe_unused]] const Instruction &inst,
                     [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DUP_TOP_TWO([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_ROT_FOUR([[maybe_unused]] const Instruction &inst,
                      [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_NOP([[maybe_unused]] const Instruction &inst,
                 [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_UNARY_POSITIVE([[maybe_unused]] const Instruction &inst,
                            [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_UNARY_NEGATIVE([[maybe_unused]] const Instruction &inst,
                            [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_UNARY_NOT([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_UNARY_INVERT([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_MATRIX_MULTIPLY([[maybe_unused]] const Instruction &inst,
                                    [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_MATRIX_MULTIPLY([[maybe_unused]] const Instruction &inst,
                                     [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_POWER([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_MULTIPLY([[maybe_unused]] const Instruction &inst,
                             [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_MODULO([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_ADD([[maybe_unused]] const Instruction &inst,
                        State &state) {
  auto &curFrame = state.getCurFrame();
  auto tos1 = curFrame.popGetTos();
  auto tos2 = curFrame.popGetTos();

  curFrame.push(tos1->add(tos2.get()));
}
void execute_BINARY_SUBTRACT([[maybe_unused]] const Instruction &inst,
                             State &state) {
  auto &curFrame = state.getCurFrame();
  auto tos1 = curFrame.popGetTos();
  auto tos2 = curFrame.popGetTos();

  curFrame.push(tos2->sub(tos1.get()));
}
void execute_BINARY_SUBSCR([[maybe_unused]] const Instruction &inst,
                           State &state) {
  auto &curFrame = state.getCurFrame();
  auto idx = curFrame.popGetTos();
  auto tuple = curFrame.popGetTos();

  curFrame.push(tuple->subscript(idx.get()));
}
void execute_BINARY_FLOOR_DIVIDE([[maybe_unused]] const Instruction &inst,
                                 [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_TRUE_DIVIDE([[maybe_unused]] const Instruction &inst,
                                State &state) {
  auto &curFrame = state.getCurFrame();
  auto two = curFrame.popGetTos();
  auto one = curFrame.popGetTos();

  curFrame.push(one->div(two.get()));
}
void execute_INPLACE_FLOOR_DIVIDE([[maybe_unused]] const Instruction &inst,
                                  [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_TRUE_DIVIDE([[maybe_unused]] const Instruction &inst,
                                 [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_GET_LEN([[maybe_unused]] const Instruction &inst,
                     [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_MATCH_MAPPING([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_MATCH_SEQUENCE([[maybe_unused]] const Instruction &inst,
                            [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_MATCH_KEYS([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_COPY_DICT_WITHOUT_KEYS([[maybe_unused]] const Instruction &inst,
                                    [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_WITH_EXCEPT_START([[maybe_unused]] const Instruction &inst,
                               [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_GET_AITER([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_GET_ANEXT([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BEFORE_ASYNC_WITH([[maybe_unused]] const Instruction &inst,
                               [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_END_ASYNC_FOR([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_ADD([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_SUBTRACT([[maybe_unused]] const Instruction &inst,
                              [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_MULTIPLY([[maybe_unused]] const Instruction &inst,
                              [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_MODULO([[maybe_unused]] const Instruction &inst,
                            [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_STORE_SUBSCR([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DELETE_SUBSCR([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_LSHIFT([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_RSHIFT([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_AND([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_XOR([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BINARY_OR([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_POWER([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_GET_ITER([[maybe_unused]] const Instruction &inst,
                      [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_GET_YIELD_FROM_ITER([[maybe_unused]] const Instruction &inst,
                                 [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_PRINT_EXPR([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_YIELD_FROM([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_GET_AWAITABLE([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LOAD_ASSERTION_ERROR([[maybe_unused]] const Instruction &inst,
                                  [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_LSHIFT([[maybe_unused]] const Instruction &inst,
                            [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_RSHIFT([[maybe_unused]] const Instruction &inst,
                            [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_AND([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_XOR([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_INPLACE_OR([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LIST_TO_TUPLE([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_RETURN_VALUE([[maybe_unused]] const Instruction &inst,
                          State &state) {
  auto &fstack = state.funcStack;
  auto tos = state.getCurFrame().popGetTos();

  fstack.pop();

  if (!fstack.empty()) {
    state.nextPC = state.getCurFrame().getRet();
    state.getCurFrame().push(tos);
  }
}
void execute_IMPORT_STAR([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_SETUP_ANNOTATIONS([[maybe_unused]] const Instruction &inst,
                               [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_YIELD_VALUE([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_POP_BLOCK([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_POP_EXCEPT([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_HAVE_ARGUMENT([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_STORE_NAME([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DELETE_NAME([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_UNPACK_SEQUENCE([[maybe_unused]] const Instruction &inst,
                             [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_FOR_ITER([[maybe_unused]] const Instruction &inst,
                      [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_UNPACK_EX([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DELETE_ATTR([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_STORE_GLOBAL([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DELETE_GLOBAL([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_ROT_N([[maybe_unused]] const Instruction &inst,
                   [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LOAD_CONST(const Instruction &inst, State &state) {
  auto &curFrame = state.getCurFrame();
  curFrame.push(curFrame.getConst(inst.getArg()));
}
void execute_LOAD_NAME([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BUILD_TUPLE([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BUILD_LIST([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BUILD_SET([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BUILD_MAP([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_COMPARE_OP(const Instruction &inst, State &state) {
  auto &curFrame = state.getCurFrame();
  auto op = static_cast<CmpOp>(inst.getArg());
  auto tos1 = curFrame.popGetTos();
  auto tos2 = curFrame.popGetTos();

  bool res = tos2->compare(tos1.get(), op);

  curFrame.emplace<IntObj>(static_cast<Integer>(res));
}
void execute_IMPORT_NAME([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_IMPORT_FROM([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_JUMP_FORWARD([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_JUMP_IF_FALSE_OR_POP([[maybe_unused]] const Instruction &inst,
                                  [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_JUMP_IF_TRUE_OR_POP([[maybe_unused]] const Instruction &inst,
                                 [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_JUMP_ABSOLUTE([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_POP_JUMP_IF_FALSE(const Instruction &inst, State &state) {
  auto &curFrame = state.getCurFrame();
  auto dest = inst.getArg();
  auto tos = curFrame.popGetTos();
  IntObj fal(0);

  bool res = tos->compare(&fal, CmpOp::EQ);
  if (res)
    state.nextPC = dest;
}
void execute_POP_JUMP_IF_TRUE(const Instruction &inst, State &state) {
  auto &curFrame = state.getCurFrame();
  auto dest = inst.getArg();
  auto tos = curFrame.popGetTos();
  IntObj fal(0);

  bool res = tos->compare(&fal, CmpOp::EQ);
  if (!res)
    state.nextPC = dest;
}
void execute_LOAD_GLOBAL([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_IS_OP([[maybe_unused]] const Instruction &inst,
                   [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_CONTAINS_OP([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_RERAISE([[maybe_unused]] const Instruction &inst,
                     [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_JUMP_IF_NOT_EXC_MATCH([[maybe_unused]] const Instruction &inst,
                                   [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_SETUP_FINALLY([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LOAD_FAST(const Instruction &inst, State &state) {
  auto &curFrame = state.getCurFrame();
  auto name = curFrame.getName(inst.getArg());
  curFrame.push(curFrame.getVar(name));
}
void execute_STORE_FAST(const Instruction &inst, State &state) {
  auto &curFrame = state.getCurFrame();
  auto name = curFrame.getName(inst.getArg());
  curFrame.setVar(name, curFrame.top());
}
void execute_DELETE_FAST([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_GEN_START([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_RAISE_VARARGS([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_CALL_FUNCTION(const Instruction &inst, State &state) {
  auto &curFrame = state.getCurFrame();
  auto idx = inst.getArg();
  auto fName = std::string(curFrame.getName(idx));

  auto *fMeta = &state.pFile->meta.funcs.at(fName);
  state.nextPC = fMeta->addr;

  /* Get args from data stack */
  std::vector<pLeechObj> args(curFrame.stackSize());
  std::generate(args.begin(), args.end(),
                [&curFrame] { return curFrame.popGetTos(); });

  curFrame.setRet(state.pc + 1);
  state.funcStack.emplace(fMeta);

  state.getCurFrame().fillArgs(args.begin(), args.end());
}
void execute_MAKE_FUNCTION([[maybe_unused]] const Instruction &inst,
                           [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BUILD_SLICE([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LOAD_CLOSURE([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LOAD_DEREF([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_STORE_DEREF([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DELETE_DEREF([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_CALL_FUNCTION_KW([[maybe_unused]] const Instruction &inst,
                              [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_CALL_FUNCTION_EX([[maybe_unused]] const Instruction &inst,
                              [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_SETUP_WITH([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_EXTENDED_ARG([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LIST_APPEND([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_SET_ADD([[maybe_unused]] const Instruction &inst,
                     [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_MAP_ADD([[maybe_unused]] const Instruction &inst,
                     [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LOAD_CLASSDEREF([[maybe_unused]] const Instruction &inst,
                             [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_MATCH_CLASS([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_SETUP_ASYNC_WITH([[maybe_unused]] const Instruction &inst,
                              [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_FORMAT_VALUE([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BUILD_CONST_KEY_MAP([[maybe_unused]] const Instruction &inst,
                                 [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_BUILD_STRING([[maybe_unused]] const Instruction &inst,
                          [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_LOAD_METHOD([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}

void execute_LIST_EXTEND([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_SET_UPDATE([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DICT_MERGE([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_DICT_UPDATE([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  throw std::logic_error{"Function is not implemented yet"};
}
void execute_PRINT([[maybe_unused]] const Instruction &inst, State &state) {
  state.getCurFrame().popGetTos()->print();
  std::cout << std::endl;
}

//CLASS OPERATION FUNCTIONS
void printDebugInfo(
  [[maybe_unused]] std::string_view op,
  [[maybe_unused]] std::string_view name,
  [[maybe_unused]] const State & state,
  [[maybe_unused]] std::shared_ptr<ClassObj> pClassObj) {
  #ifdef DEBUG_PRINT
  std::cout << std::endl;
  std::cout << "PC = " <<  state.pc << std::endl << op << " " << name << std::endl;
  pClassObj ->print();
  std::cout << std::endl;
  #endif
}
void printDebugInfo(
  [[maybe_unused]] std::string_view op,
  [[maybe_unused]] std::string_view name,
  [[maybe_unused]] const State & state,
  [[maybe_unused]] std::shared_ptr<ClassObj> pClassObj,
  [[maybe_unused]] std::shared_ptr<LeechObj> attr) {
  #ifdef DEBUG_PRINT
  std::cout << std::endl;
  std::cout << "PC = " <<  state.pc << std::endl << op << " " << name << " = ";
  attr->print();
  std::cout << std::endl;
  pClassObj ->print();
  std::cout << std::endl;
  #endif
}

std::shared_ptr<ClassObj> safeConvertToClass(std::shared_ptr<LeechObj> pLeech,
  std::string_view funcName) {
  if (pLeech->getType() != ValueType::Class) {
    auto msg = std::string(funcName) += std::string(" Trying to convert class from invalid leechObj");
    throw std::runtime_error(msg.c_str());
  }
  return std::static_pointer_cast<ClassObj>(pLeech);
}

void execute_LOAD_BUILD_CLASS([[maybe_unused]] const Instruction &inst,
                              [[maybe_unused]] State &state) {
  auto &curFrame = state.getCurFrame();
  curFrame.push(std::make_shared<ClassObj>());
}

void execute_STORE_BUILD_CLASS([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  auto &curFrame = state.getCurFrame();
  auto name = curFrame.getName(inst.getArg());
  auto leechObj = curFrame.popGetTos();
  auto pClassObj = safeConvertToClass(leechObj, "StoreBuildClass");
  curFrame.setVar(name, pClassObj);
  printDebugInfo("StoreBuildClass", name, state, pClassObj);
}


void execute_STORE_ATTR([[maybe_unused]] const Instruction &inst,
                        [[maybe_unused]] State &state) {
  auto &curFrame = state.getCurFrame();
  auto name = curFrame.getName(inst.getArg());
  auto attr = curFrame.popGetTos();
  auto leechObj = curFrame.top();
  auto pClassObj = safeConvertToClass(leechObj, "StoreAttr");
  pClassObj->updateField(name, attr);
  printDebugInfo("StoreAttr", name, state, pClassObj, attr);
}

void execute_LOAD_ATTR([[maybe_unused]] const Instruction &inst,
                       [[maybe_unused]] State &state) {
  auto &curFrame = state.getCurFrame();
  auto name = curFrame.getName(inst.getArg());
  auto leechObj = curFrame.top();
  auto pClassObj = safeConvertToClass(leechObj, "StoreBuildClass");
  auto attr = pClassObj->getField(name);
  curFrame.push(attr);
  printDebugInfo("LoadAttr", name, state, pClassObj, attr);
}

void execute_INSTANCE_CLASS([[maybe_unused]] const Instruction &inst,
                              [[maybe_unused]] State &state) {
  auto &curFrame = state.getCurFrame();
  auto name = curFrame.getName(inst.getArg());
  auto leechObj = curFrame.getVar(name);
  auto pClassObj = safeConvertToClass(leechObj, "StoreBuildClass");
  curFrame.push(pClassObj->clone());
  printDebugInfo("InstanceClass", name, state, pClassObj);
}
void execute_REGISTER_METHOD([[maybe_unused]] const Instruction &inst,
                             [[maybe_unused]] State &state) {
  auto &curFrame = state.getCurFrame();
  auto idx = inst.getArg();
  auto fName = std::string(curFrame.getName(idx));

  auto leechObj = curFrame.top();
  auto pClassObj = safeConvertToClass(leechObj, "REGISTER_METHOD");
  pClassObj->registerMethod(fName);
  printDebugInfo("Method registered", fName, state, pClassObj);
}
void execute_CALL_METHOD([[maybe_unused]] const Instruction &inst,
                         [[maybe_unused]] State &state) {
  auto &curFrame = state.getCurFrame();
  auto idx = inst.getArg();
  auto fName = std::string(curFrame.getName(idx));

  auto *fMeta = &state.pFile->meta.funcs.at(fName);
  state.nextPC = fMeta->addr;
  auto numArgs = fMeta->argNum;

  /* Get args from data stack */
  std::vector<pLeechObj> args(numArgs);
  std::generate(args.begin(), args.end(),
                [&curFrame] { return curFrame.popGetTos(); });

  auto leechObj = curFrame.top();
  auto pClassObj = safeConvertToClass(leechObj, "CallMethod");
  pClassObj->checkMethod(fName);


  curFrame.setRet(state.pc + 1);
  state.funcStack.emplace(fMeta);

  state.getCurFrame().push(pClassObj);
  state.getCurFrame().fillArgs(args.begin(), args.end());
}

} // namespace

const std::unordered_map<leech::Opcodes, leech::Instruction::Callback>
    leech::Instruction::opcToCallback = {
#define LEECH_MAKE_OPCODE(op) {Opcodes::op, execute_##op},
#include "common/opcodes.ii"
#undef LEECH_MAKE_OPCODE
};
