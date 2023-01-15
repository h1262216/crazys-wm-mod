# TODO
## Engine and Refactorings
* [ ] file system abstraction (physfs? / boost::filesystem?)
* [ ] libarchive for archive handling
* [ ] Integrate boost::stacktrace for better debugging
* [ ] Async loading of animations
* [ ] for logging? https://github.com/gabime/spdlog
* [ ] named actions for screens -- so we can make configurable key bindings
* [ ] redo all the item code :( 

## Code
* [ ] Move all remaining game code out of the user interface
  - [ ] cScreenSlaveMarket
  - [ ] cScreenGang 
  - [ ] cScreenDungeon
* [ ] Improve performance of animations (upscale only when blitting?)
* [ ] De-interlacing
* [ ] Convert job functions to classes
  - [ ] Torturer
  - [ ] PersonalBedWarmer
  - [ ] Free Time
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
* [ ] LoadGame delete button
* [ ] Inventory screen resets
* [ ] Restore Property Management screen

## Gameplay
* [ ] Generic event: girls ask for job change
* [ ] Choose your job interaction

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
* [ ] WalkTown image appears too early
