# TODO
## Engine and Refactorings
* [ ] file system abstraction (physfs? / boost::filesystem?)
* [ ] libarchive for archive handling
* [ ] Integrate boost::stacktrace for better debugging
* [ ] Async loading of animations
* [ ] for logging? https://github.com/gabime/spdlog
* [ ] Girl identifiers -- so we can have stable references in the save file e.g. for movie scenes
* [ ] named actions for screens -- so we can make configurable key bindings
* [ ] redo all the item code :( 

## Code
* [ ] Move all remaining game code out of the user interface
  - [ ] cScreenSlaveMarket
  - [ ] cScreenGang 
  - [ ] cScreenDungeon
* [ ] Replace image name based tags by an xml file
* [ ] Improve image paths handling / image selection
* [ ] Improve performance of animations (upscale only when blitting?)
* [ ] Pre-generate list of all images for each girl on game start
* [ ] De-interlacing
* [ ] Convert job functions to classes
  - [ ] Torturer
  - [ ] PersonalBedWarmer
  - [ ] Free Time
* [ ] Partially share XML between different jobs
* [ ] adjustable unique chance for overworld encounters

## Scripting
* [ ] Extend lua scripting to cover everything the old scripts did
* [ ] New Commands:
  - [ ] Player inventory
* [ ] New Triggers:
  - [ ] OnBought
  - [ ] Next Week
  
## Movies
* [ ] More text variety
* [ ] Trait text influences for scenes
* [ ] Fame bonus for girls/crew whose movies are good/hyped
* [ ] Initial hype bonus for movies of famous girls/crew


## UI
* [ ] Per building/global anti preg setting
* [ ] Game-Mode files
* [ ] Active Building Widget
* [ ] LoadGame delete button
* [ ] Inventory screen resets
* [ ] Restore Property Management screen

## Gameplay
* [ ] Generic event: girls ask for job change
* [ ] Choose your job interaction

## Text
* [x] MMF/FFM event in bedroom
* [ ] Training job

## Bugs
* [ ] Pregnancies of runaway/prison girls currently don't work
* [ ] GetRandomRivalToSabotage function is wrong
* [ ] async scripts and messages may get scrambled
* [ ] ffmpeg frame duration off by one
* [ ] bugs in shop/inventory screen
  - [ ] "your inventory is full", but seems to buy anyway
  - [ ] "your inventory is full" for sell button
  - [ ] Equip / unequip shown simultaneously
* [ ] gracefully handle missing/corrupt config file
* [ ] animated webps don't load
* [ ] order of events:
  When matron puts girl into resting state, 
  the announcement isn't put into the correct spot in the event listing. 
  It is always at the top, instead of in the correct time sequence. 
  This appears to also be a problem with girls who get in fights or are attacked, 
  and if the girl refuses a customer due to sexual preference.
* [x] Recruiters, cooks and cleaners in the house do not appear to cost any money???
* [ ] WalkTown image appears too early
* [ ] WalkTown thrown into dungeon but still appears in regular building