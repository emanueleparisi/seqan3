// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Sara Hetzel <sara.hetzel AT fu-berlin.de>
 * \brief Provides seqan3::view::translate_join.
 */

#pragma once

#include <vector>
#include <stdexcept>

#include <seqan3/core/type_traits/range.hpp>
#include <seqan3/range/container/small_string.hpp>
#include <seqan3/range/detail/random_access_iterator.hpp>
#include <seqan3/range/view/translate.hpp>
#include <seqan3/std/concepts>
#include <seqan3/std/ranges>

namespace seqan3::detail
{

// ============================================================================
//  view_translate_join (range definition)
// ============================================================================

/*!\brief The return type of seqan3::view::translate_join.
 * \implements std::ranges::View
 * \implements std::ranges::SizedRange
 * \implements std::ranges::RandomAccessRange
 * \tparam urng_t The type of the range being translated.
 * \param[in] tf Translation frames to be used.
 * \ingroup view
 */
template <std::ranges::View urng_t>
class view_translate_join : public ranges::view_base
{
private:
    //!\brief The data members of view_translate_join.
    urng_t urange;
    //!\brief The frames that should be used for translation.
    translation_frames tf;
    //!\brief The selected frames corresponding to the frames required.
    small_vector<translation_frames, 6> selected_frames{};

protected:
    /*!\name Associated types
     * \{
     */
    //!\brief The reference_type.
    using reference         = view_translate_single<std::ranges::all_view<reference_t<urng_t>>>;
    //!\brief The const_reference type.
    using const_reference   = reference;
    //!\brief The value_type (which equals the reference_type with any references removed).
    using value_type        = reference;
    //!\brief The size_type.
    using size_type         = size_type_t<reference_t<urng_t>>;
    //!\brief A signed integer type, usually std::ptrdiff_t.
    using difference_type   = difference_type_t<reference_t<urng_t>>;
    //!\brief The iterator type of this view (a random access iterator).
    using iterator          = detail::random_access_iterator<view_translate_join>;
    //!\brief The const iterator type of this view (same as iterator, because it's a view).
    using const_iterator    = detail::random_access_iterator<view_translate_join const>;
    //!\}

public:

    static_assert(dimension_v<urng_t> == 2,
        "This adaptor only handles range-of-range (two dimensions) as input.");
    static_assert(std::ranges::ViewableRange<urng_t>,
        "The range parameter to view::translate_join cannot be a temporary of a non-view range.");
    static_assert(std::ranges::ViewableRange<reference_t<urng_t>>,
        "The inner range of the range parameter to view::translate_join cannot be a temporary of a non-view range.");
    static_assert(std::ranges::SizedRange<urng_t>,
        "The range parameter to view::translate_join must model std::ranges::SizedRange.");
    static_assert(std::ranges::SizedRange<reference_t<urng_t>>,
        "The inner range of the range parameter to view::translate_join must model std::ranges::SizedRange.");
    static_assert(std::ranges::RandomAccessRange<urng_t>,
        "The range parameter to view::translate_join must model std::ranges::RandomAccessRange.");
    static_assert(std::ranges::RandomAccessRange<reference_t<urng_t>>,
        "The inner range of the range parameter to view::translate_join must model std::ranges::RandomAccessRange.");
    static_assert(NucleotideAlphabet<reference_t<reference_t<urng_t>>>,
        "The range parameter to view::translate_join must be over a range over elements of seqan3::NucleotideAlphabet.");

    /*!\name Constructors, destructor and assignment
     * \{
     */
    view_translate_join()                                                      noexcept = default; //!< Defaulted.
    constexpr view_translate_join(view_translate_join const & rhs)             noexcept = default; //!< Defaulted.
    constexpr view_translate_join(view_translate_join && rhs)                  noexcept = default; //!< Defaulted.
    constexpr view_translate_join & operator=(view_translate_join const & rhs) noexcept = default; //!< Defaulted.
    constexpr view_translate_join & operator=(view_translate_join && rhs)      noexcept = default; //!< Defaulted.
    ~view_translate_join()                                                     noexcept = default; //!< Defaulted.

    /*!\brief Construct from another view.
     * \param[in] _urange The underlying range (of ranges).
     * \param[in] _tf The frames that should be used for translation.
     */
    view_translate_join(urng_t _urange, translation_frames const _tf = translation_frames::SIX_FRAME)
        : urange{std::move(_urange)}, tf{_tf}
    {
        if ((_tf & translation_frames::FWD_FRAME_0) == translation_frames::FWD_FRAME_0)
            selected_frames.push_back(translation_frames::FWD_FRAME_0);
        if ((_tf & translation_frames::FWD_FRAME_1) == translation_frames::FWD_FRAME_1)
            selected_frames.push_back(translation_frames::FWD_FRAME_1);
        if ((_tf & translation_frames::FWD_FRAME_2) == translation_frames::FWD_FRAME_2)
            selected_frames.push_back(translation_frames::FWD_FRAME_2);
        if ((_tf & translation_frames::REV_FRAME_0) == translation_frames::REV_FRAME_0)
            selected_frames.push_back(translation_frames::REV_FRAME_0);
        if ((_tf & translation_frames::REV_FRAME_1) == translation_frames::REV_FRAME_1)
            selected_frames.push_back(translation_frames::REV_FRAME_1);
        if ((_tf & translation_frames::REV_FRAME_2) == translation_frames::REV_FRAME_2)
            selected_frames.push_back(translation_frames::REV_FRAME_2);
    }

    /*!\brief Construct from another range.
     * \param[in] _urange The underlying range (of ranges).
     * \param[in] _tf The frames that should be used for translation.
     */
    template <typename rng_t>
    //!\cond
        requires !std::Same<remove_cvref_t<rng_t>, view_translate_join> &&
                 std::ranges::ViewableRange<rng_t> &&
                 std::Constructible<urng_t, ranges::ref_view<std::remove_reference_t<rng_t>>>
    //!\endcond
    view_translate_join(rng_t && _urange, translation_frames const _tf = translation_frames::SIX_FRAME)
     : view_translate_join{std::view::all(std::forward<rng_t>(_urange)), _tf}
    {}
    //!\}

    /*!\name Iterators
     * \{
     */
    /*!\brief Returns an iterator to the first element of the container.
     * \returns Iterator to the first element.
     *
     * If the container is empty, the returned iterator will be equal to end().
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    iterator begin() noexcept
    {
        return {*this, 0};
    }

    //!\overload
    const_iterator begin() const noexcept
        requires ConstIterableRange<urng_t>
    {
        return {*this, 0};
    }

    //!\overload
    const_iterator cbegin() const noexcept
        requires ConstIterableRange<urng_t>
    {
        return begin();
    }

    /*!\brief Returns an iterator to the element following the last element of the container.
     * \returns Iterator to the first element.
     *
     * This element acts as a placeholder; attempting to dereference it results in undefined behaviour.
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    iterator end() noexcept
    {
        return {*this, size()};
    }

    //!\overload
    const_iterator end() const noexcept
        requires ConstIterableRange<urng_t>
    {
        return {*this, size()};
    }

    //!\overload
    const_iterator cend() const noexcept
        requires ConstIterableRange<urng_t>
    {
        return end();
    }
    //!\}

    /*!\brief Returns the number of elements in the view.
     * \returns The number of elements in the container.
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    size_type size() noexcept
    {
        return (size_type) seqan3::size(urange) * selected_frames.size();
    }

    //!\overload
    size_type size() const noexcept
        requires ConstIterableRange<urng_t>
    {
        return (size_type) seqan3::size(urange) * selected_frames.size();
    }

    /*!\name Element access
     * \{
     */
    /*!\brief Return the n-th element.
     * \param[in] n The element to retrieve.
     * \returns Either a writable proxy to the element or a copy (if called in const context).
     *
     * Accessing an element behind the last causes undefined behaviour. In debug mode an assertion checks the size of
     * the container.
     *
     * ### Exceptions
     *
     * Strong exception guarantee (never modifies data).
     *
     * ### Complexity
     *
     * Constant.
     */
    reference operator[](size_type const n)
    {
        assert(n < size());
        size_type index_frame = n % selected_frames.size();
        size_type index_urange = (n - index_frame) / selected_frames.size();
        return urange[index_urange] | view::translate_single(selected_frames[index_frame]);
    }

    //!\overload
    const_reference operator[](size_type const n) const
        requires ConstIterableRange<urng_t>
    {
        assert(n < size());
        size_type index_frame = n % selected_frames.size();
        size_type index_urange = (n - index_frame) / selected_frames.size();
        return urange[index_urange] | view::translate_single(selected_frames[index_frame]);
    }
    //!\}
};

//!\brief Class template argument deduction for view_translate_join.
template <typename urng_t>
view_translate_join(urng_t &&, translation_frames const = translation_frames{}) -> view_translate_join<std::ranges::all_view<urng_t>>;

// ============================================================================
//  translate_fn (adaptor definition for both views)
// ============================================================================

//!\brief Definition of the range adaptor object type for seqan3::view::translate_join.
struct translate_join_fn
{
    //!\brief Store the argument and return a range adaptor closure object.
    constexpr auto operator()(translation_frames const tf = translation_frames::SIX_FRAME) const
    {
        return detail::adaptor_from_functor{*this, tf};
    }

    /*!\brief            Directly return an instance of the view, initialised with the given parameters.
     * \param[in] urange The underlying range.
     * \param[in] tf     The frame that should be used for translation.
     * \returns          A range of translated sequence(s).
     */
    template <std::ranges::Range urng_t>
    constexpr auto operator()(urng_t && urange, translation_frames const tf = translation_frames::SIX_FRAME) const
    {
        static_assert(dimension_v<urng_t> == 2,
            "This adaptor only handles range-of-range (two dimensions) as input.");
        static_assert(std::ranges::ViewableRange<urng_t>,
            "The range parameter to view::translate_join cannot be a temporary of a non-view range.");
        static_assert(std::ranges::ViewableRange<reference_t<urng_t>>,
            "The inner range of the range parameter to view::translate_join cannot be a temporary of a non-view range.");
        static_assert(std::ranges::SizedRange<urng_t>,
            "The range parameter to view::translate_join must model std::ranges::SizedRange.");
        static_assert(std::ranges::SizedRange<reference_t<urng_t>>,
            "The inner range of the range parameter to view::translate_join must model std::ranges::SizedRange.");
        static_assert(std::ranges::RandomAccessRange<urng_t>,
            "The range parameter to view::translate_join must model std::ranges::RandomAccessRange.");
        static_assert(std::ranges::RandomAccessRange<reference_t<urng_t>>,
            "The inner range of the range parameter to view::translate_join must model std::ranges::RandomAccessRange.");
        static_assert(NucleotideAlphabet<reference_t<reference_t<urng_t>>>,
            "The range parameter to view::translate_join must be over a range over elements of seqan3::NucleotideAlphabet.");

        return detail::view_translate_join{std::forward<urng_t>(urange), tf};
    }

    //!\brief This adaptor is usable without setting the frames parameter in which case the default is chosen.
    template <std::ranges::Range urng_t>
    constexpr friend auto operator|(urng_t && urange, translate_join_fn const & me)
    {
        return me(std::forward<urng_t>(urange));
    }
};

} // namespace seqan3::detail

// ============================================================================
//  translate (adaptor object)
// ============================================================================

namespace seqan3::view
{

/*!\name Alphabet related views
 * \{
 */

/*!\brief A view that translates nucleotide into aminoacid alphabet with 1, 2, 3 or 6 frames. Input and output range are always two-dimensional.
 * \tparam urng_t The type of the range being processed.
 * \param[in] urange The range being processed. Needs to be a range of ranges (two-dimensional).
 * \param[in] tf A value of seqan3::tanslation_frames that indicates the desired frames.
 * \returns A range of ranges containing frames with aminoacid sequence. See below for the properties of the returned range.
 * \ingroup view
 *
 * \details
 *
 * This view can be used to translate nucleotide sequences into aminoacid sequences (see translation_frames for possible combination of frames).
 * This view only operates on two-dimensional input (range of ranges) and outputs a range of ranges no matter the number of input sequences or
 * the number of translation frames given. Therefore, it has the same capabilities as the standard view_translate but concatenates the different
 * frames of the different input sequences rather than having a separate range for each input sequence containing the translated frames. In the output,
 * frames are ordered in a way, that all requested frames are listed per sequence directly after each other in the order of input sequences.
 * improved and efficient downstream post-processing if needed. However, the index of a frame for a specific sequence needs to be calculated via
 * modulo operations in this case. The i-th frame of the j-th sequence can be calculated by n = (i * s) + j, where s is the number of frames used
 * for translation (index starting at zero).
 *
 * In short, this views behaves the same as:
 * ```cpp
 * std::vector<std::vector<dna5>> vec {...};
 * auto v = vec | view::translate | std::view::join;
 * Except that the performance is better and the returned range still models std::ranges::RandomAccessRange and std::ranges::SizedRange.
 * ```
 *
 * **Header**
 * ```cpp
 *      #include <seqan3/range/view/translate_join.hpp>
 * ```
 *
 * ### View properties
 *
 * | range concepts and reference_t  | `urng_t` (underlying range type)      | `rrng_t` (returned range type)                     |
 * |---------------------------------|:-------------------------------------:|:--------------------------------------------------:|
 * | std::ranges::InputRange         | *required*                            | *preserved*                                        |
 * | std::ranges::ForwardRange       | *required*                            | *preserved*                                        |
 * | std::ranges::BidirectionalRange | *required*                            | *preserved*                                        |
 * | std::ranges::RandomAccessRange  | *required*                            | *preserved*                                        |
 * | std::ranges::ContiguousRange    |                                       | *lost*                                             |
 * |                                 |                                       |                                                    |
 * | std::ranges::ViewableRange      | *required*                            | *guaranteed*                                       |
 * | std::ranges::View               |                                       | *guaranteed*                                       |
 * | std::ranges::SizedRange         | *required*                            | *preserved*                                        |
 * | std::ranges::CommonRange        |                                       | *guaranteed*                                       |
 * | std::ranges::OutputRange        |                                       | *lost*                                             |
 * | seqan3::ConstIterableRange      | *required*                            | *preserved*                                        |
 * |                                 |                                       |                                                    |
 * | seqan3::reference_t             | seqan3::NucleotideAlphabet            | std::ranges::View && std::ranges::RandomAccessRange && std::ranges::SizedRange |
 *
 * * `urng_t` is the type of the range modified by this view (input).
 * * `rrng_t` is the type of the range returned by this view.
 * * for more details, see \ref view.
 *
 * ### Example
 *
 * Operating on a range of seqan3::dna5:
 * \snippet test/snippet/range/view/translate_join.cpp example
 * \hideinitializer
 */
inline constexpr auto translate_join = detail::translate_join_fn{};
//!\}

} // namespace seqan3::view
