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

#include <utils/sPercent.h>
 
struct cRng
{
  std::mt19937 gen_;

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
    int d100() { return random(100) + 1; }
/*
 *    constructor and destructor
 */
    cRng()  : gen_(std::random_device()()) {}
    ~cRng() = default;

    /// Chooses one string to return, with equal probability.
    const char* select_text(std::initializer_list<const char*> options);
};

#endif
