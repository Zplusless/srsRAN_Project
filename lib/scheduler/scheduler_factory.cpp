/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/scheduler_factory.h"
#include "scheduler_impl.h"

using namespace srsgnb;

std::unique_ptr<mac_scheduler> srsgnb::create_scheduler(const scheduler_config&       sched_cfg,
                                                        sched_configuration_notifier& config_notifier)
{
  return std::make_unique<scheduler_impl>(sched_cfg, config_notifier);
}
