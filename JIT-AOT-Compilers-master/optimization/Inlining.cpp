#include "EmptyBlocksRemoval.h"
#include "GraphChecker.h"
#include "Inlining.h"
#include "InstructionBuilder.h"
#include "Traversals.h"


namespace ir {
bool InliningPass::Run() {
    PassManager::Run<RPO>(graph);
    auto instructions_count = graph->CountInstructions();
    if (instructions_count >= maxInstrsAfterInlining) {
        GetLogger(utils::LogPriority::INFO) << "Skip function due to too much instructions: " << instructions_count;
        return false;
    }

    bool inlined = false;
    for (auto *bblock : graph->GetRPO()) {
        for (auto *instr : *bblock) {
            if (!instr->IsCall()) {
                continue;
            }

            auto *call = static_cast<CallInstruction *>(instr);
            auto *calleeGraph = canInlineFunction(call, instructions_count);
            if (!calleeGraph) {
                continue;
            }
            auto *copyGraph = graph->GetCompiler()->CopyGraph(
                calleeGraph,
                graph->GetInstructionBuilder());

            doInlining(call, copyGraph);
            postInlining();
            // TODO: optimize instructions' counting
            instructions_count = graph->CountInstructions();
            inlined = true;
        }
    }

    return inlined;
}

const Graph *InliningPass::canInlineFunction(CallInstruction *call,
                                             size_t callerInstrsCount) {
    // without a fully-functioning IRBuilder InliningPass must rely on compiler
    // already having a Graph for the callee function
    const auto *callee = graph->GetCompiler()->GetFunction(call->GetCallTarget());
    if (callee == nullptr) {
        GetLogger(utils::LogPriority::INFO) << "No IR graph found, skipping. id = " << call->GetCallTarget();
        return nullptr;
    }

    auto instrsCount = callerInstrsCount;
    if (callee != graph) {
        instrsCount = callee->CountInstructions();
    }

    if (instrsCount >= maxCalleeInstrs) {
        GetLogger(utils::LogPriority::INFO) << 
            "Too many instructions: " << instrsCount << " when limit is " << maxCalleeInstrs <<
            ". id = " << call->GetCallTarget();
        return nullptr;
    }
    if (callerInstrsCount + instrsCount >= maxInstrsAfterInlining) {
        GetLogger(utils::LogPriority::INFO) << 
            "Too many instructions after inlining: " << callerInstrsCount + instrsCount <<
            " when limit is " << maxInstrsAfterInlining;
        return nullptr;
    }

    return callee;
}

void InliningPass::doInlining(CallInstruction *call, Graph *callee) {
    ASSERT((call) && (callee));
    graph->SetMarkerIndex(callee->GetMarkerIndex());

    auto *callBlock = call->GetBasicBlock();
    auto *postCallBlock = callBlock->SplitAfterInstruction(call, false);

    propagateArguments(call, callee);
    propagateReturnValue(call, callee, postCallBlock);
    call->RemoveUserFromInputs();
    callBlock->UnlinkInstruction(call);

    inlineReadyGraph(callee, callBlock, postCallBlock);

    GetLogger(utils::LogPriority::INFO) << "Inlined function #" << callee->GetId();
}

void InliningPass::propagateArguments(CallInstruction *call, Graph *callee) {
    auto *bblock = callee->GetFirstBasicBlock();
    ASSERT(bblock);
    auto *argInstr = bblock->GetFirstInstruction();
    for (auto &arg : call->GetInputs()) {
        ASSERT((argInstr) && argInstr->GetOpcode() == Opcode::ARG);
        auto *nextInstr = argInstr->GetNextInstruction();
        argInstr->ReplaceInputInUsers(arg.GetInstruction());
        // unlinking argInstr is redundant because all instructions from the first block
        // will be removed with the block itself
        // bblock->UnlinkInstruction(argInstr);
        argInstr = nextInstr;
    }
}

void InliningPass::propagateReturnValue(CallInstruction *call,
                                        Graph *callee,
                                        BasicBlock *postCallBlock) {
    ASSERT((postCallBlock)
        && (callee->GetLastBasicBlock())
        && !callee->GetLastBasicBlock()->GetPredecessors().empty());
    if (call->GetType() == OperandType::VOID) {
        removeVoidReturns(callee);
        return;
    }

    auto lastBlockPreds = callee->GetLastBasicBlock()->GetPredecessors();
    InstructionBase *newInputForUsers = nullptr;
    if (lastBlockPreds.size() > 1) {
        // in case of multiple returns in callee we must collect all of them into a single
        // PHI instruction, which will be used in caller
        auto *phiReturnValue = callee->GetInstructionBuilder()->CreatePHI(call->GetType());
        for (auto *pred : lastBlockPreds) {
            auto *instr = pred->GetLastInstruction();
            ASSERT((instr) && instr->GetOpcode() == Opcode::RET);

            auto *retInstr = static_cast<RetInstruction *>(instr);
            auto phiInput = retInstr->GetInput(0);
            phiReturnValue->AddPhiInput(phiInput, pred);
            phiInput->RemoveUser(retInstr);
            pred->UnlinkInstruction(retInstr);
        }
        postCallBlock->PushForwardInstruction(phiReturnValue);
        newInputForUsers = phiReturnValue;
    } else {
        ASSERT(lastBlockPreds.size() == 1);
        auto *pred = lastBlockPreds[0];
        auto *instr = pred->GetLastInstruction();
        ASSERT((instr) && instr->GetOpcode() == Opcode::RET);

        auto *retInstr = static_cast<RetInstruction *>(instr);
        newInputForUsers = retInstr->GetInput(0).GetInstruction();
        newInputForUsers->RemoveUser(retInstr);
        pred->UnlinkInstruction(retInstr);
    }
    call->ReplaceInputInUsers(newInputForUsers);
}

void InliningPass::removeVoidReturns(Graph *callee) {
    for (auto *bblock : callee->GetLastBasicBlock()->GetPredecessors()) {
        auto *lastInstr = bblock->GetLastInstruction();
        ASSERT(lastInstr->GetOpcode() == Opcode::RETVOID);
        // void returns can be simply removed as they don't participate in data flow
        bblock->UnlinkInstruction(lastInstr);
    }
}

void InliningPass::moveConstants(Graph *callee) {
    ASSERT(callee && callee->GetFirstBasicBlock());
    auto *firstBlock = graph->GetFirstBasicBlock();
    ASSERT(firstBlock);
    auto *instr = callee->GetFirstBasicBlock()->GetLastInstruction();
    while (instr != nullptr && instr->IsConst()) {
        auto *prev = instr->GetPrevInstruction();
        firstBlock->MoveConstantUnsafe(instr);
        instr = prev;
    }
}

void InliningPass::removeFirstBlock(Graph *callee) {
    ASSERT(callee);
    auto *calleeFirstBlock = callee->GetFirstBasicBlock();
    ASSERT(calleeFirstBlock);
    auto succs = calleeFirstBlock->GetSuccessors();
    ASSERT(succs.size() == 1);

    callee->UnlinkBasicBlock(calleeFirstBlock);
    callee->SetFirstBasicBlock(succs[0]);
    succs[0]->GetPredecessors().clear();
}

void InliningPass::inlineReadyGraph(Graph *callee,
                                    BasicBlock *callBlock,
                                    BasicBlock *postCallBlock) {
    ASSERT((callee) && (callBlock) && (postCallBlock));
    moveConstants(callee);
    removeFirstBlock(callee);
    relinkBasicBlocks(callBlock->GetGraph(), callee);
    graph->ConnectBasicBlocks(callBlock, callee->GetFirstBasicBlock());

    auto *lastBlock = callee->GetLastBasicBlock();
    ASSERT((lastBlock) && lastBlock->IsEmpty() && !lastBlock->GetPredecessors().empty());
    for (auto *prevLast : lastBlock->GetPredecessors()) {
        prevLast->ReplaceSuccessor(lastBlock, postCallBlock);
        postCallBlock->AddPredecessor(prevLast);
    }
}

void InliningPass::relinkBasicBlocks(Graph *callerGraph, Graph *calleeGraph) {
    ASSERT((callerGraph) && (calleeGraph));
    auto doRelink = [&callerGraph](BasicBlock *bblock) {
        if (!bblock->IsLastInGraph()) {
            callerGraph->AddBasicBlock(bblock);
        }
    };
    calleeGraph->ForEachBasicBlock(doRelink);
}

void InliningPass::postInlining() {
    ASSERT(PassManager::Run<GraphChecker>(graph));

    PassManager::SetInvalid<
        AnalysisFlag::DOM_TREE,
        AnalysisFlag::LOOP_ANALYSIS,
        AnalysisFlag::RPO>(graph);

    // TODO: may move post-pass routine into PassBase by providing type traits
    PassManager::Run<EmptyBlocksRemoval>(graph);

    ASSERT(PassManager::Run<GraphChecker>(graph));
}
}   // namespace ir
