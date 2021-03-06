// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lite/core/mir/subgraph/subgraph_pass.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include "lite/core/mir/pass_registry.h"
#include "lite/core/mir/subgraph/subgraph_detector.h"

namespace paddle {
namespace lite {
namespace mir {

void NPUSubgraphPass::Apply(const std::unique_ptr<SSAGraph>& graph) {
  std::unordered_set<std::string> supported_lists;
#define USE_SUBGRAPH_BRIDGE(op_type, target) supported_lists.insert(#op_type);
#include "lite/kernels/npu/bridges/paddle_use_bridges.h"
#undef USE_SUBGRAPH_BRIDGE
  auto teller = [&](Node* node) {
    if (!node->IsStmt()) return false;
    auto& stmt = node->AsStmt();
    return supported_lists.count(stmt.op_type()) != 0;
  };
  SubgraphFuser fuser(graph.get(), teller, 1 /* min_subgraph_size */);
  fuser();
}

void XPUSubgraphPass::Apply(const std::unique_ptr<SSAGraph>& graph) {
  std::unordered_set<std::string> supported_lists;
#define USE_SUBGRAPH_BRIDGE(op_type, target) supported_lists.insert(#op_type);
#include "lite/kernels/xpu/bridges/paddle_use_bridges.h"
#undef USE_SUBGRAPH_BRIDGE
  auto teller = [&](Node* node) {
    if (!node->IsStmt()) return false;
    auto& stmt = node->AsStmt();
    return supported_lists.count(stmt.op_type()) != 0;
  };
  SubgraphFuser fuser(graph.get(), teller, 1 /* min_subgraph_size */);
  fuser();
}

void BMSubgraphPass::Apply(const std::unique_ptr<SSAGraph>& graph) {
  std::unordered_set<std::string> supported_lists;
#define USE_SUBGRAPH_BRIDGE(op_type, target) supported_lists.insert(#op_type);
#include "lite/kernels/bm/bridges/paddle_use_bridges.h"
#undef USE_SUBGRAPH_BRIDGE
  auto teller = [&](Node* node) {
    if (!node->IsStmt()) return false;
    auto& stmt = node->AsStmt();
    return supported_lists.count(stmt.op_type()) != 0;
  };
  SubgraphFuser fuser(graph.get(), teller, 1 /* min_subgraph_size */);
  fuser();
}

}  // namespace mir
}  // namespace lite
}  // namespace paddle

REGISTER_MIR_PASS(npu_subgraph_pass, paddle::lite::mir::NPUSubgraphPass)
    .BindTargets({TARGET(kNPU)});
REGISTER_MIR_PASS(xpu_subgraph_pass, paddle::lite::mir::XPUSubgraphPass)
    .BindTargets({TARGET(kXPU)});
REGISTER_MIR_PASS(bm_subgraph_pass, paddle::lite::mir::BMSubgraphPass)
    .BindTargets({TARGET(kBM)});
