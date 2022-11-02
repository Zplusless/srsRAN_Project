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

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1c/common/f1ap_packet_procedures.h"
#include "srsgnb/f1c/du/f1ap_du.h"
#include <cstdio>
#include <memory>

namespace srsgnb {

class f1c_du_packet_handler //: public f1ap_input_gateway
{
public:
  explicit f1c_du_packet_handler(std::unique_ptr<f1ap_packet_procedures> procedure_);

  void handle(const asn1::f1ap::f1_ap_pdu_c& pdu); // override;

private:
  srslog::basic_logger& logger;

  std::unique_ptr<f1ap_packet_procedures> procedure;
};

} // namespace srsgnb