// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/alphabet/nucleotide/rna4.hpp>
#include <seqan3/alphabet/nucleotide/rna5.hpp>
#include <seqan3/alphabet/quality/phred42.hpp>
#include <seqan3/alphabet/quality/qualified.hpp>
#include <seqan3/range/view/persist.hpp>
#include <seqan3/range/view/convert.hpp>

#include "../alignment/aligned_sequence_test_template.hpp"

using namespace seqan3;

template <typename container_type>
    requires AlignedSequence<container_type>
class aligned_sequence<container_type> : public ::testing::Test
{
public:
    // Initializer function is needed for the typed test because the gapped_decorator
    // will be initialized differently than the naive vector<gapped<dna>>.
    void initialise_typed_test_container(container_type & container, dna4_vector const & target)
    {
        container.clear();
        for (auto & val : target)
        {
            container.push_back(assign_char_to(to_char(val), typename container_type::value_type{}));
        }
    }
};

using test_types = ::testing::Types<std::vector<gapped<dna4>>,
                                    std::vector<gapped<qualified<dna4, phred42>>>>;

INSTANTIATE_TYPED_TEST_CASE_P(container_of_gapped_alphabets, aligned_sequence, test_types);
