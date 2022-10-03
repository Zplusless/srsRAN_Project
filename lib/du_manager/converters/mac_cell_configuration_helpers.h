/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_manager_config.h"
#include "f1c_configuration_helpers.h"
#include "srsgnb/asn1/asn1_utils.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/mac/mac_ue_configurator.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

namespace srsgnb {

namespace test_helpers {

inline mac_ue_create_request_message make_default_ue_creation_request()
{
  mac_ue_create_request_message msg{};

  msg.ue_index   = to_du_ue_index(0);
  msg.crnti      = to_rnti(0x4601);
  msg.cell_index = to_du_cell_index(0);

  msg.serv_cell_cfg.init_dl_bwp.emplace();
  bwp_downlink_dedicated& dl_bwp = *msg.serv_cell_cfg.init_dl_bwp;
  dl_bwp.pdcch_cfg.emplace();
  dl_bwp.pdcch_cfg->coreset_to_addmod_list.emplace_back(config_helpers::make_default_coreset_config());
  coreset_configuration& cs_cfg = dl_bwp.pdcch_cfg->coreset_to_addmod_list.back();
  cs_cfg.id                     = to_coreset_id(1);
  dl_bwp.pdsch_cfg.emplace();
  dl_bwp.pdsch_cfg->data_scrambling_id_pdsch = 0;

  scheduling_request_to_addmod sr_0{.sr_id = scheduling_request_resource_id::SR_ID_MIN, .max_tx = sr_max_tx::n64};
  msg.mac_cell_group_cfg.scheduling_request_config.emplace_back(sr_0);

  dl_bwp.pdcch_cfg->ss_to_addmod_list.emplace_back(config_helpers::make_default_ue_search_space_config());
  return msg;
}

} // namespace test_helpers

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
inline sched_cell_configuration_request_message
make_sched_cell_config_req(du_cell_index_t cell_index, const du_cell_config& du_cfg, unsigned sib1_payload_size)
{
  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index     = cell_index;
  sched_req.pci            = du_cfg.pci;
  sched_req.dl_carrier     = du_cfg.dl_carrier;
  sched_req.ul_carrier     = du_cfg.ul_carrier;
  sched_req.dl_cfg_common  = du_cfg.dl_cfg_common;
  sched_req.ul_cfg_common  = du_cfg.ul_cfg_common;
  sched_req.scs_common     = du_cfg.scs_common;
  sched_req.ssb_config     = du_cfg.ssb_cfg;
  sched_req.dmrs_typeA_pos = du_cfg.dmrs_typeA_pos;

  sched_req.nof_beams     = 1;
  sched_req.nof_ant_ports = 1;
  sched_req.nof_ant_ports = 1;

  /// SIB1 parameters.
  sched_req.coreset0          = du_cfg.coreset0_idx;
  sched_req.searchspace0      = du_cfg.searchspace0_idx;
  sched_req.sib1_mcs          = 5;
  sched_req.sib1_rv           = 0;
  sched_req.sib1_dci_aggr_lev = aggregation_level::n4;
  sched_req.sib1_retx_period  = sib1_rtx_periodicity::ms160;
  sched_req.sib1_payload_size = sib1_payload_size;

  return sched_req;
}

/// Derives MAC Cell Configuration from DU Cell Configuration.
inline mac_cell_creation_request make_mac_cell_config(du_cell_index_t cell_index, const du_cell_config& du_cfg)
{
  mac_cell_creation_request mac_cfg{};
  mac_cfg.cell_index       = cell_index;
  mac_cfg.pci              = du_cfg.pci;
  mac_cfg.scs_common       = du_cfg.scs_common;
  mac_cfg.ssb_cfg          = du_cfg.ssb_cfg;
  mac_cfg.dl_carrier       = du_cfg.dl_carrier;
  mac_cfg.ul_carrier       = du_cfg.ul_carrier;
  mac_cfg.cell_barred      = du_cfg.cell_barred;
  mac_cfg.intra_freq_resel = du_cfg.intra_freq_resel;
  mac_cfg.bcch_dl_sch_payload.append(srs_du::make_asn1_rrc_cell_bcch_dl_sch_msg(du_cfg));
  mac_cfg.sched_req =
      make_sched_cell_config_req(cell_index, du_cfg, static_cast<unsigned>(mac_cfg.bcch_dl_sch_payload.length()));
  return mac_cfg;
}

} // namespace srsgnb
