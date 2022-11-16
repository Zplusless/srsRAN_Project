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

/// \file
/// \brief Configuration structs passed to scheduler implementation.

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/sch_mcs.h"

namespace srsgnb {

struct scheduler_ue_config {
  optional<sch_mcs_index> fixed_dl_mcs;
  optional<sch_mcs_index> fixed_ul_mcs;
  unsigned                max_nof_harq_retxs;
};

struct scheduler_ra_config {
  sch_mcs_index rar_mcs_index;
  sch_mcs_index msg3_mcs_index;
  unsigned      max_nof_msg3_harq_retxs;
};

struct scheduler_config {
  scheduler_ra_config ra;
  scheduler_ue_config ue;
};

} // namespace srsgnb