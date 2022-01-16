/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
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

#ifndef WM_IMAGE_IDS_H
#define WM_IMAGE_IDS_H

#include <array>

// This is the data the is submitted to an event when determining an image
enum class EBaseImage {
    // Basic Sex types
    ANAL,
    BDSM,
    SEX,
    BEAST,
    GROUP,
    FFM,
    MMF,
    ORGY,
    LESBIAN,
    ORAL,
    MAST,
    TITTY,
    HAND,
    FOOT,
    DEEPTHROAT,
    EATOUT,                 // licking the pussy of another girl
    DILDO,
    STRAPON,
    LES69ING,
    SCISSOR,
    LICK,                   // getting her pussy licked
    FINGER,                 // getting her pussy fingered
    FISTING,                // getting her pussy fisted
    SUCKBALLS,
    COWGIRL,
    REVCOWGIRL,
    SEXDOGGY,
    RAPE,
    RAPE_BEAST,

    // NSFW
    STRIP,
    ECCHI,
    NUDE,
    BUNNY,
    ESCORT,                         // clothed with a client
    MASSAGE,                        // massaging a client
    MILK,                           // getting milked
    DOM,                            // acting as a dominatrix (torturer, dominatrix job, ...)
    SUB,                            // acting as a submissive

    // Jobs
    MAID,
    SING,
    WAIT,
    CARD,
    NURSE,
    PIANO,                          // Playing the piano
    MUSIC,                          // Playing another instrument
    STAGEHAND,
    PURIFIER,
    DIRECTOR,
    CAMERA_MAGE,
    STUDIO_CREW,
    SECURITY,                       // Security
    COOK,                           // Cooking
    BAKE,                           // Baking
    FARM,                           // Generic farm work
    HERD,                           // working with animals
    ADVERTISE,                      // (prvls. SIGN)

    // Activities
    SPORT,                          // for the free time job, doing sports
    STUDY,                          // reading a book or looking clever
    TEACHER,                        // dressed or acting as a teacher
    COMBAT,                         // fighting/with weapons
    MAGIC,                          // performing a spell
    BED,                            // On a bed. (Used e.g. for bedroom interactions)
    REST,                           // free time resting
    BATH,                           // taking a bath
    SWIM,                           // going swimming or dressed in a swimsuit
    SHOP,                           // free time going shopping
    CRAFT,                          // creating items
    DANCE,                          // dancing

    BIRTH_HUMAN,                    // Giving birth to a human
    BIRTH_MULTI,                    // Giving birth to (human) twins/triplets/etc
    BIRTH_BEAST,                    // Giving birth to a beast

    // other
    REFUSE,                         // refuse to do a job
    DEATH,                          // she died
    PROFILE,                        // generic profile image
    PORTRAIT,                       // portrait-style profile image
    JAIL,                           // In jail or dungeon
    TORTURE,                        // Getting tortured
    FORMAL,                         // wearing formal clothes
    PRESENTED,                      // used for Slave Market
    BRAND,                          // getting branded
    PUPPYGIRL,                      // acting or looking like a puppy
    PONYGIRL,                       // acting or looking like a pony
    CATGIRL,                        // acting or looking like a cat

    NUM_TYPES
};

const std::array<const char*, (int)EBaseImage::NUM_TYPES>& get_imgtype_names();
EBaseImage get_image_id(const std::string& name);
const char* get_image_name(EBaseImage image);

/*
IMGTYPE_IMPREGSEX,                    // `J` new .06.03.01 for DarkArk
IMGTYPE_IMPREGGROUP,                // `J` new .06.03.01 for DarkArk
IMGTYPE_IMPREGBDSM,                    // `J` new .06.03.01 for DarkArk
IMGTYPE_IMPREGBEAST,                // `J` new .06.03.01 for DarkArk
IMGTYPE_WATER            - "water*."            - "Watersports"        - watersports
IMGTYPE_PETPROFILE        - "pet*."            - "Pet"                - profile, nude
IMGTYPE_PETORAL            - "petoral*."        - "PetOral"            - oral, lick, deepthroat, titty, petprofile, nude, bdsm
IMGTYPE_PETSEX            - "petsex*."        - "PetSex"            - sex, nude, anal, petprofile, bdsm
IMGTYPE_PETBEAST        - "petbeast*."        - "PetBeastiality"    - beast, sex, anal, bdsm, nude
IMGTYPE_PETFEED            - "petfeed*."        - "PetFeed"            - oral, lick, petprofile, bdsm, nude
IMGTYPE_PETPLAY            - "petplay*."        - "PetPlay"            - petprofile, bdsm, nude
IMGTYPE_PETTOY            - "pettoy*."        - "PetToy"            - dildo, petprofile, oral, mast, bdsm, nude
IMGTYPE_PETWALK            - "petwalk*."        - "PetWalk"            - petprofile, bdsm, nude
IMGTYPE_PETLICK            - "petlick*."        - "PetLick"            - lick, oral, petprofile, bdsm, nude, titty
*/

#endif //WM_IMAGE_IDS_H
