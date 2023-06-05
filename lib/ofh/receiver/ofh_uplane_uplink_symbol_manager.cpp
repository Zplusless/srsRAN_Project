/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "ofh_uplane_uplink_symbol_manager.h"
#include "srsran/ran/prach/prach_constants.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/resource_block.h"
#include "srsran/support/format_utils.h"

using namespace srsran;
using namespace ofh;

uplane_uplink_symbol_manager::uplane_uplink_symbol_manager(const uplane_uplink_symbol_manager_config& config) :
  du_ul_nof_prbs(config.du_ul_nof_prbs),
  logger(config.logger),
  notifier(config.notifier),
  packet_handler(config.packet_handler),
  prach_repo(config.prach_repo),
  ul_slot_repo(config.ul_slot_repo)
{
}

void uplane_uplink_symbol_manager::on_new_frame(span<const uint8_t> payload)
{
  expected<uplane_message_decoder_results> decoding_results = packet_handler.decode_packet(payload);

  // Do nothing on decoding error.
  if (decoding_results.is_error()) {
    return;
  }

  const uplane_message_decoder_results& results = decoding_results.value();

  // Copy the PRBs into the PRACH buffer.
  if (results.params.filter_index == filter_index_type::ul_prach_preamble_1p25khz) {
    handle_prach_prbs(results);

    return;
  }

  // Copy the PRBs into the resource grid.
  if (results.params.filter_index == filter_index_type::standard_channel_filter) {
    handle_grid_prbs(results);

    return;
  }
}

void uplane_uplink_symbol_manager::handle_prach_prbs(const uplane_message_decoder_results& results)
{
  // NOTE: RU notifies the PRACH in slot 1, but MAC asked for that in slot 0.
  slot_point slot(results.params.slot.numerology(), results.params.slot.sfn(), results.params.slot.subframe_index(), 0);

  ul_prach_context prach_context = prach_repo.get(slot);
  if (prach_context.empty()) {
    return;
  }

  srsran_assert(prach_context.context.nof_fd_occasions == 1, "Only supporting one frequency domain occasion");
  srsran_assert(prach_context.context.nof_td_occasions == 1, "Only supporting one time domain occasion");

  span<cf_t> buffer = prach_context.buffer->get_symbol(prach_context.context.port,
                                                       prach_context.context.nof_fd_occasions - 1,
                                                       prach_context.context.nof_td_occasions - 1,
                                                       results.params.symbol_id);

  // Note assuming all PRBs in 1 packet.
  const uplane_section_params& sect_params        = results.sections.front();
  unsigned                     prach_length_in_re = (is_long_preamble(prach_context.context.format))
                                                        ? prach_constants::LONG_SEQUENCE_LENGTH
                                                        : prach_constants::SHORT_SEQUENCE_LENGTH;

  srsran_assert(is_long_preamble(prach_context.context.format), "SHORT PRACH format not supported");

  const prach_preamble_information& prem_info = get_prach_preamble_long_info(prach_context.context.format);
  unsigned nof_re_to_prach_data = prach_frequency_mapping_get(prem_info.scs, prach_context.context.pusch_scs).k_bar;

  if (sect_params.nof_prbs * NOF_SUBCARRIERS_PER_RB < nof_re_to_prach_data + prach_length_in_re) {
    logger.error("PRACH message segmentation not supported");

    return;
  }

  // Grab the data.
  span<const cf_t> prach_data =
      span<const cf_t>(sect_params.iq_samples).subspan(nof_re_to_prach_data, prach_length_in_re);

  std::copy(prach_data.begin(), prach_data.end(), buffer.begin());
  prach_context.nof_re_written = prach_length_in_re;

  notifier.on_new_prach_window_data(prach_context.context, *(prach_context.buffer));
  prach_context.is_notified = true;
}

void uplane_uplink_symbol_manager::handle_grid_prbs(const uplane_message_decoder_results& results)
{
  const slot_point slot            = results.params.slot;
  ul_slot_context  ul_data_context = ul_slot_repo.get(slot);
  if (ul_data_context.empty()) {
    return;
  }

  const unsigned symbol = results.params.symbol_id;
  for (const auto& sect : results.sections) {
    // Section PRBs are above the last PRB of the DU. Do not copy.
    if (sect.start_prb >= du_ul_nof_prbs) {
      continue;
    }

    // By default, try to copy all the expected PRBs.
    unsigned nof_prbs_to_write = du_ul_nof_prbs - sect.start_prb;

    // Section contains less PRBs than the grid. Copy the whole section.
    if (sect.start_prb + sect.nof_prbs < du_ul_nof_prbs) {
      nof_prbs_to_write = sect.nof_prbs;
    }

    ul_data_context.grid->put(
        0,
        symbol,
        0,
        span<const cf_t>(sect.iq_samples)
            .subspan(sect.start_prb * NOF_SUBCARRIERS_PER_RB, nof_prbs_to_write * NOF_SUBCARRIERS_PER_RB));

    ul_data_context.prb_written[symbol] += du_ul_nof_prbs;
  }

  // Check if all PRBs have been written.
  if (ul_data_context.prb_written[symbol] != du_ul_nof_prbs) {
    return;
  }

  if (!ul_data_context.notified_symbols.test(symbol)) {
    notifier.on_new_uplink_symbol({ul_data_context.context.slot, symbol}, *ul_data_context.grid);
    ul_data_context.notified_symbols.set(symbol);
  }
}