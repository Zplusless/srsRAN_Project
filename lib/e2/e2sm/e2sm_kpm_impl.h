/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "e2sm_kpm_common.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {

class e2sm_kpm_impl : public e2sm_interface
{
public:
  // constructor takes logger as argument
  e2sm_kpm_impl(srslog::basic_logger&    logger_,
                e2sm_handler&            e2sm_packer_,
                e2_du_metrics_interface& du_metrics_interface_);

  e2sm_handler& get_e2sm_packer() override;

  bool action_supported(const asn1::e2ap::ri_caction_to_be_setup_item_s& ric_action) override;

  std::unique_ptr<e2sm_report_service> get_e2sm_report_service(const srsran::byte_buffer& action_definition) override;

  static bool supported_test_cond_type(asn1::e2sm_kpm::test_cond_type_c test_cond_type)
  {
    if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::cqi) {
      return true;
    } else if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::rsrp) {
      return true;
    } else if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::rsrq) {
      return true;
    } else {
      return false;
    }
  }

private:
  // helper functions to check whether subscription actions are supported
  bool cell_supported(const asn1::e2sm_kpm::cgi_c& cell_global_id);
  bool ue_supported(const asn1::e2sm_kpm::ueid_c& ueid);
  bool test_cond_supported(const asn1::e2sm_kpm::test_cond_type_c& test_cond_type);
  bool metric_supported(const asn1::e2sm_kpm::meas_type_c& meas_type,
                        const e2sm_kpm_label_enum&         label,
                        const e2sm_kpm_metric_level_enum   level,
                        const bool&                        cell_scope);

  bool process_action_def_meas_info_list(const asn1::e2sm_kpm::meas_info_list_l& meas_info_list,
                                         const e2sm_kpm_metric_level_enum&       level,
                                         const bool&                             cell_scope);

  bool process_action_definition_format1(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_format1_s& action_definition,
                                         const e2sm_kpm_metric_level_enum                             level);

  bool process_action_definition_format1(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format2(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format3(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format4(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);
  bool process_action_definition_format5(const asn1::e2sm_kpm::e2_sm_kpm_action_definition_s& action_def_generic);

  srslog::basic_logger&           logger;
  e2sm_handler&                   e2sm_packer;
  e2_du_metrics_interface&        du_metrics_interface;
  std::vector<std::string>        supported_metrics = {};
};

} // namespace srsran
