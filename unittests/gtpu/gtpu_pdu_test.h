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

#include "../../lib/gtpu/gtpu_pdu.h"
#include "srsgnb/gtpu/gtpu_dl.h"
#include "srsgnb/gtpu/gtpu_ul.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>

namespace srsgnb {

const uint8_t gtpu_ping_vec[] = {
    0x30, 0xff, 0x00, 0x54, 0x00, 0x00, 0x00, 0x01, 0x45, 0x00, 0x00, 0x54, 0xe8, 0x83, 0x40, 0x00, 0x40, 0x01, 0xfa,
    0x00, 0xac, 0x10, 0x00, 0x03, 0xac, 0x10, 0x00, 0x01, 0x08, 0x00, 0x2c, 0xbe, 0xb4, 0xa4, 0x00, 0x01, 0xd3, 0x45,
    0x61, 0x63, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x20, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

class gtpu_test_dl : public gtpu_dl_lower_layer_notifier
{
  void on_new_pdu(byte_buffer buf) final {}
};
class gtpu_test_ul : public gtpu_ul_upper_layer_notifier
{
  void on_new_sdu(byte_buffer buf) final {}
};

/// Fixture class for GTP-U PDU tests
class gtpu_test : public ::testing::Test
{
public:
  gtpu_test() :
    logger(srslog::fetch_basic_logger("TEST", false)), gtpu_logger(srslog::fetch_basic_logger("GTPU", false))
  {
  }

protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init GTPU logger
    gtpu_logger.set_level(srslog::basic_levels::debug);
    gtpu_logger.set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  // Test logger
  srslog::basic_logger& logger;

  // GTP-U logger
  srslog::basic_logger& gtpu_logger;
};
} // namespace srsgnb
