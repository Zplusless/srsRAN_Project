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

#include "lib/pdcp/pdcp_entity_tx.h"
#include "pdcp_test_vectors.h"
#include "pdcp_tx_test_helpers.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

namespace srsgnb {

/// Fixture class for PDCP tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_tx_metrics_test : public pdcp_tx_test_helper,
                             public ::testing::Test,
                             public ::testing::WithParamInterface<pdcp_sn_size>
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("PDCP", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};
} // namespace srsgnb