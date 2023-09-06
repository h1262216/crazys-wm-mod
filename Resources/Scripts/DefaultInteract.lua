---@param girl wm.Girl
function InteractDungeon(girl)
    DungeonInteractChoice(girl)
end

---@param girl wm.Girl
function InteractBrothel(girl)
    BrothelInteractChoice(girl)
end


---@param girl wm.Girl
function DungeonInteractChoice(girl)
    wm.UpdateImage(wm.IMG.JAIL)
    local choice = ChoiceBox("What would you like to do with her?", "Torture", "Chat", "Scold", "Ask", "Force")
    if choice == 0 then
        girl:torture()
        wm.UpdateImage(wm.IMG.TORTURE)
    elseif choice == 1 then
        return girl:trigger("girl:chat:dungeon")
    elseif choice == 2 then
        ScoldGirl(girl)
    elseif choice == 3 then
        local have_sex = "To have sex"
        if girl:pclove() > 90 then
            have_sex = "To make love"
        end

        choice = ChoiceBox("", have_sex, "To have sex with another girl", "To have sex with a beast",
                "To be in a bondage session", "For a blowjob", "For some anal sex", "For a threesome (not yet working)",
                "To join in with a group session", "Go Back")
        if choice == 0 then
            if girl:pclove() > 90 then
                girl:happiness(5)
                girl:libido(1)
                girl:experience(2)
                girl:tiredness(1)
                girl:obedience(1)
                girl:pcfear(-1)
                girl:pclove(3)
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You both have passionate wild sex, and then bask in each others glow.")
                PlayerFucksGirlUpdate(girl)
            elseif girl:obey_check(wm.ACTIVITIES.FUCKING) then
                girl:happiness(2)
                girl:libido(1)
                girl:tiredness(2)
                girl:pclove(1)
                wm.UpdateImage(wm.IMG.SEX)
                Dialog("You both enjoy fucking one another.")
                PlayerFucksGirlUpdate(girl)
            else
                Dialog("She refuses to sleep with you.")
            end
        elseif choice == 1 then
            if girl:obey_check(wm.ACTIVITIES.FUCKING) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.LESBIAN)
                Dialog("She enjoy you watching her while another girl fucks her.")
            else
                Dialog("She refused to have sex with another girl.")
            end
        elseif choice == 2 then
            if girl:obey_check(wm.ACTIVITIES.FUCKING) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.BEAST)
                Dialog("She enjoys you watching her being fucked by all sorts of tentacled sex fiends.")
            else
                Dialog("She refused to have sex with creatures.")
            end
        elseif choice == 3 then
            if girl:obey_check(wm.ACTIVITIES.FUCKING) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.BDSM)
                Dialog("She allows you to tie her up and spank her while you both cum hard.")
                local preg = girl:calc_player_pregnancy(false, 1.0);
                if preg then Dialog("${firstname} has gotten pregnant") end
            else
                Dialog("She refused to do this.")
            end
        elseif choice == 4 then
            choice = ChoiceBox("", "Deepthroat", "Regular", "Go Back")
            if choice == 0 then
                if girl:obey_check(wm.ACTIVITIES.FUCKING) then
                    girl:dignity(-1)
                    wm.UpdateImage(wm.IMG.DEEPTHROAT)
                    Dialog("She lets you shove your cock deep down the back of her throat until you cum into her head.")
                else
                    Dialog("She refuses to do this.")
                    return
                end
            elseif choice == 1 then
                if girl:obey_check(wm.ACTIVITIES.FUCKING) then
                    girl:dignity(-1)
                    wm.UpdateImage(wm.IMG.ORAL)
                    Dialog("She sucks your cock until you cum in her mouth.")
                else
                    Dialog("She refuses to do this.")
                    return
                end
            else
                return DungeonInteractChoice(girl)      -- tail call
            end
            -- TODO what???
            girl:happiness(1)
            girl:libido(1)
            girl:spirit(-1)
            girl:anal(1)
        elseif choice == 5 then
            if girl:obey_check(wm.ACTIVITIES.FUCKING) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                wm.UpdateImage(wm.IMG.ANAL)
                Dialog("She lets you fuck her in her tight little ass until you both cum.")
                -- ANAL SEX
            else
                Dialog("She refused to let you fuck her ass.")
            end
        elseif choice == 6 then
            -- THREESOME
            return DungeonInteractChoice(girl)      -- tail call
        elseif choice == 7 then
            if girl:obey_check(wm.ACTIVITIES.FUCKING) then
                girl:happiness(1)
                girl:libido(1)
                girl:tiredness(1)
                PlayerFucksGirl_Group(girl)
                Dialog("You and a group of your male servants take the poor girl in all way, she was hurt.")
            else
                Dialog("She refuse to be fucked in a gangbang")
            end

        else
            return DungeonInteractChoice(girl)      -- tail call
        end
    elseif choice == 4 then -- FORCE
        choice = ChoiceBox("", "To have sex with you", "To have sex with another girl", "To have sex with a beast",
                "To be in a bondage session", "For a blowjob", "For some anal sex", "For a threesome (not yet working)",
                "To join in with a group session", "Go Back")
        if choice == 0 then
            PlayerRapeGirl(girl)
            Dialog("She struggles to no avail as you force yourself inside her, you fuck her roughly until you unload yourself into her.")
        elseif choice == 1 then
            wm.UpdateImage(wm.IMG.LESBIAN)
            Dialog("You call a female customer who fuck the poor girl with dildo.")
        elseif choice == 2 then
            wm.UpdateImage(wm.IMG.BEAST)
            Dialog("You forcefully tie her down and let in a tentacle creature followed by several other creatures to have their way with her.")
        elseif choice == 3 then
            wm.UpdateImage(wm.IMG.BDSM)
            Dialog("You hog-tie her and pour hot wax all over her body before spanking her with a paddle.")
        elseif choice == 4 then
            choice = ChoiceBox("", "Deepthroat", "Regular", "Go Back")
            if choice == 0 then
                wm.UpdateImage(wm.IMG.DEEPTHROAT)
                girl:dignity(-2)
                Dialog("You grab her by the back of her head and force your cock into her throat, she gags as you cum into her stomach.")
            elseif choice == 1 then
                wm.UpdateImage(wm.IMG.ORAL)
                Dialog("You grab her and force her to suck your cock, she struggles but cannot stop you.")
            else
                return DungeonInteractChoice(girl)      -- tail call
            end
            -- TODO what???
            girl:happiness(1)
            girl:libido(1)
            girl:spirit(-1)
            girl:anal(1)
        elseif choice == 5 then
            wm.UpdateImage(wm.IMG.ANAL)
            Dialog("Although she tries to keep her ass closed you manage to get inside her and proceed to fuck her painfully.")
        elseif choice == 6 then
           -- threesome (placeholder)
            return DungeonInteractChoice(girl)
        elseif choice == 7 then
            wm.UpdateImage(wm.IMG.GROUP)
            Dialog("One of your servant holds his head in his hands while your sperm spreads his face. All your servants are happy to unload on her pretty face")
            girl:dignity(-1)
        else
            return DungeonInteractChoice(girl)
        end
        girl:happiness(-4)
        girl:make_horny(1)
        girl:confidence(-1)
        girl:obedience(1)
        girl:spirit(-1)
        girl:tiredness(4)
        girl:health(-1)
        girl:pcfear(2)
        girl:pclove(-3)
    end
end


function BrothelInteractChoice(girl)
    wm.UpdateImage(wm.IMG.PROFILE)
    local options = {"Reward ${firstname}" ,
                     "Chat with ${firstname}", "Visit ${firstname}'s Bedroom",
                     "Call ${firstname} to your office", "Invite ${firstname} to your private chambers",
                     "Train ${firstname}", "Scold ${firstname}", "Punish ${firstname}"}
    if wm.IsCheating() then
        table.insert(options, "Cheat")
    end
    local choice = ChoiceBox("What would you like to do?", table.unpack(options))

    if choice == 0 then
        local reward = ChoiceBox("What?", "Money (100G)",
                "Let her go to a concert (50G)", "Sent her to see a movie (10G)")
        if reward == 0 then
            Dialog("You give ${firstname} some spending money to cheer her up.")
            girl:give_money(100)
            girl:happiness(2)
            girl:pclove(2)
        elseif reward == 1 then
            girl:give_money(50)
            local text, image = girl:run_event("girl:free-time:concert")
            Dialog(text)
        elseif reward == 2 then
            girl:give_money(10)
            local text, image = girl:run_event("girl:free-time:movie")
            Dialog(text)
        end
    elseif choice == 1 then
        return girl:trigger("girl:chat:brothel")
    elseif choice == 2 then
        return girl:trigger("girl:interact:bedroom")
    elseif choice == 3  then -- Office
        return girl:trigger("girl:interact:office")
    elseif choice == 4 then
        Dialog("\"Hello My Dear, I wanted to ask you to come by my private quarters this evening.  Perhaps, we can get to know each other better.\" ")
        if girl:obey_check(wm.ACTIVITIES.FUCKING) then
            return girl:trigger("girl:interact:private")
        else
            Dialog("She declines your invitation.")
            return girl:trigger("girl:refuse")
        end
    elseif choice == 5 then
        return girl:trigger("girl:training")
    elseif choice == 6 then
        ScoldGirl(girl)
    elseif choice == 7 then
        PunishGirl(girl)
    else
        -- cheat
        CheatMenu(girl)
    end
end

function Refuse(girl)
    wm.UpdateImage(wm.IMG.REFUSE)
    local choice = ChoiceBox("", "Allow Her to Refuse.", "Scold Her", "Spank Her", "Take Her Clothing.",
        "Force Sex")
    if choice == 0 then
        Dialog("You say nothing and go about your other business.")
        if girl:has_trait(wm.TRAITS.RECENTLY_SCOLDED) then
            girl:pcfear(-3)
            girl:obedience(-1)
        end
        wm.SetPlayerDisposition(3)
        girl:pcfear(-1)
    elseif choice == 1 then
        ScoldGirl(girl)
    elseif choice == 2 then
        Dialog("\"You will learn to obey me!\" You yell as you grab her arm and drag her across your knees.")
        wm.UpdateImage(wm.IMG.SPANKING)
        Dialog("She begins to cry as you pull her clothing and expose her ass.  \"Perhaps this will teach you some discipline.\" \n" ..
                "You smack her ass until her cheeks are rosy red and send her away sobbing.")
        wm.SetPlayerDisposition(-3)
        girl:happiness(-3)
        girl:health(-1)
        girl:obedience(3)
        girl:pcfear(5)
        girl:add_trait(wm.TRAITS.RECENTLY_PUNISHED, 3)
    elseif choice == 3 then
        Dialog("\"Oh I see.  You feel you have no need to obey me?\"  You ask calmly.  \"Perhaps then you also have no need for the things I have given you?\" ")
        Dialog("\"I'll just be taking a few things back then.\"  You order your guards to strip her naked and make her stand in front of the brothel all day and night.")
        Dialog("As she is lead outside you remark. \"Perhaps next time you will be more mindful of who it is that takes care of you.\"")
        wm.UpdateImage(wm.IMG.NUDE)
        wm.SetPlayerDisposition(-3)
        girl:happiness(-3)
        girl:pclove(-10)
        girl:obedience(5)
        girl:add_trait(wm.TRAITS.RECENTLY_PUNISHED, 1)
    elseif choice == 4 then
        Dialog("Your eyes flash with rage. \"You dare refuse? I'll show you what happens to whores that refuse to do their master's bidding\"")
        Dialog("You knock her down and begin to tear away her clothing.  She cries out as you force yourself inside her.")
        Dialog("You release you semen deep inside her and leave her sobbing on the floor.")
        PlayerRapeGirl(girl)
        girl:happiness(-5)
        girl:obedience(5)
        girl:pclove(-20)
        girl:pcfear(10)
        wm.SetPlayerDisposition(-3)
        girl:add_trait(wm.TRAITS.RECENTLY_PUNISHED, 4)
    end
end

--- @param girl wm.Girl
function CheatMenu(girl)
    local what = ChoiceBox("", "Add Trait", "Increase Stat", "Increase Skill", "Done")
    if what == 0 then
        local sel = ChoiceBox("Which trait?", "A-B", "C", "D-E", "F-G", "H-L", "M", "N-O", "P-R", "S", "T-W", "X-Z")
        local trait = ""
        if sel == 0 then
            trait = _AskTrait("A", "B")
        elseif sel == 1 then
            trait = _AskTrait("C", "C")
        elseif sel == 2 then
            trait = _AskTrait("D", "E")
        elseif sel == 3 then
            trait = _AskTrait("F", "G")
        elseif sel == 4 then
            trait = _AskTrait("H", "L")
        elseif sel == 5 then
            trait = _AskTrait("M", "M")
        elseif sel == 6 then
            trait = _AskTrait("N", "O")
        elseif sel == 7 then
            trait = _AskTrait("P", "R")
        elseif sel == 8 then
            trait = _AskTrait("S", "S")
        elseif sel == 9 then
            trait = _AskTrait("T", "W")
        elseif sel == 10 then
            trait = _AskTrait("X", "Z")
        end
        girl:add_trait(trait)
        return CheatMenu(girl)
    elseif what == 1 then
        local stat_list = {}
        for k, v in pairs(wm.STATS) do
            stat_list[v + 1] = k
        end
        local sel = ChoiceBox("Which stat?", table.unpack(stat_list))
        girl:stat(sel, 10)
        return CheatMenu(girl)
    elseif what == 2 then
        local stat_list = {}
        for k, v in pairs(wm.SKILLS) do
            stat_list[v + 1] = k
        end
        local sel = ChoiceBox("Which skill?", table.unpack(stat_list))
        girl:skill(sel, 10)
        return CheatMenu(girl)
    end
end

function _AskTrait(first, last)
    local trait_list = {}
    for k, v in pairs(wm.TRAITS) do
        if v:byte(1) >= first:byte() and v:byte(1) <= last:byte() then
            table.insert(trait_list, v)
        end
    end
    local sel = ChoiceBox("Which trait?", table.unpack(trait_list))
    return trait_list[sel + 1]
end
