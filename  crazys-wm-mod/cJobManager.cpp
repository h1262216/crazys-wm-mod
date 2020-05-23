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
#include <algorithm>
#include "cJobManager.h"
#include "src/buildings/cBrothel.h"
#include "src/buildings/queries.hpp"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "src/Game.hpp"

extern cRng g_Dice;


extern	bool	g_CTRLDown;

void cJobManager::Setup()
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager.cpp > Setup

	// much simplier then trying to get the sJob working with this (plus a smaller memory footprint...?maybe)
	// General Jobs
	JobFilterName[JOBFILTER_GENERAL] = "General";
	JobFilterDesc[JOBFILTER_GENERAL] = "These are general jobs that work individually in any building.";
	JobFilterIndex[JOBFILTER_GENERAL] = JOB_RESTING;
	JobName[JOB_RESTING] = "Free Time";
	JobQkNm[JOB_RESTING] = "TOff";
	JobDesc[JOB_RESTING] = "She will take some time off, maybe do some shopping or walk around town. If the girl is unhappy she may try to escape.";
	JobFunc[JOB_RESTING] = &WorkFreetime;
	JobName[JOB_TRAINING] = "Practice Skills";
	JobQkNm[JOB_TRAINING] = "Prtc";
	JobDesc[JOB_TRAINING] = "She will train either alone or with others to improve her skills.";
	JobFunc[JOB_TRAINING] = &WorkTraining;
	JobPerf[JOB_TRAINING] = &JP_Training;
	JobName[JOB_CLEANING] = "Cleaning";
	JobQkNm[JOB_CLEANING] = "Cln";
	JobDesc[JOB_CLEANING] = "She will clean the building, as filth will put off some customers.";
	JobFunc[JOB_CLEANING] = &WorkCleaning;
	JobPerf[JOB_CLEANING] = &JP_Cleaning;
	JobName[JOB_SECURITY] = "Security";
	JobQkNm[JOB_SECURITY] = "Sec";
	JobDesc[JOB_SECURITY] = "She will patrol the building, stopping mis-deeds.";
	JobFunc[JOB_SECURITY] = &WorkSecurity;
	JobPerf[JOB_SECURITY] = &JP_Security;
	JobName[JOB_ADVERTISING] = "Advertising";
	JobQkNm[JOB_ADVERTISING] = "Adv";
	JobDesc[JOB_ADVERTISING] = "She will advertise the building's features in the city.";
	JobFunc[JOB_ADVERTISING] = &WorkAdvertising;
	JobPerf[JOB_ADVERTISING] = &JP_Advertising;
	JobName[JOB_CUSTOMERSERVICE] = "Customer Service";
	JobQkNm[JOB_CUSTOMERSERVICE] = "CS";
	JobDesc[JOB_CUSTOMERSERVICE] = "She will look after customer needs.";
	JobFunc[JOB_CUSTOMERSERVICE] = &WorkCustService;
	JobPerf[JOB_CUSTOMERSERVICE] = &JP_CustService;
	JobName[JOB_MATRON] = "Matron";
	JobQkNm[JOB_MATRON] = "Mtrn";
	JobDesc[JOB_MATRON] = "This girl will look after the other girls. Only non-slave girls can have this position and you must pay them 300 gold per week. Also, it takes up both shifts. (max 1)";
	JobFunc[JOB_MATRON] = &WorkMatron;
	JobPerf[JOB_MATRON] = &JP_Matron;
	JobName[JOB_TORTURER] = "Torturer";
	JobQkNm[JOB_TORTURER] = "Trtr";
	JobDesc[JOB_TORTURER] = "She will torture the prisoners in addition to your tortures, she will also look after them to ensure they don't die. (max 1 for all brothels)";
	JobFunc[JOB_TORTURER] = &WorkTorturer;
	JobPerf[JOB_TORTURER] = &JP_Torturer;
	JobName[JOB_EXPLORECATACOMBS] = "Explore Catacombs";
	JobQkNm[JOB_EXPLORECATACOMBS] = "ExC";
	JobDesc[JOB_EXPLORECATACOMBS] = "She will explore the catacombs looking for treasure and capturing monsters and monster girls. Needless to say, this is a dangerous job.";
	JobFunc[JOB_EXPLORECATACOMBS] = &WorkExploreCatacombs;
	JobPerf[JOB_EXPLORECATACOMBS] = &JP_ExploreCatacombs;
	JobName[JOB_BEASTCARER] = "Beast Carer";
	JobQkNm[JOB_BEASTCARER] = "BstC";
	JobDesc[JOB_BEASTCARER] = "She will look after the needs of the beasts in your Brothel.";
	JobFunc[JOB_BEASTCARER] = &WorkBeastCare;
	JobPerf[JOB_BEASTCARER] = &JP_BeastCare;
	// - Bar Jobs
	JobFilterName[JOBFILTER_BAR] = "Bar";
	JobFilterDesc[JOBFILTER_BAR] = "These are jobs for running a bar.";
	JobFilterIndex[JOBFILTER_BAR] = JOB_BARMAID;
	JobName[JOB_BARMAID] = "Barmaid";
	JobQkNm[JOB_BARMAID] = "BrMd";
	JobDesc[JOB_BARMAID] = "She will staff the bar and serve drinks.";
	JobFunc[JOB_BARMAID] = &WorkBarmaid;
	JobPerf[JOB_BARMAID] = &JP_Barmaid;
	JobName[JOB_WAITRESS] = "Waitress";
	JobQkNm[JOB_WAITRESS] = "Wtrs";
	JobDesc[JOB_WAITRESS] = "She will bring drinks and food to customers at the tables.";
	JobFunc[JOB_WAITRESS] = &WorkBarWaitress;
	JobPerf[JOB_WAITRESS] = &JP_BarWaitress;
	JobName[JOB_SINGER] = "Singer";
	JobQkNm[JOB_SINGER] = "Sngr";
	JobDesc[JOB_SINGER] = "She will sing for the customers.";
	JobFunc[JOB_SINGER] = &WorkBarSinger;
	JobPerf[JOB_SINGER] = &JP_BarSinger;
	JobName[JOB_PIANO] = "Piano";
	JobQkNm[JOB_PIANO] = "Pno";
	JobDesc[JOB_PIANO] = "She will play the piano for the customers.";
	JobFunc[JOB_PIANO] = &WorkBarPiano;
	JobPerf[JOB_PIANO] = &JP_BarPiano;
	JobName[JOB_ESCORT] = "Escort";
	JobQkNm[JOB_ESCORT] = "Scrt";
	JobDesc[JOB_ESCORT] = "She will be an escort.";
	JobFunc[JOB_ESCORT] = &WorkEscort;
	JobPerf[JOB_ESCORT] = &JP_Escort;
	JobName[JOB_BARCOOK] = "Bar Cook";
	JobQkNm[JOB_BARCOOK] = "BrCk";
	JobDesc[JOB_BARCOOK] = "She will cook food for the bar.";
	JobFunc[JOB_BARCOOK] = &WorkBarCook;
	JobPerf[JOB_BARCOOK] = &JP_Barcook;
	// - Gambling Hall Jobs
	JobFilterName[JOBFILTER_GAMBHALL] = "Gambling Hall";
	JobFilterDesc[JOBFILTER_GAMBHALL] = "These are jobs for running a gambling hall.";
	JobFilterIndex[JOBFILTER_GAMBHALL] = JOB_DEALER;
	JobName[JOB_DEALER] = "Game Dealer";
	JobQkNm[JOB_DEALER] = "Dlr";
	JobDesc[JOB_DEALER] = "She will manage a game in the gambling hall.";
	JobFunc[JOB_DEALER] = &WorkHallDealer;
	JobPerf[JOB_DEALER] = &JP_HallDealer;
	JobName[JOB_ENTERTAINMENT] = "Entertainer";
	JobQkNm[JOB_ENTERTAINMENT] = "Entn";
	JobDesc[JOB_ENTERTAINMENT] = "She will provide entertainment to the customers.";
	JobFunc[JOB_ENTERTAINMENT] = &WorkHallEntertainer;
	JobPerf[JOB_ENTERTAINMENT] = &JP_HallEntertainer;
	JobName[JOB_XXXENTERTAINMENT] = "XXX Entertainer";
	JobQkNm[JOB_XXXENTERTAINMENT] = "XXXE";
	JobDesc[JOB_XXXENTERTAINMENT] = "She will provide sexual entertainment to the customers.";
	JobFunc[JOB_XXXENTERTAINMENT] = &WorkHallXXXEntertainer;
	JobPerf[JOB_XXXENTERTAINMENT] = &JP_HallXXXEntertainer;
	JobName[JOB_WHOREGAMBHALL] = "Hall Whore";
	JobQkNm[JOB_WHOREGAMBHALL] = "HWhr";
	JobDesc[JOB_WHOREGAMBHALL] = "She will give her sexual favors to the customers.";
	JobFunc[JOB_WHOREGAMBHALL] = &WorkWhore;
	JobPerf[JOB_WHOREGAMBHALL] = &JP_Whore;
	// - Sleazy Bar Jobs		// Changing all references to Strip Club, just sounds better to me and more realistic. -PP
	JobFilterName[JOBFILTER_SLEAZYBAR] = "Strip Club";
	JobFilterDesc[JOBFILTER_SLEAZYBAR] = "These are jobs for running a Strip Club.";
	JobFilterIndex[JOBFILTER_SLEAZYBAR] = JOB_SLEAZYBARMAID;
	JobName[JOB_SLEAZYBARMAID] = "Strip Club Barmaid";
	JobQkNm[JOB_SLEAZYBARMAID] = "SBmd";
	JobDesc[JOB_SLEAZYBARMAID] = "She will staff the bar and serve drinks while dressed in lingerie or fetish costumes.";
	JobFunc[JOB_SLEAZYBARMAID] = &WorkSleazyBarmaid;
	JobPerf[JOB_SLEAZYBARMAID] = &JP_SleazyBarmaid;
	JobName[JOB_SLEAZYWAITRESS] = "Strip Club Waitress";
	JobQkNm[JOB_SLEAZYWAITRESS] = "SWtr";
	JobDesc[JOB_SLEAZYWAITRESS] = "She will bring drinks and food to the customers at the tables while dressed in lingerie or fetish costumes.";
	JobFunc[JOB_SLEAZYWAITRESS] = &WorkSleazyWaitress;
	JobPerf[JOB_SLEAZYWAITRESS] = &JP_SleazyWaitress;
	JobName[JOB_BARSTRIPPER] = "Strip Club Stripper";
	JobQkNm[JOB_BARSTRIPPER] = "SSrp";
	JobDesc[JOB_BARSTRIPPER] = "She will strip on the tables and stage for the customers.";
	JobFunc[JOB_BARSTRIPPER] = &WorkBarStripper;
	JobPerf[JOB_BARSTRIPPER] = &JP_BarStripper;
	JobName[JOB_BARWHORE] = "Strip Club Whore";
	JobQkNm[JOB_BARWHORE] = "SWhr";
	JobDesc[JOB_BARWHORE] = "She will provide sex to the customers.";
	JobFunc[JOB_BARWHORE] = &WorkWhore;
	JobPerf[JOB_BARWHORE] = &JP_Whore;
	// - Brothel Jobs
	JobFilterName[JOBFILTER_BROTHEL] = "Brothel";
	JobFilterDesc[JOBFILTER_BROTHEL] = "These are jobs for running a brothel.";
	JobFilterIndex[JOBFILTER_BROTHEL] = JOB_MASSEUSE;
	JobName[JOB_MASSEUSE] = "Masseuse";
	JobQkNm[JOB_MASSEUSE] = "Msus";
	JobDesc[JOB_MASSEUSE] = "She will give massages to the customers.";
	JobFunc[JOB_MASSEUSE] = &WorkBrothelMasseuse;
	JobPerf[JOB_MASSEUSE] = &JP_BrothelMasseuse;
	JobName[JOB_BROTHELSTRIPPER] = "Brothel Stripper";
	JobQkNm[JOB_BROTHELSTRIPPER] = "BStp";
	JobDesc[JOB_BROTHELSTRIPPER] = "She will strip for the customers.";
	JobFunc[JOB_BROTHELSTRIPPER] = &WorkBrothelStripper;
	JobPerf[JOB_BROTHELSTRIPPER] = &JP_BrothelStripper;
	JobName[JOB_PEEP] = "Peep Show";
	JobQkNm[JOB_PEEP] = "Peep";
	JobDesc[JOB_PEEP] = "She will let people watch her change and maybe more...";
	JobFunc[JOB_PEEP] = &WorkPeepShow;
	JobPerf[JOB_PEEP] = &JP_PeepShow;
	JobName[JOB_WHOREBROTHEL] = "Brothel Whore";
	JobQkNm[JOB_WHOREBROTHEL] = "BWhr";
	JobDesc[JOB_WHOREBROTHEL] = "She will whore herself to customers within the building's walls. This is safer but a little less profitable.";
	JobFunc[JOB_WHOREBROTHEL] = &WorkWhore;
	JobPerf[JOB_WHOREBROTHEL] = &JP_Whore;
	JobName[JOB_WHORESTREETS] = "Whore on Streets";
	JobQkNm[JOB_WHORESTREETS] = "StWr";
	JobDesc[JOB_WHORESTREETS] = "She will whore herself on the streets. It is more dangerous than whoring inside but more profitable.";
	JobFunc[JOB_WHORESTREETS] = &WorkWhore;
	JobPerf[JOB_WHORESTREETS] = &JP_Whore;





	// Studio Crew
	JobFilterName[JOBFILTER_STUDIOCREW] = "Studio Crew";
	JobFilterDesc[JOBFILTER_STUDIOCREW] = "These are jobs for running a movie studio.";
	JobFilterIndex[JOBFILTER_STUDIOCREW] = JOB_FILMFREETIME;
	JobName[JOB_FILMFREETIME] = "Time off";
	JobQkNm[JOB_FILMFREETIME] = "TOff";
	JobDesc[JOB_FILMFREETIME] = "She takes time off resting and recovering.";
	JobFunc[JOB_FILMFREETIME] = &WorkFreetime;
	JobPerf[JOB_FILMFREETIME] = &JP_Freetime;
	JobName[JOB_DIRECTOR] = "Director";
	JobQkNm[JOB_DIRECTOR] = "Dir";
	JobDesc[JOB_DIRECTOR] = "She directs the filming, and keeps the girls in line. (max 1)";
	JobFunc[JOB_DIRECTOR] = &WorkFilmDirector;
	JobPerf[JOB_DIRECTOR] = &JP_FilmDirector;
	JobName[JOB_PROMOTER] = "Promoter";
	JobQkNm[JOB_PROMOTER] = "Prmt";
	JobDesc[JOB_PROMOTER] = "She advertises the movies. (max 1)";
	JobFunc[JOB_PROMOTER] = &WorkFilmPromoter;
	JobPerf[JOB_PROMOTER] = &JP_FilmPromoter;
	JobName[JOB_CAMERAMAGE] = "Camera Mage";
	JobQkNm[JOB_CAMERAMAGE] = "CM";
	JobDesc[JOB_CAMERAMAGE] = "She will film the scenes. (requires at least 1 to create a scene)";
	JobFunc[JOB_CAMERAMAGE] = &WorkCameraMage;
	JobPerf[JOB_CAMERAMAGE] = &JP_CameraMage;
	JobName[JOB_CRYSTALPURIFIER] = "Crystal Purifier";
	JobQkNm[JOB_CRYSTALPURIFIER] = "CP";
	JobDesc[JOB_CRYSTALPURIFIER] = "She will clean up the filmed scenes. (requires at least 1 to create a scene)";
	JobFunc[JOB_CRYSTALPURIFIER] = &WorkCrystalPurifier;
	JobPerf[JOB_CRYSTALPURIFIER] = &JP_CrystalPurifier;
	//JobName[JOB_SOUNDTRACK] = ("Sound Track");
	//JobQkNm[JOB_SOUNDTRACK] = "SndT";
	//JobDesc[JOB_SOUNDTRACK] = ("She will clean up the audio and add music to the scenes. (not required but helpful)");
	//JobFunc[JOB_SOUNDTRACK] = &WorkSoundTrack;
	//JobPerf[JOB_SOUNDTRACK] = &JP_SoundTrack;
	JobName[JOB_FLUFFER] = "Fluffer";
	JobQkNm[JOB_FLUFFER] = "Fluf";
	JobDesc[JOB_FLUFFER] = "She will keep the porn stars aroused.";
	JobFunc[JOB_FLUFFER] = &WorkFluffer;
	JobPerf[JOB_FLUFFER] = &JP_Fluffer;
	JobName[JOB_STAGEHAND] = "Stagehand";
	JobQkNm[JOB_STAGEHAND] = "StgH";
	JobDesc[JOB_STAGEHAND] = "She helps setup equipment, and keeps the studio clean.";
	JobFunc[JOB_STAGEHAND] = &WorkFilmStagehand;
	JobPerf[JOB_STAGEHAND] = &JP_FilmStagehand;


	// `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cJobManager.cpp > Setup

	// Studio - Non-Sex Scenes
	JobFilterName[JOBFILTER_STUDIONONSEX] = "Non-Sex Scenes";
	JobFilterDesc[JOBFILTER_STUDIONONSEX] = "These are scenes without sex.";
	JobFilterIndex[JOBFILTER_STUDIONONSEX] = JOB_FILMACTION;
	JobName[JOB_FILMACTION] = "Action";
	JobQkNm[JOB_FILMACTION] = "FAct";
	JobDesc[JOB_FILMACTION] = "She will be the scantily clad heroine in fighting/magical action scenes.";
	JobFunc[JOB_FILMACTION] = &WorkFilmAction;
	JobPerf[JOB_FILMACTION] = &JP_FilmAction;
	JobName[JOB_FILMCHEF] = "The Naked Chef";
	JobQkNm[JOB_FILMCHEF] = "FChf";
	JobDesc[JOB_FILMCHEF] = "She will be filmed preparing food. And talking slowly. Sensuously. In sexy outfits. Because no reason.";
	JobFunc[JOB_FILMCHEF] = &WorkFilmChef;
	JobPerf[JOB_FILMCHEF] = &JP_FilmChef;
	//JobName[JOB_FILMCOMEDY] = ("Comedy Scene");
	//JobQkNm[JOB_FILMCOMEDY] = "FCmd";
	//JobDesc[JOB_FILMCOMEDY] = ("She will perform a scene intended to make people laugh.");
	//JobFunc[JOB_FILMCOMEDY] = &WorkFilmComedy;
	//JobPerf[JOB_FILMCOMEDY] = &JP_FilmComedy;
	//JobName[JOB_FILMDRAMA] = ("Drama Scene");
	//JobQkNm[JOB_FILMDRAMA] = "FDrm";
	//JobDesc[JOB_FILMDRAMA] = ("She will film a Dramatic scene.");
	//JobFunc[JOB_FILMDRAMA] = &WorkFilmDrama;
	//JobPerf[JOB_FILMDRAMA] = &JP_FilmDrama;
	//JobName[JOB_FILMHORROR] = ("Horror Scene");
	//JobQkNm[JOB_FILMHORROR] = "FHrr";
	//JobDesc[JOB_FILMHORROR] = ("She will film a Horror scene.");
	//JobFunc[JOB_FILMHORROR] = &WorkFilmHorror;
	//JobPerf[JOB_FILMHORROR] = &JP_FilmHorror;
	//JobName[JOB_FILMIDOL] = ("Idol (Aidoru)");
	//JobQkNm[JOB_FILMIDOL] = "FIdl";
	//JobDesc[JOB_FILMIDOL] = ("She will be presented to the public as an idol: a cute, sexy woman.");
	//JobFunc[JOB_FILMIDOL] = &WorkFilmIdol;
	//JobPerf[JOB_FILMIDOL] = &JP_FilmIdol;
	JobName[JOB_FILMMUSIC] = "Music";
	JobQkNm[JOB_FILMMUSIC] = "FMus";
	JobDesc[JOB_FILMMUSIC] = "She will make a music video.";
	JobFunc[JOB_FILMMUSIC] = &WorkFilmMusic;
	JobPerf[JOB_FILMMUSIC] = &JP_FilmMusic;

	// Studio - Softcore Porn
	JobFilterName[JOBFILTER_STUDIOSOFTCORE] = "Softcore Scenes";
	JobFilterDesc[JOBFILTER_STUDIOSOFTCORE] = "These are scenes with just the one girl.";
	JobFilterIndex[JOBFILTER_STUDIOSOFTCORE] = JOB_FILMMAST;
	JobName[JOB_FILMMAST] = "Film Masturbation";
	JobQkNm[JOB_FILMMAST] = "FMst";
	JobDesc[JOB_FILMMAST] = "She will perform in a masturbation scene.";
	JobFunc[JOB_FILMMAST] = &WorkFilmMast;
	JobPerf[JOB_FILMMAST] = &JP_FilmMast;
	JobName[JOB_FILMSTRIP] = "Film Strip tease";
	JobQkNm[JOB_FILMSTRIP] = "FStp";
	JobDesc[JOB_FILMSTRIP] = "She will perform in a strip tease scene.";
	JobFunc[JOB_FILMSTRIP] = &WorkFilmStrip;
	JobPerf[JOB_FILMSTRIP] = &JP_FilmStrip;
	JobName[JOB_FILMTEASE] = "Teaser Video";
	JobQkNm[JOB_FILMTEASE] = "FTea";
	JobDesc[JOB_FILMTEASE] = "She will flirt and remove her clothes and tease seductively, but won't show all. If you freeze-frame you might just catch a nipple.";
	JobFunc[JOB_FILMTEASE] = &WorkFilmTease;
	JobPerf[JOB_FILMTEASE] = &JP_FilmTease;

	// Studio - Porn
	JobFilterName[JOBFILTER_STUDIOPORN] = "Porn Scenes";
	JobFilterDesc[JOBFILTER_STUDIOPORN] = "These are regular sex scenes.";
	JobFilterIndex[JOBFILTER_STUDIOPORN] = JOB_FILMANAL;
	JobName[JOB_FILMANAL] = "Film Anal";
	JobQkNm[JOB_FILMANAL] = "FAnl";
	JobDesc[JOB_FILMANAL] = "She will film an anal scenes.";
	JobFunc[JOB_FILMANAL] = &WorkFilmAnal;
	JobPerf[JOB_FILMANAL] = &JP_FilmAnal;
	JobName[JOB_FILMFOOTJOB] = "Film Foot Job";
	JobQkNm[JOB_FILMFOOTJOB] = "FFJ";
	JobDesc[JOB_FILMFOOTJOB] = "She will perform in a foot job scene.";
	JobFunc[JOB_FILMFOOTJOB] = &WorkFilmFootJob;
	JobPerf[JOB_FILMFOOTJOB] = &JP_FilmFootJob;
//	JobName[JOB_FILMFUTA] = ("Film Futa Scene");
//	JobQkNm[JOB_FILMFUTA] = "FFta";
//	JobDesc[JOB_FILMFUTA] = ("She will perform in a Futa scene.");
//	JobFunc[JOB_FILMFUTA] = &WorkFilmFuta;
//	JobPerf[JOB_FILMFUTA] = &JP_FilmFuta;
	JobName[JOB_FILMHANDJOB] = "Film Hand Job";
	JobQkNm[JOB_FILMHANDJOB] = "FHJ";
	JobDesc[JOB_FILMHANDJOB] = "She will perform in a hand job scene.";
	JobFunc[JOB_FILMHANDJOB] = &WorkFilmHandJob;
	JobPerf[JOB_FILMHANDJOB] = &JP_FilmHandJob;
	JobName[JOB_FILMLESBIAN] = "Film Lesbian";
	JobQkNm[JOB_FILMLESBIAN] = "FLes";
	JobDesc[JOB_FILMLESBIAN] = "She will do a lesbian scene.";
	JobFunc[JOB_FILMLESBIAN] = &WorkFilmLesbian;
	JobPerf[JOB_FILMLESBIAN] = &JP_FilmLesbian;
	JobName[JOB_FILMORAL] = "Film Oral Sex";
	JobQkNm[JOB_FILMORAL] = "FOrl";
	JobDesc[JOB_FILMORAL] = ("She will perform in an oral sex scene.");
	JobFunc[JOB_FILMORAL] = &WorkFilmOral;
	JobPerf[JOB_FILMORAL] = &JP_FilmOral;
	JobName[JOB_FILMSEX] = ("Film Sex");
	JobQkNm[JOB_FILMSEX] = "FSex";
	JobDesc[JOB_FILMSEX] = ("She will film a normal sex scenes.");
	JobFunc[JOB_FILMSEX] = &WorkFilmSex;
	JobPerf[JOB_FILMSEX] = &JP_FilmSex;
	JobName[JOB_FILMTITTY] = ("Film Titty Fuck");
	JobQkNm[JOB_FILMTITTY] = "FTit";
	JobDesc[JOB_FILMTITTY] = ("She will perform in a titty fuck scene.");
	JobFunc[JOB_FILMTITTY] = &WorkFilmTitty;
	JobPerf[JOB_FILMTITTY] = &JP_FilmTitty;

	// Studio - Hardcore porn
	JobFilterName[JOBFILTER_STUDIOHARDCORE] = ("Hardcore Scenes");
	JobFilterDesc[JOBFILTER_STUDIOHARDCORE] = ("These are rough scenes that not all girls would be comfortable with.");
	JobFilterIndex[JOBFILTER_STUDIOHARDCORE] = JOB_FILMBEAST;
	JobName[JOB_FILMBEAST] = ("Film Bestiality");
	JobQkNm[JOB_FILMBEAST] = "FBst";
	JobDesc[JOB_FILMBEAST] = ("She will film a bestiality scenes.");
	JobFunc[JOB_FILMBEAST] = &WorkFilmBeast;
	JobPerf[JOB_FILMBEAST] = &JP_FilmBeast;
	JobName[JOB_FILMBONDAGE] = ("Film Bondage");
	JobQkNm[JOB_FILMBONDAGE] = "FBnd";
	JobDesc[JOB_FILMBONDAGE] = ("She will perform a in bondage scenes.");
	JobFunc[JOB_FILMBONDAGE] = &WorkFilmBondage;
	JobPerf[JOB_FILMBONDAGE] = &JP_FilmBondage;
	JobName[JOB_FILMBUKKAKE] = ("Cumslut/Bukkake");
	JobQkNm[JOB_FILMBUKKAKE] = "FBuk";											//update in cGirls.cpp
	JobDesc[JOB_FILMBUKKAKE] = ("A large group of men will cum on, over and inside her.");
	JobFunc[JOB_FILMBUKKAKE] = &WorkFilmBuk;
	JobPerf[JOB_FILMBUKKAKE] = &JP_FilmBuk;
	JobName[JOB_FILMFACEFUCK] = ("Face-fuck");
	JobQkNm[JOB_FILMFACEFUCK] = "FFac";											//update in cGirls.cpp
	JobDesc[JOB_FILMFACEFUCK] = ("She will be used in gag and face-fuck scenes.");
	JobFunc[JOB_FILMFACEFUCK] = &WorkFilmThroat;
	JobPerf[JOB_FILMFACEFUCK] = &JP_FilmThroat;
	JobName[JOB_FILMGROUP] = ("Film Group");
	JobQkNm[JOB_FILMGROUP] = "FGrp";
	JobDesc[JOB_FILMGROUP] = ("She will film a group sex scene.");
	JobFunc[JOB_FILMGROUP] = &WorkFilmGroup;
	JobPerf[JOB_FILMGROUP] = &JP_FilmGroup;
	JobName[JOB_FILMPUBLICBDSM] = ("Public Torture");
	JobQkNm[JOB_FILMPUBLICBDSM] = "FTor";											//update in cGirls.cpp
	JobDesc[JOB_FILMPUBLICBDSM] = ("She will be tied up in a public place, where she will be stripped, whipped, tortured and fully-used. On film.");
	JobFunc[JOB_FILMPUBLICBDSM] = &WorkFilmPublicBDSM;
	JobPerf[JOB_FILMPUBLICBDSM] = &JP_FilmPublicBDSM;

	//JobName[JOB_FILMDOM] = ("Film Dominatrix");
	//JobQkNm[JOB_FILMDOM] = "FDom";											//update in cGirls.cpp
	//JobDesc[JOB_FILMDOM] = ("She will perform in a Dominatrix scene.");
	//JobFunc[JOB_FILMDOM] = &WorkFilmDominatrix;
	//JobPerf[JOB_FILMDOM] = &JP_FilmDom;

	// Studio - Random
	JobFilterName[JOBFILTER_RANDSTUDIO] = ("Random Scenes");
	JobFilterDesc[JOBFILTER_RANDSTUDIO] = ("Let the game choose what scene the girl will film.");
	JobFilterIndex[JOBFILTER_RANDSTUDIO] = JOB_FILMRANDOM;
	JobName[JOB_FILMRANDOM] = ("Film a random scene");
	JobQkNm[JOB_FILMRANDOM] = "FRnd";
	JobDesc[JOB_FILMRANDOM] = ("She will perform in a random sex scene.");
	JobFunc[JOB_FILMRANDOM] = &WorkFilmRandom;		// This job is handled different than others, it is in cMovieStudio.cpp UpdateGirls()
	JobPerf[JOB_FILMRANDOM] = &JP_FilmRandom;


	// - Arena Jobs
	JobFilterName[JOBFILTER_ARENA] = ("Arena");
	JobFilterDesc[JOBFILTER_ARENA] = ("These are jobs for running an arena.");
	JobFilterIndex[JOBFILTER_ARENA] = JOB_FIGHTBEASTS;
	JobName[JOB_FIGHTBEASTS] = ("Fight Beasts");
	JobQkNm[JOB_FIGHTBEASTS] = "FiBs";
	JobDesc[JOB_FIGHTBEASTS] = ("She will fight to the death against beasts you own. Dangerous.");
	JobFunc[JOB_FIGHTBEASTS] = &WorkFightBeast;
	JobPerf[JOB_FIGHTBEASTS] = &JP_FightBeast;
	JobName[JOB_FIGHTARENAGIRLS] = ("Cage Match");
	JobQkNm[JOB_FIGHTARENAGIRLS] = "Cage";
	JobDesc[JOB_FIGHTARENAGIRLS] = ("She will fight against other girls. Dangerous.");
	JobFunc[JOB_FIGHTARENAGIRLS] = &WorkFightArenaGirls;
	JobPerf[JOB_FIGHTARENAGIRLS] = &JP_FightArenaGirls;
	JobName[JOB_FIGHTTRAIN] = ("Combat Training");
	JobQkNm[JOB_FIGHTTRAIN] = "CT";
	JobDesc[JOB_FIGHTTRAIN] = ("She will practice combat.");
	JobFunc[JOB_FIGHTTRAIN] = &WorkCombatTraining;
	JobPerf[JOB_FIGHTTRAIN] = &JP_CombatTraining;
	/*JobName[JOB_JOUSTING] = ("Jousting");
	JobQkNm[JOB_JOUSTING] = "JstM";
	JobDesc[JOB_JOUSTING] = ("She will put on a Jousting match.");
	JobFunc[JOB_JOUSTING] = &WorkArenaJousting;
	JobPerf[JOB_JOUSTING] = &JP_ArenaJousting;*/
	//- Arena Staff
	JobFilterName[JOBFILTER_ARENASTAFF] = ("Arena Staff");
	JobFilterDesc[JOBFILTER_ARENASTAFF] = ("These are jobs that help run an arena.");
	JobFilterIndex[JOBFILTER_ARENASTAFF] = JOB_ARENAREST;
	JobName[JOB_ARENAREST] = ("Time off");
	JobQkNm[JOB_ARENAREST] = "TOff";
	JobDesc[JOB_ARENAREST] = ("She will rest.");
	JobFunc[JOB_ARENAREST] = &WorkFreetime;
	JobPerf[JOB_ARENAREST] = &JP_Freetime;
	JobName[JOB_DOCTORE] = ("Doctore");
	JobQkNm[JOB_DOCTORE] = "Dtre";
	JobDesc[JOB_DOCTORE] = ("She will watch over the girls in the arena.");
	JobFunc[JOB_DOCTORE] = &WorkDoctore;
	JobPerf[JOB_DOCTORE] = &JP_Doctore;
	JobName[JOB_CITYGUARD] = ("City Guard");
	JobQkNm[JOB_CITYGUARD] = "CGrd";
	JobDesc[JOB_CITYGUARD] = ("She will help keep crossgate safe.");
	JobFunc[JOB_CITYGUARD] = &WorkCityGuard;
	JobPerf[JOB_CITYGUARD] = &JP_CityGuard;
	JobName[JOB_BLACKSMITH] = ("Blacksmith");
	JobQkNm[JOB_BLACKSMITH] = "Bksm";
	JobDesc[JOB_BLACKSMITH] = ("She will make weapons and armor.");
	JobFunc[JOB_BLACKSMITH] = &WorkBlacksmith;
	JobPerf[JOB_BLACKSMITH] = &JP_Blacksmith;
	JobName[JOB_COBBLER] = ("Cobbler");
	JobQkNm[JOB_COBBLER] = "Cblr";
	JobDesc[JOB_COBBLER] = ("She will make shoes and leather items.");
	JobFunc[JOB_COBBLER] = &WorkCobbler;
	JobPerf[JOB_COBBLER] = &JP_Cobbler;
	JobName[JOB_JEWELER] = ("Jeweler");
	JobQkNm[JOB_JEWELER] = "Jwlr";
	JobDesc[JOB_JEWELER] = ("She will make jewelery.");
	JobFunc[JOB_JEWELER] = &WorkJeweler;
	JobPerf[JOB_JEWELER] = &JP_Jeweler;
	JobName[JOB_CLEANARENA] = ("Grounds Keeper");
	JobQkNm[JOB_CLEANARENA] = "GKpr";
	JobDesc[JOB_CLEANARENA] = ("She will clean the arena.");
	JobFunc[JOB_CLEANARENA] = &WorkCleanArena;
	JobPerf[JOB_CLEANARENA] = &JP_CleanArena;

	// - Community Centre Jobs
	JobFilterName[JOBFILTER_COMMUNITYCENTRE] = ("Community Centre");
	JobFilterDesc[JOBFILTER_COMMUNITYCENTRE] = ("These are jobs for running a community centre.");
	JobFilterIndex[JOBFILTER_COMMUNITYCENTRE] = JOB_CENTREREST;
	JobName[JOB_CENTREREST] = ("Time off");
	JobQkNm[JOB_CENTREREST] = "TOff";
	JobDesc[JOB_CENTREREST] = ("She will rest.");
	JobFunc[JOB_CENTREREST] = &WorkFreetime;
	JobPerf[JOB_CENTREREST] = &JP_Freetime;
	JobName[JOB_CENTREMANAGER] = ("Centre Manager");
	JobQkNm[JOB_CENTREMANAGER] = "CMgr";
	JobDesc[JOB_CENTREMANAGER] = ("She will look after the girls working in the centre.");
	JobFunc[JOB_CENTREMANAGER] = &WorkCentreManager;
	JobPerf[JOB_CENTREMANAGER] = &JP_CentreManager;
	JobName[JOB_FEEDPOOR] = ("Feed Poor");
	JobQkNm[JOB_FEEDPOOR] = "Feed";
	JobDesc[JOB_FEEDPOOR] = ("She will work in a soup kitchen.");
	JobFunc[JOB_FEEDPOOR] = &WorkFeedPoor;
	JobPerf[JOB_FEEDPOOR] = &JP_FeedPoor;
	JobName[JOB_COMUNITYSERVICE] = ("Community Service");
	JobQkNm[JOB_COMUNITYSERVICE] = "CmSr";
	JobDesc[JOB_COMUNITYSERVICE] = ("She will go around town and help out where she can.");
	JobFunc[JOB_COMUNITYSERVICE] = &WorkComunityService;
	JobPerf[JOB_COMUNITYSERVICE] = &JP_ComunityService;
	JobName[JOB_CLEANCENTRE] = ("Clean Centre");
	JobQkNm[JOB_CLEANCENTRE] = "ClnC";
	JobDesc[JOB_CLEANCENTRE] = ("She will clean the centre.");
	JobFunc[JOB_CLEANCENTRE] = &WorkCleanCentre;
	JobPerf[JOB_CLEANCENTRE] = &JP_CleanCentre;
	// - Counseling Centre Jobs
	JobFilterName[JOBFILTER_COUNSELINGCENTRE] = ("Counseling Centre");
	JobFilterDesc[JOBFILTER_COUNSELINGCENTRE] = ("These are jobs for running a counseling centre.");
	JobFilterIndex[JOBFILTER_COUNSELINGCENTRE] = JOB_COUNSELOR;
	JobName[JOB_COUNSELOR] = ("Counselor");
	JobQkNm[JOB_COUNSELOR] = "Cnsl";
	JobDesc[JOB_COUNSELOR] = ("She will help girls get over their addictions and problems.");
	JobFunc[JOB_COUNSELOR] = &WorkCounselor;
	JobPerf[JOB_COUNSELOR] = &JP_Counselor;
	JobName[JOB_REHAB] = ("Rehab");
	JobQkNm[JOB_REHAB] = "Rehb";
	JobDesc[JOB_REHAB] = ("She will go to rehab to get over her addictions.");
	JobFunc[JOB_REHAB] = &WorkRehab;
	JobPerf[JOB_REHAB] = &JP_Rehab;
	JobName[JOB_THERAPY] = ("Therapy");
	JobQkNm[JOB_THERAPY] = "Thrp";
	JobDesc[JOB_THERAPY] = ("She will go to therapy to get over her mental problems. (Nervous, Dependant, Pessimist)");
	JobFunc[JOB_THERAPY] = &WorkCentreTherapy;
	JobPerf[JOB_THERAPY] = &JP_CentreTherapy;
	JobName[JOB_EXTHERAPY] = ("Extreme Therapy");
	JobQkNm[JOB_EXTHERAPY] = "EThr";
	JobDesc[JOB_EXTHERAPY] = ("She will go to extreme therapy to get over her hardcore mental problems. (Mind Fucked, Broken Will)");
	JobFunc[JOB_EXTHERAPY] = &WorkCentreExTherapy;
	JobPerf[JOB_EXTHERAPY] = &JP_CentreExTherapy;
	JobName[JOB_ANGER] = ("Anger Management");
	JobQkNm[JOB_ANGER] = "AMng";
	JobDesc[JOB_ANGER] = ("She will go to anger management to get over her anger problems. (Aggressive, Tsundere, Yandere)");
	JobFunc[JOB_ANGER] = &WorkCentreAngerManagement;
	JobPerf[JOB_ANGER] = &JP_CentreAngerManagement;

	// - Clinic Surgeries
	JobFilterName[JOBFILTER_CLINIC] = ("Medical Clinic");
	JobFilterDesc[JOBFILTER_CLINIC] = ("These are procerures the girls can have done in the Medical Clinic.");
	JobFilterIndex[JOBFILTER_CLINIC] = JOB_GETHEALING;
	JobName[JOB_GETHEALING] = ("Get Healing");
	JobQkNm[JOB_GETHEALING] = "Heal";
	JobDesc[JOB_GETHEALING] = ("She will have her wounds attended.");
	JobFunc[JOB_GETHEALING] = &WorkHealing;
	JobPerf[JOB_GETHEALING] = &JP_Healing;
	JobName[JOB_GETREPAIRS] = ("Get Repaired");
	JobQkNm[JOB_GETREPAIRS] = "Repr";
	JobDesc[JOB_GETREPAIRS] = ("Construct girls will be quickly repaired here.");
	JobFunc[JOB_GETREPAIRS] = &WorkRepairShop;
	JobPerf[JOB_GETREPAIRS] = &JP_RepairShop;
	JobName[JOB_CUREDISEASES] = ("Cure Diseases");
	JobQkNm[JOB_CUREDISEASES] = "Cure";
	JobDesc[JOB_CUREDISEASES] = ("She will try to get her diseases cured.");
	JobFunc[JOB_CUREDISEASES] = &WorkCureDiseases;
	JobPerf[JOB_CUREDISEASES] = &JP_CureDiseases;
	JobName[JOB_GETABORT] = ("Abortion");
	JobQkNm[JOB_GETABORT] = "Abrt";
	JobDesc[JOB_GETABORT] = ("She will get an abortion, removing pregnancy and/or insemination.\n*(Takes 2 days or 1 if a Nurse is on duty)");
	JobFunc[JOB_GETABORT] = &WorkGetAbort;
	JobPerf[JOB_GETABORT] = &JP_GetAbort;
	JobName[JOB_COSMETICSURGERY] = ("Cosmetic Surgery");
	JobQkNm[JOB_COSMETICSURGERY] = "CosS";
	JobDesc[JOB_COSMETICSURGERY] = ("She will undergo magical surgery to \"enhance\" her appearance.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_COSMETICSURGERY] = &WorkCosmeticSurgery;
	JobPerf[JOB_COSMETICSURGERY] = &JP_CosmeticSurgery;
	JobName[JOB_LIPO] = ("Liposuction");
	JobQkNm[JOB_LIPO] = "Lipo";
	JobDesc[JOB_LIPO] = ("She will undergo liposuction to \"enhance\" her figure.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_LIPO] = &WorkLiposuction;
	JobPerf[JOB_LIPO] = &JP_Liposuction;
	JobName[JOB_BREASTREDUCTION] = ("Breast Reduction Surgery");
	JobQkNm[JOB_BREASTREDUCTION] = "BRS";
	JobDesc[JOB_BREASTREDUCTION] = ("She will undergo breast reduction surgery.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_BREASTREDUCTION] = &WorkBreastReduction;
	JobPerf[JOB_BREASTREDUCTION] = &JP_BreastReduction;
	JobName[JOB_BOOBJOB] = ("Boob Job");
	JobQkNm[JOB_BOOBJOB] = "BbJb";
	JobDesc[JOB_BOOBJOB] = ("She will undergo surgery to \"enhance\" her bust.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_BOOBJOB] = &WorkBoobJob;
	JobPerf[JOB_BOOBJOB] = &JP_BoobJob;
	JobName[JOB_VAGINAREJUV] = ("Vaginal Rejuvenation");
	JobQkNm[JOB_VAGINAREJUV] = "VagR";
	JobDesc[JOB_VAGINAREJUV] = ("She will undergo surgery to make her a virgin again.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_VAGINAREJUV] = &WorkGetVaginalRejuvenation;
	JobPerf[JOB_VAGINAREJUV] = &JP_GetVaginalRejuvenation;
	JobName[JOB_FACELIFT] = ("Face Lift");
	JobQkNm[JOB_FACELIFT] = "FLft";
	JobDesc[JOB_FACELIFT] = ("She will undergo surgery to make her younger.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_FACELIFT] = &WorkGetFacelift;
	JobPerf[JOB_FACELIFT] = &JP_GetFacelift;
	JobName[JOB_ASSJOB] = ("Arse Job");
	JobQkNm[JOB_ASSJOB] = "AssJ";
	JobDesc[JOB_ASSJOB] = ("She will undergo surgery to \"enhance\" her ass.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_ASSJOB] = &WorkGetAssJob;
	JobPerf[JOB_ASSJOB] = &JP_GetAssJob;
	JobName[JOB_TUBESTIED] = ("Tubes Tied");
	JobQkNm[JOB_TUBESTIED] = "TTid";
	JobDesc[JOB_TUBESTIED] = ("She will undergo surgery to make her sterile.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_TUBESTIED] = &WorkGetTubesTied;
	JobPerf[JOB_TUBESTIED] = &JP_GetTubesTied;
	JobName[JOB_FERTILITY] = ("Fertility Treatmeant");
	JobQkNm[JOB_FERTILITY] = "FrtT";
	JobDesc[JOB_FERTILITY] = ("She will undergo surgery to make her fertile.\n*(Takes up to 5 days, less if a Nurse is on duty)");
	JobFunc[JOB_FERTILITY] = &WorkGetFertility;
	JobPerf[JOB_FERTILITY] = &JP_GetFertility;
	// Clinic staff
	JobFilterName[JOBFILTER_CLINICSTAFF] = ("Clinic Staff");
	JobFilterDesc[JOBFILTER_CLINICSTAFF] = ("These are jobs that help run a medical clinic.");
	JobFilterIndex[JOBFILTER_CLINICSTAFF] = JOB_CLINICREST;
	JobName[JOB_CLINICREST] = ("Time off");
	JobQkNm[JOB_CLINICREST] = "TOff";
	JobDesc[JOB_CLINICREST] = ("She will rest");
	JobFunc[JOB_CLINICREST] = &WorkFreetime;
	JobPerf[JOB_CLINICREST] = &JP_Freetime;
	JobName[JOB_CHAIRMAN] = ("Chairman");
	JobQkNm[JOB_CHAIRMAN] = "Crmn";
	JobDesc[JOB_CHAIRMAN] = ("She will watch over the staff of the clinic");
	JobFunc[JOB_CHAIRMAN] = &WorkChairMan;
	JobPerf[JOB_CHAIRMAN] = &JP_ChairMan;
	JobName[JOB_DOCTOR] = ("Doctor");
	JobQkNm[JOB_DOCTOR] = "Doc";
	JobDesc[JOB_DOCTOR] = ("She will become a doctor. Doctors earn extra cash from treating locals. (requires at least 1 to perform surgeries)");
	JobFunc[JOB_DOCTOR] = &WorkDoctor;
	JobPerf[JOB_DOCTOR] = &JP_Doctor;
	JobName[JOB_NURSE] = ("Nurse");
	JobQkNm[JOB_NURSE] = "Nurs";
	JobDesc[JOB_NURSE] = ("Will help the doctor and heal sick people.");
	JobFunc[JOB_NURSE] = &WorkNurse;
	JobPerf[JOB_NURSE] = &JP_Nurse;
	JobName[JOB_MECHANIC] = ("Mechanic");
	JobQkNm[JOB_MECHANIC] = "Mech";
	JobDesc[JOB_MECHANIC] = ("Will help the doctor and repair Constructs.");
	JobFunc[JOB_MECHANIC] = &WorkMechanic;
	JobPerf[JOB_MECHANIC] = &JP_Mechanic;
	JobName[JOB_INTERN] = ("Intern");
	JobQkNm[JOB_INTERN] = "Ntrn";
	JobDesc[JOB_INTERN] = ("Will train in how to be a nurse.");
	JobFunc[JOB_INTERN] = &WorkIntern;
	JobPerf[JOB_INTERN] = &JP_Intern;
	JobName[JOB_JANITOR] = ("Janitor");
	JobQkNm[JOB_JANITOR] = "Jntr";
	JobDesc[JOB_JANITOR] = ("She will clean the clinic");
	JobFunc[JOB_JANITOR] = &WorkJanitor;
	JobPerf[JOB_JANITOR] = &JP_Janitor;

	// - Farm Staff
	JobFilterName[JOBFILTER_FARMSTAFF] = "Farm Staff";
	JobFilterDesc[JOBFILTER_FARMSTAFF] = "These are jobs for running a farm";
	JobFilterIndex[JOBFILTER_FARMSTAFF] = JOB_FARMREST;
	JobName[JOB_FARMREST] = "Time off";
	JobQkNm[JOB_FARMREST] = "TOff";
	JobDesc[JOB_FARMREST] = "She will take time off and rest.";
	JobFunc[JOB_FARMREST] = &WorkFreetime;
	JobPerf[JOB_FARMREST] = &JP_Freetime;
	JobName[JOB_FARMMANGER] = "Farm Manager";
	JobQkNm[JOB_FARMMANGER] = "FMgr";
	JobDesc[JOB_FARMMANGER] = "She will watch over the farm and girls working there.";
	JobFunc[JOB_FARMMANGER] = &WorkFarmManager;
	JobPerf[JOB_FARMMANGER] = &JP_FarmManager;
	JobName[JOB_VETERINARIAN] = "Veterinarian";
	JobQkNm[JOB_VETERINARIAN] = "Vet";
	JobDesc[JOB_VETERINARIAN] = "She will tend to sick animals.";
	JobFunc[JOB_VETERINARIAN] = &WorkFarmVeterinarian;
	JobPerf[JOB_VETERINARIAN] = &JP_FarmVeterinarian;
	JobName[JOB_MARKETER] = "Marketer";
	JobQkNm[JOB_MARKETER] = "Mrkt";
	JobDesc[JOB_MARKETER] = "She will buy and sell things.";
	JobFunc[JOB_MARKETER] = &WorkFarmMarketer;
	JobPerf[JOB_MARKETER] = &JP_FarmMarketer;
	JobName[JOB_RESEARCH] = "Researcher";
	JobQkNm[JOB_RESEARCH] = "Rsrc";
	JobDesc[JOB_RESEARCH] = "She will research how to improve various things.";
	JobFunc[JOB_RESEARCH] = &WorkFarmResearch;
	JobPerf[JOB_RESEARCH] = &JP_FarmResearch;
	JobName[JOB_FARMHAND] = "Farm Hand";
	JobQkNm[JOB_FARMHAND] = "FHnd";
	JobDesc[JOB_FARMHAND] = "She will clean up around the farm.";
	JobFunc[JOB_FARMHAND] = &WorkFarmHand;
	JobPerf[JOB_FARMHAND] = &JP_FarmHand;
	// - Laborers
	JobFilterName[JOBFILTER_LABORERS] = ("Laborers");
	JobFilterDesc[JOBFILTER_LABORERS] = ("These are jobs your girls can do at your farm.");
	JobFilterIndex[JOBFILTER_LABORERS] = JOB_FARMER;
	JobName[JOB_FARMER] = ("Farmer");
	JobQkNm[JOB_FARMER] = "Frmr";
	JobDesc[JOB_FARMER] = ("She will tend to your crops.");
	JobFunc[JOB_FARMER] = &WorkFarmer;
	JobPerf[JOB_FARMER] = &JP_Farmer;
	JobName[JOB_GARDENER] = ("Gardener");
	JobQkNm[JOB_GARDENER] = "Grdn";
	JobDesc[JOB_GARDENER] = ("She will produce herbs.");
	JobFunc[JOB_GARDENER] = &WorkGardener;
	JobPerf[JOB_GARDENER] = &JP_Gardener;
	JobName[JOB_SHEPHERD] = ("Shepherd");
	JobQkNm[JOB_SHEPHERD] = "SHrd";
	JobDesc[JOB_SHEPHERD] = ("She will tend to food animals.");
	JobFunc[JOB_SHEPHERD] = &WorkShepherd;
	JobPerf[JOB_SHEPHERD] = &JP_Shepherd;
	JobName[JOB_RANCHER] = ("Rancher");
	JobQkNm[JOB_RANCHER] = "Rncr";
	JobDesc[JOB_RANCHER] = ("She will tend to larger animals.");
	JobFunc[JOB_RANCHER] = &WorkRancher;
	JobPerf[JOB_RANCHER] = &JP_Rancher;
	JobName[JOB_CATACOMBRANCHER] = ("Catacombs Rancher");
	JobQkNm[JOB_CATACOMBRANCHER] = "CRnc";
	JobDesc[JOB_CATACOMBRANCHER] = ("She will tend to strange beasts.");
	JobFunc[JOB_CATACOMBRANCHER] = &WorkCatacombRancher;
	JobPerf[JOB_CATACOMBRANCHER] = &JP_CatacombRancher;
	JobName[JOB_BEASTCAPTURE] = "Beast Capture";
	JobQkNm[JOB_BEASTCAPTURE] = "BCpt";
	JobDesc[JOB_BEASTCAPTURE] = "She will go out into the wilds to capture beasts.";
	JobFunc[JOB_BEASTCAPTURE] = &WorkBeastCapture;
	JobPerf[JOB_BEASTCAPTURE] = &JP_BeastCapture;
	JobName[JOB_MILKER] = ("Milker");
	JobQkNm[JOB_MILKER] = "Mlkr";
	JobDesc[JOB_MILKER] = ("She will milk the various animals and girls you own.");
	JobFunc[JOB_MILKER] = &WorkMilker;
	JobPerf[JOB_MILKER] = &JP_Milker;
	JobName[JOB_MILK] = "Get Milked";
	JobQkNm[JOB_MILK] = "Mlkd";
	JobDesc[JOB_MILK] = "She will have her breasts milked";
	JobFunc[JOB_MILK] = &WorkMilk;
	JobPerf[JOB_MILK] = &JP_Milk;
	// - Producers
	JobFilterName[JOBFILTER_PRODUCERS] = ("Producers");
	JobFilterDesc[JOBFILTER_PRODUCERS] = ("These are jobs your girls can do at your farm.");
	JobFilterIndex[JOBFILTER_PRODUCERS] = JOB_BUTCHER;
	JobName[JOB_BUTCHER] = ("Butcher");
	JobQkNm[JOB_BUTCHER] = "Bchr";
	JobDesc[JOB_BUTCHER] = ("She will produce food from animals you own.");
	JobFunc[JOB_BUTCHER] = &WorkButcher;
	JobPerf[JOB_BUTCHER] = &JP_Butcher;
	JobName[JOB_BAKER] = ("Baker");
	JobQkNm[JOB_BAKER] = "Bkr";
	JobDesc[JOB_BAKER] = ("She will produce food from the crops you own.");
	JobFunc[JOB_BAKER] = &WorkBaker;
	JobPerf[JOB_BAKER] = &JP_Baker;
	JobName[JOB_BREWER] = ("Brewer");
	JobQkNm[JOB_BREWER] = "Brwr";
	JobDesc[JOB_BREWER] = ("She will make various beers and wines.");
	JobFunc[JOB_BREWER] = &WorkBrewer;
	JobPerf[JOB_BREWER] = &JP_Brewer;
	JobName[JOB_TAILOR] = ("Tailor");
	JobQkNm[JOB_TAILOR] = "Talr";
	JobDesc[JOB_TAILOR] = ("She will make clothes and other items from fabrics.");
	JobFunc[JOB_TAILOR] = &WorkTailor;
	JobPerf[JOB_TAILOR] = &JP_Tailor;
	JobName[JOB_MAKEITEM] = ("Make Items");
	JobQkNm[JOB_MAKEITEM] = "MkIt";
	JobDesc[JOB_MAKEITEM] = ("She will make various items.");
	JobFunc[JOB_MAKEITEM] = &WorkMakeItem;
	JobPerf[JOB_MAKEITEM] = &JP_MakeItem;
	JobName[JOB_MAKEPOTIONS] = ("Make Potions");
	JobQkNm[JOB_MAKEPOTIONS] = "MkPt";
	JobDesc[JOB_MAKEPOTIONS] = ("She will make various potions.");
	JobFunc[JOB_MAKEPOTIONS] = &WorkMakePotions;
	JobPerf[JOB_MAKEPOTIONS] = &JP_MakePotions;

	// - House
	JobFilterName[JOBFILTER_HOUSE] = ("Player House");
	JobFilterDesc[JOBFILTER_HOUSE] = ("These are jobs your girls can do at your house.");
	JobFilterIndex[JOBFILTER_HOUSE] = JOB_HOUSEREST;
	JobName[JOB_HOUSEREST] = ("Time off");
	JobQkNm[JOB_HOUSEREST] = "TOff";
	JobDesc[JOB_HOUSEREST] = ("She takes time off resting and recovering.");
	JobFunc[JOB_HOUSEREST] = &WorkFreetime;
	JobPerf[JOB_HOUSEREST] = &JP_Freetime;
	JobName[JOB_HEADGIRL] = ("Head Girl");
	JobQkNm[JOB_HEADGIRL] = "HGrl";
	JobDesc[JOB_HEADGIRL] = ("She takes care of the girls in your house.");
	JobFunc[JOB_HEADGIRL] = &WorkHeadGirl;
	JobPerf[JOB_HEADGIRL] = &JP_HeadGirl;
	JobName[JOB_RECRUITER] = "Recruiter";
	JobQkNm[JOB_RECRUITER] = "Rcrt";
	JobDesc[JOB_RECRUITER] = "She will go out and try and recruit girls for you.";
	JobFunc[JOB_RECRUITER] = &WorkRecruiter;
	JobPerf[JOB_RECRUITER] = &JP_Recruiter;
	JobName[JOB_PERSONALBEDWARMER] = ("Bed Warmer");
	JobQkNm[JOB_PERSONALBEDWARMER] = "BdWm";
	JobDesc[JOB_PERSONALBEDWARMER] = ("She will stay in your bed at night with you.");
	JobFunc[JOB_PERSONALBEDWARMER] = &WorkPersonalBedWarmer;
	JobPerf[JOB_PERSONALBEDWARMER] = &JP_PersonalBedWarmer;
	JobName[JOB_HOUSECOOK] = ("House Cook");
	JobQkNm[JOB_HOUSECOOK] = "Hcok";
	JobDesc[JOB_HOUSECOOK] = ("She will cook for your house.");
	JobFunc[JOB_HOUSECOOK] = &WorkHouseCook;
	JobPerf[JOB_HOUSECOOK] = &JP_HouseCook;
	//JobName[JOB_PONYGIRL] = ("Pony Girl");
	//JobQkNm[JOB_PONYGIRL] = "PGil";
	//JobDesc[JOB_PONYGIRL] = ("She will be trained to become a pony girl.");
	//JobFunc[JOB_PONYGIRL] = &WorkFarmPonyGirl;
	JobName[JOB_CLEANHOUSE] = ("Clean House");
	JobQkNm[JOB_CLEANHOUSE] = "ClnH";
	JobDesc[JOB_CLEANHOUSE] = ("She will clean your house.");
	JobFunc[JOB_CLEANHOUSE] = &WorkCleanHouse;
	JobPerf[JOB_CLEANHOUSE] = &JP_CleanHouse;

	JobFilterName[JOBFILTER_HOUSETTRAINING] = ("Sex Training");
	JobFilterDesc[JOBFILTER_HOUSETTRAINING] = ("You will take a personal intrerst in training the girl in sexual matters.");
	JobFilterIndex[JOBFILTER_HOUSETTRAINING] = JOB_PERSONALTRAINING;
	JobName[JOB_PERSONALTRAINING] = ("Personal Training");
	JobQkNm[JOB_PERSONALTRAINING] = "PTrn";
	JobDesc[JOB_PERSONALTRAINING] = ("You will oversee her training personal.");
	JobFunc[JOB_PERSONALTRAINING] = &WorkPersonalTraining;
	JobPerf[JOB_PERSONALTRAINING] = &JP_PersonalTraining;
	JobName[JOB_HOUSEPET] = ("House Pet");
	JobQkNm[JOB_HOUSEPET] = "Hpet";
	JobDesc[JOB_HOUSEPET] = ("She will be trained to become the house pet.");
	JobFunc[JOB_HOUSEPET] = &WorkHousePet;
	JobName[JOB_SO_STRAIGHT] = ("SO Straight");
	JobQkNm[JOB_SO_STRAIGHT] = "SOSt";
	JobDesc[JOB_SO_STRAIGHT] = ("You will make sure she only likes having sex with men.");
	JobFunc[JOB_SO_STRAIGHT] = &WorkSOStraight;
	JobPerf[JOB_SO_STRAIGHT] = &JP_SOStraight;
	JobName[JOB_SO_BISEXUAL] = ("SO Bisexual");
	JobQkNm[JOB_SO_BISEXUAL] = "SOBi";
	JobDesc[JOB_SO_BISEXUAL] = ("You will make sure she likes having sex with both men and women.");
	JobFunc[JOB_SO_BISEXUAL] = &WorkSOBisexual;
	JobPerf[JOB_SO_BISEXUAL] = &JP_SOBisexual;
	JobName[JOB_SO_LESBIAN] = ("SO Lesbian");
	JobQkNm[JOB_SO_LESBIAN] = "SOLe";
	JobDesc[JOB_SO_LESBIAN] = ("You will make sure she only likes having sex with women.");
	JobFunc[JOB_SO_LESBIAN] = &WorkSOLesbian;
	JobPerf[JOB_SO_LESBIAN] = &JP_SOLesbian;
	JobName[JOB_FAKEORGASM] = ("Fake Orgasm Expert");
	JobQkNm[JOB_FAKEORGASM] = "FOEx";
	JobDesc[JOB_FAKEORGASM] = ("You will teach her how to fake her orgasms.");
	JobFunc[JOB_FAKEORGASM] = &WorkFakeOrgasm;
	JobPerf[JOB_FAKEORGASM] = &JP_FakeOrgasm;








	JobFilterName[JOBFILTER_NONE] = ("Non-Job");
	JobFilterDesc[JOBFILTER_NONE] = ("These are NOT jobs. They are special cases.");
	JobFilterIndex[JOBFILTER_NONE] = JOB_INDUNGEON;
	JobName[JOB_INDUNGEON]	= ("In the Dungeon");
	JobQkNm[JOB_INDUNGEON]	= "Dngn";
	JobDesc[JOB_INDUNGEON]	= ("She is languishing in the dungeon.");
	JobName[JOB_RUNAWAY]	= ("Runaway");
	JobQkNm[JOB_RUNAWAY]	= "RunA";
	JobDesc[JOB_RUNAWAY]	= ("She has run away.");




#if 0
	// - Community Centre
	JobFunc[JOB_COLLECTDONATIONS] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_ARTISAN] = &WorkMakeItem;	// ************** TODO
	JobFunc[JOB_SELLITEMS] = &WorkVoid;	// ************** TODO
	// - Drug Lab (these jobs gain bonus if in same building as a clinic)
	JobFunc[JOB_VIRASPLANTFUCKER] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_SHROUDGROWER] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_FAIRYDUSTER] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_DRUGDEALER] = &WorkVoid;	// ************** TODO
	// - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
	JobFunc[JOB_FINDREGENTS] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_BREWPOTIONS] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_POTIONTESTER] = &WorkVoid;	// ************** TODO
	// - Arena (these jobs gain bonus if in same building as a clinic)
	JobFunc[JOB_FIGHTBEASTS] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_WRESTLE] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_FIGHTTODEATH] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_FIGHTVOLUNTEERS] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_COLLECTBETS] = &WorkVoid;	// ************** TODO
	// - Skills Centre
	JobFunc[JOB_TEACHBDSM] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_TEACHSEX] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_TEACHBEAST] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_TEACHMAGIC] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_TEACHCOMBAT] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_DAYCARE] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_SCHOOLING] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_TEACHDANCING] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_TEACHSERVICE] = &WorkVoid;	// ************** TODO
	JobFunc[JOB_TRAIN] = &WorkVoid;	// ************** TODO

#endif




# if 0

	//- Community Centre
	JobFilterName[JOBFILTER_COMMUNITYCENTRE] = ("Community Centre");
	JobFilterDesc[JOBFILTER_COMMUNITYCENTRE] = ("These are jobs for running a community centre.");
	JobFilterIndex[JOBFILTER_COMMUNITYCENTRE] = JOB_COLLECTDONATIONS;
	JobName[JOB_COLLECTDONATIONS] = ("Collect Donations");
	JobDesc[JOB_COLLECTDONATIONS] = ("She will collect money to help the poor.");
	JobName[JOB_ARTISAN] = ("Make Crafts");
	JobDesc[JOB_ARTISAN] = ("She will craft cheap handmade items.");
	JobName[JOB_SELLITEMS] = ("Sell Crafts");
	JobDesc[JOB_SELLITEMS] = ("She will go out and sell previously crafted items.");

	JobFilterName[JOBFILTER_DRUGLAB] = ("Drug Lab");
	JobFilterDesc[JOBFILTER_DRUGLAB] = ("These are jobs for running a drug lab.");
	JobFilterIndex[JOBFILTER_DRUGLAB] = JOB_VIRASPLANTFUCKER;
	JobName[JOB_VIRASPLANTFUCKER] = ("Fuck Viras Plants");
	JobDesc[JOB_VIRASPLANTFUCKER] = ("She will allow Viras plants to inseminate her, creating Vira Blood.");
	JobName[JOB_SHROUDGROWER] = ("Grow Shroud Mushrooms");
	JobDesc[JOB_SHROUDGROWER] = ("She will grow Shroud Mushrooms, which require a lot of care and may explode.");
	JobName[JOB_FAIRYDUSTER] = ("Fairy Duster");
	JobDesc[JOB_FAIRYDUSTER] = ("She will capture fairies and grind them into a fine powder known as Fairy Dust.");
	JobName[JOB_DRUGDEALER] = ("Drug Dealer");
	JobDesc[JOB_DRUGDEALER] = ("She will go out and sell your drugs on the streets.");

	JobFilterName[JOBFILTER_ALCHEMISTLAB] = ("Alchemist Lab");
	JobFilterDesc[JOBFILTER_ALCHEMISTLAB] = ("These are jobs for running an alchemist lab.");
	JobFilterIndex[JOBFILTER_ALCHEMISTLAB] = JOB_FINDREGENTS;
	JobName[JOB_FINDREGENTS] = ("Find Reagents");
	JobDesc[JOB_FINDREGENTS] = ("She will go out and look for promising potion ingredients.");
	JobName[JOB_BREWPOTIONS] = ("Brew Potions");
	JobDesc[JOB_BREWPOTIONS] = ("She will brew a random potion using available ingredients.");
	JobName[JOB_POTIONTESTER] = ("Guinea Pig");
	JobDesc[JOB_POTIONTESTER] = ("She will test out random potions. Who knows what might happen? (requires 1)");

	JobFilterName[JOBFILTER_ARENA] = ("Arena");
	JobFilterDesc[JOBFILTER_ARENA] = ("These are jobs for running an arena.");
	JobFilterIndex[JOBFILTER_ARENA] = JOB_FIGHTBEASTS;
	JobName[JOB_FIGHTBEASTS] = ("Fight Beasts");
	JobDesc[JOB_FIGHTBEASTS] = ("She will fight to the death against beasts you own. Dangerous.");
	JobName[JOB_WRESTLE] = ("Wrestle");
	JobDesc[JOB_WRESTLE] = ("She will take part in a wrestling match. Not dangerous.");
	JobName[JOB_FIGHTTODEATH] = ("Cage Match");
	JobDesc[JOB_FIGHTTODEATH] = ("She will fight to the death against other girls. Dangerous.");
	JobName[JOB_FIGHTVOLUNTEERS] = ("Fight Volunteers");
	JobDesc[JOB_FIGHTVOLUNTEERS] = ("She will fight against customer volunteers for gold prizes. Not dangerous.");
	JobName[JOB_COLLECTBETS] = ("Collect Bets");
	JobDesc[JOB_COLLECTBETS] = ("She will collect bets from the customers. (requires 1)");

	JobFilterName[JOBFILTER_TRAININGCENTRE] = ("Training Centre");
	JobFilterDesc[JOBFILTER_TRAININGCENTRE] = ("These are jobs for running a training centre.");
	JobFilterIndex[JOBFILTER_TRAININGCENTRE] = JOB_TEACHBDSM;
	JobName[JOB_TEACHBDSM] = ("Teach BDSM");
	JobDesc[JOB_TEACHBDSM] = ("She will teach other girls in BDSM skills.");
	JobName[JOB_TEACHSEX] = ("Teach Sex");
	JobDesc[JOB_TEACHSEX] = ("She will teach other girls the ins and outs of regular sex.");
	JobName[JOB_TEACHBEAST] = ("Teach Bestiality");
	JobDesc[JOB_TEACHBEAST] = ("She will teach other girls how to safely fuck beasts.");
	JobName[JOB_TEACHMAGIC] = ("Teach Magic");
	JobDesc[JOB_TEACHMAGIC] = ("She will teach other girls how to wield magic.");
	JobName[JOB_TEACHCOMBAT] = ("Teach Combat");
	JobDesc[JOB_TEACHCOMBAT] = ("She will teach other girls how to fight.");
	JobName[JOB_DAYCARE] = ("Daycare");
	JobDesc[JOB_DAYCARE] = ("She will look after local children. (non-sexual)");
	JobName[JOB_SCHOOLING] = ("Schooling");
	JobDesc[JOB_SCHOOLING] = ("She will teach the local children. (non-sexual)");
	JobName[JOB_TEACHDANCING] = ("Teach Dancing");
	JobDesc[JOB_TEACHDANCING] = ("She will teach other girls dancing and social skills.");
	JobName[JOB_TEACHSERVICE] = ("Teach Service");
	JobDesc[JOB_TEACHSERVICE] = ("She will teach other girls a variety of service skills.");
	JobName[JOB_TRAIN] = ("Student");
	JobDesc[JOB_TRAIN] = ("She will learn from all of the other girls who are teaching here.");

#endif

	// extra JobFilterIndex, needed by FOR loops (looping from JobFilterIndex[XYZ] to JobFilterIndex[XYZ+1])
	JobFilterIndex[NUMJOBTYPES] = NUM_JOBS;

}

#ifdef _DEBUG
void cJobManager::freeJobs()
{
}
#else
void cJobManager::free()
{
}
#endif

// ----- Misc

bool cJobManager::WorkVoid(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
	summary += ("This job isn't implemented yet");
	girl->m_Events.AddMessage(("This job isn't implemented yet"), IMGTYPE_PROFILE, EVENT_DEBUG);
	return false;
}

sCustomer cJobManager::GetMiscCustomer(IBuilding& brothel)
{
    sCustomer cust = g_Game->GetCustomer(brothel);
	brothel.m_MiscCustomers+=1;
	return cust;
}

// ----- Job related

void cJobManager::do_advertising(IBuilding& brothel, bool Day0Night1)
{  // advertising jobs are handled before other jobs, more particularly before customer generation
	brothel.m_AdvertisingLevel = 1.0;  // base multiplier
    for (auto& current : brothel.girls())
    {
		string summary;
		bool refused = false;
		// Added test for current shift, was running each shift twice -PP
		if ((current->m_DayJob == JOB_ADVERTISING) && (Day0Night1 == SHIFT_DAY))
		{
			current->m_Refused_To_Work_Day = WorkAdvertising(current, false, summary, g_Dice);
		}
		if ((current->m_NightJob == JOB_ADVERTISING) && (Day0Night1 == SHIFT_NIGHT))
		{
			current->m_Refused_To_Work_Night = WorkAdvertising(current, true, summary, g_Dice);
		}
	}
}

void cJobManager::do_whorejobs(IBuilding& brothel, bool Day0Night1)
{ // Whores get first crack at any customers.
    for (auto& current : brothel.girls())
    {
		string summary;
		u_int sw = 0;						//	Job type
		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);

		bool refused = false;
		switch (sw)
		{
		case JOB_WHOREBROTHEL:
        case JOB_BARWHORE:
        case JOB_WHOREGAMBHALL:
            refused = WorkWhore(current, Day0Night1, summary, g_Dice);
            break;
		default:
			break;
		}
		if (refused) (Day0Night1 ? current->m_Refused_To_Work_Night = true : current->m_Refused_To_Work_Day = true);
	}
}

void cJobManager::do_custjobs(IBuilding& brothel, bool Day0Night1)
{ // Customer taking jobs get first crack at any customers before customer service.
    for (auto& current : brothel.girls())
    {
		string summary;
		u_int sw = 0;						//	Job type
		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);

		bool refused = false;
		switch (sw)
		{
		case JOB_BARMAID:
        case JOB_WAITRESS:
        case JOB_SINGER:
        case JOB_PIANO:
        case JOB_DEALER:
        case JOB_ENTERTAINMENT:
        case JOB_XXXENTERTAINMENT:
        case JOB_SLEAZYBARMAID:
        case JOB_SLEAZYWAITRESS:
        case JOB_BARSTRIPPER:
        case JOB_MASSEUSE:
        case JOB_BROTHELSTRIPPER:
        case JOB_PEEP:
            refused = JobFunc[sw](current, Day0Night1, summary, g_Dice);
            break;
		default:
			break;
		}
	}
}

bool cJobManager::FullTimeJob(u_int Job)
{
	return (
		Job == JOB_MATRON ||
		Job == JOB_TORTURER ||
		// - Arena
		Job == JOB_DOCTORE ||
		// - Clinic
		Job == JOB_GETHEALING ||
		Job == JOB_GETREPAIRS ||
		Job == JOB_GETABORT ||
		Job == JOB_COSMETICSURGERY ||
		Job == JOB_LIPO ||
		Job == JOB_BREASTREDUCTION ||
		Job == JOB_BOOBJOB ||
		Job == JOB_VAGINAREJUV ||
		Job == JOB_FACELIFT ||
		Job == JOB_ASSJOB ||
		Job == JOB_TUBESTIED ||
		Job == JOB_FERTILITY ||
		// - clinic staff
		Job == JOB_CHAIRMAN ||
		Job == JOB_DOCTOR ||
		Job == JOB_NURSE ||
		Job == JOB_MECHANIC ||
		// - Community Centre
		Job == JOB_CENTREMANAGER ||
		// - drug centre
		Job == JOB_COUNSELOR ||
		Job == JOB_REHAB ||
		Job == JOB_ANGER ||
		Job == JOB_EXTHERAPY ||
		Job == JOB_THERAPY ||
		// - House
		Job == JOB_HEADGIRL ||
		Job == JOB_RECRUITER ||
		Job == JOB_FAKEORGASM ||
		Job == JOB_SO_STRAIGHT ||
		Job == JOB_SO_BISEXUAL ||
		Job == JOB_SO_LESBIAN ||
		Job == JOB_HOUSEPET ||
		// - Farm
		Job == JOB_FARMMANGER ||
		//Job == JOB_PONYGIRL ||
		false);
}

bool cJobManager::is_job_Paid_Player(u_int Job)
{
/*
 *	WD:	Tests for jobs that the player pays the wages for
 *
 *	Jobs that earn money for the player are commented out
 *
 */

	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager::is_job_Paid_Player
	return (
		// ALL JOBS

		// - General
		//Job ==	JOB_RESTING				||	// relaxes and takes some time off
		Job ==	JOB_TRAINING			||	// trains skills at a basic level
		Job ==	JOB_CLEANING			||	// cleans the building
		Job ==	JOB_SECURITY			||	// Protects the building and its occupants
		Job ==	JOB_ADVERTISING			||	// Goes onto the streets to advertise the buildings services
		Job ==	JOB_CUSTOMERSERVICE		||	// looks after customers needs (customers are happier when people are doing this job)
		Job ==	JOB_MATRON				||	// looks after the needs of the girls (only 1 allowed)
		Job ==	JOB_TORTURER			||	// Tortures the people in the dungeons to help break their will (this is in addition to player torture) (only 1 allowed)
		//Job ==	JOB_EXPLORECATACOMBS	||	// goes adventuring in the catacombs
		Job ==	JOB_BEASTCAPTURE		||	// captures beasts for use (only 2 allowed) (creates beasts resource)
		Job ==	JOB_BEASTCARER			||	// cares for captured beasts (only 1 allowed) (creates beasts resource and lowers their loss)

		// Clinic
		Job ==	JOB_DOCTOR				||	// becomes a doctor (requires 1) (will make some extra cash for treating locals)
		Job ==	JOB_CHAIRMAN			||	//
		Job ==	JOB_JANITOR			    ||	//

		// - Movie Crystal Studio
		Job == JOB_FILMACTION			||	// films this sort of scene in the movie
		Job == JOB_FILMCHEF				||	// films this sort of scene in the movie
		Job == JOB_FILMMUSIC			||	// films this sort of scene in the movie
		Job == JOB_FILMMAST				||	// films this sort of scene in the movie
		Job == JOB_FILMSTRIP			||	// films this sort of scene in the movie
		Job == JOB_FILMTEASE			||	// films this sort of scene in the movie
		Job == JOB_FILMANAL				||	// films this sort of scene in the movie
		Job == JOB_FILMFOOTJOB			||	// films this sort of scene in the movie
		Job == JOB_FILMHANDJOB			||	// films this sort of scene in the movie
		Job == JOB_FILMLESBIAN			||	// films this sort of scene in the movie
		Job == JOB_FILMORAL				||	// films this sort of scene in the movie
		Job == JOB_FILMSEX				||	// films this sort of scene in the movie
		Job == JOB_FILMTITTY			||	// films this sort of scene in the movie
		Job == JOB_FILMBEAST			||	// films this sort of scene in the movie (uses beast resource)
		Job == JOB_FILMBONDAGE			||	// films this sort of scene in the movie
		Job == JOB_FILMBUKKAKE			||	// films this sort of scene in the movie
		Job == JOB_FILMFACEFUCK			||	// films this sort of scene in the movie
		Job == JOB_FILMGROUP			||	// films this sort of scene in the movie
		Job == JOB_FILMPUBLICBDSM		||	// films this sort of scene in the movie
		Job == JOB_FILMRANDOM			||	// films this sort of scene in the movie

		Job ==	JOB_DIRECTOR			||	// Direcets the movies
		Job ==	JOB_PROMOTER			||	// Advertises the studio's films
		Job ==	JOB_CAMERAMAGE			||	// Uses magic to record the scenes to crystals (requires at least 1)
		Job ==	JOB_CRYSTALPURIFIER		||	// Post editing to get the best out of the film (requires at least 1)
		Job ==	JOB_FLUFFER				||	// Keeps the porn stars and animals aroused
		Job ==	JOB_STAGEHAND			||	// Sets up equipment, and keeps studio clean
		Job ==	JOB_FILMFREETIME		||	// Taking a day off

		// - Community Centre
		Job ==	JOB_FEEDPOOR			||	// work in a soup kitchen
		Job ==	JOB_COMUNITYSERVICE		||	// Goes around town helping where they can
		Job ==	JOB_CENTREMANAGER		||	// work in a soup kitchen
		Job ==	JOB_CLEANCENTRE		    ||	// Goes around town helping where they can

		//arena
		Job ==	JOB_DOCTORE		        ||	//
		Job ==	JOB_CLEANARENA		    ||	//
		Job ==	JOB_FIGHTTRAIN		    ||	//
		Job ==	JOB_FIGHTARENAGIRLS		||	//
		Job ==	JOB_FIGHTBEASTS			||	//

		//farm
		Job ==	JOB_FARMMANGER			||	//
		Job ==	JOB_FARMHAND			||	//

		//house
		Job ==	JOB_HEADGIRL			||	//
		Job ==	JOB_RECRUITER			||	//
		Job ==	JOB_PERSONALBEDWARMER	||	//
		Job ==	JOB_HOUSECOOK			||	//
		Job ==	JOB_CLEANHOUSE			||	//
		Job ==	JOB_PERSONALTRAINING	||	//
		Job ==	JOB_FAKEORGASM			||	//
		Job ==	JOB_SO_STRAIGHT			||	//
		Job ==	JOB_SO_BISEXUAL			||	//
		Job ==	JOB_SO_LESBIAN			||	//
		Job ==	JOB_HOUSEPET			||	//

		// - Brothel
		//Job ==	JOB_WHOREBROTHEL		||	// whore herself inside the building
		//Job ==	JOB_WHORESTREETS		||	// whore self on the city streets
		//Job ==	JOB_BROTHELSTRIPPER		||	// strips for customers and sometimes sex
		//Job ==	JOB_MASSEUSE			||	// gives massages to patrons and sometimes sex

		// - Gambling Hall - Using WorkHall() or WorkWhore()
		//Job ==	JOB_WHOREGAMBHALL		||	// looks after customers sexual needs
		//Job ==	JOB_DEALER				||	// dealer for gambling tables
		//Job ==	JOB_ENTERTAINMENT		||	// sings, dances and other shows for patrons
		//Job ==	JOB_XXXENTERTAINMENT	||	// naughty shows for patrons

		// - Bar Using - WorkBar() or WorkWhore()
		//Job ==	JOB_BARMAID				||	// serves at the bar (requires 1 at least)
		//Job ==	JOB_WAITRESS			||	// waits on the tables (requires 1 at least)
		//Job ==	JOB_STRIPPER			||	// strips for the customers and gives lapdances
		//Job ==	JOB_WHOREBAR			||	// offers sexual services to customers
		//Job ==	JOB_SINGER				||	// sings int the bar (girl needs singer trait)

/*
 *		Not Implemented
 */

#if 0

		// - Community Centre
		Job ==	JOB_COLLECTDONATIONS	||	// collects money to help the poor
		Job ==	JOB_ARTISAN				||	// makes cheap crappy handmade items for selling to raise money (creates handmade item resource)
		Job ==	JOB_SELLITEMS			||	// goes out and sells the made items (sells handmade item resource)

		// - Drug Lab (these jobs gain bonus if in same building as a clinic)
		Job ==	JOB_VIRASPLANTFUCKER	||	// the plants need to inseminate with other humanoids to proper-gate, creates vira blood items
		Job ==	JOB_SHROUDGROWER		||	// They require lots of care, and may explode. Produces shroud mushroom item.
		Job ==	JOB_FAIRYDUSTER			||	// captures and pounds faries to dust, produces fairy dust item
		Job ==	JOB_DRUGDEALER			||	// goes out onto the streets to sell the items made with the other jobs

		// - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
		Job ==	JOB_FINDREGENTS			||	// goes around and finds ingredients for potions, produces ingredients resource.
		Job ==	JOB_BREWPOTIONS			||	// will randomly produce any items with the word "potion" in the name, uses ingredients resource
		Job ==	JOB_POTIONTESTER		||	// Potion tester tests potions, they may die doing this or random stuff may happen. (requires 1)

		// - Arena (these jobs gain bonus if in same building as a clinic)
		Job ==	JOB_FIGHTBEASTS			||	// customers come to place bets on who will win, girl may die (uses beasts resource)
		Job ==	JOB_WRESTLE				||	// as above no chance of dying
		Job ==	JOB_FIGHTTODEATH		||	// as above but against other girls (chance of dying)
		Job ==	JOB_FIGHTVOLUNTEERS		||	// fight against customers who volunteer for prizes of gold
		Job ==	JOB_COLLECTBETS			||	// collects the bets from customers (requires 1)

		// - Skills Centre
		Job ==	JOB_TEACHBDSM			||	// teaches BDSM skills
		Job ==	JOB_TEACHSEX			||	// teaches general sex skills
		Job ==	JOB_TEACHBEAST			||	// teaches beastiality skills
		Job ==	JOB_TEACHMAGIC			||	// teaches magic skills
		Job ==	JOB_TEACHCOMBAT			||	// teaches combat skills
		Job ==	JOB_DAYCARE				||	// looks after the local kids (non sex stuff of course)
		Job ==	JOB_SCHOOLING			||	// teaches the local kids (non sex stuff of course)
		Job ==	JOB_TEACHDANCING		||	// teaches dancing and social skills
		Job ==	JOB_TEACHSERVICE		||	// teaches misc skills
		Job ==	JOB_TRAIN				||	// trains the girl in all the disicplince for which there is a teacher

		// - Clinic
		Job ==	JOB_DOCTOR				||	// becomes a doctor (requires 1) (will make some extra cash for treating locals)
		Job ==	JOB_GETABORT			||	// gets an abortion (takes 2 days)
		Job ==	JOB_COSMETICSURGERY		||	// magical plastic surgery (takes 5 days)
		Job ==	JOB_GETHEALING			||	// takes 1 days for each wound trait received.
		Job ==	JOB_GETREPAIRS			||	// construct girls can get repaired quickly
#endif

		// - extra unassignable
		//Job ==	JOB_INDUNGEON			||
		//Job ==	JOB_RUNAWAY				||

		false );


}

// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager.cpp > HandleSpecialJobs
bool cJobManager::HandleSpecialJobs(sGirl* Girl, int JobID, int OldJobID, bool Day0Night1, bool fulltime)
{
	bool MadeChanges = true;  // whether a special case applies to specified job or not
	g_CTRLDown = false;

	/*	`J` added to reset working days only if her job actually changes
	 * m_WorkingDay is only used for full time jobs that take more than 1 day to complete
	 * such as getting surgerys or rehab
	*/
	if (Girl->m_WorkingDay > 0)	// `J` Save it and reset it to 0 so it is only backed up once...
	{
		if ((Girl->m_WorkingDay < 2 && (Girl->m_YesterDayJob == JOB_GETABORT )) ||
			(Girl->m_WorkingDay < 3 && (Girl->m_YesterDayJob == JOB_REHAB)) ||
			(Girl->m_WorkingDay < 5 && Girl->m_YesterDayJob != JOB_GETABORT && cJobManager::is_Surgery_Job(Girl->m_YesterDayJob)) ||
			// `J` training jobs use m_WorkingDay as a percent learned
			(Girl->m_YesterDayJob == JOB_FAKEORGASM || Girl->m_YesterDayJob == JOB_SO_STRAIGHT || Girl->m_YesterDayJob == JOB_SO_BISEXUAL || Girl->m_YesterDayJob == JOB_SO_LESBIAN)
			)
		{
			Girl->m_PrevWorkingDay = Girl->m_WorkingDay;
		}
		Girl->m_WorkingDay = 0;
	}
	if (Girl->m_YesterDayJob == JobID)	// `J` ...so that if you decide to put her back onto the job...
	{
		Girl->m_WorkingDay = Girl->m_PrevWorkingDay;	// `J` ...it will restore the previous days
	}
	u_int rest = JOB_RESTING;
	if(Girl->m_Building)
	    rest = Girl->m_Building->m_RestJob;

	// rest jobs
	if (u_int(JobID) == JOB_FILMFREETIME)
	{
		Girl->m_NightJob = Girl->m_DayJob = JobID;
	}
	else if (u_int(JobID) == rest)
	{
		/*   */if (fulltime)	Girl->m_NightJob = Girl->m_DayJob = JobID;
		else if (Day0Night1)	Girl->m_NightJob = JobID;
		else/*            */	Girl->m_DayJob = JobID;
	}
// Special Brothel Jobs
#if 1
	else if (u_int(JobID) == JOB_MATRON)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_MATRON, Day0Night1) > 0)
			g_Game->push_message(("You can only have one matron per brothel."), 0);
		else if (Girl->is_slave())
			g_Game->push_message(("The matron cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_MATRON;
	}
	else if (u_int(JobID) == JOB_TORTURER)
	{
		if (random_girl_on_job(g_Game->buildings(), JOB_TORTURER, 0))
			g_Game->push_message(("You can only have one torturer among all of your brothels."), 0);
		else if (Girl->is_slave())
			g_Game->push_message(("The torturer cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_TORTURER;
	}
#endif
	// Special House Jobs
#if 1
	else if (u_int(JobID) == JOB_HEADGIRL)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_HEADGIRL, Day0Night1) > 0)	g_Game->push_message(("There can be only one Head Girl!"), 0);
		else if (Girl->is_slave())/*            */	g_Game->push_message(("The Head Girl cannot be a slave."), 0);
		else /*                                 */	Girl->m_NightJob = Girl->m_DayJob = JOB_HEADGIRL;
	}
	else if (u_int(JobID) == JOB_RECRUITER)
	{
		if (Girl->is_slave())/*                 */	g_Game->push_message(("The recruiter cannot be a slave."), 0);
		else /*                                 */	Girl->m_NightJob = Girl->m_DayJob = JOB_RECRUITER;
	}
	else if (u_int(JobID) == JOB_FAKEORGASM)
	{
		if (Girl->has_trait("Fake Orgasm Expert"))	g_Game->push_message(("She already has \"Fake Orgasm Expert\"."), 0);
		else /*                                 */	Girl->m_DayJob = Girl->m_NightJob = JOB_FAKEORGASM;
	}
	else if (u_int(JobID) == JOB_SO_BISEXUAL)
	{
		if (Girl->has_trait("Bisexual"))/*      */	g_Game->push_message(("She is already Bisexual."), 0);
		else /*                                 */	Girl->m_DayJob = Girl->m_NightJob = JOB_SO_BISEXUAL;
	}
	else if (u_int(JobID) == JOB_SO_LESBIAN)
	{
		if (Girl->has_trait("Lesbian"))/*       */	g_Game->push_message(("She is already a Lesbian."), 0);
		else /*                                 */	Girl->m_DayJob = Girl->m_NightJob = JOB_SO_LESBIAN;
	}
	else if (u_int(JobID) == JOB_SO_STRAIGHT)
	{
		if (Girl->has_trait("Straight"))/*      */	g_Game->push_message(("She is already Straight."), 0);
		else /*                                 */	Girl->m_DayJob = Girl->m_NightJob = JOB_SO_STRAIGHT;
	}
	else if (u_int(JobID) == JOB_HOUSEPET)
	{
		if (Girl->is_slave())/*                 */	Girl->m_NightJob = Girl->m_DayJob = JOB_HOUSEPET;
		else /*                                 */	g_Game->push_message(("Only slaves can take this training."), 0);
	}
//	else if (u_int(JobID) == JOB_PONYGIRL)
//	{
//		if (Girl->is_slave())	Girl->m_NightJob = Girl->m_DayJob = JOB_PONYGIRL;
//		else					g_Game->push_message(("Only slaves can take this training."), 0);
//	}
#endif
	// Special Farm Jobs
#if 1
	else if (u_int(JobID) == JOB_FARMMANGER)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_FARMMANGER, Day0Night1) > 0)
			g_Game->push_message(("There can be only one Farm Manager!"), 0);
		else if (Girl->is_slave())
			g_Game->push_message(("The Farm Manager cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_FARMMANGER;
	}
	else if (u_int(JobID) == JOB_MARKETER)
	{
		if (Girl->is_slave())
			g_Game->push_message(("The Farm Marketer cannot be a slave."), 0);
		else
		{
			if (Day0Night1 == SHIFT_DAY || fulltime)
			{
				if (Girl->m_Building->num_girls_on_job(JOB_MARKETER, SHIFT_DAY) > 0)
					g_Game->push_message(("There can be only one Farm Marketer on each shift!"), 0);
				else Girl->m_DayJob = JOB_MARKETER;
			}
			if (Day0Night1 == SHIFT_NIGHT || fulltime)
			{
				if (Girl->m_Building->num_girls_on_job(JOB_MARKETER, SHIFT_NIGHT) > 0)
					g_Game->push_message(("There can be only one Farm Marketer on each shift!"), 0);
				else Girl->m_NightJob = JOB_MARKETER;
			}

		}
	}
#endif
	// Special Arena Jobs
#if 1
	else if (u_int(JobID) == JOB_DOCTORE)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_DOCTORE, Day0Night1) > 0)
			g_Game->push_message(("There can be only one Doctore!"), 0);
		else if (Girl->is_slave())
			g_Game->push_message(("The Doctore cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_DOCTORE;
	}
	else if (u_int(JobID) == JOB_CITYGUARD && Girl->is_slave())
	{
		g_Game->push_message(("Slaves can not work as City Guards."), 0);
	}
	else if (u_int(JobID) == JOB_FIGHTTRAIN && (Girl->combat() > 99 && Girl->magic() > 99 && Girl->agility() > 99 && Girl->constitution() > 99))
	{	// `J` added then modified
		g_Game->push_message(("There is nothing more she can learn here."), 0);
		if (Girl->m_DayJob == JOB_FIGHTTRAIN)	Girl->m_DayJob = rest;
		if (Girl->m_NightJob == JOB_FIGHTTRAIN)	Girl->m_NightJob = rest;
	}
#endif
	// Special Clinic Jobs
#if 1
	else if (u_int(JobID) == JOB_CHAIRMAN)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_CHAIRMAN, Day0Night1)>0)
			g_Game->push_message(("There can be only one Chairman!"), 0);
		else if (Girl->is_slave())
			g_Game->push_message(("The Chairman cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_CHAIRMAN;
	}

	else if (Girl->has_trait( "AIDS") && (u_int(JobID) == JOB_DOCTOR || u_int(JobID) == JOB_NURSE || u_int(JobID) == JOB_INTERN))
	{
		g_Game->push_message(("Health laws prohibit anyone with AIDS from working in the Medical profession"), 0);
		if (Girl->m_DayJob == JOB_INTERN || Girl->m_DayJob == JOB_NURSE || Girl->m_DayJob == JOB_DOCTOR)
			Girl->m_DayJob = rest;
		if (Girl->m_NightJob == JOB_INTERN || Girl->m_NightJob == JOB_NURSE || Girl->m_NightJob == JOB_DOCTOR)
			Girl->m_NightJob = rest;
	}
	else if (u_int(JobID) == JOB_DOCTOR)
	{
		if (Girl->is_slave())
		{
			g_Game->push_message(("The Doctor cannot be a slave."), 0);
		}
		else if (Girl->medicine() < 50 || Girl->intelligence() < 50)
		{
			stringstream ss;
			ss << Girl->m_Realname << " does not have enough training to work as a Doctor. Doctors require 50 Medicine and 50 Intelligence.";
			g_Game->push_message(ss.str(), 0);
		}
		else Girl->m_NightJob = Girl->m_DayJob = JOB_DOCTOR;
	}
	else if (u_int(JobID) == JOB_INTERN && Girl->medicine() > 99 && Girl->intelligence() > 99 && Girl->charisma() > 99)
	{
		stringstream ss;
		ss << "There is nothing more she can learn here.\n";
		if (Girl->is_free())
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_DOCTOR;
			ss << Girl->m_Realname << " has been assigned as a Doctor instead.";
		}
		else
		{
			if (fulltime)
				Girl->m_DayJob = Girl->m_NightJob = JOB_NURSE;
			else if (Day0Night1 == SHIFT_DAY)
				Girl->m_DayJob = JOB_NURSE;
			else
				Girl->m_NightJob = JOB_NURSE;
			ss << Girl->m_Realname << " has been assigned as a Nurse instead.";
		}
		g_Game->push_message(ss.str(), 0);
	}
	else if (u_int(JobID) == JOB_MECHANIC)
	{
		Girl->m_NightJob = Girl->m_DayJob = JOB_MECHANIC;
	}
	else if (u_int(JobID) == JOB_GETHEALING && Girl->has_trait( "Construct"))
	{
		if (fulltime)
			Girl->m_DayJob = Girl->m_NightJob = JOB_GETREPAIRS;
		else if (Day0Night1 == SHIFT_DAY)
			Girl->m_DayJob = JOB_GETREPAIRS;
		else
			Girl->m_NightJob = JOB_GETREPAIRS;
		g_Game->push_message(("The Doctor does not work on Constructs so she sends ") + Girl->m_Realname + (" to the Mechanic."), 0);
	}
	else if (u_int(JobID) == JOB_GETREPAIRS && !Girl->has_trait( "Construct") && !Girl->has_trait( "Half-Construct"))
	{
		if (fulltime)
			Girl->m_DayJob = Girl->m_NightJob = JOB_GETHEALING;
		else if (Day0Night1 == SHIFT_DAY)
			Girl->m_DayJob = JOB_GETHEALING;
		else
			Girl->m_NightJob = JOB_GETHEALING;
		g_Game->push_message(("The Mechanic only works on Constructs so she sends ") + Girl->m_Realname + (" to the Doctor."), 0);
	}
	// `J` condensed clinic surgery jobs into one check
	else if (
		u_int(JobID) == JOB_CUREDISEASES ||
		u_int(JobID) == JOB_GETABORT ||
		u_int(JobID) == JOB_COSMETICSURGERY ||
		u_int(JobID) == JOB_BREASTREDUCTION ||
		u_int(JobID) == JOB_BOOBJOB ||
		u_int(JobID) == JOB_ASSJOB ||
		u_int(JobID) == JOB_FACELIFT ||
		u_int(JobID) == JOB_VAGINAREJUV ||
		u_int(JobID) == JOB_LIPO ||
		u_int(JobID) == JOB_TUBESTIED ||
		u_int(JobID) == JOB_FERTILITY)
	{
		bool jobgood = true;
		if (Girl->m_Building->num_girls_on_job(JOB_DOCTOR, Day0Night1) == 0)
		{
			g_Game->push_message(("You must have a Doctor on duty to perform surgery."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_CUREDISEASES && !Girl->has_disease())
		{
			g_Game->push_message(("Oops, the girl does not have any diseases."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_GETABORT && !Girl->is_pregnant())
		{
			g_Game->push_message(("Oops, the girl is not pregant."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_COSMETICSURGERY)
		{
		}
		else if (u_int(JobID) == JOB_BREASTREDUCTION && Girl->has_trait( "Flat Chest"))
		{
			g_Game->push_message(("Her boobs can't get no smaller."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_BOOBJOB && Girl->has_trait( "Titanic Tits"))
		{
			g_Game->push_message(("Her boobs can't get no bigger."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_ASSJOB && Girl->has_trait( "Great Arse"))
		{
			g_Game->push_message(("Her ass can't get no better."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_FACELIFT && Girl->age() <= 21)
		{
			g_Game->push_message(("She is to young for a face lift."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_VAGINAREJUV && Girl->check_virginity())
		{
			g_Game->push_message(("She is a virgin and has no need of this operation."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_LIPO && Girl->has_trait( "Great Figure"))
		{
			g_Game->push_message(("She already has a great figure and doesn't need this."), 0);
			jobgood = false;
		}
		else if (u_int(JobID) == JOB_TUBESTIED)
		{
			if (Girl->is_pregnant())
			{
				g_Game->push_message(Girl->m_Realname + (" is pregant.\nShe must either have her baby or get an abortion before She can get her Tubes Tied."), 0);
				jobgood = false;
			}
			else if (Girl->has_trait( "Sterile"))
			{
				g_Game->push_message(("She is already Sterile and doesn't need this."), 0);
				jobgood = false;
			}
		}
		else if (u_int(JobID) == JOB_FERTILITY)
		{
			if (Girl->is_pregnant())
			{
				g_Game->push_message(Girl->m_Realname +
					" is pregant.\nShe must either have her baby or get an abortion before She can get recieve any more fertility treatments.", 0);
				jobgood = false;
			}
			else if (Girl->has_trait( "Broodmother"))
			{
				g_Game->push_message("She is already as Fertile as she can be and doesn't need any more fertility treatments.", 0);
				jobgood = false;
			}
		}
		Girl->m_DayJob = Girl->m_NightJob = jobgood ? JobID : rest;
	}
#endif
	// Special Centre Jobs
#if 1
	else if (u_int(JobID) == JOB_CENTREMANAGER)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_CENTREMANAGER, Day0Night1) >0)
			g_Game->push_message(("There can be only one Centre Manager!"), 0);
		else if (Girl->is_slave())
			g_Game->push_message(("The Centre Manager cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_CENTREMANAGER;
	}
	else if (u_int(JobID) == JOB_COUNSELOR)
	{
		if (Girl->is_slave())
			g_Game->push_message(("The Counselor cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_COUNSELOR;
	}
	else if (u_int(JobID) == JOB_REHAB)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
			g_Game->push_message(("You must have a counselor for rehab."), 0);
		else if (!Girl->has_trait( "Shroud Addict")
			&& !Girl->has_trait( "Smoker")
			&& !Girl->has_trait( "Cum Addict")
			&& !Girl->has_trait( "Fairy Dust Addict")
			&& !Girl->has_trait( "Alcoholic")
			&& !Girl->has_trait( "Viras Blood Addict"))
			g_Game->push_message(("She has no addictions."), 0);
		else
			Girl->m_DayJob = Girl->m_NightJob = JOB_REHAB;
	}
	else if (u_int(JobID) == JOB_ANGER)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
			g_Game->push_message(("You must have a counselor for anger management."), 0);
		else if (!Girl->has_trait( "Aggressive")
			&& !Girl->has_trait( "Tsundere")
			&& !Girl->has_trait( "Yandere"))
			g_Game->push_message(("She has no anger issues."), 0);
		else
			Girl->m_DayJob = Girl->m_NightJob = JOB_ANGER;
	}
	else if (u_int(JobID) == JOB_EXTHERAPY)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
			g_Game->push_message(("You must have a counselor for extreme therapy."), 0);
		else if (!Girl->has_trait( "Mind Fucked")
			&& !Girl->has_trait( "Broken Will"))
			g_Game->push_message(("She has no extreme issues."), 0);
		else
			Girl->m_DayJob = Girl->m_NightJob = JOB_EXTHERAPY;
	}
	else if (u_int(JobID) == JOB_THERAPY)
	{
		if (Girl->m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
			g_Game->push_message(("You must have a counselor for therapy."), 0);
		else if (!Girl->has_trait( "Nervous")
			&& !Girl->has_trait( "Dependant")
			&& !Girl->has_trait( "Pessimist"))
			g_Game->push_message(("She has no need of therapy."), 0);
		else
			Girl->m_DayJob = Girl->m_NightJob = JOB_THERAPY;
	}
#endif
	// Special Movie Studio Jobs
#if 1
	else if (u_int(JobID) == JOB_DIRECTOR && Girl->m_Building->num_girls_on_job(JOB_DIRECTOR, SHIFT_NIGHT) >0)
	{
		g_Game->push_message(("There can be only one Director!"), 0);
	}
	else if (u_int(JobID) == JOB_DIRECTOR && Girl->is_slave())
	{
		g_Game->push_message(("The Director cannot be a slave."), 0);
	}
	else if (u_int(JobID) == JOB_PROMOTER && Girl->m_Building->num_girls_on_job(JOB_PROMOTER, SHIFT_NIGHT) > 0)
	{
		g_Game->push_message(("There can be only one Promoter."), 0);
	}
	else if (u_int(JobID) == JOB_PROMOTER && Girl->is_slave())
	{
		g_Game->push_message(("The Promoter cannot be a slave."), 0);
	}
	else if (is_Actress_Job(JobID) &&
		(Girl->m_Building->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) < 1 ||
                Girl->m_Building->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) < 1))
	{
		g_Game->push_message("You must have one cameramage and one crystal purifier.", 0);
		Girl->m_DayJob = Girl->m_NightJob = rest;
	}
#endif

// Special cases were checked and don't apply, just set the studio job as requested
#if 1
	else if (Girl->m_Building && Girl->m_Building->type() == BuildingType::STUDIO)
	{
		MadeChanges = false;
		Girl->m_DayJob = rest;
		Girl->m_NightJob = u_int(JobID);
	}
	else
	{
		MadeChanges = false;
		if (fulltime)/*                */	Girl->m_DayJob = Girl->m_NightJob = JobID;
		else if (Day0Night1 == SHIFT_DAY)	Girl->m_DayJob = JobID;
		else/*                         */	Girl->m_NightJob = JobID;
	}
#endif
// handle instances where special job has been removed, specifically where it actually matters
	if (JobID != OldJobID)
	{
		// if old job was full time but new job is not, switch leftover day or night job back to resting
		if (!fulltime && FullTimeJob(OldJobID) && !FullTimeJob(JobID))		// `J` greatly simplified the check
			(Day0Night1 ? Girl->m_DayJob = rest : Girl->m_NightJob = rest);

	}

	return MadeChanges;
}

// ------ Work Related Violence fns

// MYR: Rewrote the work_related_violence to add the security guard job.
//      Girls and gangs now fight using the attack/defense/agility style combat.
//      This will have to be changed to the new style of combat eventually.
//      A new kind of weaker gang not associated with any of the enemy organizations.
//      This "gang" doesn't have weapon levels or access to healing potions.
//      They're just a bunch of toughs out to make life hard for our working girls.

/*
 * return true if violence was committed against the girl
 * false if nothing happened, or if violence was committed
 * against the customer.
 */
bool cJobManager::work_related_violence(sGirl* girl, bool Day0Night1, bool streets)
{
	// the base chance of an attempted rape is higher on the streets
	int rape_chance = (streets ? (int)cfg.prostitution.rape_streets() : (int)cfg.prostitution.rape_brothel());

	IBuilding * Brothl = girl->m_Building;

	// `J` adjusted this a bit so gangs spying on the girl can help also
	vector<sGang *> gangs_guarding = g_Game->gang_manager().gangs_watching_girls();

	int gang_coverage = guard_coverage(&gangs_guarding);

	// night time doubles the chance of attempted rape and reduces the chance for a gang to catch it by 20%
	if (Day0Night1) { rape_chance *= 2; gang_coverage = (int)((float)gang_coverage*0.8f); }

	// if the player has a -ve disposition, this can scare the would-be rapist into behaving himself
	if (g_Dice.percent(g_Game->player().disposition() * -1)) rape_chance = 1;

	if (rape_chance < 1) rape_chance = 1;	// minimum of 1%
	if (g_Dice.percent(rape_chance))
	{
		/*sGang *gang = g_Game->gang_manager().random_gang(gang_v);

		if(gang_stops_rape(girl, gang, gang_chance, Day0Night1))
		return false;
		if(girl_fights_rape(girl, Day0Night1))
		return false; */

		// Get a random weak gang. These are street toughs looking to abuse a girl.
		// They have low stats and smaller numbers. Define it here so that even if
		// the security girl or defending gang is defeated, any casualties they inflicts
		// carry over to the next layer of defense.
		sGang *enemy_gang = g_Game->gang_manager().GetTempWeakGang();
        enemy_gang->give_potions(10);
		// There is also between 1 and 15 of them, not 15 every time
		enemy_gang->m_Num = (max(1, g_Dice.bell(-5, 15)));

		// Three more lines of defense

		// first subtract 10 security point per gang member that is attacking
		Brothl->m_SecurityLevel = Brothl->m_SecurityLevel - enemy_gang->m_Num * 10;	// `J` moved and doubled m_SecurityLevel loss
		// 1. Brothel security
		if (security_stops_rape(girl, enemy_gang, Day0Night1)) return false;
		// 2. Defending gangs
		if (gang_stops_rape(girl, gangs_guarding, enemy_gang, gang_coverage, Day0Night1)) return false;
		// 3. The attacked girl herself
		if (girl_fights_rape(girl, enemy_gang, Day0Night1)) return false;
		// If all defensive measures fail...
		// subtract 10 security points per gang member left
		Brothl->m_SecurityLevel = Brothl->m_SecurityLevel - enemy_gang->m_Num * 10;	// `J` moved and doubled m_SecurityLevel loss
		customer_rape(girl, enemy_gang->m_Num);
		return true;
	}
	girl->upd_Enjoyment(ACTION_SEX, +1);
	/*
	 *	the fame thing could work either way.
	 *	of course, that road leads to us keeping reputation for
	 *	different things - beating the customer would be a minus
	 *	for the straight sex crowd, but a big pull for the
	 *	femdom lovers...
	 *
	 *	But let's get what we have working first
	 */
	if ((girl->has_trait("Yandere") || girl->has_trait("Tsundere") || girl->has_trait("Aggressive")) && g_Dice.percent(30))
	{
		switch (g_Dice % 5)
			{
			case 0:
				girl->m_Events.AddMessage(("She beat the customer silly."), IMGTYPE_COMBAT, Day0Night1);
				break;
			case 1:
				girl->m_Events.AddMessage(("The customer's face annoyed her, so she punched it until it went away."), IMGTYPE_COMBAT, Day0Night1);
				break;
			case 2:
				girl->m_Events.AddMessage(("The customer acted like he owned her - so she pwned him."), IMGTYPE_COMBAT, Day0Night1);
				break;
			case 3:
				girl->m_Events.AddMessage(("The customer's attitude was bad. She corrected it."), IMGTYPE_COMBAT, Day0Night1);
				break;
			case 4:
				girl->m_Events.AddMessage(("He tried to insert a bottle into her, so she 'gave it' to him instead."), IMGTYPE_COMBAT, Day0Night1);
				break;
			default:
				girl->m_Events.AddMessage(("Did som(E)thing violent."), IMGTYPE_COMBAT, Day0Night1);
				break;
			}
		girl->fame(-1);
	}
	return false;
}

// We need a cGuards guard manager. Or possible a cGuardsGuards manager.
int cJobManager::guard_coverage(vector<sGang*> *vpt)
{
	int pc = 0;
	vector<sGang*> v = g_Game->gang_manager().gangs_on_mission(MISS_GUARDING);
	if (vpt != nullptr) *vpt = v;
	for (u_int i = 0; i < v.size(); i++)
	{
		sGang *gang = v[i];
		// our modifier is (gang int - 50) so that gives us a value from -50 to + 50
		float mod = (float)gang->intelligence() - 50;
		// divide by 50 to make it -1.0 to +1.0
		mod /= 50;
		// add one so we have a value from 0.0 to 2.0
		mod += 1;
		/*
		 *		and that's the multiplier
		 *
		 *		a base 6% per gang member give a full strength gang
		 *		with average int a 90% chance of stopping a rape
		 *		at 100 int they get 180.
		 */
		pc += int(6 * gang->m_Num * mod);
	}
	/*
	 *	with 6 brothels, that one gang gives a 15% chance to stop
	 *	any given rape - rising to 30 for 100 intelligence
	 *
	 *	so 3 gangs on guard duty, at full strength, get you 90%
	 *	coverage. And 4 get you 180 which should be bulletproof
	 */
	return pc / g_Game->buildings().num_buildings(BuildingType::BROTHEL);
}

// True means security intercepted the perp(s)
bool cJobManager::security_stops_rape(sGirl * girl, sGang *enemy_gang, int day_night)
{
	IBuilding* Brothl = girl->m_Building;
	int SecLev = Brothl->m_SecurityLevel, OrgNumMem = enemy_gang->m_Num;
	sGirl * SecGuard;

	int p_seclev = 90 + (SecLev / 1000);
	if (p_seclev > 99) p_seclev = 99;
	// A gang takes 5 security points per member to stop
	if (SecLev > OrgNumMem * 5 && g_Dice.percent(p_seclev) &&
		(Brothl->num_girls_on_job(JOB_SECURITY, day_night == SHIFT_DAY) > 0 ||
		!g_Game->gang_manager().gangs_on_mission(MISS_GUARDING).empty()))
		return true;

	// Security guards on duty this shift
	vector<sGirl *> SecGrd = Brothl->girls_on_job(JOB_SECURITY, day_night == SHIFT_DAY);
	// Security guards with enough health to fight
	vector<sGirl *> SecGrdWhoCanFight;

	if (SecGrd.empty()) return false;

	// Remove security guards who are too wounded to fight
	// Easier to work from the end
	for(int i = SecGrd.size() - 1; i >= 0 ; i--)
	{
	  SecGuard = SecGrd[i];
	  if (SecGuard->m_Stats[STAT_HEALTH] <= 50) SecGrd.pop_back();
	  else
	  {
		SecGrdWhoCanFight.push_back(SecGrd.back());
		SecGrd.pop_back();
	  }
	}

	// If all the security guards are too wounded to fight
	if (SecGrdWhoCanFight.empty()) return false;

	// Get a random security guard
	SecGuard = SecGrdWhoCanFight.at(g_Dice%SecGrdWhoCanFight.size());

	string SecName = SecGuard->m_Realname;

	// Most of the rest of this is a copy-paste from customer_rape
	bool res = g_Game->gang_manager().GirlVsEnemyGang(SecGuard, enemy_gang);

	// Earn xp for all kills, even if defeated
	int xp = 3;

	if (SecGuard->has_trait("Quick Learner"))		xp += 2;
	else if (SecGuard->has_trait("Slow Learner"))	xp -= 2;

	int num = OrgNumMem - enemy_gang->m_Num;
	SecGuard->exp(num * xp);

	if (res)  // Security guard wins
	{
		SecGuard->combat(1);
		SecGuard->magic(1);
		SecGuard->upd_temp_stat(STAT_LIBIDO, num, true);  // There's nothing like killin ta make ya horny!
		SecGuard->confidence(num);
		SecGuard->fame(num);
		girl->upd_Enjoyment(ACTION_COMBAT, num);
		girl->upd_Enjoyment(ACTION_WORKSECURITY, num);

		stringstream Gmsg;
		stringstream SGmsg;

		// I decided to not say gang in the text. It can be confused with a player or enemy organization's
		// gang, when it is neither.
		if (OrgNumMem == 1)
		{
			bool female = g_Dice.percent(30);
			Gmsg << "A customer tried to attack " << girl->m_Realname << ", but " << SecName << " intercepted and beat ";
			SGmsg << "Security Report:\nA customer tried to attack " << girl->m_Realname << ", but " << SecName << " intercepted and beat ";
			if (female)
			{
				Gmsg << "her.";
				SGmsg << "her.";
				string item;
				if (g_Game->player().inventory().has_item("Brainwashing Oil"))
				{
					item = "Brainwashing Oil";
					SGmsg << "\n \n" << SecName << " forced a bottle of Brainwashing Oil down her throat. After a few minutes of struggling, your new slave, ";
				}
				else if (g_Game->player().inventory().has_item("Necklace of Control"))
				{
					item = "Necklace of Control";
					SGmsg << "\n \n" << SecName << " placed a Necklace of Control around her neck. After a few minutes of struggling, the magic in the necklace activated and your new slave, ";
				}
				else if (g_Game->player().inventory().has_item("Slave Band"))
				{
					item = "Slave Band";
					SGmsg << "\n \n" << SecName << " placed a Slave Band on her arm. After a few minutes of struggling, the magic in the Slave Band activated and your new slave, ";
				}
				if (!item.empty())
				{
					stringstream CGmsg;

					// `J` create the customer
					sGirl* custgirl = g_Game->CreateRandomGirl(g_Dice % 40 + 18, false, true, false, (g_Dice % 3 == 1));	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live

					// `J` and adjust her stats
					sInventoryItem* item_p = g_Game->inventory_manager().GetItem(item);
					g_Game->inventory_manager().Equip(custgirl, custgirl->add_inv(item_p), true);
					g_Game->player().inventory().remove_item(item_p);
                    custgirl->add_trait("Emprisoned Customer", max(5, g_Dice.bell(0, 20)));	// add temp trait
					custgirl->pclove(-(g_Dice % 50 + 50));
					custgirl->pcfear(g_Dice % 50 + 50);
					custgirl->pchate(g_Dice % 50 + 50);
					custgirl->m_Stats[STAT_HEALTH];
					custgirl->m_Enjoyment[ACTION_COMBAT] -= (g_Dice % 50 + 20);
					custgirl->m_Enjoyment[ACTION_SEX] -= (g_Dice % 50 + 20);
                    g_Game->player().suspicion(g_Dice % 10);
                    g_Game->player().disposition(-(g_Dice % 10));
                    g_Game->player().customerfear(g_Dice % 10);

					// `J` do all the messages
					SGmsg << custgirl->m_Realname << " was sent to your dungeon.";
					Gmsg << "\n" << girl->m_Realname << " escorted " << custgirl->m_Realname << " to the dungeon after "
						<< SecName << " gave her attacker a " << item << ", all the while scolding her for her actions.";
					CGmsg << custgirl->m_Realname << " was caught attacking a girl under your employ. She was given a "
						<< item << " and sent to the dungeon as your newest slave.";
					custgirl->m_Events.AddMessage(CGmsg.str(), IMGTYPE_DEATH, EVENT_WARNING);
					// `J` add the customer to the dungeon
					g_Game->dungeon().AddGirl(custgirl, DUNGEON_CUSTBEATGIRL);
				}
			}
			else
			{
				Gmsg << "him.";
				SGmsg << "him.";
				int dildo = 0;
				/* */if (SecGuard->has_item("Compelling Dildo") != -1)	dildo = 1;
				else if (SecGuard->has_item("Dreidel Dildo") != -1)	dildo = 2;
				else if (SecGuard->has_item("Double Dildo") != -1)		dildo = 3;
				if (dildo > 0)
				{
					SGmsg << "\n \n" << SecName << " decided to give this customer a taste of his own medicine and shoved her ";
					/* */if (dildo == 1) SGmsg << "Compelling Dildo";
					else if (dildo == 2) SGmsg << "Dreidel Dildo";
					else if (dildo == 3) SGmsg << "Double Dildo";
					SGmsg << " up his ass.";
                    g_Game->player().suspicion(g_Dice % 2);
                    g_Game->player().disposition(-(g_Dice % 2));
                    g_Game->player().customerfear(g_Dice % 3);
				}
			}
		}
		else
		{
			int i = enemy_gang->m_Num;
			stringstream Tmsg;
			Gmsg << "A group of ";
			SGmsg << "Security Report:\n" << "A group of ";
			if (enemy_gang->m_Num == 0)
				Tmsg << "customers tried to attack " << girl->m_Realname << ". " << SecName << " intercepted and thrashed all " << OrgNumMem;
			else
				Tmsg << OrgNumMem << " customers tried to attack " << girl->m_Realname << ". They fled after " << SecName << " intercepted and thrashed " << num;
			Tmsg << " of them.";
			Gmsg << Tmsg.str();
			SGmsg << Tmsg.str();
		}
		SecGuard->m_Events.AddMessage(SGmsg.str(), IMGTYPE_COMBAT, EVENT_WARNING);
		girl->m_Events.AddMessage(Gmsg.str(), IMGTYPE_COMBAT, EVENT_WARNING);
	}
	else  // Loss
	{
		u_int attacktype = SKILL_COMBAT;												// can be anything
		if (enemy_gang->m_Num > 1 && g_Dice.percent(40)) attacktype = SKILL_GROUP;		// standard group attack
		else if (enemy_gang->m_Num > 1 && g_Dice.percent(40)) attacktype = SKILL_BDSM;	// special group attack
		else if (g_Dice.percent(20)) attacktype = SKILL_LESBIAN;						// female attacker
		else if (g_Dice.percent(40)) attacktype = SKILL_NORMALSEX;						// single male attacker


		stringstream ss;
		ss << ("Security Problem:\n") << ("Trying to defend ") << girl->m_Realname << (". You defeated ")
			<< num << (" of ") << OrgNumMem << (" before:\n") << SecGuard->m_Realname << GetGirlAttackedString(attacktype);

		SecGuard->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);

		// Bad stuff
		SecGuard->happiness(-40);
		SecGuard->confidence(-40);
		SecGuard->obedience(-10);
		SecGuard->spirit(-40);
		SecGuard->libido(-4);
		SecGuard->upd_temp_stat(STAT_LIBIDO, -40, true);
		SecGuard->tiredness(60);
		SecGuard->pcfear(20);
		SecGuard->pclove(-20);
		SecGuard->pchate(20);
		cGirls::GirlInjured(SecGuard, 10); // MYR: Note
		SecGuard->upd_Enjoyment(ACTION_WORKSECURITY, -30);
		SecGuard->upd_Enjoyment(ACTION_COMBAT, -30);
	}
	return res;
}

bool cJobManager::gang_stops_rape(sGirl* girl, vector<sGang *> gangs_guarding, sGang *enemy_gang, int coverage, int day_night)
{
	if (!g_Dice.percent(coverage)) return false;

	sGang *guarding_gang = gangs_guarding.at(g_Dice%gangs_guarding.size());

	// GangVsGang returns true if enemy_gang loses
	bool guards_win = g_Game->gang_manager().GangBrawl(guarding_gang, enemy_gang);

	if (!guards_win) {
		stringstream gang_s, girl_s;
		gang_s << guarding_gang->name() << (" was defeated defending ") << girl->m_Realname << (".");
		girl_s << guarding_gang->name() << (" was defeated defending you from a gang of rapists.");
		guarding_gang->m_Events.AddMessage(gang_s.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_Events.AddMessage(girl_s.str(), IMGTYPE_DEATH, EVENT_WARNING);
		return false;
	}

	/*
	 * options: smart guards send them off with a warning
	 * dead customers should impact disposition and suspicion
	 * might also need a bribe to cover it up
	 */
	stringstream gang_ss, girl_ss;
	int roll = g_Dice.d100();
	if (roll < guarding_gang->intelligence() / 2)
	{
		gang_ss << ("One of the ") << guarding_gang->name() << (" found a customer getting violent with ")
			<< girl->m_Realname << (" and sent him packing, in fear for his life.");
		girl_ss << ("Customer attempt to rape her, but guards ") << guarding_gang->name() << (" scared him off.");
	}
	else if (roll < guarding_gang->intelligence())
	{
		gang_ss << ("One of the ") << guarding_gang->name() << (" found a customer hurting  ") << girl->m_Realname
			<< (" and beat him bloody before sending him away.");
		girl_ss << ("Customer attempt to rape her, but guards ") << guarding_gang->name() << (" gave him a beating.");
	}
	else
	{
		gang_ss << ("One of the ") << guarding_gang->name() << (" killed a customer who was trying to rape ")
			<< girl->m_Realname << (".");
		girl_ss << ("Customer attempt to rape her, but guards ") << guarding_gang->name() << (" killed them.");
	}
	cerr << "gang ss=" << gang_ss.str() << endl;
	cerr << "girl ss=" << girl_ss.str() << endl;

	girl->m_Events.AddMessage(girl_ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
	guarding_gang->m_Events.AddMessage(gang_ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

	return true;
}

#if 0
/*
 * options: smart guards send them off with a warning
 * dead customers should impact disposition and suspicion
 * might also need a bribe to cover it up
 */
bool cJobManager::gang_stops_rape(sGirl* girl, sGang *gang, int chance, int day_night)
{
	if(g_Dice.percent(chance) == false)
		return false;

	bool gang_wins = false;
	int wlev = *g_Game->gang_manager().GetWeaponLevel();
/*
 *	the odds are tipped very much in favour of the gang
 *
 *	they need to blow both magic and combat rolls three times
 *	to lose - even for a new gang with about 30%, that works out
 *	about an 89% chance to win, rising to 98% with weapon level 4
 *
 *	for gangs in the 90% range, you're looking at 1 in a million chances
 */
 	int bonus = 0;
/*
 *	make the gang leader tougher
 */
	if(gang->m_Num == 1)
		bonus = 10;

	int combat = gang->combat() + 4 * wlev + bonus;
	int magic = gang->magic() + 4 * wlev + bonus;
	for(int i = 0; i < 3; i++) {
		gang_wins = g_Dice.percent(combat) || g_Dice.percent(magic) ;
		if(gang_wins)
						break;

	}
/*
 *	if the gang member lost, tell the player about it
 *	no point in subtelties of simulation if the customer
 *	never knows
 */
	if(!gang_wins) {
		string s;
		if(gang->m_Num == 1)
			s =	("The leader of this gang is dead; killed attempting to prevent a rape. You may want to hire a new gang.");
		else
			s =	("One of this gang's members turned up dead. It looks like he attacked a rapist and lost.");

		gang->m_Num --;
		gang->m_Events.AddMessage(s, 0, 0);
	return false;
	}

	stringstream gang_ss, girl_ss;
	int roll = g_Dice.random(100) + 1;
	if(roll < gang->intelligence() / 2) {
		gang_ss <<	("One of the ") <<	gang->name() <<	(" found a customer getting violent with ")
			<<	girl->m_Realname <<	(" and sent him packing, in fear for his life");
		girl_ss <<	("Customer attempt to rape her ") <<("but the guard scared him off");
	}
	else if(roll < gang->intelligence()) {
		gang_ss	<<	("One of the ") << gang->name() << (" found a customer hurting  ") << girl->m_Realname
			<< (" and beat him bloody before sending him away");
		girl_ss	<<	("Customer attempt to rape her but ") << ("the guard gave him a beating");
	}
	else {
		gang_ss	<<	("One of the ") << gang->name() << (" killed a customer who was trying to rape ")
			<<	girl->m_Realname;
		girl_ss	<<	("Customer attempt to rape her but ") << ("the was killed by the guard.");
	}
	cerr << "gang ss=" << gang_ss.str() << endl;
	cerr << "girl ss=" << girl_ss.str() << endl;
	gang->m_Events.AddMessage(gang_ss.str(), 0, 0);
	girl->m_Events.AddMessage(girl_ss.str(), IMGTYPE_DEATH, day_night);
	return true;
}
#endif

// true means she won

bool cJobManager::girl_fights_rape(sGirl* girl, sGang *enemy_gang, int day_night)
{
	int OrgNumMem = enemy_gang->m_Num;

	bool res = g_Game->gang_manager().GirlVsEnemyGang(girl, enemy_gang);

	// Earn xp for all kills, even if defeated
	int xp = 3;
	if (girl->has_trait( "Quick Learner"))		xp += 2;
	else if (girl->has_trait( "Slow Learner"))	xp -= 2;

	int num = OrgNumMem - enemy_gang->m_Num;

	girl->exp(num * xp);

	if (res)  // She wins.  Yay!
	{
		girl->combat(1);
		girl->magic(1);
		girl->agility(1);
		girl->upd_temp_stat(STAT_LIBIDO, num, true);  // There's nothing like killin ta make ya horny!
		girl->confidence(num);
		girl->fame(num);

		girl->upd_Enjoyment(ACTION_COMBAT, num);

		stringstream msg;

		// MYR: I decided to not say gang in the text. It can be confused with a player or enemy organization's
		//     gang, when it is neither.
		if (OrgNumMem == 1)
		{
		  msg << ("A customer tried to attack her. She killed ") << (g_Dice.percent(20) ? ("her.") : ("him."));
		}
		else
		{
		  if (enemy_gang->m_Num == 0)
		    msg << ("A group of customers tried to assault her. She thrashed all ") << OrgNumMem << (" of them.");
		  else
			msg << ("A group of ") << OrgNumMem << (" customers tried to assault her. They fled after she killed ")
			<< num << (" of them.");
		}
		girl->m_Events.AddMessage(msg.str(), IMGTYPE_COMBAT, EVENT_WARNING);
	}

    // Losing is dealt with later in customer_rapes (called from work_related_violence)
	return res;
}
#if 0
/*
 * Again, we can elaborate this
 *
 * some traits should give a bonus. There should be a chance of
 * dead customers. There should probably be a chance of a dead girls
 *
 * returns true if the girl is successful in fighting the rapist off
 */
bool cJobManager::girl_fights_rape(sGirl* girl, int day_night)
{
	string msg;
	bool res = false;
	int roll = g_Dice%100;

	if(girl->has_trait( "Merciless")) {
		msg =	("The customer tried to rape this girl.  She leaves him beaten and bloody, but still breathing.");
		res = true;
	}
	else if(roll < girl->combat() && roll < girl->magic()) {
		msg =	("The customer attempted to rape her but she remembered her self-defence training and fought him off");
		res = true;
	}
	girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, day_night);
	return false;
}
#endif

/*
 * I think these next three could use a little detail
 * MYR: Added the requested detail (in GetGirlAttackedString() below)
 */
void cJobManager::customer_rape(sGirl* girl, int numberofattackers)
{
	u_int attacktype = SKILL_COMBAT;												// can be anything
	/* */if (numberofattackers > 1 && g_Dice.percent(40)) attacktype = SKILL_GROUP;	// standard group attack
	else if (numberofattackers > 1 && g_Dice.percent(40)) attacktype = SKILL_BDSM;	// special group attack
	else if (g_Dice.percent(20)) attacktype = SKILL_LESBIAN;						// female attacker
	else if (g_Dice.percent(40)) attacktype = SKILL_NORMALSEX;						// single male attacker

	stringstream ss;
	ss << girl->m_Realname << GetGirlAttackedString(attacktype);

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);

	// Made this more harsh, so the player hopefully notices it
	//girl->health(-(g_Dice%10 + 5));  // Oops, can drop health below zero after combat is considered
	girl->happiness(-40);
	girl->confidence(-40);
	girl->obedience(-10);
	girl->spirit(-40);
	girl->libido(-4);
	girl->upd_temp_stat(STAT_LIBIDO, -40, true);
	girl->tiredness(60);
	girl->pcfear(20);
	girl->pclove(-20);
	girl->pchate(20);
	cGirls::GirlInjured(girl, 10); // MYR: Note
	girl->upd_Enjoyment(ACTION_SEX, -30);

	// `J` do Pregnancy and/or STDs
	bool preg = false, std = false, a = false, c = false, h = false, s = false;
	sCustomer Cust = GetMiscCustomer(*girl->m_Building);
	Cust.m_Amount = numberofattackers;

	if (attacktype == SKILL_GROUP || attacktype == SKILL_NORMALSEX)
	{
		Cust.m_IsWoman = false;
		preg = !girl->calc_pregnancy(5 + (Cust.m_Amount * 5), STATUS_PREGNANT, Cust.m_Stats, Cust.m_Skills);
	}
	if (attacktype == SKILL_LESBIAN)
	{
		Cust.m_IsWoman = true;
	}
	if (Cust.m_HasAIDS || Cust.m_HasChlamydia || Cust.m_HasHerpes || Cust.m_HasSyphilis || g_Dice.percent(5))
	{
		std = true;
		if (Cust.m_HasAIDS)			a = true;
		if (Cust.m_HasChlamydia)	c = true;
		if (Cust.m_HasSyphilis)		s = true;
		if (Cust.m_HasHerpes)		h = true;
		if (!a && !c && !s && !h)
		{
			/* */if (g_Dice.percent(20))	a = true;
			else if (g_Dice.percent(20))	c = true;
			else if (g_Dice.percent(20))	s = true;
			else /*                   */	h = true;
		}
	}

	if (preg || std)
	{
		ss.str("");
		ss << girl->m_Realname << "'s rapist";
		if (numberofattackers > 1) ss << "s left their";
		else ss << " left " << (Cust.m_IsWoman ? "her" : "his");
		ss << " calling card behind, ";
		if (preg)			{ ss << "a baby in her belly"; }
		if (preg && std)	{ ss << " and "; }
		else if (preg)		{ ss << ".\n \n"; }
		if (a || c || s || h)	{ bool _and = false;
			if (a)	{ girl->add_trait("AIDS");		ss << "AIDS"; }
			if (a && (c || s || h))							{	ss << " and ";		_and = true; }
			if (c)	{ girl->add_trait("Chlamydia");	ss << "Chlamydia";	_and = false; }
			if (!_and && (a || c) && (s || h))				{	ss << " and ";		_and = true; }
			if (s)	{ girl->add_trait("Syphilis");	ss << "Syphilis";	_and = false; }
			if (!_and && (a || c || s) && h)				{	ss << " and "; }
			if (h)	{ girl->add_trait("Herpes");		ss << "Herpes"; }
			ss << ".\n \n";
		}

		girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
	}
}

// MYR: Lots of different ways to say the girl had a bad day
// doc: let's have this return a string, instead of a stringstream:
// the caller doesn't need the stream and gcc is giving weird type coercion
// errors

string cJobManager::GetGirlAttackedString(u_int attacktype)
{
	int roll1 = 0, roll2 = 0, roll3 = 0, rolla = 0;
	int maxroll1 = 21, maxroll2 = 20, maxroll3 = 21;

	stringstream ss;
	ss << (" was ");

	if (attacktype == SKILL_GROUP)			// standard group attack
	{
		roll1 = g_Dice % maxroll1 + 1;
		roll2 = g_Dice % maxroll2 + 1;
		rolla = g_Dice % 5 + 1;
		switch (rolla)
		{
		case 1:		roll3 = 1;	break;	// "customers."
		case 2:		roll3 = 8;	break;	// "priests."
		case 3:		roll3 = 9;	break;	// "orcs. (What?)"
		case 4:		roll3 = 18;	break;	// "members of the militia."
		case 5:		roll3 = 20;	break;	// "tentacles from the sewers."
		default:	roll3 = g_Dice % maxroll3 + 1;	break;	// fallback just in case
		}
	}
	if (attacktype == SKILL_BDSM)			// special group attack
	{
		roll1 = g_Dice % maxroll1 + 1;	// if roll1 is a BDSM text...
		if (roll1 == 3	||	// "put on the wood horse"
			roll1 == 4	||	// "tied to the bed"
			roll1 == 7	||	// "tied up"
			roll1 == 8	||	// "wrestled to the floor"
			roll1 == 9	||	// "slapped in irons"
			roll1 == 10	||	// "cuffed"
			roll1 == 11	||	// "put in THAT machine"
			roll1 == 14	||	// "hand-cuffed during kinky play"
			roll1 == 18	||	// "chained to the porch railing"
			roll1 == 19	||	// "tied up BDSM-style"
			roll1 == 20	||	// "stretched out on the torture table"
			roll1 == 21	)	// "tied up and hung from the rafters"
		{
			roll2 = g_Dice % maxroll2 + 1;	// ...then roll2 can be anything
		}
		else	// ...otherwise roll2 needs to be BDSM
		{
			rolla = g_Dice % 6 + 1;
			switch (rolla)
			{
			case 1:		roll2 = 1;	break;	// "abused"
			case 2:		roll2 = 2;	break;	// "whipped"
			case 3:		roll2 = 13;	break;	// "had her fleshy bits pierced"
			case 4:		roll2 = 15;	break;	// "penetrated by some object"
			case 5:		roll2 = 17;	break;	// "tortured"
			case 6:		roll2 = 20;	break;	// "her skin was pierced by sharp things"
			default:	roll2 = g_Dice % maxroll2 + 1;	break;	// fallback just in case
			}
		}
		roll3 = g_Dice % maxroll3 + 1;
	}
	if (attacktype == SKILL_LESBIAN)		// female attacker
	{
		roll1 = g_Dice % maxroll1 + 1;
		roll2 = g_Dice % maxroll2 + 1;
		rolla = g_Dice % 13 + 1;
		switch (rolla)
		{
		case 1:		roll3 = 1;		break;	// "customers."
		case 2: 	roll3 = 4;		break;	// "the other girls."
		case 3: 	roll3 = 6;		break;	// "a ghost (She claims.)"
		case 4: 	roll3 = 7;		break;	// "an enemy gang member."
		case 5: 	roll3 = 10;		break;	// "a doppleganger disguised as a human."
		case 6: 	roll3 = 11;		break;	// "a jealous wife."
		case 7: 	roll3 = 13;		break;	// "a public health official."
		case 8: 	roll3 = 15;		break;	// "some raving loonie."
		case 9: 	roll3 = 16;		break;	// "a ninja!"
		case 10: 	roll3 = 17;		break;	// "a pirate. (Pirates rule! Ninjas drool!)"
		case 11: 	roll3 = 18;		break;	// "members of the militia."
		case 12: 	roll3 = 19;		break;	// "your mom (It runs in the family.)"
		case 13: 	roll3 = 21;		break;	// "a vengeful family member."
		default:	roll3 = g_Dice % maxroll3 + 1;	break;	// fallback just in case
		}
	}
	if (attacktype == SKILL_NORMALSEX)		// single male attacker
	{
		roll1 = g_Dice % maxroll1 + 1;
		roll2 = g_Dice % maxroll2 + 1;
		rolla = g_Dice % 12 + 1;
		switch (rolla)
		{
		case 1: 	roll3 = 2;		break;	// "some sadistic monster."
		case 2: 	roll3 = 3;		break;	// "a police officer."
		case 3: 	roll3 = 5;		break;	// "Batman! (Who knew?)"
		case 4: 	roll3 = 6;		break;	// "a ghost (She claims.)"
		case 5: 	roll3 = 7;		break;	// "an enemy gang member."
		case 6: 	roll3 = 10;		break;	// "a doppleganger disguised as a human."
		case 7: 	roll3 = 12;		break;	// "a jealous husband."
		case 8: 	roll3 = 13;		break;	// "a public health official."
		case 9: 	roll3 = 15;		break;	// "some raving loonie."
		case 10: 	roll3 = 16;		break;	// "a ninja!"
		case 11: 	roll3 = 17;		break;	// "a pirate. (Pirates rule! Ninjas drool!)"
		case 12: 	roll3 = 21;		break;	// "a vengeful family member."
		default:	roll3 = g_Dice % maxroll3 + 1;	break;	// fallback just in case
		}
	}
	// if nothing above worked then just do the default anything goes
	if (attacktype == SKILL_COMBAT || roll1 == 0 || roll2 == 0 || roll3 == 0)	// can be anything
	{
		roll1 = g_Dice % maxroll1 + 1;
		roll2 = g_Dice % maxroll2 + 1;
		roll3 = g_Dice % maxroll3 + 1;
	}

	switch (roll1)
	{
	  case 1: ss << ("cornered"); break;
	  case 2: ss << ("hauled to the dungeon"); break;
	  case 3: ss << ("put on the wood horse"); break;
	  case 4: ss << ("tied to the bed"); break;
	  case 5: ss << ("locked in the closet"); break;
	  case 6: ss << ("drugged"); break;
	  case 7: ss << ("tied up"); break;
	  case 8: ss << ("wrestled to the floor"); break;
	  case 9: ss << ("slapped in irons"); break;
	  case 10: ss << ("cuffed"); break;
	  case 11: ss << ("put in THAT machine"); break;
	  case 12: ss << ("konked on the head"); break;
	  case 13: ss << ("calmly sat down"); break;
	  case 14: ss << ("hand-cuffed during kinky play"); break;
	  case 15: ss << ("caught off guard during fellatio"); break;
	  case 16: ss << ("caught cheating at cards"); break;
	  case 17: ss << ("found sleeping on the job"); break;
	  case 18: ss << ("chained to the porch railing"); break;
	  case 19: ss << ("tied up BDSM-style"); break;
	  case 20: ss << ("stretched out on the torture table"); break;
	  case 21: ss << ("tied up and hung from the rafters"); break;
	}

	ss << (" and ");

	switch (roll2)
	{
	  case 1: ss << ("abused"); break;
	  case 2: ss << ("whipped"); break;
	  case 3: ss << ("yelled at"); break;
	  case 4: ss << ("assaulted"); break;
	  case 5: ss << ("raped"); break;
	  case 6: ss << ("her cavities were explored"); break;
	  case 7: ss << ("her hair was done"); break;
	  case 8: ss << ("her feet were licked"); break;
	  case 9: ss << ("relentlessly tickled"); break;
	  case 10: ss << ("fisted"); break;
	  case 11: ss << ("roughly fondled"); break;
	  case 12: ss << ("lectured to"); break;
	  case 13: ss << ("had her fleshy bits pierced"); break;
	  case 14: ss << ("slapped around"); break;
	  case 15: ss << ("penetrated by some object"); break;
	  case 16: ss << ("shaved"); break;
	  case 17: ss << ("tortured"); break;
	  case 18: ss << ("forced outside"); break;
	  case 19: ss << ("forced to walk on a knotted rope"); break;
	  case 20: ss << ("her skin was pierced by sharp things"); break;
	}

	ss << (" by ");

	switch (roll3)
	{
	  case 1: ss << ("customers."); break;
	  case 2: ss << ("some sadistic monster."); break;
	  case 3: ss << ("a police officer."); break;
	  case 4: ss << ("the other girls."); break;
	  case 5: ss << ("Batman! (Who knew?)"); break;
	  case 6: ss << ("a ghost (She claims.)"); break;
	  case 7: ss << ("an enemy gang member."); break;
	  case 8: ss << ("priests."); break;
	  case 9: ss << ("orcs. (What?)"); break;
	  case 10: ss << ("a doppleganger disguised as a human."); break;
	  case 11: ss << ("a jealous wife."); break;
	  case 12: ss << ("a jealous husband."); break;
	  case 13: ss << ("a public health official."); break;
	  case 14: ss << ("you!"); break;
	  case 15: ss << ("some raving loonie."); break;
	  case 16: ss << ("a ninja!"); break;
	  case 17: ss << ("a pirate. (Pirates rule! Ninjas drool!)"); break;
	  case 18: ss << ("members of the militia."); break;
	  case 19: ss << ("your mom (It runs in the family.)"); break;
	  case 20: ss << ("tentacles from the sewers."); break;
	  case 21: ss << ("a vengeful family member."); break;
	}

	return ss.str();
}

// ------ Training

/*
 * let's look at this a little differently...
 */
void cJobManager::get_training_set(vector<sGirl*> &v, vector<sGirl*> &t_set)
{
	u_int max = 4;
	u_int v_siz = v.size();

	// empty out the trainable set
	t_set.clear();
 	if (v_siz == 0u) return;	// if there's nothing in the vector, return with t_set empty
	if (v_siz == 1u)			// if there's just one, return with t_set containing that
	{
		t_set.push_back(v.back());
		v.pop_back();
		return;
	}
/*
 *	we want to split the girls into groups of four
 *	but if possible we don't want any singletons
 *
 *	So... if there would be one left over,
 *	we make the first group a group of three.
 *	subsequent groups will have modulus 2 and thus
 *	generate quads, until the end when we have a pair left.
 *
 *	Easier to explain in code than words, really.
 */
 	if((v_siz % 4) == 1)
		max --;		// reduce it by one, this time only
/*
 *	get the limiting variable for the loop
 *	we don't compare against v.size() because
 *	that will change as the loop progresses
 */
	u_int lim = (max < v_siz ? max : v_siz);
/*
 *	now loop until we hit max, or run out of girls
 */
 	for(u_int i = 0; i < lim; i++)
	{
		t_set.push_back(v.back());
		v.pop_back();
	}
}

bool cJobManager::WorkTraining(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
	return false;
}
double cJobManager::JP_Training(sGirl* girl, bool estimate)
{
	double jobperformance = 0.0;

	jobperformance +=
		(100 - cGirls::GetAverageOfSexSkills(girl)) +
		(100 - cGirls::GetAverageOfNSxSkills(girl))
		;

	return jobperformance;
}

void cJobManager::do_solo_training(sGirl *girl, bool Day0Night1)
{
	TrainableGirl trainee(girl);
	girl->m_Events.AddMessage(("She trained during this shift by herself, so learning anything worthwhile was difficult."), IMGTYPE_PROFILE, Day0Night1);
/*
 *	50% chance nothing happens
 */
 	if(g_Dice.percent(50))
	{
		girl->m_Events.AddMessage(("Sadly, she didn't manage to improve herself in any way."), IMGTYPE_PROFILE, EVENT_SUMMARY);
		return;
	}
/*
 *	otherwise, pick a random attribute and raise it 1-3 points
 */
	int amt = 1 + g_Dice%3;
	string improved = trainee.update_random(amt);
	stringstream ss;
	ss.str("");
	ss << ("She managed to gain ") << amt << " " << improved << (".");
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
}

void cJobManager::do_training_set(vector<sGirl*> girls, bool Day0Night1)
{
	sGirl *girl;
	stringstream ss;
	/*
	*	we're getting a vector of 1-4 girls here
	*	(the one is possible if only one girl trains)
	*/
	if (girls.empty())
	{  // no girls? that shouldn't happen
		g_LogFile.ss() << "Logic Error in cJobManager::do_training_set: empty set passed for training!";
		g_LogFile.ssend();
		return;
	}
	else if (girls.size() == 1)
	{  // special case for only one girl
		do_solo_training(girls[0], Day0Night1);
		return;
	}
	/*
	*	OK. Now, as I was saying. We have an array of sGirl* pointers..
	*	We need that to be a list of TrainableGirl objects:
	*/
	vector<TrainableGirl> set;
	/*
	*	4 is the maximum set size. I should probably consider
	*	making that a class constant - or a static class member
	*	initialised from the config file. Later for that.
	*/
	u_int num_girls = girls.size();
	for (u_int i = 0; i < num_girls; i++)
	{
		girl = girls[i];
		if (girl == nullptr) break;
		set.push_back(TrainableGirl(girl));
	}
	/*
	*	now get an idealized composite of the girls in the set
	*
	*	and the get a vector of the indices of the most efficient
	*	three attributes for them to train
	*/
	IdealGirl ideal(set);
	vector<int> indices = ideal.training_indices();
	/*
	*	OK. Loop over the girls, and then over the indices
	*/
	for (u_int g_idx = 0; g_idx < set.size(); g_idx++)
	{
		TrainableGirl &trainee = set[g_idx];

		for (u_int i_idx = 0; i_idx < indices.size(); i_idx++)
		{
			int index = indices[i_idx];
			/*
			*			the base value for any increase is the difference
			*			between the value of girl's stat, and the value of best
			*			girl in the set
			*/
			int aaa = ideal[index].value(); //    [index].value();
			int bbb = trainee[index].value();
			int diff = aaa - bbb;
			if (diff < 0)  // don't want a negative training value
				diff = 0;
			/*
			*			plus one - just so the teacher gets a chance to
			*			learn something
			*/
			diff++;
			/*
			*			divide by 10 for the increase
			*/
			int inc = (int)floor(1.0 * (double)diff / 10.0);
			/*
			*			if there's any left over (or if the diff
			*			was < 10 to begin with, there's a 10%
			*			point for each difference point
			*/
			int mod = diff % 10;
			if (g_Dice.percent(mod * 10)) inc++;
			trainee[index].upd(inc);
		}

		sGirl *girl = trainee.girl();
		/*
		*		need to do the  "she trained hard with ..." stuff here
		*/
		//int n_girls = set.size();
		ss.str("");
		ss << ("She trained during this shift in the following grouping: ");
		for (u_int i = 0; i < num_girls; i++)
		{
			ss << girls[i]->m_Realname;
			if (i == 0 && num_girls == 2)	ss << (" and ");
			else if (i < num_girls - 2)		ss << (", ");
			else if (i == num_girls - 2)	ss << (", and ");
		}
		ss << (".");

		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
		ss.str("");
		ss << ("She trained during this shift");

		bool raised = false;
		for (u_int i_idx = 0; i_idx < indices.size(); i_idx++)
		{
			int index = indices[i_idx];
			int gain = trainee[index].gain();
			if (gain == 0) continue;
			if (!raised)
			{
				raised = true;
				ss << (" and gained ");
			}
			else ss << (", ");

			ss << ("+") << gain << " " << trainee[index].name();
		}
		ss << (".");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
		if (girl->has_trait( "Lesbian") && set.size() > 1) girl->upd_temp_stat(STAT_LIBIDO, set.size() - 1, true);
	}
}

void cJobManager::do_training(sBrothel* brothel, bool Day0Night1)
{
	vector<sGirl*> t_set;
	vector<sGirl*> girls = brothel->girls_on_job(JOB_TRAINING, Day0Night1);

	for (u_int i = girls.size(); i-- > 0;)
	{  // no girls sneaking in training if she gave birth
		if ((girls[i]->m_WeeksPreg > 0 &&
			girls[i]->m_WeeksPreg + 1 >= (girls[i]->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant()))
			|| (girls[i]->m_JustGaveBirth && Day0Night1 == SHIFT_NIGHT))
			girls.erase(girls.begin() + i);
	}
	random_shuffle(girls.begin(), girls.end());
	for (;;)
	{
		get_training_set(girls, t_set);
		if (t_set.empty()) break;
		do_training_set(t_set, Day0Night1);
	}
	/*
	*	a few bookkeeping details here
	*	dirt and training costs, for a start
	*/
	brothel->m_Filthiness += girls.size();
	brothel->m_Finance.girl_training(g_Game->tariff().girl_training() * girls.size());
	/*
	*	and then each girl gets to feel tired and horny
	*	as a result of training
	*/
	for (u_int i = 0; i < girls.size(); i++)
	{
		sGirl *girl = girls[i];
		int libido = (girl->has_trait( "Nymphomaniac")) ? 4 : 2;
		girl->upd_temp_stat(STAT_LIBIDO, libido);
	}
}

// ----- Film & related

string cJobManager::GirlPaymentText(IBuilding * brothel, sGirl * girl, int totalTips, int totalPay, int totalGold,
                                    bool Day0Night1)
{
	stringstream ss;
	string girlName = girl->m_Realname;
	u_int sw = (Day0Night1 ? girl->m_NightJob : girl->m_DayJob);

	// `J` if a slave does a job that is normally paid by you but you don't pay your slaves...
	if (girl->is_slave() && is_job_Paid_Player(sw) && !cfg.initial.slave_pay_outofpocket())
	{
		ss << "\nYou own her and you don't pay your slaves.";
	}
	else if (totalGold > 0)
	{
		ss << girlName << " earned a total of " << totalGold << " gold";

		// if it is a player paid job and she is not a slave or she is a slave but you pay slaves out of pocket.
		if (is_job_Paid_Player(sw) && (!girl->is_slave() ||	(girl->is_slave() && cfg.initial.slave_pay_outofpocket())))
		{
			ss << " directly from you.\nShe gets to keep it all.";
		}
		else if (girl->house() <= 0)
		{
			ss << " and she gets to keep it all.";
		}
		else if (totalTips > 0 &&										// if there are tips
			((cfg.initial.girls_keep_tips() && !girl->is_slave()) ||	// and free girls keep tips
			(cfg.initial.slave_keep_tips() && girl->is_slave())))		// or slave girls keep tips
		{
			int hpay = int(double(totalPay * double(girl->m_Stats[STAT_HOUSE] * 0.01)));
			int gpay = totalPay - hpay;
			ss << ".\nShe keeps the " << totalTips << " she got in tips and her cut ("
				<< 100 - girl->m_Stats[STAT_HOUSE] << "%) of the payment amounting to " << gpay
				<< " gold.\n \nYou got " << hpay << " gold (" << girl->m_Stats[STAT_HOUSE] << "%).";
		}
		else
		{
			int hpay = int(double(totalGold * double(girl->m_Stats[STAT_HOUSE] * 0.01)));
			int gpay = totalGold - hpay;
			ss << ".\nShe keeps " << gpay << " gold. (" << 100 - girl->m_Stats[STAT_HOUSE]
				<< "%)\nYou keep " << hpay << " gold (" << girl->m_Stats[STAT_HOUSE] << "%).";
		}
	}
	else if (totalGold == 0)	{ ss << girlName << " made no money."; }
	else if (totalGold < 0)		{ ss << "ERROR: She has a loss of " << totalGold << " gold\n \nPlease report this to the Pink Petal Devloment Team at http://pinkpetal.org\n \nGirl Name: " << girl->m_Realname << "\nJob: " << JobName[(Day0Night1 ? girl->m_NightJob : girl->m_DayJob)] << "\nPay:     " << girl->m_Pay << "\nTips:   " << girl->m_Tips << "\nTotal: " << totalGold; }
	return ss.str();
}

void cJobManager::do_job(sGirl& girl, bool is_night, std::string&)
{
    unsigned job_id = is_night ? girl.m_NightJob : girl.m_DayJob;
}

// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cClinic.cpp >> is_Surgery_Job
bool cJobManager::is_Surgery_Job(int testjob) {
    if (testjob == JOB_CUREDISEASES ||
        testjob == JOB_GETABORT ||
        testjob == JOB_COSMETICSURGERY ||
        testjob == JOB_LIPO ||
        testjob == JOB_BREASTREDUCTION ||
        testjob == JOB_BOOBJOB ||
        testjob == JOB_VAGINAREJUV ||
        testjob == JOB_TUBESTIED ||
        testjob == JOB_FERTILITY ||
        testjob == JOB_FACELIFT ||
        testjob == JOB_ASSJOB)
        return true;
    return false;
}

void cJobManager::CatchGirl(sGirl* girl, stringstream& fuckMessage, const sGang* guardgang) {// try to find an item
    sInventoryItem* item;
    if(g_Game->player().inventory().has_item("Brainwashing Oil")) {
        item = g_Game->inventory_manager().GetItem("Brainwashing Oil");
    } else if(g_Game->player().inventory().has_item("Necklace of Control")) {
        item = g_Game->inventory_manager().GetItem("Necklace of Control");
    } else if(g_Game->player().inventory().has_item("Slave Band")) {
        item = g_Game->inventory_manager().GetItem("Slave Band");
    }
    if (item || g_Dice.percent(10)) {
        string itemname;
        stringstream itemtext;
        itemtext << "Your gang " << (guardgang ? guardgang->name() : "");
        stringstream CGmsg;
        // `J` create the customer
        sGirl* custgirl = g_Game->CreateRandomGirl(18 + (max(0, g_Dice % 40 - 10)), false, true, false, (g_Dice % 3 ==
                                                                                                         1));    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
        int emprisontraittime = 1;
        custgirl->pclove(-(g_Dice % 50 + 50));
        custgirl->pcfear(g_Dice % 50 + 50);
        custgirl->pchate(g_Dice % 50 + 50);
        custgirl->m_Enjoyment[ACTION_COMBAT] -= (g_Dice % 50 + 20);
        custgirl->m_Enjoyment[ACTION_SEX] -= (g_Dice % 50 + 20);

        if (!item)    // no item
        {
            itemtext << " beat up " << custgirl->m_Realname;
            fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
            itemtext
                    << " until she stops fighting back. They place your brand on her and bring your new slave to the dungeon.";
            emprisontraittime = 5 + g_Dice % 21;
            custgirl->set_stat(STAT_HEALTH, 5 + g_Dice % 70);
            custgirl->set_stat(STAT_HAPPINESS, g_Dice % 50);
            custgirl->set_stat( STAT_TIREDNESS, 50 + g_Dice % 51);
            girl->obedience(g_Dice % 10);
            girl->pchate(g_Dice % 10);
            girl->pcfear(g_Dice % 10);
            girl->pclove(-(g_Dice % 10));
            girl->spirit(-(g_Dice % 10));
        } else        // item was found
        {
            itemname = item->m_Name;
            g_Game->inventory_manager().Equip(custgirl, custgirl->add_inv(item), true);
            g_Game->player().inventory().remove_item(item);
            if (itemname == "Brainwashing Oil") {
                emprisontraittime += g_Dice % 5;
                itemtext << " forced a bottle of Brainwashing Oil down her throat";
                fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
                itemtext << ". After a few minutes of struggling,";
            } else if (itemname == "Necklace of Control") {
                emprisontraittime += g_Dice % 10;
                itemtext << " placed a Necklace of Control around her neck";
                fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
                itemtext << ". After a few minutes of struggling, the magic in the necklace activated and";
            } else if (itemname == "Slave Band") {
                emprisontraittime += g_Dice % 15;
                itemtext << " placed a Slave Band on her arm";
                fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
                itemtext << ". After a few minutes of struggling, the magic in the Slave Band activated and";
            }
            itemtext << " your new slave, " << custgirl->m_Realname << ", was taken to the dungeon.";

        }
        // `J` do all the messages
        CGmsg << custgirl->m_Realname << " was caught trying to run out without paying for services provided by "
              << girl->m_Realname << ".\n \n" << itemtext.str();
        custgirl->add_trait("Emprisoned Customer", emprisontraittime);    // add temp trait
        if (g_Dice.percent(75)) custgirl->lose_virginity();                // most of the time she will not be a virgin
        g_Game->player().suspicion(g_Dice % 10);
        g_Game->player().disposition(-(g_Dice % 10));
        g_Game->player().customerfear(g_Dice % 10);
        custgirl->m_Money = 0;

        custgirl->m_Events.AddMessage(CGmsg.str(), IMGTYPE_DEATH, EVENT_WARNING);
        // `J` add the customer to the dungeon
        g_Game->dungeon().AddGirl(custgirl, DUNGEON_CUSTNOPAY);
    } else SetGameFlag(FLAG_CUSTNOPAY);        // if not taken as a slave, use the old code
}
