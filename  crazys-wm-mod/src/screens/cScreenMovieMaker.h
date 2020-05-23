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
#pragma once

#include "DirPath.h"
#include "interface/cInterfaceWindow.h"

class cScreenMovieMaker : public cInterfaceWindowXML
{
private:
	int back_id;			// Back button
	int autocreatemovies_id;// auto create movies checkbox
	int sceneslist_id;		// Scenes listbox
	int makethismovie_id;	// Make this movie
	int releasemovie_id;	// Release movie
	int girlimage_id;		// Girl image
	int scrapscene_id;		// Scrap scene
	int moveup_id;			// Move scene up in the movie
	int movedown_id;		// Move scene down in the movie
	int addscene_id;		// add selected scene
	int removescene_id;		// removes selected scene
	int scenedetails_id;	// scene details
	int moviedetails_id;	// movie details

	void set_ids() override;
public:
	cScreenMovieMaker();

	void init(bool back) override;
	void process() override { };
	void update_image();

    sMovieStudio& getStudio() const;

    void movie_scene_up();
    void movie_scene_down();
    void movie_add_scene();
    void movie_remove_scene();

    void on_select_source_scene(int selection);

    void on_select_movie_scene(int selection);
};
