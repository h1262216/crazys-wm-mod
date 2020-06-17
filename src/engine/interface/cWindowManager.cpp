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
#include "interface/cInterfaceWindowXML.h"
#include "widgets/cScreenGetInput.h"
#include "interface/cWindowManager.h"
#include "CLog.h"
#include "widgets/cMessageBox.h"
#include "cChoiceMessage.h"
#include "interface/cInterfaceObject.h"

#include <cassert>
#include <utility>

extern cScreenGetInput*    g_GetInput;

cWindowManager::cWindowManager(CGraphics* g) : m_GFX(g) {

}

cWindowManager::~cWindowManager() = default;

void cWindowManager::push(const std::string& window_name)
{
    if (windows.find(window_name) == windows.end())                        //check the screen exists
    {
        g_LogFile.error("interface", "cWindowManager::Push: can't find window named '", window_name, "'");
        return;
    }

    m_WindowStack.push_back(windows[window_name]);
    try {
        m_WindowStack.back()->init(false);
    } catch(...) {
        m_WindowStack.pop_back();
        throw;
    }
}

void cWindowManager::replace(const std::string& window_name)
{
    auto current = m_WindowStack.back();
    m_WindowStack.pop_back();
    try {
        push(window_name);
    } catch(...) {
        // in case of exception, roll back the pop
        m_WindowStack.push_back(current);
        throw;
    }
}


// remove function from the stack
void cWindowManager::Pop()
{
    m_WindowStack.pop_back();
    if(!m_WindowStack.empty())
        m_WindowStack.back()->init(true);
}

void cWindowManager::PopToWindow(const std::string& window_name)
{
    if (windows.find(window_name) == windows.end())                        //check the screen exists
    {
        g_LogFile.error("interface", "cWindowManager::Push: can't find window named '", window_name, "'");
        return;
    }

    while (!m_WindowStack.empty() && m_WindowStack.back() != windows[window_name]) {
        m_WindowStack.pop_back();
    }

    if(m_WindowStack.empty()) {
        m_WindowStack.push_back(windows[window_name]);
    }

    m_WindowStack.back()->init(true);
}

void cWindowManager::UpdateCurrent()
{
    if (!m_WindowStack.empty()) {
        m_WindowStack.back()->update();
    }
}

void cWindowManager::UpdateMouseMovement(int x, int y)
{
    if(!m_WindowStack.empty())
        m_WindowStack.back()->UpdateWindow(x, y);
}

void cWindowManager::OnMouseClick(int x, int y, bool down)
{
    if(!m_WindowStack.empty())
        m_WindowStack.back()->MouseClick(x, y, down);
}

void cWindowManager::OnMouseWheel(int x, int y, bool mouseWheelDown)
{
    if(!m_WindowStack.empty())
        m_WindowStack.back()->MouseWheel(x, y, mouseWheelDown);
}

void cWindowManager::OnTextInput(const char* text) {
    if(!m_WindowStack.empty())
        m_WindowStack.back()->TextInput(text);
}

cInterfaceWindow* cWindowManager::GetWindow(bool allow_modal)
{
    if(m_WindowStack.empty())
        return nullptr;
    if(allow_modal) {
        return m_WindowStack.back().get();
    } else {
        auto tp = m_WindowStack.back().get();
        if(tp->IsTransparent()) {
            return m_WindowStack.at(m_WindowStack.size() - 2).get();
        } else {
            return tp;
        }
    }

}

void cWindowManager::Draw()
{
    // figure out which windows to draw
    if(!m_WindowStack.empty()) {
        auto found = std::find_if_not(m_WindowStack.rbegin(), m_WindowStack.rend(),
                                      [](auto &w) { return w->IsTransparent(); });
        if(found == m_WindowStack.rend())
            --found;

        while(true) {
            (*found)->Draw(*m_GFX);
            if(found == m_WindowStack.rbegin())
                break;
            --found;
        }
    }
}

void cWindowManager::OnKeyPress(SDL_Keysym key)
{
    GetWindow()->OnKeyPress(key);
}

void cWindowManager::add_window(std::string name, std::shared_ptr<cInterfaceWindow> win)
{
    win->load(this);
    windows[std::move(name)] = std::move(win);
}

IBuilding * cWindowManager::GetActiveBuilding() const
{
    return m_ActiveBuilding;
}

void cWindowManager::SetActiveBuilding(IBuilding * building)
{
    m_ActiveBuilding = building;
}

void cWindowManager::InputInteger(std::function<void(int)> callback, int def_value)
{
    g_GetInput->ModeGetInt(std::move(callback));
    g_GetInput->SetText(std::to_string(def_value).c_str());
    push("GetInput");
}

void cWindowManager::InputConfirm(std::function<void()> callback)
{
    g_GetInput->ModeConfirm(std::move(callback));
    push("GetInput");
}

void cWindowManager::InputString(std::function<void(const std::string&)> callback)
{
    g_GetInput->ModeGetString(std::move(callback));
    push("GetInput");
}

void cWindowManager::PushMessage(std::string text, int color, std::function<void()> callback)
{
    auto window = GetModalWindow();
    auto cb = std::make_unique<cMessageBox>(window.get());
    cb->PushMessage(std::move(text), color);
    cb->SetCallback(std::move(callback));
    window->AddWidget(std::move(cb));
}

sGirl * cWindowManager::GetActiveGirl() const
{
    if(m_SelectedGirls.empty())
        return nullptr;
    return m_SelectedGirls.front();
}

void cWindowManager::SetActiveGirl(sGirl * girl)
{
    m_SelectedGirls.clear();
    m_SelectedGirls.push_back(girl);
}

void cWindowManager::InputChoice(std::string question, std::vector<std::string> options, std::function<void(int)> callback)
{
    auto choice_window = GetModalWindow();
    auto cb = std::make_unique<cChoice>(224, 112, 352, 384, 0, std::move(question), std::move(options), 16, choice_window.get());
    cb->SetCallback( std::move(callback) );
    choice_window->AddWidget(std::move(cb));
}

void cWindowManager::PopAll() {
    m_WindowStack.clear();
}

CGraphics& cWindowManager::GetGraphics()
{
    return *m_GFX;
}

std::shared_ptr<cModalWindow> cWindowManager::GetModalWindow() {
    std::shared_ptr<cModalWindow> choice_window = std::dynamic_pointer_cast<cModalWindow>(m_WindowStack.back());
    bool is_new = false;
    if(!choice_window) {
        is_new = true;
        choice_window = std::make_shared<cModalWindow>();
    }

    if(is_new) {
        choice_window->load(this);
        m_WindowStack.push_back(choice_window);
        try {
            m_WindowStack.back()->init(false);
        } catch (...) {
            m_WindowStack.pop_back();
            throw;
        }
    }

    assert(choice_window);
    return std::move(choice_window);
}

void cWindowManager::EnableTextInput() {
    if(m_TextInputEnabled == 0)
        SDL_StartTextInput();
    m_TextInputEnabled += 1;
}

void cWindowManager::DisableTextInput() {
    m_TextInputEnabled -= 1;
    if(m_TextInputEnabled == 0)
        SDL_StopTextInput();
}

// ---------------------------------------------------------------------------------------------------------------------

static std::unique_ptr<cWindowManager> WindowManager;

cWindowManager& window_manager()
{
    assert(WindowManager);
    return *WindowManager;
}

void InitInterface(CGraphics* g) {
    assert(!WindowManager);
    WindowManager = std::make_unique<cWindowManager>(g);
}

void ShutdownInterface()
{
    g_LogFile.info("interface", "Releasing Interface");
    WindowManager.reset();
}

// TODO this does not belong here
CGraphics& cUIWidget::GetGraphics()
{
    return m_Parent->GetGraphics();
}

cInterfaceObject::cInterfaceObject() {
    g_LogFile.debug("interface", "Create interface object ", this);
}

cInterfaceObject::~cInterfaceObject() {
    g_LogFile.debug("interface", "Delete interface object ", this);
}
