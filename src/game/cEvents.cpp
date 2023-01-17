/*
 * Copyright 2009-2023 The Pink Petal Development Team.
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

#include <algorithm>
#include <tuple>
#include "cEvents.h"
#include "interface/constants.h"


std::string CEvent::TitleText() const
{
    if(!m_Title.empty())    return m_Title;

    switch (m_Event)
    {
    case EVENT_DAYSHIFT:
        return "Day Shift";
    case EVENT_NIGHTSHIFT:
        return "Night Shift";
    case EVENT_WARNING:
        return "Warning";
    case EVENT_DANGER:
        return "!DANGER!";
    case EVENT_GOODNEWS:
        return "!GOODNEWS!";
    case EVENT_LEVELUP:
        return "!LEVEL UP!";
    case EVENT_GAIN_TRAIT:
        return "Gained Trait";
    case EVENT_LOSE_TRAIT:
        return "Lost Trait";
    case EVENT_SUMMARY:
        return "Summary";
    case EVENT_DUNGEON:
        return "Dungeon";
    case EVENT_MATRON:
        return "Matron";
    case EVENT_GANG:
        return "Gang Report";
    case EVENT_NOWORK:
        return "Refused Job";
    case EVENT_BACKTOWORK:
        return "Back to Work";
    case EVENT_DEBUG:
        return "#DEBUG#";
    default:
        return "Event";
    }
}

unsigned int CEvent::ListboxColour() const
{
/*
 *    Returns default listbox colours
 *    
 *    Used to simplify code in TurnSummary
 *
 */
    switch (m_Event)
    {
    case EVENT_WARNING:
    case EVENT_NOWORK:
        return COLOR_EMPHASIS;
    case EVENT_DANGER:
        return COLOR_WARNING;
    case EVENT_GOODNEWS:
    case EVENT_LEVELUP:
    case EVENT_GAIN_TRAIT:
    case EVENT_LOSE_TRAIT:
        return COLOR_POSITIVE;
    case EVENT_DEBUG:
        return COLOR_EMPHASIS;
    default:
        return COLOR_NEUTRAL;
    }
}

bool CEvent::IsGoodNews() const
{
    return m_Event == EVENT_GOODNEWS || m_Event == EVENT_LEVELUP || m_Event == EVENT_GAIN_TRAIT || m_Event == EVENT_LOSE_TRAIT;
}

bool CEvent::IsUrgent() const
{
    return m_Event == EVENT_DANGER || m_Event == EVENT_WARNING || m_Event == EVENT_NOWORK ||
           m_Event == EVENT_GOODNEWS || m_Event == EVENT_LEVELUP;
}

bool CEvent::IsDanger() const
{
    return m_Event == EVENT_DANGER;
}

bool CEvent::IsWarning() const
{
    return m_Event == EVENT_WARNING || m_Event == EVENT_NOWORK;
}


void cEvents::Clear()
{
    events.clear();
    m_bSorted = false;
}

bool cEvents::HasGoodNews() const
{
    for(auto& event : events) {
        if(event.IsGoodNews())
            return true;
    }
    return false;
}


bool cEvents::HasUrgent() const
{
    for(auto& event : events) {
        if(event.IsUrgent())
            return true;
    }
    return false;
}

bool cEvents::HasDanger() const
{
    for(auto& event : events) {
        if(event.IsDanger())
            return true;
    }
    return false;
}

bool cEvents::HasWarning() const
{
    for(auto& event : events) {
        if(event.IsWarning())
            return true;
    }
    return false;
}

bool CEvent::IsCombat() const {
    return m_Report != nullptr;
}

void cEvents::AddMessage(std::string message, sImageSpec image, EEventType event_type)
{
    events.emplace_back(event_type, image, std::move(message), nullptr);
    m_bSorted = false;
}

void cEvents::AddMessage(std::string summary, EEventType event, std::shared_ptr<CombatReport> rep) {
    events.emplace_back(event, sImageSpec{EImageBaseType::COMBAT, ESexParticipants::ANY,
                                          ETriValue::Maybe, ETriValue::Maybe, ETriValue::Maybe},
                        std::move(summary), std::move(rep));
    m_bSorted = false;
}

const CEvent& cEvents::GetMessage(int id) const
{
    return events.at(id);
}

auto make_sort_tuple(const CEvent& e) {
    return std::make_tuple(!e.IsDanger(), !e.IsWarning(), e.GetEvent() != EVENT_NOWORK, e.GetEvent() != EVENT_MATRON,
            e.GetEvent() != EVENT_DUNGEON);
}

bool CEvent::CmpEventPredicate(const CEvent& eFirst, const CEvent& eSecond)
{
    return make_sort_tuple(eFirst) < make_sort_tuple(eSecond);
}

void cEvents::DoSort()
{
    if (!m_bSorted)
    {
        std::stable_sort(events.begin(), events.end(), CEvent::CmpEventPredicate);
        m_bSorted = true;
    }
}

void cEvents::AddMessage(std::string summary, EEventType event) {
    AddMessage(std::move(summary), sImageSpec(), event);
}

CEvent::CEvent(EEventType event, const sImageSpec& spec, std::string message, std::shared_ptr<CombatReport> rep) :
        m_Event(event), m_Image(spec), m_Message(std::move(message)), m_Report(std::move(rep))
{
}
