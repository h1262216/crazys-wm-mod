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
#ifndef __CEVENTS_H
#define __CEVENTS_H

#include <string>
#include <vector>

#include "Constants.h"
#include "images/sImageSpec.h"
#include <memory>

struct CombatReport {
    std::vector<std::string> rounds;
};

class CEvent
{
public:
    CEvent(EEventType event, const sImageSpec&, std::string message, std::shared_ptr<CombatReport> rep);

    std::string     TitleText()     const;        //    Default listbox Text
    unsigned int    ListboxColour() const;        //    Default Listbox Colour

    bool            IsGoodNews() const;
    bool            IsUrgent()   const;
    bool            IsDanger()   const;
    bool            IsWarning()  const;
    bool            IsCombat()   const;

    static bool        CmpEventPredicate(const CEvent& eFirst, const CEvent& eSecond);

    const std::string&  GetMessage() const { return m_Message; }
    const sImageSpec&   GetImage() const   { return m_Image; }
    EEventType          GetEvent() const   { return m_Event; }
    std::shared_ptr<const CombatReport> GetReport() const { return m_Report; }

private:
    EEventType         m_Event;                  // type of event
    sImageSpec         m_Image;
    std::string        m_Message;
    std::shared_ptr<CombatReport> m_Report = nullptr;
};

class cEvents
{
public:
    cEvents()        { m_bSorted = false; }    // constructor
    ~cEvents()        { Clear();}                // destructor

    void            Clear();

    void            AddMessage(std::string message, sImageSpec image, EEventType event_type);
    void            AddMessage(std::string summary, EEventType event, std::shared_ptr<CombatReport> rep);
    void            AddMessage(std::string summary, EEventType event);
    const CEvent&   GetMessage(int id) const;
    int             GetNumEvents() const { return events.size(); }
    bool            IsEmpty()      const { return events.empty() ; }
    bool            HasGoodNews() const;
    bool            HasUrgent() const;
    bool            HasDanger() const;
    bool            HasWarning() const;
    void            DoSort();


private:
    std::vector<CEvent>    events;
    bool            m_bSorted;                // flag to only allow sort once
};

#endif
