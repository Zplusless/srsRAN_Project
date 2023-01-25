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

#include "lib/rlc/rlc_bearer_logger.h"
#include "rlc_stress_test_args.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/srslog/srslog.h"
#include <random>

namespace srsgnb {
class f1_dummy : public pdcp_tx_lower_notifier,
                 public rlc_tx_upper_layer_data_notifier,
                 public rlc_tx_upper_layer_control_notifier,
                 public rlc_rx_upper_layer_data_notifier
{
  rlc_bearer_logger logger;

  rlc_tx_upper_layer_data_interface* rlc_tx_upper  = nullptr;
  pdcp_rx_lower_interface*           pdcp_rx_lower = nullptr;

public:
  f1_dummy(uint32_t id) : logger("F1", {id, drb_id_t::drb1}) {}

  // PDCP -> F1 -> RLC
  void on_new_pdu(pdcp_tx_pdu pdu) final
  {
    rlc_sdu sdu = {};
    sdu.buf     = std::move(pdu.buf);
    sdu.pdcp_sn = pdu.pdcp_sn;
    logger.log_info("Passing F1 PDU to RLC");
    rlc_tx_upper->handle_sdu(std::move(sdu));
  }

  // PDCP -> F1 -> RLC
  void on_discard_pdu(uint32_t pdcp_sn) final
  {
    logger.log_debug("Discard PDU called");
    // TODO
  }

  // RLC -> F1 -> PDCP
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn) final
  {
    logger.log_debug("Transmitted SDU called");
    // TODO
  }

  // RLC -> F1 -> PDCP
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) final
  {
    logger.log_debug("Delivered SDU called");
    // TODO
  }

  // RLC -> F1 -> PDCP
  void on_new_sdu(byte_buffer_slice_chain pdu) final
  {
    logger.log_debug("Passing SDU to PDCP");
    // TODO for now we copy to a new byte buffer
    byte_buffer buf;
    for (uint8_t byte : pdu) {
      buf.append(byte);
    }
    pdcp_rx_lower->handle_pdu(byte_buffer_slice_chain{std::move(buf)});
  }

  // RLC -> F1 -> RRC
  void on_protocol_failure() final {}
  void on_max_retx() final {}

  void set_rlc_tx_upper_data(rlc_tx_upper_layer_data_interface* rlc_tx_upper_) { this->rlc_tx_upper = rlc_tx_upper_; }

  void set_pdcp_rx_lower(pdcp_rx_lower_interface* pdcp_rx_lower_) { this->pdcp_rx_lower = pdcp_rx_lower_; }
};

} // namespace srsgnb
