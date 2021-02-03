/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
//mod alter the rng for better randomness
// note all you have to to is a global find and replace g_Dice with whatever you choose as the name of the variable.  I declared this in main as g_Dice and just let it be external.

#ifndef _H_CRNG
#define _H_CRNG

#include <initializer_list>
#include <random>
#include <array>
#include <cassert>
#include <iterator>

#include <utils/sPercent.h>
 
struct cRng
{
  std::ranlux48 gen_;

/*
 *    changed this to be the base random number generator 
 *    because it's easier to build other funcs around a
 *    function than an operator. Well, less messy, anyway.
 */

    /// Returns a random integer `x` within `0 <= x < a` (a half-open
    /// interval).
    ///
    /// All integers within the interval has the same probability.

    // @{
    int random(int a)
    {
        // -1 because `uniform_int_distribution` works with closed
        // intervals.
        std::uniform_int_distribution<> dist(0, a-1);
        return dist(gen_);
    }

    /*
 *    modulus operator re-implemented using random()
 */
    int operator %(int n) {
        return  random(n);
    }
    // @}

    /// Returns a random real `x` within `0 <= x < a` (a half-open
    /// interval).
    ///
    /// All values within the interval has the same probability.
    double randomd(double a) // returns double between 0 and a - `J` added
    {
        std::uniform_real_distribution<> dist(0, a);
        return dist(gen_);
    }

/*
 *    returns a number randomly distributed between
 *    min and max.
 *
 *    if min > max, then returns number in the range 0 to 100
 *    in order to replicate how the girl stat generation works
 */

    /// Returns a random integer `x` within `min <= x < max` (a
    /// half-open interval).
    ///
    /// All integers within the interval has the same probability.
    ///
    /// Note: If `min > max` then the interval `0 <= x < range` is
    /// used instead. This replicates the girl stat generation
    /// algorithm.
    int in_range(int min, int max, int range=101);

    /// Returns a random integer `x` within `lo <= x <= hi` (a closed
    /// interval).
    ///
    /// All integers within the interval have the same probability.
    int flat(int lo, int hi)
    {
        std::uniform_int_distribution<> dist(lo, hi);
        return dist(gen_);
    }

    /// Returns a random real number `x` within `lo <= x < hi` (a
    /// half-open interval).
    ///
    /// All reals within the interval have the same probability.
    double flat(double lo, double hi)
    {
#if 0
       // An known bug on many platforms makes this occasionally
       // return `hi`. (See 'Notes' in
       // <https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution>.)
        std::uniform_real_distribution<> dist(lo, hi);
        return dist(gen_);
#else
        auto hi1 = std::nextafter(hi, std::numeric_limits<double>::min());
        if(lo == hi1) return lo; // singleton interval.

        std::uniform_real_distribution<> dist(lo, hi);
        auto val = dist(gen_);
        while(val == hi)        // retry
           val = dist(gen_);

        return val;
#endif
    }

/*
 *    `J` trying to add a bell curve
*/


    /// Returns a random integer `x` within `min <= x < max`.
    ///
    /// The distribution has a triangular shape with the peak (== avg)
    /// at mid-point `(min + max)/2`.
    ///
    /// \note I believe the author was trying for a Gaussian ("bell")
    /// distribution.
    int bell(int min, int max);

    /// Returns a random integer `x` within `lo <= x <= hi` (a closed
    /// interval).
    ///
    /// This distribution is better at faking a Gaussian distribution,
    /// with the mid-point `(lo + hi)/2` as the average.
    int gauss(int lo, int hi);

/*
 *    returns true n percent of the time. 
 *    so g_Dice.percent(20) will return true 20% of the time
 */

    /// Returns `true` with a probability `p/100` (or `p` percents).

    // @{

    bool percent(int p)
    {
       std::bernoulli_distribution dist((double)p / 100.0);
       return dist(gen_);
    }

    bool percent(double p)
    {
       std::bernoulli_distribution dist(p / 100.0);
       return dist(gen_);
    }

    bool percent(sPercent p)
    {
       std::bernoulli_distribution dist((double)(float)p);
       return dist(gen_);
    }
    // @}

/*
 *    we generate d100 rolls a lot
 */
    /// Simulates a d100 die.
    ///
    /// The returned random integer `x` is within `1 <= x <= 100`.
    int d100() { return flat(1, 100); }
/*
 *    constructor and destructor
 */
    cRng()  : gen_(std::random_device()()) {}
    ~cRng() = default;

private:
   // one_of_
   //
   /// Helper functions for `one_of()`.
   ///
   /// These helpers take a third parameter for type dispatch, and
   /// then do the actual work.

   // @{

   template<class RandomIt>
   auto&& one_of_(RandomIt first, RandomIt last,
                  std::random_access_iterator_tag)
   {
      assert(first != last);
      return first[random(last - first)];
   }

   // Effectively, a reimplementation of RandomSelector in
   // streaming_random_selection.hpp.
   template<class FwdIt>
   auto&& one_of_(FwdIt first, FwdIt last,
                  std::forward_iterator_tag)
   {
      assert(first != last);

      auto it_picked = first;
      ++first;

      size_t cnt = 2;           // length so far
      while(first != last)
      {
         if(random(cnt) == 0)   // 1/cnt chance to replace
            it_picked = first;
         ++cnt;
         ++first;
      }

      return *it_picked;
   }

   // input iter, we don't do that
   template<class InputIt>
   auto one_of_(InputIt first, InputIt last,
                std::input_iterator_tag) = delete;

   // @}

public:

   // one_of
   //
   /// Returns one random element from a range.
   ///
   /// Returns one randomly chosen element from a non-empty range,
   /// with equal probability of choosing each element.
   ///
   /// (See the C++17 algorithm `std::sample()`.)
   ///
   /// \param first,last the range of elements to choose from
   ///
   /// \param container a container holding elements to choose from
   ///
   /// \param initlist an initializer list holding elements to choose from
   ///
   /// \note Only random-access iterators (and containers) are
   /// currently supported.

   // @{
   template<class Iter>
   auto&& one_of(Iter first, Iter last)
   {
      return one_of_(first, last,
                     typename std::iterator_traits<Iter>::iterator_category());
   }

   template<class Container>
   auto&& one_of(Container&& container)
   {
      using std::begin; using std::end;
      return one_of(begin(container), end(container));
   }

   template<class T>
   auto&& one_of(std::initializer_list<T> initlist)
   {
      using std::begin; using std::end;
      return one_of(begin(initlist), end(initlist));
   }

   // @}

    /// Chooses one string to return, with equal probability.
    const char* select_text(std::initializer_list<const char*> options)
    {
       return one_of(options);
    }
};

#endif
