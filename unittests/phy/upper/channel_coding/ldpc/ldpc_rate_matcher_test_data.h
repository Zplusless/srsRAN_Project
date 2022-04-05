#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_MATCHER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_MATCHER_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsLDPCRateMatcherUnittest.m"

#include "srsgnb/phy/modulation_scheme.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  unsigned             rm_length  = 0;
  unsigned             rv         = 0;
  modulation_scheme    mod_scheme = {};
  unsigned             n_ref      = 0;
  bool                 is_lbrm    = false;
  unsigned             nof_filler = 0;
  file_vector<uint8_t> full_cblock;
  file_vector<uint8_t> rm_cblock;
};

static const std::vector<test_case_t> ldpc_rate_matcher_test_data = {
    // clang-format off
  {462, 0, modulation_scheme::BPSK, 700, false, 28, {"ldpc_rate_matcher_test_input0.dat"}, {"ldpc_rate_matcher_test_output0.dat"}},
  {738, 1, modulation_scheme::QPSK, 700, true, 28, {"ldpc_rate_matcher_test_input1.dat"}, {"ldpc_rate_matcher_test_output1.dat"}},
  {924, 2, modulation_scheme::QAM16, 700, false, 28, {"ldpc_rate_matcher_test_input2.dat"}, {"ldpc_rate_matcher_test_output2.dat"}},
  {1104, 3, modulation_scheme::QAM64, 700, false, 28, {"ldpc_rate_matcher_test_input3.dat"}, {"ldpc_rate_matcher_test_output3.dat"}},
  {344, 1, modulation_scheme::QAM256, 700, false, 12, {"ldpc_rate_matcher_test_input4.dat"}, {"ldpc_rate_matcher_test_output4.dat"}},
  {560, 0, modulation_scheme::QAM16, 700, true, 12, {"ldpc_rate_matcher_test_input5.dat"}, {"ldpc_rate_matcher_test_output5.dat"}},
  {700, 3, modulation_scheme::BPSK, 700, true, 12, {"ldpc_rate_matcher_test_input6.dat"}, {"ldpc_rate_matcher_test_output6.dat"}},
  {840, 2, modulation_scheme::QPSK, 700, true, 12, {"ldpc_rate_matcher_test_input7.dat"}, {"ldpc_rate_matcher_test_output7.dat"}},
  {696, 0, modulation_scheme::QAM64, 700, true, 12, {"ldpc_rate_matcher_test_input8.dat"}, {"ldpc_rate_matcher_test_output8.dat"}},
  {1104, 0, modulation_scheme::QAM256, 700, true, 28, {"ldpc_rate_matcher_test_input9.dat"}, {"ldpc_rate_matcher_test_output9.dat"}},
  {924, 1, modulation_scheme::BPSK, 700, false, 28, {"ldpc_rate_matcher_test_input10.dat"}, {"ldpc_rate_matcher_test_output10.dat"}},
  {840, 1, modulation_scheme::QAM16, 700, true, 12, {"ldpc_rate_matcher_test_input11.dat"}, {"ldpc_rate_matcher_test_output11.dat"}},
  {462, 2, modulation_scheme::QAM64, 700, true, 28, {"ldpc_rate_matcher_test_input12.dat"}, {"ldpc_rate_matcher_test_output12.dat"}},
  {560, 2, modulation_scheme::BPSK, 700, false, 12, {"ldpc_rate_matcher_test_input13.dat"}, {"ldpc_rate_matcher_test_output13.dat"}},
  {462, 3, modulation_scheme::QPSK, 700, false, 28, {"ldpc_rate_matcher_test_input14.dat"}, {"ldpc_rate_matcher_test_output14.dat"}},
  {560, 3, modulation_scheme::QAM256, 700, true, 12, {"ldpc_rate_matcher_test_input15.dat"}, {"ldpc_rate_matcher_test_output15.dat"}},
  {1108, 0, modulation_scheme::BPSK, 700, false, 28, {"ldpc_rate_matcher_test_input16.dat"}, {"ldpc_rate_matcher_test_output16.dat"}},
  {700, 0, modulation_scheme::QPSK, 700, true, 12, {"ldpc_rate_matcher_test_input17.dat"}, {"ldpc_rate_matcher_test_output17.dat"}},
  {460, 3, modulation_scheme::QAM16, 700, false, 28, {"ldpc_rate_matcher_test_input18.dat"}, {"ldpc_rate_matcher_test_output18.dat"}},
  {558, 1, modulation_scheme::QAM64, 700, true, 12, {"ldpc_rate_matcher_test_input19.dat"}, {"ldpc_rate_matcher_test_output19.dat"}},
  {920, 2, modulation_scheme::QAM256, 700, false, 28, {"ldpc_rate_matcher_test_input20.dat"}, {"ldpc_rate_matcher_test_output20.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_MATCHER_TEST_DATA_H
