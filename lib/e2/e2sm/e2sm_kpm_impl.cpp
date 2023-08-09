/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2sm_kpm_impl.h"
#include "e2sm_kpm_report_service_impl.h"
#include "srsran/asn1/asn1_utils.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;
using namespace srsran;

e2sm_kpm_impl::e2sm_kpm_impl(srslog::basic_logger&    logger_,
                             e2sm_handler&            e2sm_packer_,
                             e2_du_metrics_interface& du_metrics_interface_) :
  logger(logger_), e2sm_packer(e2sm_packer_), du_metrics_interface(du_metrics_interface_)
{
  // array of supported metrics in string format
  supported_metrics = {"CQI", "RSRP", "RSRQ"};
}

bool e2sm_kpm_impl::action_supported(const asn1::e2ap::ri_caction_to_be_setup_item_s& ric_action)
{
  auto action_def = e2sm_packer.handle_packed_e2sm_kpm_action_definition(ric_action.ric_action_definition);

  switch (action_def.ric_style_type) {
    case 1:
      return process_action_definition_format1(action_def);
    case 2:
      return process_action_definition_format2(action_def);
    case 3:
      return process_action_definition_format3(action_def);
    case 4:
      return process_action_definition_format4(action_def);
    case 5:
      return process_action_definition_format5(action_def);
    default:
      logger.info("Unknown RIC style type %i -> do not admit action %i (type %i)",
                  action_def.ric_style_type,
                  ric_action.ric_action_id,
                  ric_action.ric_action_type);
  }
  return false;
}

bool e2sm_kpm_impl::cell_supported(const cgi_c& cell_global_id)
{
  // TODO: check if CELL is supported
  return true;
};

bool e2sm_kpm_impl::ue_supported(const ueid_c& ueid)
{
  // TODO: check if UE is supported
  return true;
};

bool e2sm_kpm_impl::test_cond_supported(const asn1::e2sm_kpm::test_cond_type_c& test_cond_type)
{
  // TODO: check if test condition is supported
  return true;
}

bool e2sm_kpm_impl::metric_supported(const meas_type_c&               meas_type,
                                     const e2sm_kpm_label_enum&       label,
                                     const e2sm_kpm_metric_level_enum level,
                                     const bool&                      cell_scope)
{
  for (auto& metric : supported_metrics) {
    if (strcmp(meas_type.meas_name().to_string().c_str(), metric.c_str()) == 0) {
      return true;
    }
  }
  // TODO: check if metric supported with required label, level and cell_scope
  return false;
}

bool e2sm_kpm_impl::process_action_def_meas_info_list(const meas_info_list_l&           meas_info_list,
                                                      const e2sm_kpm_metric_level_enum& level,
                                                      const bool&                       cell_scope)
{
  // process meas_info_list, if at least one metric is not supported do not admit action
  std::map<std::string, e2sm_kpm_label_enum> admitted_value_type_labels;

  for (uint32_t i = 0; i < meas_info_list.size(); i++) {
    meas_type_c meas_type = meas_info_list[i].meas_type;
    std::string meas_name = meas_info_list[i].meas_type.meas_name().to_string();

    for (uint32_t l = 0; l < meas_info_list[i].label_info_list.size(); l++) {
      const meas_label_s& meas_label = meas_info_list[i].label_info_list[l].meas_label;

      // TODO: check all labels
      if (meas_label.no_label_present) {
        if (metric_supported(meas_type, NO_LABEL, level, cell_scope)) {
          admitted_value_type_labels[meas_name] = NO_LABEL;
        } else {
          return false;
        }
      } else {
        logger.debug("Currently only NO_LABEL metric supported");
        return false;
      }
    }
  }

  printf("Admitted action with the following metrics and labels: \n");
  for (const auto& it : admitted_value_type_labels) {
    std::string meas_name = it.first;
    std::string label_str = e2sm_kpm_label_2_str(it.second);
    printf("--- Metric: \"%s\" with value_type_label: %s\n", meas_name.c_str(), label_str.c_str());
  }
  return true;
}

bool e2sm_kpm_impl::process_action_definition_format1(
    const asn1::e2sm_kpm::e2_sm_kpm_action_definition_format1_s& action_definition,
    const e2sm_kpm_metric_level_enum                             level)
{
  bool             cell_scope     = action_definition.cell_global_id_present;
  uint64_t         granul_period  = action_definition.granul_period;
  meas_info_list_l meas_info_list = action_definition.meas_info_list;

  if (granul_period == 0) {
    logger.debug("Action granularity period of %i is not supported -> do not admitted action\n", granul_period);
    return false;
  }

  if (cell_scope) {
    if (not cell_supported(action_definition.cell_global_id)) {
      logger.debug("Cell not available -> do not admitted action\n");
      return false;
    }
  }

  return process_action_def_meas_info_list(meas_info_list, level, cell_scope);
}

bool e2sm_kpm_impl::process_action_definition_format1(const e2_sm_kpm_action_definition_s& action_def_generic)
{
  const e2_sm_kpm_action_definition_format1_s& action_definition =
      action_def_generic.action_definition_formats.action_definition_format1();
  e2sm_kpm_metric_level_enum level = E2_NODE_LEVEL;

  return process_action_definition_format1(action_definition, std::move(level));
}

bool e2sm_kpm_impl::process_action_definition_format2(const e2_sm_kpm_action_definition_s& action_def_generic)
{
  const e2_sm_kpm_action_definition_format2_s& action_definition =
      action_def_generic.action_definition_formats.action_definition_format2();

  e2sm_kpm_metric_level_enum                   level          = UE_LEVEL;
  const ueid_c&                                ueid           = action_definition.ue_id;
  const e2_sm_kpm_action_definition_format1_s& subscript_info = action_definition.subscript_info;

  if (not ue_supported(ueid)) {
    return false;
  }

  return process_action_definition_format1(subscript_info, std::move(level));
}

bool e2sm_kpm_impl::process_action_definition_format3(const e2_sm_kpm_action_definition_s& action_def_generic)
{
  const e2_sm_kpm_action_definition_format3_s& action_definition =
      action_def_generic.action_definition_formats.action_definition_format3();

  bool     cell_scope    = action_definition.cell_global_id_present;
  uint64_t granul_period = action_definition.granul_period;

  if (granul_period == 0) {
    logger.debug("Action granularity period of %i is not supported -> do not admitted action\n", granul_period);
    return false;
  }

  if (cell_scope) {
    if (not cell_supported(action_definition.cell_global_id)) {
      logger.debug("Cell not available -> do not admit action\n");
      return false;
    }
  }

  for (uint32_t i = 0; i < action_definition.meas_cond_list.size(); ++i) {
    const meas_type_c& meas_type = action_definition.meas_cond_list[i].meas_type;
    for (uint32_t m = 0; m < action_definition.meas_cond_list[i].matching_cond.size(); ++m) {
      const matching_cond_item_s&        matching_cond_item = action_definition.meas_cond_list[i].matching_cond[m];
      matching_cond_item_choice_c::types test_type          = matching_cond_item.matching_cond_choice.type();
      if (test_type == matching_cond_item_choice_c::types_opts::test_cond_info) {
        const test_cond_info_s& test_cond_info = matching_cond_item.matching_cond_choice.test_cond_info();
        if (not test_cond_supported(test_cond_info.test_type)) {
          logger.debug("Matching UE test condition not supported -> do not admit action");
          return false;
        }
      } else {
        // test_type == matching_cond_item_choice_c::types_opts::meas_label
        const meas_label_s& meas_label = matching_cond_item.matching_cond_choice.meas_label();
        e2sm_kpm_label_enum label      = asn1_label_2_enum(meas_label);
        if (not metric_supported(meas_type, label, UE_LEVEL, cell_scope)) {
          logger.debug("Matching UE test condition not supported -> do not admit action");
          return false;
        }
      }
    }
  }
  return true;
}

bool e2sm_kpm_impl::process_action_definition_format4(const e2_sm_kpm_action_definition_s& action_def_generic)
{
  const e2_sm_kpm_action_definition_format4_s& action_definition =
      action_def_generic.action_definition_formats.action_definition_format4();

  for (uint32_t i = 0; i < action_definition.matching_ue_cond_list.size(); ++i) {
    const test_cond_type_c& test_cond_type = action_definition.matching_ue_cond_list[i].test_cond_info.test_type;

    if (not test_cond_supported(test_cond_type)) {
      logger.debug("Matching UE test condition not supported -> do not admit action");
      return false;
    }
  }

  e2sm_kpm_metric_level_enum                   level             = UE_LEVEL;
  const e2_sm_kpm_action_definition_format1_s& subscription_info = action_definition.subscription_info;
  return process_action_definition_format1(subscription_info, level);
}

bool e2sm_kpm_impl::process_action_definition_format5(const e2_sm_kpm_action_definition_s& action_def_generic)
{
  const e2_sm_kpm_action_definition_format5_s& action_definition =
      action_def_generic.action_definition_formats.action_definition_format5();

  e2sm_kpm_metric_level_enum level = UE_LEVEL;
  for (uint32_t i = 0; i < action_definition.matching_ueid_list.size(); ++i) {
    const ueid_c& ueid = action_definition.matching_ueid_list[i].ue_id;
    // if at least one UE not present -> do not admit
    if (not ue_supported(ueid)) {
      return false;
    }
  }

  const e2_sm_kpm_action_definition_format1_s& subscript_info = action_definition.subscription_info;
  return process_action_definition_format1(subscript_info, level);
}

std::unique_ptr<e2sm_report_service>
e2sm_kpm_impl::get_e2sm_report_service(const srsran::byte_buffer& action_definition)
{
  e2_sm_kpm_action_definition_s action_def = e2sm_packer.handle_packed_e2sm_kpm_action_definition(action_definition);
  uint32_t                      ric_style_type = action_def.ric_style_type;
  switch (ric_style_type) {
    case 1:
      return std::make_unique<e2sm_kpm_report_service_style1>(std::move(action_def), du_metrics_interface);
      break;
    case 2:
      return nullptr;
      break;
    case 3:
      return std::make_unique<e2sm_kpm_report_service_style3>(std::move(action_def), du_metrics_interface);
      break;
    case 4:
      return nullptr;
      break;
    case 5:
      return nullptr;
      break;
    default:
      logger.info("Unknown RIC style type %i", ric_style_type);
      return nullptr;
  }
}

e2sm_handler& e2sm_kpm_impl::get_e2sm_packer()
{
  return e2sm_packer;
}