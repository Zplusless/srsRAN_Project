/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_reconfiguration_procedure.h"
#include "mac_config.h"
#include "mac_scheduler_configurator.h"
#include "proc_logger.h"

using namespace srsran;

mac_ue_reconfiguration_procedure::mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request& req_,
                                                                   mac_control_config&                   cfg_,
                                                                   mac_ul_configurator&                  mac_ul_,
                                                                   mac_dl_configurator&                  mac_dl_,
                                                                   mac_scheduler_configurator&           sched_cfg_) :
  req(req_), cfg(cfg_), logger(cfg.logger), ul_unit(mac_ul_), dl_unit(mac_dl_), sched_cfg(sched_cfg_)
{
}

void mac_ue_reconfiguration_procedure::operator()(coro_context<async_task<mac_ue_reconfiguration_response>>& ctx)
{
  CORO_BEGIN(ctx);
  logger.debug("{}: started...", mac_log_prefix(req.ue_index, req.crnti, name()));

  // If there are bearers to add or modify.
  if (not req.bearers_to_addmod.empty()) {
    // > Add/Mod logical channels in the DEMUX.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.addmod_bearers(req.ue_index, req.bearers_to_addmod));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }

    // > Add/Mod logical channels in the MUX.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.addmod_bearers(req.ue_index, req.pcell_index, req.bearers_to_addmod));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }
  }

  // > Reconfigure UE in Scheduler.
  logger.debug("{}: started...", mac_log_prefix(req.ue_index, req.crnti, "Sched UE Config"));
  CORO_AWAIT_VALUE(sched_conf_res, sched_cfg.handle_ue_reconfiguration_request(req));
  logger.info("{}: successfully finished", mac_log_prefix(req.ue_index, req.crnti, "Sched UE Config"));
  if (not sched_conf_res) {
    CORO_EARLY_RETURN(handle_result(false));
  }

  // If there are bearers to remove.
  if (not req.bearers_to_rem.empty()) {
    // > Remove logical channels in the DEMUX.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.remove_bearers(req.ue_index, req.bearers_to_rem));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }

    // > Remove logical channels in the MUX.
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.remove_bearers(req.ue_index, req.pcell_index, req.bearers_to_rem));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_result(false));
    }
  }

  // > After UE insertion, send response to DU manager.
  CORO_RETURN(handle_result(true));
}

mac_ue_reconfiguration_response mac_ue_reconfiguration_procedure::handle_result(bool result)
{
  if (result) {
    logger.info("{}: finished successfully", mac_log_prefix(req.ue_index, req.crnti, name()));
  } else {
    logger.warning("{}: failed", mac_log_prefix(req.ue_index, req.crnti, name()));
  }

  // Respond back to DU manager with result
  mac_ue_reconfiguration_response resp{};
  resp.ue_index = req.ue_index;
  resp.result   = result;
  return resp;
}
