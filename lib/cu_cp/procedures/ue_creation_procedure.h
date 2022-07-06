/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_UE_CREATION_PROCEDURE_H
#define SRSGNB_CU_CP_UE_CREATION_PROCEDURE_H

#include "../cu_cp_manager_config.h"
#include "../cu_cp_manager_interfaces.h"
#include "../log_format.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class ue_creation_procedure
{
public:
  ue_creation_procedure(ue_index_t                     ue_index_candidate,
                        du_cell_index_t                pcell_index,
                        const f1ap_initial_ul_rrc_msg& init_ul_rrc_msg,
                        const cu_cp_manager_config_t&  cfg_,
                        ue_manager_ctrl_configurer&    ue_mng_);
  void               operator()(coro_context<async_task<void>>& ctx);
  static const char* name() { return "UE Create"; }

private:
  void clear_ue()
  {
    // TODO
  }

  void create_srb0();

  const cu_cp_manager_config_t& cfg;
  srslog::basic_logger&         logger;

  f1ap_initial_ul_rrc_msg msg;

  ue_manager_ctrl_configurer& ue_mng;

  ue_context ue_ctx{};
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_CU_CP_UE_CREATION_PROCEDURE_H
