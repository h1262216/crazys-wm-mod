# Version 7.2.2alpha1
Started reworking the UI

* Added a new in-game main screen (player's office) as a central location  
* Added ability to hide dialog window by pressing `h`.
* re-enable "Preg" image variations without `images.xml`

# Version 7.2.1beta8
Small bug fixes and improvements.

* Actually enabled the new pre-defined game settings
* Fixed saving / loading of cheat mode
* Improved some error messages
* New texts for training job
* Made Auto-Use work for both consumables and equipment
* Added `[Eq.]` marker to show equipped items in the inventory
* Fixed button graphics
* Added `Select all girls`/`Select all customers` buttons in dungeon screen


# Version 7.2.1beta7
This version is mostly small bugfixes, and introduces image style preferences for the new tagging. It also
allows loading presets for the came config in the New Game screen.

## Tagger
* Allow attaching `style` information to an image: It can be a `drawing`, `rendering`, or `photo`. In the 
main menu, the player can select which types of images they prefer, which will give the game a nudge towards
those images --- If there are lots of images with a given tag then the game will only use those of the preferred
style, but if there are only very few it will use them all.

## UI
* Added ability to load game settings from pre-defined configurations in `Resources/Presets`. Comes with three
pre-defined files for simple adjustments of difficulty. `Easy.xml` makes things a bit cheaper, `Hard.xml` a bit
more expensive.

## Bugfixes
* Fixed `Dependant`/`Dependent` spelling
* Added missing `traits.html` file
* Fixed an error in job xml conditions

# Version 7.2.1alpha6
This version contains new game mechanics for gaining traits by doing jobs. In the old version, 
girls would slowly start to like doing their assigned job more and more, and once a certain threshold
was passed, they would gain the trait. Now, each girl keeps track of the progress she has made towards
gaining/losing a trait, and only once this reaches 100% does she get/lose the trait. The progress towards
traits can be seen under "More Details" in the girl details screen, below her job preferences. If a 
trait does not make any progress for 10 consecutive shifts, its progress starts to decay towards zero. 
An overview of which trait can be gained/lost by which job is given [here](Docs&Tools/traits.html).

## Gameplay
* Reworked the way how traits are gained from working jobs.
* Added a `Compassionate` tag as the opposite of `Sadistic`/`Merciless`
* Split `Libido` stat into (permanent) `Libido` and (temporary) `Lust`. Lust increases more quickly with high libido 
(and decreases if Libido is too low), and is the stat generally used for checks.

## Other
* Trait xml files now specify which traits are gained / lost on level-up
* No longer sort danger/warning messages into the first slot in the turn event screen, so
that the girl messages now should always be chronological
* Added specific `Gained Trait` and `Lost Trait` event types for the turn summary

## Tagger
* Prefer game `ImageFiles.xml` over bundled one, allowing for user overrides
* `Add Directory` adds files with matching extension now case-insensitively

## Bugfixes
* Fixed two bugs reported by big fan of fans
* Fixed some text conditions for `Waitress` and `Singer`

# Version 7.2.1alpha5

## Gameplay:
* Tweaks for movies
  * Movie goes will avoid movies much cheaper than what they expect to spend
  * Slightly increased spending power of rich audience classes
  * Actresses gain fame if their movies are seen by many people. Filming a scene no longer gives fame.
  * Decreased chance to refuse `Tease` scenes, increased chance to refuse `Extreme` scenes.
  * Initial movie hype depends on fame of its actresses

* Some extensions to free time events by @crazywm
* Movie and Concert events can now also be triggered as rewards in the interaction menu

## Images
* Added `Pray` image type to be used for the "Visit Church" free time event.

## Bugfixes
* Make Movie button no longer overlaps prev/next buttons
* Do not mix up bold and regular rendering if the same text is used
* Fixed image selection Turn Summary screen -- no longer reuses wrong images 
* Fixed stack corruption in lua script handling
* Run production jobs in arena
* Back button in interaction @crazy

## Other
* Treatment-type jobs (Therapy, Surgery etc.) now have more unified code and some
configuration through xml files.
* Some Free Time events are now handled by lua scripts 
* Some more functions available in lua scripts

# Version 7.2.1alpha4
Nothing new for image tagging; mostly UI improvements and bug fixes.

## UI 
* Added previous/next building buttons to girl management screens
* Adjusted (increased) font size in 4:3 mode
* Mark different Trait types in girl details UI
* Added a help screen with info about different traits, found by clicking the `*` button in girl details

## Bugfixes
* Correctly cycle through buildings when selecting previous of first building
* Fixed 4:3 gallery screen
* Game no longer ignores Player-Daughter girls
* Fixed superfluous space in player name

# Version 7.2.1alpha3

## Tagging
* Added creator, comment, and date fields to image packs
* Added an "AnyGroup" participant, not for use in image tags. If the game request an image with this,
it will accept any other group participant as a cheap fallback.
* Added a filter function to the tagger tool.
* `CityGuard` job now uses `Security` tag
* Added `Creampie` image tag. Not yet used by the game.
* `Bath` images now default to `Solo`, but in the "Bath interaction"
script the game may look for the `Hetero` variation if the player decides to join
* Tagger will autocomplete if there is only a single matching option left
* Tagger can autocomplete some presets, e.g. `lesbian` sets tag to `sex` and participants to `lesbian`


## Bugfixes
* Fixed error in image selection algorithm that allowed it to choose the same image as part of the candidate set twice.
* Fixed paying wages to unpaid slaves
* Saving the pack will pick up the changes made to the current image

## Other
* Some internal job code work
* Added "Ask her to take a bath" action in her bedroom. 
* Allow `${name}` also in lua script dialogs.
* SkillCaps can now be influenced by traits

# Version 7.2.1alpha2
Further work on new image tagging system. Many things are still experimental,
but the basic functionality should be there.

## Gameplay
* More work on bedroom interactions
* More variety in `Cocktails` interactions

## Image Tags
* Made `participants` its own image attributes, making `les`, `group`, etc tags redundant
* New `spanking`, `kiss`, `fondle`, `chastity`, `dinner`, `drinks` tags

## Other
* Made fraction of items replaced in the shop each week configurable. Default is the old value of 50%

# Version 7.2.1alpha1
Initial work on new image tagging system.

## Gameplay
* New `group` interactions in her bedroom

## Image Tags
* added `ffm`, `mmf`, and `orgy` as specialisations of `group`
* added `scissor` as a specialisation for `lesbian`
* added `security` for the security job. This is already used in some packs, so we might as well use the name. Fallback is combat.
* added `bake` for Baker job and as fallback for cook. Many older packs contain `bake` images, so let's use them.


# Version 7.1.2beta4

## Gameplay
* Added two new temporary traits, "Recently Punished" and "Recently Scolded", 
that get activated through your interactions. If you scold a recently scolded girl,
it is less effective, if you scold a recently punished girl it is more effective. Allowing
a girl to refuse an interaction while the "Recently Scolded" trait is active will decrease her
obedience.
* Updates to the interaction script

## UI
* Slightly increased dialog font size
* New image types `drink`, `dinner`, `kiss`, currently only used as part of the interactions
* Added some more default images

## Fixes
* Added missing `Weight` parameter to `Blacksmith.xml`
* Traits now update correctly after a temporary trait is added
* Duplicate girl bug when walking streets
* Added skill gains for waitress job
* Fixed error in TownMeet kidnap scenario
* Image reset if no file could be loaded
* Correctly present stat/skill changes after loading girl
* Fixed some errors in lua scripts
 
# Version 7.1.2beta3
## Gameplay
* Training job less likely to select skills close to skill cap
* New Tattoo/Piercing management in office interactions
* Prevent gangs from accidentally killing their catacomb captures
* Girls at very low health, but with constitution and stamina, 
will regenerate health more quickly but lose stamina/strength 
* Clinic will generate a warning message or status update for the number of treatments performed/requested/possible.

## Fixes
* Set correct matron job for brothel. This fixes obedience calculations.
* Fixed some errors in lua bindings.
* Fixed crash for runaway when branding in dungeon
* Fixed hiring of multiple gangs at the same time in non-default order
* Fixed saving/loading of beasts and other stored goods
* Added missing refuse text for `Intern` job
* Fixed disease traits to actually be registered as diseases.
* Full-Time job correctly applied to both day- and night-shift
* Fix text in dungeon screen, and joined together message for multiple brandings
* Fixed error message if not enough doctors are present


# Version 7.1.2beta2

## Gameplay
* In the first two weeks, "Walk around town" will always lead to an encounter
* Adapted success chance in skill checks in the lua scripts. 
  If too much skill is lacking, chance is now 0%
* Removed rival inventory simulation

## Fixes
* Fixed event type for Advertise job
* Fixed money computation for Barmaid and Catacombs
* Improved behaviour if no suitable random girl is found
* Fixed error in Office lua script
* Fixed spelling of `Dependent` tag; still accepts old spelling.

# Version 7.1.2beta1

## Gameplay changes since 7.1.1
* Added dynamic skill caps
* Merged PCHate and PCLove stats 
* Adjusted XP calculations
* New image tags `music`, `piano`, `refuse`

## Other changes since 7.1.1
* Reworked rendering implementation
* More game aspects configurable by xml
* Further general code cleanup
* Some UI improvements
* Added Theme support and a simple dark theme.

## Fixes since 7.1.2alpha5
* Show dungeon events in turn summary
* Fixed wages for house cook
* Fixed some stats being accidentally set to 100
* Fixed `Waitress` trait name
* Fixed display of `DUMMY` job name

For a detailed changelog see [Docs&Tools](Docs&Tools/changelog-7.1.2beta.md)

# Version 7.1.1beta2
## Changes
* included new meeting script by CaptC

# Version 7.1.1beta

## Major Changes
### Practice Skills Job Reworked
now moved to the house, and requiring a **Mistress** for optimal results.
  - Practice Skill: now a House job
  - Practice Skill: only increases sex skills and Performance
  - Only one girl can be personally trained at the same time
  - Mistress: strongly boosts the Practice Skills results for one or two girls, depending on Mistress skill
  - Mistress: can give temporary obedience increase to disobedient girls around the house

### More Game Aspects Defined Through XML
* Trait modifiers
* Item crafting
* Job performance, requirements, and job texts
  - Not for all jobs yet
  - Some jobs have only been partially converted

### Image Scaling and Video Support
* Small images and animations won't be stretched to full screen size. Instead, each image in the UI 
  now has a maximum and a minimum size, and will only be stretched if it is too small or too large. 
  To restore the old behaviour, you can edit the interface xml files so that they have identical maximum and minimum settings.

* The game can now read a variety of video files including mp4, avi, webm. 
  (Animated webp is not supported at the moment)

### Major internal code rework
Reduced the amount of C++ code to about half, without removing much from the game.
Some parts are still a bit broken, though. Much of the text generation for the girls'
jobs has been moved to XML, but this is still work in progress.

Also updated these libraries to more recent versions:
* SDL1.2 -> SLD2
  - now allows for non-ascci text input
* Lua 5.1 -> Lua 5.3
* tinyxml -> tinyxml2

### Reworked Movie Studio
The Movie Studio has been rewritten almost entirely. The most important changes are:
* You can (and should) have more than one camera mage, crystal purifier, and promoter
* Finished movies are now tracked explicitly in the movie maker screen. There you can also
  set the ticket prices
* There are now different types of target audiences, which have differing requirements on
  the movie.
* In the turn summary, BUILDINGS section, the Movie Studio will now have additional info about
  created scenes and screened movies

**The new movie mechanics are still preliminary, and haven't really been balanced yet**

* Split Director into Executive (Matron) and Director (Filming scenes)
* New MarketResearch job and corresponding MarketResearch screen
* JobRating column in Studio Screen
* Working as Pornstar makes girls lose the Chaste trait
* Added a new "send for training" interaction to improve performance skill
* Added new image tags for the studio crew jobs
* Ad Campaign and Survey actions to help promote your movies

### Other
* scripting ported to lua script
* support for multiple girl pack locations and recursively scan character folder for girl files
* New combat mechanics / combat reports

## Minor Changes
* More info in Load Game screen
* Removed Dump Bodies option for sell/fire girls
* At the centre, you need one counselor for each therapy
* Removed Mechanic and Get Repaired jobs. Construct girls will now also be treated by normal doctors.
* Cosmetic surgery no longer increases Charisma
* Liposuction can remove Fat trait, but no longer changes Charisma and Beauty
* Don't auto-assign new counselors if the counselor refuses for a shift
* added `massage` images for masseuse job, with fallback to `hand`
* Allow hiring multiple gangs at once
* supports expanding ~ to home directory on linux
* Select theme and fullscreen from settings
* rival actions now create no popup messages, but have their own tab in the turn summary
* Allow changing player settings (i.e. default house percentage) in-game in the house screen

