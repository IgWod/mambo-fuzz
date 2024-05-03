/*
  Copyright 2024 Igor Wodiany
  Copyright 2024 The University of Manchester

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifdef PLUGINS_NEW

#include <stdio.h>
#include <assert.h>
#include "../plugins.h"

// Expose LLVM coverage instrumentation signatures. The actual implementation
// should provided independently to the plugin, e.g., libhfuzz (https://github.com/google/honggfuzz).
void __sanitizer_cov_trace_pc_indir(void *callee);
void __sanitizer_cov_trace_pc();

int fuzz_pre_inst_handler(mambo_context *ctx) {
  mambo_branch_type type = mambo_get_branch_type(ctx);
  if (type & BRANCH_RETURN) {
    ; // -fsanitize-coverage=trace-pc,indirect-calls does not instrument returns
  } else if (type & BRANCH_DIRECT) {
    emit_safe_fcall(ctx, __sanitizer_cov_trace_pc, 0);
  } else if (type & BRANCH_INDIRECT) {
    void *inst_source_address = mambo_get_source_addr(ctx);
    a64_instruction inst_type = (a64_instruction) mambo_get_inst(ctx);
    int rn;
    switch (inst_type) {
    case A64_BR:
        a64_BR_decode_fields(inst_source_address, &rn);
        break;
    case A64_BLR:
        a64_BLR_decode_fields(inst_source_address, &rn);
        break;
    default:
        assert(0);
    }
    emit_push(ctx, (1 << x0) | (1 << x1));
    emit_mov(ctx, x0, rn);
    emit_safe_fcall(ctx, __sanitizer_cov_trace_pc_indir, 1);
    emit_pop(ctx, (1 << x0) | (1 << x1));
  }
}

__attribute__((constructor))
void fuzz_init_plugin() {
  mambo_context *ctx = mambo_register_plugin();
  assert(ctx != NULL);

  mambo_register_pre_inst_cb(ctx, &fuzz_pre_inst_handler);
}
#endif
