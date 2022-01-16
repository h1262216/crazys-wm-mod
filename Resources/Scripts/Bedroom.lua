function VisitBedroom(girl)
    local choice
    if girl:pclove() > 90 then
        choice = ChoiceBox("What do you want to do?",
                "Make Love",
                "Watch her have sex with another girl",
                "Watch her have sex with your pet sex beast",
                "Play your favorite bondage roleplay",
                "Ask her to lick your love shaft",
                "Worship her cute little ass hole",
                "Watch her masturbate for you",
                "Have someone join the two of you for some fun",
                "Have her tease you",
                "Pleasure her",
                "Go Back")
    else
        choice = ChoiceBox("What do you want to do?",
                "Have Sex",
                "Ask her to have sex with another girl",
                "Ask her to have sex with a beast",
                "Ask her to be in a bondage session",
                "Ask for a blowjob",
                "Have anal sex",
                "Ask her to masturbate while you watch",
                "Have group sex",
                "Ask her to show you her skills on the stripper pole",
                "Pleasure her",
                "Go Back")
    end
    if choice == 0 then
        return NormalSex(girl)
    elseif choice == 1 then
        return LesbianSex(girl)
    elseif choice == 2 then
        return BeastSex(girl)
    elseif choice == 3 then
        return BDSMSex(girl)
    elseif choice == 4 then
        Dialog("You almost trip over her as you enter the room. \"My Dear, what are you doing kneeling on the floor?\"  Seeing her kneeling before you gives you an idea...")
        local choice = ChoiceBox("", "Blowjob", "Deep Throat", "Titty Fuck", "Go Back")
        Dialog("\"Well never mind the previous reason you're down there.  I shall give you a new one.\"  You pull your member from you pants and  bring it within inches of her face.")
        if choice == 0 then
            return Blowjob(girl)
        elseif choice == 1 then
            return Deepthroat(girl)
        elseif choice == 2 then
            return Tittyfuck(girl)
        elseif choice == 3 then
            return VisitBedroom(girl)
        end
    elseif choice == 5 then
        return AnalSex(girl)
    elseif choice == 6 then
        return MasturbateSex(girl)
    elseif choice == 7 then
        choice = ChoiceBox("Who shall join you?",
                    "Another girl",
                    "One of your goons",
                    "Everyone!",
                    "Go Back")
        if choice == 0 then
            return FFMSex(girl)
        elseif choice == 1 then
            return MMFSex(girl)
        elseif choice == 2 then
            return OrgySex(girl)
        elseif choice == 3 then
            return VisitBedroom(girl)
        end
    elseif choice == 8 then
        return Strip(girl)
     elseif choice == 9 then
        choice = ChoiceBox("How?",
                "Fingering",
                "Licking",
                "Go Back")
        if choice == 0 then
            return Fingering(girl)
        elseif choice == 1 then
            return Licking(girl)
        elseif choice == 2 then
            return VisitBedroom(girl)
        end
    elseif choice == 10 then
        Dialog("Go Back")
        return girl:trigger("girl:interact:brothel")
    end
end


---@param girl wm.Girl
function NormalSex(girl)
    if girl:obey_check(wm.ACTIONS.SEX) then
        HandleNormalSex(girl)
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to have sex; mumbling some half hearted excuse.")
        return girl:trigger("girl:refuse")
    end
end

function HandleNormalSex(girl)
    wm.UpdateImage(wm.IMG.SEX)
    girl:experience(6)
    girl:tiredness(3)
    if girl:skill_check(wm.SKILLS.NORMALSEX, 75) then
        Dialog("As the head of your penis passes her labia you feel her squeeze her muscles around your member.  She starts rocking her hips and pushing against you.")
        Dialog("Without you realizing it she has switched positions and is now bouncing and grinding on top of you.")
        Dialog("Many positions and orgasms later you both lie next to each other completely exhausted and satisfied.")
        girl:happiness(1)
        girl:normalsex(1)
        PlayerFucksGirlUpdate(girl)
    else
        Dialog("You begin with slow thrusts hoping for a long night of pleasure.")
        Dialog("As you continue to thrust she barely moves and hardly makes a sound.")
        Dialog("Bored by her lackluster performance you finish quickly and leave her room.")
        girl:normalsex(2)
        girl:tiredness(3)
        girl:libido(-1)
        PlayerFucksGirlUpdate(girl)
    end
end

---@param girl wm.Girl
function LesbianSex(girl)
    wm.UpdateImage(wm.IMG.BED)
    Dialog("She notices that you have not come alone.  Following her gaze you speak \"Ah I see you noticed.  I'd like you two girls to get to know each other better and...  :you wink slyly:  I'd like to watch.\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        HandleLesbianSex(girl, RandomGirlName())
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She wrinkles her nose in disgust and refuses.")
        return girl:trigger("girl:refuse")
    end
end


---@param girl wm.Girl
function HandleLesbianSex(girl, other_name)
    girl:experience(6)
    girl:tiredness(3)
    Dialog("You sit down on the bed and make yourself comfortable as the girls approach one another.")
    if girl:skill_check(wm.SKILLS.LESBIAN, 75) then
        wm.UpdateImage(wm.IMG.STRIP)
        Dialog("The girls lose themselves in passionate kisses. They take turns removing each others clothes with their mouths; some of the intimate articles are playfully tossed your way.")
        wm.UpdateImage(wm.IMG.LESBIAN)
        Dialog("They join you on the bed and slowly and expertly probe each other with fingers and tongues. You alternate fondling their lithe bodies and stroking your member.")
        local position = wm.Range(0, 2)
        if position == 0 then
            wm.UpdateImage(wm.IMG.LES69)
            Dialog("They straddle each other and each girl buries her tongue in the other's cunt. Before long, both are panting and moaning loudly.")
            Dialog("Their bodies quiver with each orgasm that overpowers them.")
        elseif position == 1 then
            wm.UpdateImage(wm.IMG.SCISSOR)
            local beauty_text = ""
            if girl:beauty() > 35 then
                beauty_text = "You appreciate the gesture, mesmerized by the sight in front of you as they start rubbing their pussies together. "
            else
                beauty_text = "Unfortunately, " .. girl:firstname() .. " isn't really that impressive to look at, and you resort to closing your eyes and focus on her moans, which do get you excited. "
            end
            Dialog("Now that they are both properly excited, they arrange themselves in a scissor position, making sure that they give you a good view. " .. beauty_text ..
                    "Their cries increase in volume until they start to quiver and collapse onto each other.")
        elseif position == 2 then
            wm.UpdateImage(wm.IMG.EATOUT)
            Dialog(other_name .. " lies down on her back, and " .. girl:firstname() ..
                    " starts munching on her pussy. Judging by the moans and sight, she seems to be doing a rather good job, and soon " ..
                    other_name .. " shudders as she climaxes. ")
        end

        Dialog("They lay beside each other on the bed looking into each others eyes.")
        if girl:skill_check(wm.SKILLS.ORALSEX, 50) and not girl:has_trait("Lesbian") then
            wm.UpdateImage(wm.IMG.ORAL)
            Dialog("You stand over them; your erect member inches from their faces. \"What about me, girls?\" They giggle and begin to lick and stroke your staff with skill. The sensation is amazing and you shoot a large stream of semen across those cute faces.")
        end
        girl:happiness(2)
        girl:lesbian(1)
    else
        Dialog("The girls hesitantly move closer. A few awkward pecks on the cheek later you find the need to direct them.")
        wm.UpdateImage(wm.IMG.STRIP)
        Dialog("You tell them to get undressed after which they begin to cautiously touch one another.  \"No, no, no!\" you exclaim, \"Lick her damn cunt!\"  They both immediately try to comply and bump heads.")
        wm.UpdateImage(wm.IMG.EATOUT)
        Dialog("You shake your head as the girls make faces after each time tongue meets pussy.")
        Dialog("After all the time you spent directing you had no chance to enjoy the show; awkward as it was.  You sigh and pray to goddess of Yuri that they at least learned something from the experience.")
        girl:lesbian(2)
        girl:tiredness(3)
    end
end

---@param girl wm.Girl
function BeastSex(girl)
    Dialog("I wonder if you would cheer up my pet Malboro. He's been down lately and could really use a good fucking.")
    if girl:obey_check(wm.ACTIONS.SEX) then
        wm.UpdateImage(wm.IMG.BEAST)
        girl:experience(6)
        girl:tiredness(5)
        Dialog("She smiles and nods. I've always liked that \"little\" guy.  Let's go cheer him up.")
        if girl:skill_check(wm.SKILLS.BEASTIALITY, 75) then
            Dialog("She smartly removes her clothing before she enters the cage. Beasts don't really care about what their fuck toys look like anyway.")
            Dialog("She approaches the massive tentacled beast with skill of and professional handler.  She finds just the right spots to arouse the monster.")
            Dialog("The malboro immediately responds and several tentacles seize her arms and legs.  She squeals with delight as tentacles enter her pussy and anus.")
            Dialog("After a good long while of probing her with tentacles the malboro releases the girls and slithers off satisfied.")
            girl:happiness(1)
            girl:beastiality(1)
        else
            Dialog("She enters the cage fully clothed, which turns out to be a mistake when she walks up behind the creature and startles it.")
            Dialog("The creature goes into a blind sexual fury and shreds her clothing and violently shoves multiple tentacles into her orifices.")
            Dialog("Her screams of terror are muffled by the tentacles in her mouth, but they are still audible enough for you to hear as a fifth tentacle approaches her ass.")
            Dialog("Fearing for her safety you and your men rush to the rescue.  You are forced to injure your pet in the process, not to mention the cost to replace her clothing.")
            girl:health(-5)
            girl:libido(-2)
            girl:tiredness(10)
            girl:beastiality(2)
            -- TODO what happens if the player doesn't have the funds?
            wm.TakePlayerGold(wm.Range(50, 75))
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to have sex with a beast.")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function AnalSex(girl)
    Dialog("As you enter the room, the sight of her luscious ass assails your delighted eyes.  She looks back at you and greets you with a smile.")
    Dialog("A thought pops into your head as you continue to admire the view.  You voice your thought \"I think we should try something a little different tonight?\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        wm.UpdateImage(wm.IMG.ANAL)
        Dialog("She shakes her ass and bends over further; slightly pulling down her panties.  'Well, come and get it.\" she purrs")
        girl:experience(6)
        girl:tiredness(3)
        if girl:skill_check(wm.SKILLS.ANAL, 75) then
            Dialog("You move toward her and slide the panties down to the floor.  She reaches back and grips your rod, rubbing it against her wet pussy for lubrication and finally presses the now slick tip against her anus.")
            Dialog("A shiver of anticipation washes over you before you thrust deep into her ass.  A gasp of delight escapes her lips and she begins to rock back against you")
            if girl:has_trait("Great Arse") then
                Dialog("You can't help but be amazed by the sight of your cock penetrating her truly fine ass.  The enjoyment overwhelms you and you explode deep inside her ass.")
            else
                Dialog("She expertly moves and teases you with her ass and you both collapse in ecstasy.")
            end
            girl:happiness(2)
            girl:anal(1)
        else
            Dialog("She visibly winces as she feels your throbbing manhood pressing against her anus.  A tear streaks down her cheek as you penetrate her.")
            Dialog("She begins crying as you begin to move back and forth inside her.  You try to enjoy yourself inspite of her ,but the crying proves too distracting and you begin to lose interest.")
            Dialog("You pull out your cock and manually finish yourself on her ass.")
            girl:happiness(-5)
            girl:libido(-5)
            girl:tiredness(5)
            girl:anal(2)
            girl:libido(-2)
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She quickly stands up and turns around.  \"I know what your thinking and the answer is NO.\"")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function BDSMSex(girl)
    Dialog("As you enter her room you accidentally jostle the crate you're carrying. Hearing the clinking and clanging she eyes the crate. \"What do you have got in the crate?\" She asks.")
    Dialog("\"Take a look.\" you say. Her eyes widen at the sight of the whips, chains, and harnesses.")
    if girl:obey_check(wm.ACTIONS.SEX) then
        wm.UpdateImage(wm.IMG.BDSM)
        girl:experience(6)
        girl:tiredness(5)
        Dialog("A smile graces her face as she picks through the plethora of various bondage toys. ")
        if girl:skill_check(wm.SKILLS.BDSM, 75) then
            Dialog("She chooses a large wicked looking dildo and some anal beads from the crate. She blushes as she begs you to use them after you've tied her up.")
            Dialog("You bind her arms and legs to the large wooden X. Selecting riding crop from the crate, you proceed to spank her ass harder and harder. You begin shoving bead after bead into her eager ass hole.")
            Dialog("You unbind her from the X and chain her arms above her head in a standing position. You grasp the dildo she selected and begin ramming it into her wet cunt as you smack her tits with the crop.")
            Dialog("Each stroke draws gasps of pain and pleasure. she moans in ecstasy as you ravage her body. You leave the dildo in her pussy and quickly pull the beads from her ass;  You pinch her nipples as you ram you cock deep into her ass.  You thrust roughly until you both cum hard.  You unchain her and she collapses on the floor exhausted and ecstatic.")
            girl:happiness(2)
            girl:libido(2)
            girl:bdsm(1)
        else
            Dialog("She seems to have something to prove today and she selects the biggest and most painful instruments for you to use on her. Before you begin she tells you to hit her as hard as you like.")
            Dialog("You whip her repeatedly with the studded cat-o-nine whip. It takes a lot of effort to force the gigantic dildo into her pussy, she screams as a small trickle of blood runs down her inner thigh.")
            Dialog("Concerned you ask if you should stop. She sobs \"No, I can do this. Keep going.\" You force the massive butt plug into her sphincter bringing more tears streaming down her face.  ")
            Dialog("she loses her resolve and the now pathetic crying and begging to remove the toys causes you to lose your erection. Disgusted, You leave her bound and sobbing in the room.  A few hours later you send some men to untie her.")
            girl:happiness(-5)
            girl:libido(-5)
            girl:bdsm(2)
            girl:health(-5)
            girl:tiredness(10)
        end
        PlayerFucksGirlUpdate(girl)
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to let you tie her up.")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function MasturbateSex(girl)
    Dialog("Her back is toward you as you enter the room. You notice that one of her hands is between her legs.")
    Dialog("You sneak around her side to get a better view. A few moments later she gasps and jumps up as she notices you standing there.")
    Dialog("\"Oh, you don't have to stop on my account.\" you say with a wink.")
    if girl:obey_check(wm.ACTIONS.WORKSTRIP) then
        wm.UpdateImage(wm.IMG.STRIP)
        girl:experience(6)
        girl:tiredness(2)
        Dialog("She relaxes and grins devilishly.  \"Alright, I hope you will enjoy the show,\" she then adds \"but no touching.\"")
        if girl:skill_check(wm.SKILLS.STRIP, 75) then
            wm.UpdateImage(wm.IMG.MAST)
            Dialog("She moves to her bed and makes herself comfortable and making sure you have a good view.  She begins rubbing her mound through her panties and before long a dark wet spot begins to grow;  you also start feeling some growth.")
            if girl:has_trait("Great Figure") then
                Dialog("As she rubs, pulls, and teases her pussy; You admire her incredible figure as it undulates and gyrates from the stimulation.")
            end
            if girl:beauty() > 75 then
                Dialog("As you watch her sliding her fingers in and out you marvel at how beautiful everything about this girls is.  It occurs to you that she has one of the most fantastic vaginas you have ever seen.  A moan brings your attention back to the show.")
            end
            Dialog("She pulls her panties to the side and begins to work herself over with the precision of a practised expert.")
            Dialog("You are impressed that she is able to keep up the stimulation as she bucks wildly.  You stroke you shaft in time with her probings.")
            Dialog("Her moans become screams of pleasure as she approaches another massive orgasm.  You both cum simultaneously and some of your orgasm lands across her stomach.")
        else
            Dialog("She moves to the bed to get comfortable but gives no consideration to your view.  She reaches inside her panties and begins massaging her clit.")
            Dialog("She barely makes a sound and you can't tell what she is doing but there is certainly very little movement.")
            Dialog("After a few minutes of the same routine, she looks up and you and tells you she is done.  You leave disappointed.")
        end
        girl:service(2)
    else
        if girl:has_trait("Meek") then
            Dialog("She blushes to a deep red and pushes you out of the room without ever making eye contact.")
        else
            wm.UpdateImage(wm.IMG.REFUSE)
            Dialog("She immediately covers herself and demands you leave her room.")
            return girl:trigger("girl:refuse")
        end
    end
end

---@param girl wm.Girl
function FFMSex(girl)
    Dialog("You fetch one of your girls before going to " .. girl:name() .. "'s room'")
    local other_name = RandomGirlName()
    wm.UpdateImage(wm.IMG.BED)
    Dialog("You find her relaxing on her bed looking through some of her lingerie. She looks up as you speak." ..
           "\"I wonder if you could help me, my dear. ".. other_name .. " and I would like you to join us in some fun\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        if girl:has_trait("Lesbian") and girl:pclove() < 50 then
            if wm.Percent(50) or girl:is_slave() then
                Dialog("She looks at you skeptically, but starts to smile as soon as " .. other_name .. " enters the room. \"Sure, should be fun!\"")
            else
                Dialog("\"I'd prefer to spend the evening with ".. other_name "\", she says.")
                local choice = ChoiceBox("", "Tough luck", "If you let me watch, that's fine by me")
                if choice == 0 then
                    Dialog("\"Surely, you aren't turning down your boss after he went through, " ..
                                 "especially after he went through all the trouble of bringing an extra companion\", you chide her. \n" ..
                                 "She sights: \"If you insist.\"")
                    girl:pclove(-1)
                    girl:happiness(-1)
                else
                    HandleLesbianSex(girl, other_name)
                    girl:happiness(1)
                    return
                end
            end
        elseif girl:has_trait("Straight") and girl:pclove() > 75 and wm.Percent(50) then
            Dialog("\"I'd much rather have you all to myself\"")
            local choice = ChoiceBox("", "Sorry, but I already promised " .. other_name, "OK, it'll be just us two")
            if choice == 0 then
                Dialog("She sights: \"If you insist.\"")
                girl:pclove(-1)
                girl:happiness(-1)
            else
                HandleNormalSex(girl)
                girl:happiness(1)
                return
            end
        else
            Dialog("She nods in agreement. \"Sure, come on in!\"")
        end

        girl:experience(6)
        girl:tiredness(4)

        if girl:skill_check(wm.SKILLS.GROUP, 50) and girl:skill_check(wm.SKILLS.LESBIAN, 50) then
            wm.UpdateImage(wm.IMG.FFM)
            Dialog(other_name .. " and " .. girl:name() .. " start undressing and caressing each other, providing you with quite the show.\n" ..
                   "Once they are both naked, they beckon you to join them on the bed. " .. other_name .. " starts massaging your shaft while " .. girl:firstname() ..
                   " is making out with you. This is what heaven must be like!")
            if girl:has_trait("Lesbian") and wm.Percent(50) then
                girl:lesbian(1)
                Dialog("You notice that " .. girl:firstname() .. " appears to be more interested in " .. other_name .. " than in you. " ..
                       "You decide to let her have her fun, and let the two girls have at it while you stroke your cock.")
                wm.UpdateImage(wm.IMG.LES69)
                Dialog("And what a show you get! They are both licking each others cunt, moaning and sighing with delight until they explode with pleasure")
                Dialog("Once they are done " .. girl:firstname() .. " looks at you sheepishly: \"Sorry, I got carried away and forgot about you. Here, let me take care of that\"")
                wm.UpdateImage(wm.IMG.HAND)
                Dialog("With that, she takes your dick in her hands. After the spectacle you've just seen, it doesn't take long for you to erupt.")
                Dialog("Though this was not quite what you had in mind, you cannot really complain. Your only regret is that you did not think to bring a " ..
                       "kinematograph. If you could advertise with what you've witnessed tonight, you are sure that your establishment would be overflowing.")
            else
                girl:oralsex(1)
                Dialog("After a time, they switch things up. " .. girl:firstname() ..
                       " starts licking your cock, and you decide to pay it forward and ask " .. other_name .. " to sit on your face.\n" ..
                       "Your bury your tongue in her cunt at the same time as " .. girl:firstname() .. " takes you all the way into her mouth. \n" ..
                       "As you feel yourself approaching climax, you intensify your efforts, and are rewarded with " .. other_name ..
                       " squirting hot juices all over you just as you spurt your load in " .. girl:firstname() .. "'s mouth. With your cum still " ..
                       " on her tongue, the two girls share a passionate kiss.")
                wm.UpdateImage(wm.IMG.LICK)
                Dialog("Not wanting to show favouritism, you ask " .. girl:firstname() .. " to lie down and spread her legs for you. " ..
                       " Again your nimble tongue starts doing its work.")
                wm.UpdateImage(wm.IMG.FFM)
                Dialog("You spent the next hours with " .. girl:firstname() .. " and " .. other_name ..
                       " engaged in various positions. Why can't every evening be like this one?")
            end
            girl:happiness(5)
            girl:libido(2)
            girl:group(1)
            girl:lesbian(1)
        else
            if girl:has_trait("Straight") and wm.Percent(50) then
                wm.UpdateImage(wm.IMG.ORAL)
                Dialog(girl:name() .. " doesn't really know what to do with another woman, and is not interacting with " ..
                       other_name .. " at all. In the end, you end up receiving a blowjob while " .. other_name .. " just watches." ..
                       "Not really what you had hoped for.")
                girl:oralsex(1)
            elseif girl:has_trait("Lesbian") and wm.Percent(50) then
                wm.UpdateImage(wm.IMG.LESBIAN)
                Dialog(girl:name() .. " seems to be only interested in " .. other_name .. ", giving you basically zero attention. " ..
                       "In the end, you decide to just let the two girls enjoy each other and lean back to indulge in the spectacle.")
                if girl:skill_check(wm.SKILLS.LESBIAN, 50) then
                    wm.UpdateImage(wm.IMG.EATOUT)
                    Dialog("At least " .. girl:firstname() .. " appears to be quite skilled at pleasuring " .. other_name .. ", " ..
                           "judging by the moans and cries she elicits.")
                    Dialog("With this scenery, it doesn't take long for your cock to become rock-hard. Seeing that the two girls will be busy for a while, " ..
                           "you sigh and take matters into your own hand. Not exactly what you had hoped for, but not a total waste of a night either.")
                else
                    Dialog("However, even in that regard you end up thoroughly disappointed: " .. girl:firstname() .. " has no idea how to pleasure another girl. " ..
                           "Frustrated, you leave the room in search for better entertainment.")
                end
            else
                wm.UpdateImage(wm.IMG.FFM)
                Dialog(girl:name() .. " seemed unsure what to do, and mostly just watches as " .. other_name .. " takes your dick in her mouth." ..
                       "When you suggested that she help, she just managed to butt her head with " .. other_name .. "'s, and ended up not only " ..
                       "not participating herself, but also ruining an up-to-that-point perfectly fine blowjob. What a disastrous night!")
            end
            girl:happiness(-2)
            girl:libido(-3)
            girl:tiredness(4)
            girl:group(1)
            girl:lesbian(1)
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        if girl:has_trait("Straight") and girl:dignity() < 33 then
            Dialog("She refuses. \"I'm not having sex with another girl!\"")
        else
            Dialog("She refuses to be have sex with you and the other girl.")
        end
        wm.UpdateImage(wm.IMG.REFUSE)
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function MMFSex(girl)
    Dialog("You fetch one of your goons before going to " .. girl:name() .. "'s room'")
    local other_name = RandomChoice("John", "James", "Tommy", "Chad", "Steve", "Alex")
    Dialog("You find her relaxing on her bed looking through some of her lingerie. She looks up as you speak." ..
               "\"" .. other_name .. " and I are looking for some entertainment for the evening. A threesome would be nice, don't you think my dear...\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        if girl:has_trait("Lesbian") and wm.Percent(33) then
            Dialog("Her face looks disgusted for a second, but she catches herself and beckons \"Come in.\"")
            girl:happiness(-3)
        elseif girl:pclove() > 80 and wm.Percent(33) then
            Dialog("\"I'd much rather have you all to myself\", she says.")
            local choice = ChoiceBox("", "Sorry, but I already promised " .. other_name, "OK, it'll be just us two")
            if choice == 0 then
                Dialog("She sights: \"If you insist.\"")
                girl:pclove(-1)
                girl:happiness(-1)
            else
                HandleNormalSex(girl)
                girl:happiness(1)
                return
            end
        else
            Dialog("She nods in agreement, but asks for a few minutes to get ready.  As the door closes behind you, many rumbling and rustling sounds can be heard.  A minute or so later she declares that she is ready.")
            Dialog("When you enter the room you find her wearing only her favorite black lingerie. \"Welcome, gentlemen!\"")
        end
        girl:experience(6)
        girl:tiredness(4)

        if girl:skill_check(wm.SKILLS.GROUP, 50) then
            wm.UpdateImage(wm.IMG.MMF)
            if girl:has_trait("Lesbian") then
                Dialog("Even though her appetites go in another direction, " .. girl:name() .. " is undeniably skilled at " ..
                       "pleasing a man -- or two men, as it were. If you hadn't know, you would never have guessed that she is Lesbian.")
            else
                local option = wm.Range(0, 2)
                if option == 0 then
                    Dialog("A cock in each hand, " .. girl:name() .. " expertly demonstrates how to pleasure two men at the same time.")
                    girl:hand(1)
                elseif option == 1 then
                    Dialog("She expertly sucks " .. other_name .. "'s cock while you pound her from behind. This is exactly the kind of relaxation you needed tonight.")
                    girl:normalsex(1)
                else
                    Dialog("One cock in her hand, the other in her mouth, " .. girl:name() .. " expertly demonstrates how to pleasure two men at the same time.")
                    girl:oralsex(1)
                end
            end

            girl:happiness(4)
            girl:libido(2)
            girl:group(1)
        else
            if girl:has_trait("Lesbian") then
                wm.UpdateImage(wm.IMG.MMF)
                Dialog(girl:name() .. " doesn't know what to do with a single cock, let alone two.")
            else
                wm.UpdateImage(wm.IMG.HAND)
                Dialog("Pleasing two men at the same time proved to much for " .. girl:name() .. ". " ..
                       "While " .. other_name .. " is receiving a handjob, your are left to watch. Your clear your throat " ..
                       "loudly to get her attention, and she immediately switches to stroking your cock, now leaving " ..
                       other_name .. " with his throbbing erection to watch dumbfounded.")
                Dialog("In the end, " .. other_name .. " has to settle for waiting his turn. Not really what ")
            end
            girl:happiness(-2)
            girl:libido(-3)
            girl:tiredness(4)
            girl:group(1)
            girl:hand(1)
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to be have sex with you and the other guy.")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function OrgySex(girl)
    Dialog("You ask the group of men to wait in the hall as you enter her room.")
    wm.UpdateImage(wm.IMG.BED)
    Dialog("You find her relaxing on her bed looking through some of her lingerie.  She looks up as you speak.\"I wonder if you could help me, my dear.  I've got a group of gentlemen outside and I was wondering if you could help me entertain them?\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        girl:experience(6)
        girl:tiredness(5)
        Dialog("She nods in agreement, but asks for a few minutes to get ready.  As the door closes behind you, many rumbling and rustling sounds can be heard.  A minute or so later she declares that she is ready.")
        if girl:has_trait("Nymphomaniac") then
            wm.UpdateImage(wm.IMG.NUDE)
            Dialog("You lead the men inside and you all stand at attention for the amazing sight before you.  She stands in the center of the room surrounded by pillows and cushions.  There isn't a stitch of clothing on her body, which shines from the coating of lubricant she has applied.  She waits for the door to close before she strikes a sexy pose and exclaims \"Lets see who can catch the greased courtesan first!\"")
            wm.UpdateImage(wm.IMG.ORGY)
            Dialog("She slips and slides among the group, escaping holds to be caught by others. Before long everyone is panting and slippery. " ..
                    "She arranges you all laying on the floor and slides her body along the group taking turns and stopping at each man to ride his throbbing erection. " ..
                    "She expertly times each individual session and doesn't leave anyone wanting. The Sun breaks through the window and you awake on her floor with her laying on top of the group.")
            girl:libido(5)
            girl:happiness(2)
            girl:tiredness(6)
            girl:group(1)
        else
            if girl:skill_check(wm.SKILLS.GROUP, 75) then
                wm.UpdateImage(wm.IMG.ECCHI)
                Dialog("The group enters the room and forms a circle around the kneeling and eager girl in her favorite black lingerie.")
                wm.UpdateImage(wm.IMG.ORGY)
                Dialog("She reaches up and frees your cock from it's cloth prison.  As she begins to suck and lick the tip she reaches to her sides and liberates the other mens' dicks as well.")
                Dialog("She continues to suck your cock and stroke the men next to you with her hands as another man climbs beneath her and inserts himself into her vagina.  Another man kneels down behind her and penetrates her ass.  This continues through the night with men taking turns with all her holes.  ")
                Dialog("The men compliment you on an excellent evening and blow kisses to the completely exhausted and sleeping woman on the bed.")
                girl:happiness(5)
                girl:libido(2)
                girl:group(1)
            else
                wm.UpdateImage(wm.IMG.FORMAL)
                Dialog("The room has been arranged with a table in the center circled by chairs.  She bows \"It will be my pleasure to serve you tonight, gentlemen.\"  You sigh to yourself as you realize she has gotten the wrong idea.  She realizes her error as the group removes erect penises from their pants and move toward her.")
                wm.UpdateImage(wm.IMG.ORGY)
                Dialog("To her credit she recovers from the shock quickly and lays back with her feet on the table.  The men take turns passing and sliding her around the table.  She allows the group to enter her every orifice but does little to enhance the experience.")
                Dialog("You see the men out; lost in your thoughts of the disastrous performance.  One older gentlemen attempts to cheer you up by saying \"It was an enjoyable enough night, Sir.  Any gangbang is a good gangbang\"")
                girl:happiness(-2)
                girl:libido(-3)
                girl:tiredness(5)
                girl:group(2)
            end
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to be gangbanged like some dirty beggar on the streets.")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function Strip(girl)
    Dialog("\"Good evening, my dear.  I stopped by because I wanted to see how well you've learned to work the pole.\"")
    if girl:has_trait("Slow Learner") then
        wm.UpdateImage(wm.IMG.ORAL)
        Dialog("She immediately drops to her knees and takes you into her mouth.  Well, It's not exactly what you had in mind, but there's no point to stopping her now.  She swallows every drop as you unload in her mouth.")
    else
        if girl:obey_check(wm.ACTIONS.SEX) then
            wm.UpdateImage(wm.IMG.STRIP)
            girl:experience(4)
            girl:tiredness(2)
            Dialog("She leads you to a comfortable chair next to a small stage with a tall, metal pole in the center.")
            if girl:skill_check(wm.SKILLS.STRIP, 75) then
                -- TODO other traits?
                if girl:has_trait("Big Boobs") then
                    Dialog("You glue your eyes to her wonderfully large breasts.  They sway and bounce as she begins her dance.")
                end
                if girl:has_trait("Great Figure") then
                    Dialog("As she strips away each layer of clothing you admire her perfectly proportioned body.  You become lost in her curves as they undulate before you.")
                end

                Dialog("She moves fluidly and naturally. You marvel as each piece of clothing isn't just forcibly removed, rather it slides off her body like cascading water.  She alternates slow deliberate stretches with incredible acrobatics on the pole.  Each bend and twirl seems blended together perfectly and you are lost in the experience.")
                Dialog("She slides across the stage and continues her dance in your lap.  You can almost feel every part of her womanhood as she grinds against you crotch.  Your mind is swirling and just as you are about to explode...she comes to a complete stop, straddling your lap.")
                Dialog("She looks deep into your eyes and her lips begin to part slightly... Suddenly she sits upright and sticks out her tongue with a wink.  \"How was that for a tease?\" She asks playfully")
                girl:happiness(2)
                girl:libido(5)
                girl:strip(1)
            else
                Dialog("She nearly trips as she steps onto the stage.  She seems unsteady on tall stilettos, yet she still attempts to dance.")
                Dialog("She trips and falls several times and fumbles every time she has to undo a clasp or button.")
                Dialog("When she finally manages to remove all her clothing; she has given up on trying to move and stands clinging to the pole and doing a kind of slow wiggle.")
                Dialog("After several minutes of her standing there naked, she stops and asks \"How was that?\"  You feel bad but she needs to hear the truth.  \"That was the worst striptease I have ever seen.  You are beautiful, but stripping is not simply standing naked in front of a crowd.  You have a lot to learn.\"")
                girl:tiredness(3)
                girl:happiness(-1)
                girl:strip(2)
            end
        else
            wm.UpdateImage(wm.IMG.REFUSE)
            Dialog("She refuses to perform a striptease for you.")
            return girl:trigger("girl:refuse")
        end
    end
end

---@param girl wm.Girl
function Blowjob(girl)
    Dialog("\"Now let's see how well you've learned to suck a cock.\"")
    if girl:obey_check(wm.ACTIONS.SEX) then

        girl:experience(6)
        girl:tiredness(2)
        if girl:skill_check(wm.SKILLS.ORALSEX, 75) then
            wm.UpdateImage(wm.IMG.SUCKBALLS)
            Dialog("She smiles and reaches up to grasp your cock. She begins by gently licking and kissing the tip. Her tongue then traces down the length of your shaft and  draws circles around your balls.")
            Dialog("She gently sucks each ball into her mouth in turn. She turns her head and slides her lips back forth on your shaft. She kisses the tip again and slowly slides you into her mouth.")
            if wm.Percent(50) then
                wm.UpdateImage(wm.IMG.ORAL)
            end
            Dialog("You're not sure how long this pleasure lasted but soon you feel the volcano about to erupt. She senses it too and pulls your cock from her mouth and gently strokes it with her hands until you explode all over her face.")
            girl:oralsex(1)
            girl:happiness(2)
        else
            wm.UpdateImage(wm.IMG.ORAL)
            Dialog("She giggles a bit and quickly closes her mouth over your member.  You become a little alarmed as you start to feel her teeth grazing the sensitive skin of your cock.")
            Dialog("You become slightly distracted as her teeth begin to grind harder and harder on your soft flesh.")
            Dialog("Fearing for your dick you tell her to just lick it and use her hands.  No longer distracted you concentrate on her pretty face and how you will soon cover it in your semen.  You feel the pressure building and building. ")
            Dialog("Your dick explodes in an eruption of cum, but as you look down to admire your handiwork you realize that she dodged at the last moment.  You leave disappointed.")
            girl:happiness(-2)
            girl:tiredness(2)
            girl:oralsex(2)
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She closes her mouth tight and turns her head away from your penis.")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function Deepthroat(girl)
    Dialog("\"I want to see how much of this you can fit in your mouth.\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        wm.UpdateImage(wm.IMG.DEEPTHROAT)
        girl:experience(6)
        girl:tiredness(2)
        if girl:skill_check(wm.SKILLS.BDSM, 75) then
            Dialog("She eyes your cock hungrily and begins to suck the tip, swirling her tongue around the head.  She sucks harder and harder and you feel yourself being pulled deeper and deeper inside.")
            Dialog("You are amazed as you feel her nose brush against your abdomen. You look down at her and gaze into her eyes as you grasp the back of her head.")
            Dialog("You begin to move back and forth,  holding tight to her hair.  She trills and hums as you fuck her throat.  You feel the surge of pressure build in your cock and her eyes plead with you to release your load deep inside her.")
            Dialog("She smiles as the thick cream slides down the back of her throat.  As you slowly pull out of her mouth, she sucks and licks every last drop of cum from your penis and swallows it down with a smile.")
            girl:happiness(2)
            girl:bdsm(1)
            girl:oralsex(1)
        else
            Dialog("Her lips part and she begins to gobble down your cock like a sausage staved bavarian.  Despite her efforts she begins to gag violently.")
            Dialog("She tries to soldier on but each time your cock gets halfway into her mouth she gags and has to stop.")
            Dialog("Determined to do as she ask she tries to ram it all down at once.  The result is disastrous as she not only gags but vomits all over you penis.  She apologizes as she cleans you up and you leave unsatisfied.")
            girl:happiness(-3)
            girl:tiredness(5)
            girl:bdsm(2)
            girl:oralsex(1)
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She closes her mouth tight and shakes her head refusingly.")
        return girl:trigger("girl:refuse")
    end
end

---@param girl wm.Girl
function Tittyfuck(girl)
    Dialog("You sit down on a stool beside her. \"Why don't you use your tits and your mouth this time?\"")
    if girl:obey_check(wm.ACTIONS.SEX) then
        wm.UpdateImage(wm.IMG.TITTY)
        -- TODO TRAITS
        girl:experience(6)
        girl:tiredness(2)
        if girl:has_trait("Abnormally Large Boobs") then
            Dialog("As she frees her massive jugs from her, top you marvel at their size.  Your dick becomes lost in the undulating sea of soft pillowy flesh.")
            Dialog("Occasionally, you feel her tongue reach into the sea of breasts and lick the end of your shaft.")
            Dialog("Your mind becomes lost in a fog and you soon feel your volcano about to erupt, but like a volcano under the sea your hot sticky magma is lost under the tide of those gigantic breasts.")
            girl:libido(5)
            girl:tittysex(3)
        elseif girl:has_trait("Big Boobs") then
            Dialog("She giggles a bit as her breasts envelop your member.  She uses her wonderfully large breasts to stroke your shaft as her mouth and tongue work magic on the tip.")
            Dialog("Faster and faster her breasts move up and down your length.  She begins to alternate the motion of each breast and the stimulation begins to overwhelm you..")
            Dialog("You release a massive load of cum covering her breasts and mouth.  She licks your cum from her nipples and then cleans your member with her mouth.  You stagger from her room in ecstacy.")
            girl:libido(4)
            girl:tittysex(3)
        elseif girl:has_trait("Small Boobs") then
            Dialog("You catch her gaze as she looks down at her mostly flat chest and then you see a glint of determination in her eyes.  She begins to rub the tip of your dick around her hardening nipples.")
            Dialog("You enjoy the feeling of her smooth skin against your cock.  She tries valiantly to force her breasts together, but ultimately fails.")
            Dialog("She relies heavily on her expert tongue and her rock hard nipples brushing the tip of your shaft.  You explode suddenly across her chest and she coats her fingers with your cum and licks them like a lollipop, giving you a spritely wink.")
            girl:libido(2)
            girl:tittysex(3)
        else
            Dialog("She licks the full length of your spear to lubricate it and then forces her breasts together tightly and begins to rock up and down on your member.")
            Dialog("The feeling of her softness surrounding your manhood is beyond compare and you are lost in pleasure.")
            Dialog("She notices the sudden swelling of your cock before you do and she gently but vigorously strokes you with her hands.  You cum hard and your milk spills over her breasts.  She  cleans you both up and you leave with a smile on your face.")
            girl:libido(3)
            girl:tittysex(3)
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She stands up quickly and turns her back to you defiantly.")
        return girl:trigger("girl:refuse")
    end
end


---@param girl wm.Girl
function Fingering(girl)
    if girl:obey_check(wm.ACTIONS.SEX) then
        girl:experience(4)
        girl:tiredness(1)
        wm.UpdateImage(wm.IMG.BED)
        if girl:libido() >= 65 and girl:pclove() >= 50 then
            -- she is horny and likes you
            local thanks_text = ""
            if girl:pclove() > 80 then
                if girl:is_slave() then
                    thanks_text = "\"You truly are the best employer I could hope for!\""
                else
                    thanks_text = "\"You truly are the best master I could hope for!\""
                end
            else
                thanks_text = "\"You sure know how to take care of your employees.\""
            end
            wm.UpdateImage(wm.IMG.STRIP)
            Dialog("You haven't even finished stating your intentions when " .. girl:firstname() .. " starts to undress " ..
                    "and presents you her already glistening-wet pussy. " .. thanks_text)
            wm.UpdateImage(wm.IMG.FINGER)
            Dialog("She doesn't look like she is interested in any sort of foreplay, so without further ado you begin to move your index finger " ..
                    "in and out of her vagina, using your thumb to massage her clit. " .. girl:firstname() .. " lets out a sigh and starts playing with her already-hard nipples. " ..
                    "It doesn't take long until you feel her muscles contract and clamp down on your finger. This is encouragement for you to insert a second finger, " ..
                    "and her moans let you know just how enthusiastically she approves.")
            girl:pcfear(-1)
            if wm.Percent(50) then
                wm.UpdateImage(wm.IMG.REST)
                Dialog("At the end of your session, " .. girl:firstname() .. " seems barely coherent. She mumbles something that appears to " ..
                        "contain the words 'amazing' and 'thank you', rests her head on her pillow and is asleep before you even have a chance respond. " ..
                        "You leave the room with a self-satisfied smile.")
                girl:pclove(3)
                girl:tiredness(15)
                girl:happiness(15)
            else
                girl:pclove(1)
                girl:tiredness(5)
                girl:happiness(5)

                if girl:pclove() > 60 and wm.Percent(50) and not girl:has_trait("Lesbian") then
                    Dialog("After her fourth orgasm, she softly pushes your hand away. \"I think it's time for me do repay the favour\"" ..
                            " She unzips your pants and pulls out your cock, already standing at attention. \"Seems like someone already got excited\", she winks. ")
                    wm.UpdateImage(wm.IMG.HAND)
                    if girl:skill_check(wm.SKILLS.HANDJOB, 40) then
                        Dialog("She uses the juices between her thighs to lubricate her hand, and then starts stroking your member. " ..
                                "If only everybody could appreciate your hard work the way " .. girl:firstname() .. " does.")
                        girl:handjob(1)
                    else
                        Dialog("Despite her enthusiasm, the handjob you receive is more uncomfortable than enjoyable. However, " .. girl:firstname() ..
                                " insists that she provide you the 'perfect' experience, so you use this opportunity to impart some wisdom. By the end, her " ..
                                "technique has markedly improved.")
                        girl:handjob(5)
                    end

                else
                    Dialog("After a few more orgasms, you apologize and tell her that you have to leave to fulfill your other duties. " ..
                            "\"That's alright, I'm not sure how much more of this I could take anyway\", she jokes.")
                end
            end
        elseif girl:libido() < 65 and girl:pclove() > 0 then
            -- she isn't horny and doesn't hate you
            local horny_text = ""
            if girl:libido() < 40 then
                horny_text = "She doesn't seem overly " ..
                        " excited, so you start by placing soft kisses on her stomach, then on her breasts and inner thighs."
            end
            Dialog("You ask " .. girl:firstname() .. " to undress and lie on her back. " .. horny_text)
            wm.UpdateImage(wm.IMG.FINGER)
            Dialog("Her breathing starts to speed up, and you notice that she is getting wet. You brush your fingers " ..
                    "lightly against her outer lips, all-the-while still continuing your kisses. When the first soft moan escapes her " ..
                    "lips, you know she is ready. You carefully insert one finger, moving it slowly at first, but speeding up as her " ..
                    "excitement grows.")
            Dialog("She is moaning ecstatically now, and you add a second finger to bring her over the edge.")
            if girl:pclove() > 60 then
                Dialog("\"That was amazing\". You smile. \"Want to go for round two?\"")
                girl:libido(6)
                girl:happiness(4)
                girl:tiredness(4)
                girl:pcfear(-1)
            else
                Dialog("She collapses on her bed, tired but content. Satisfied with your work, you bid her a good night.")
                girl:libido(2)
                girl:happiness(2)
                girl:tiredness(1)
            end
        else
            -- she isn't fond of you,
            wm.UpdateImage(wm.IMG.BED)
            local pclove_text = ""
            if girl:pclove() < - 33 or girl:dignity() > 50 then
                pclove_text = "She seems disgusted by the idea, but wisely holds her tongue. "
                wm.SetPlayerDisposition(-1)
            end
            Dialog("You order " .. girl:firstname() .. " to undress and lie on her back. " .. pclove_text ..
                    "\"Let's get you in the mood, shall we,\" you proclaim as you start suckling on her breasts. At the same time, you move your hands between her " ..
                    "legs and start caressing her lower lips.")

            if girl:libido() > 50 and ((girl:pclove() < -33 and wm.GetPlayerDisposition() < -33) or girl:pclove() < - 66)  then
                -- she is horny but hates you, and your aren't a nice person, or she just really hates you
                local add_text = ""
                if wm.GetPlayerDisposition() < -50 then
                    add_text = "To you, it looks almost cute, and her humiliation just turns you on."
                end
                Dialog(" Despite her obvious discomfort at your presence, her juices start flowing almost immediately. " ..
                        "\"It's nice to see you are as excited as I am,\" you mock her, earning yourself a murderous glare. Its effect is somewhat diminished " ..
                        " as she can't help but let out a soft moan, her face turning red with embarrassment. " .. add_text)
                wm.UpdateImage(wm.IMG.FINGER)
                if wm.GetPlayerDisposition() < -33 then
                    Dialog("You unceremoniously shove three of your fingers into her pussy. \"Damn, you really are quite the slut\". " ..
                            "You continue your mockery as you show her your fingers, dripping wet. \"Well, I'll give you what you want!\"\n" ..
                            "And with that, you start finger-fucking her in earnest.")
                else
                    Dialog("You start inserting one finger. She is so wet that you decide she is ready for more, and add two more fingers. " ..
                            "Now, the finger-fucking begins in earnest.")
                end
                if wm.GetPlayerDisposition() < -65 then
                    Dialog("Her attempts to resist your stimulation are futile, and before long she is moaning loudly and you can feel her " ..
                            "spasming as she reaches her first climax. Now that she is properly warmed up, you can start taking things to the next level.")
                    wm.UpdateImage(wm.IMG.FISTING)
                    Dialog("You add a forth finger and a fifth, ignoring her protestations, and finally you push in your entire hand. That alone " ..
                            "brings her over the edge again, her muscles clamping down so hard on your hand that you couldn't take it out even if you wanted to. " ..
                            "But of course, that wasn't your intention anyway.")
                    Dialog("As you start moving your fingers inside her, your other hand moves to her nipple, pinching hard. In her extasy, she barely seems to notice.")
                    local choice = ChoiceBox("Continue?", "Yes, I'm having fun", "No, I think I've traumatized her enough")
                    if choice == 0 then
                        Dialog("You wait till you can feel her approaching climax again, then you locate her other nipple and start pulling while pressing" ..
                                " your nails in, as strongly as you dare without causing permanent injury. This at least gets a reaction. Her cry of pain gets swept "..
                                " away as she explodes in moans of pleasure.")
                        wm.UpdateImage(wm.IMG.FINGER)
                        Dialog("By now, you cock is almost painfully hard and throbbing, so you decide to take care of yourself. While you masturbate, you continue " ..
                                " to massage her pussy and manage to elicit two more orgasms. Finally, you deposit your load across her stomach. At this point, " .. girl:firstname() ..
                                " is in an almost catatonic state, so you decide that she's had enough. ")
                        wm.UpdateImage(wm.IMG.BED)
                        Dialog("You clean yourself up, pull up your pants, and leave her room, making a mental note to send of of your other girls to check on her later.")
                        girl:pclove(-35)
                        girl:happiness(-35)
                        girl:tiredness(15)
                        girl:bdsm(2)
                        wm.SetPlayerDisposition(-20)
                        girl:pcfear(4)
                    else
                        wm.UpdateImage(wm.IMG.BED)
                        Dialog("You decide that she's had enough and remove your hand. You can't resist the urge to smear her own juices across her face as a parting gift. " ..
                                "By the time you leave her room, you can hear her sobbing uncontrollably.")
                        girl:pclove(-20)
                        girl:happiness(-20)
                        girl:tiredness(5)
                        girl:pcfear(2)
                        wm.SetPlayerDisposition(-10)
                    end
                else
                    Dialog("Her attempts to resist your stimulation are futile, and before long she is moaning loudly and you can feel her " ..
                            "muscles contracting as she climaxes.")
                    wm.UpdateImage(wm.IMG.BED)
                    Dialog("You decide that you have tormented her enough for today, and leave her to clean herself up.")
                    girl:happiness(-10)
                    girl:pclove(-5)
                    girl:tiredness(2)
                    girl:pcfear(1)
                end
            elseif girl:libido() > 50 then
                -- she is very horny, and she doesn't like you, but also doesn't hate you too much
                wm.UpdateImage(wm.IMG.FINGER)
                Dialog(" Her juices start flowing almost immediately, so without much further ado you push two fingers into her vagina, curling them rhythmically inside her. " ..
                        "Before long, you have her screaming in the throes of passion.")
                -- if she really needed it, and doesn't hate you too much, the effect is positive
                if girl:pclove() > -33 and girl:libido() > 80 then
                    girl:pclove(1)
                    girl:happiness(1)
                else
                    girl:pclove(-1)
                    if girl:pclove() > -33 then
                        girl:happiness(-1)
                    else
                        girl:happiness(-3)
                    end
                end
            elseif girl:libido() < 20 and wm.GetPlayerDisposition() > -50 then
                -- she doesn't like you and isn't horny, and you are not too evil
                local stare = "an icy stare"
                if girl:pclove() < - 33 then
                    stare = "a murderous glare"
                end
                local aphro = ""
                if wm.GetPlayerDisposition() < 0 then
                    aphro = "Or maybe you just have to give her an aphrodisiac next time."
                end
                Dialog("The only thing that earns you is " .. stare .. " from " .. girl:firstname() .. ". " ..
                        "If she liked you more, you may have had more success easing her into it. " .. aphro)
                girl:happiness(-1)
                girl:libido(1)
                girl:pclove(-1)
            elseif girl:libido() < 20 then
                -- you are evil
                wm.UpdateImage(wm.IMG.FINGER)
                Dialog("She doesn't seem responsive to your stimulation, but you came here to finger-fuck, so you are " ..
                        "going to finger-fuck her, no matter what. You shove three fingers into her dry pussy, and are rewarded with a cry of pain. " ..
                        "It is a good thing that to your ears they sound just a good as moans of pleasure -- well, maybe not good for her.")
                wm.UpdateImage(wm.IMG.BED)
                Dialog("When you are done, " .. girl:name() .. " is a sobbing mess on her bed, and you imagine her pussy will feel sore for the next days. " ..
                        " She really is going to hate you!")
                girl:pclove(-20)
                girl:happiness(-20)
                girl:tiredness(10)
                girl:health(-4)
                girl:pcfear(1)
                wm.SetPlayerDisposition(-2)
            else
                -- she isn't very horny, and she doesn't like you
                Dialog("It takes you a while, but after a few minutes she is breathing heavily. You decide it she is ready for your finger. " ..
                        "With a good does of spit for additional lubrication, you start slowly probing her with your index finger.")
                wm.UpdateImage(wm.IMG.FINGER)
                girl:happiness(-1)
                girl:pclove(-1)
                Dialog("After half an hour of vigorous finger-fucking, you manage to bring her too orgasm. " ..
                        " This probably would have been easier if she liked you more.")
            end
        end
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to let you finger her. \"Don't touch me!\"")
        return girl:trigger("girl:refuse")
    end
end

function Licking(girl)
    if girl:obey_check(wm.ACTIONS.SEX) then
        wm.UpdateImage(wm.IMG.LICK)
        Dialog("Not written yet")
    else
        wm.UpdateImage(wm.IMG.REFUSE)
        Dialog("She refuses to let you lick her pussy. \"Don't touch me!\"")
        return girl:trigger("girl:refuse")
    end
end