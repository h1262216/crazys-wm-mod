/*
 * Copyright 2022, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
 * who meet on http://pinkpetal.org
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

#ifndef WM_CHARACTER_LUST_H
#define WM_CHARACTER_LUST_H

#include "Constants.h"
#include "images/ids.h"
#include "utils/sPercent.h"

class ICharacter;
class sGirl;

/// Calculate the amount by which Lust regularly changes
int get_lust_change(const ICharacter& character);

/// Perform the weekly update of Lust and Libido
void update_lust(ICharacter& character);


/*
 * The following functions are helpers to check if the girl engages in a sexual activity. There are four different
 * scenarios:
 *  1) Sex in a private setting: Masturbation, Player with love, boy/girlfriend. Determined mostly by her lust.
 *  2) Sex as part of the job: There is the threat of being fired/punished if she refuses. Requires some combination of
 *     lust and obedience.
 *  3) Sex in a public setting: With a stranger or in view of other people, but not threat if she refuses. Requires more
 *     lust and less dignity than the private counterpart.
 *
 *  Scenarios 1) and 3) come in two variations: Either she initiates it herself, or she just responds to a request.
 */


/// Does she want to engage in sexual activity in a "private"
/// setting, e.g. with someone she knows and trust, or just to
/// masturbate.
sPercent chance_horny_private(const ICharacter& character, ESexParticipants partner, SKILLS action, bool init);
/// Performs the `chance_horny_private` check for masturbation.
bool will_masturbate(const ICharacter& character, bool init=true, sPercent base_chance=sPercent{1.f});

/// Does she want to engage in sexual activity in a "public"
/// setting
sPercent chance_horny_public(const ICharacter& character, ESexParticipants partner, SKILLS action, bool init);

/// Performs the `chance_horny_public` check for masturbation.
bool will_masturbate_public(const ICharacter& character, sPercent base_chance=sPercent{1.f});

/// Performs a check with the chance calculated using `chance_horny_public`.
bool check_public_sex(const sGirl& girl, ESexParticipants partner, SKILLS action, sPercent base_chance, bool init);

#endif //WM_CHARACTER_LUST_H
