/// This file is automatically generated. Do not change, modifications will be overwritten.

#include "ScreenBase.h"

using namespace screens;


cChoiceBoxBase::cChoiceBoxBase() : cInterfaceWindowXML("ChoiceBox.xml") { };

void cChoiceBoxBase::set_ids() {


    setup_callbacks();
}

cLoadMenuBase::cLoadMenuBase() : cInterfaceWindowXML("LoadMenu.xml") { };

void cLoadMenuBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_Background_id      = get_id("Background");
    m_FileName_id        = get_id("FileName");
    m_LoadGame_id        = get_id("LoadGame");
    m_WhoreMaster_id     = get_id("WhoreMaster");

    setup_callbacks();
}

cNewGameBase::cNewGameBase() : cInterfaceWindowXML("NewGame.xml") { };

void cNewGameBase::set_ids() {
    m_Background_id      = get_id("Background");
    m_BrothelName_id     = get_id("BrothelName");
    m_Cancel_id          = get_id("Cancel");
    m_Config_id          = get_id("Config");
    m_Label1_id          = get_id("Label1");
    m_Label2_id          = get_id("Label2");
    m_Label3_id          = get_id("Label3");
    m_Label4_id          = get_id("Label4");
    m_Label5_id          = get_id("Label5");
    m_Ok_id              = get_id("Ok");
    m_PlayerBirthDayNum_id = get_id("PlayerBirthDayNum");
    m_PlayerBirthDay_id  = get_id("PlayerBirthDay");
    m_PlayerBirthMonthNum_id = get_id("PlayerBirthMonthNum");
    m_PlayerBirthMonth_id = get_id("PlayerBirthMonth");
    m_PlayerHoroscope_id = get_id("PlayerHoroscope");
    m_PlayerName_id      = get_id("PlayerName");
    m_PlayerSurname_id   = get_id("PlayerSurname");
    m_WhoreMaster_id     = get_id("Whore Master");

    setup_callbacks();
}

cTransferGirlsBase::cTransferGirlsBase() : cGameWindow("TransferGirls.xml") { };

void cTransferGirlsBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_BrothelLeft_id     = get_id("BrothelLeft");
    m_BrothelRight_id    = get_id("BrothelRight");
    m_LabelLeft_id       = get_id("LabelLeft");
    m_LabelRight_id      = get_id("LabelRight");
    m_ListLeft_id        = get_id("ListLeft");
    m_ListRight_id       = get_id("ListRight");
    m_ShiftLeft_id       = get_id("ShiftLeft");
    m_ShiftRight_id      = get_id("ShiftRight");

    setup_callbacks();
}

cTurnSummaryBase::cTurnSummaryBase() : cGameWindow("TurnSummary.xml") { };

void cTurnSummaryBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_Background_id      = get_id("Background");
    m_Category_id        = get_id("Category");
    m_CurrentBrothel_id  = get_id("CurrentBrothel");
    m_Event_id           = get_id("Event");
    m_GoTo_id            = get_id("GoTo");
    m_ImagePath_id       = get_id("ImagePath");
    m_Item_id            = get_id("Item");
    m_LabelDesc_id       = get_id("LabelDesc");
    m_LabelEvent_id      = get_id("LabelEvent");
    m_LabelItem_id       = get_id("LabelItem");
    m_NextBtn_id         = get_id("NextButton");
    m_NextWeek_id        = get_id("NextWeek");
    m_PrevBtn_id         = get_id("PrevButton");

    setup_callbacks();
}

cBankScreenBase::cBankScreenBase() : cInterfaceWindowXML("bank_screen.xml") { };

void cBankScreenBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_BankDetails_id     = get_id("BankDetails");
    m_BankIcon_id        = get_id("BankIcon");
    m_DepositAllBtn_id   = get_id("DepositAllButton");
    m_DepositBtn_id      = get_id("DepositButton");
    m_ScreenHeader_id    = get_id("ScreenHeader");
    m_WithdrawAllBtn_id  = get_id("WithdrawAllButton");
    m_WithdrawBtn_id     = get_id("WithdrawButton");

    setup_callbacks();
}

cBuildingBaseBase::cBuildingBaseBase() : cInterfaceWindowXML("building_base.xml") { };

void cBuildingBaseBase::set_ids() {
    m_Background_id      = get_id("Background");
    m_BuildingDetails_id = get_id("BuildingDetails");
    m_BuildingLabel_id   = get_id("BuildingLabel");
    m_GirlImage_id       = get_id("GirlImage");
    m_GirlManagement_id  = get_id("Girl Management");
    m_Menu_id            = get_id("Menu");
    m_NextBtn_id         = get_id("NextButton");
    m_NextWeek_id        = get_id("Next Week");
    m_PrevBtn_id         = get_id("PrevButton");
    m_Setup_id           = get_id("Setup");
    m_VisitTown_id       = get_id("Visit Town");
    m_WalkBtn_id         = get_id("WalkButton");
    m_YourOffice_id      = get_id("YourOffice");

    setup_callbacks();
}

cBuildingSetupScreenBase::cBuildingSetupScreenBase() : cGameWindow("building_setup_screen.xml") { };

void cBuildingSetupScreenBase::set_ids() {
    m_10PotionsBtn_id    = get_id("10PotionsButton");
    m_20PotionsBtn_id    = get_id("20PotionsButton");
    m_AdvertisingSlider_id = get_id("AdvertisingSlider");
    m_AdvertisingValue_id = get_id("AdvertisingValue");
    m_AutoBuyPotionToggle_id = get_id("AutoBuyPotionToggle");
    m_AvailablePotions_id = get_id("AvailablePotions");
    m_BackBtn_id         = get_id("BackButton");
    m_BuildRoomsBtn_id   = get_id("BuildRoomsButton");
    m_CurrentBrothel_id  = get_id("CurrentBrothel");
    m_Gold_id            = get_id("Gold");
    m_PotionCost_id      = get_id("PotionCost");
    m_ProhibitAnalToggle_id = get_id("ProhibitAnalToggle");
    m_ProhibitBdsmtoggle_id = get_id("ProhibitBDSMToggle");
    m_ProhibitBeastToggle_id = get_id("ProhibitBeastToggle");
    m_ProhibitFootJobToggle_id = get_id("ProhibitFootJobToggle");
    m_ProhibitGroupToggle_id = get_id("ProhibitGroupToggle");
    m_ProhibitHandJobToggle_id = get_id("ProhibitHandJobToggle");
    m_ProhibitLesbianToggle_id = get_id("ProhibitLesbianToggle");
    m_ProhibitNormalToggle_id = get_id("ProhibitNormalToggle");
    m_ProhibitOralToggle_id = get_id("ProhibitOralToggle");
    m_ProhibitStripToggle_id = get_id("ProhibitStripToggle");
    m_ProhibitTittyToggle_id = get_id("ProhibitTittyToggle");
    m_RoomAddCost_id     = get_id("RoomAddCost");
    m_SexRestrictions_id = get_id("SexRestrictions");

    setup_callbacks();
}

cClinictryScreenBase::cClinictryScreenBase() : cInterfaceWindowXML("clinictry_screen.xml") { };

void cClinictryScreenBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_Clinic2_id         = get_id("Clinic2");
    m_WalkBtn_id         = get_id("WalkButton");

    setup_callbacks();
}

cDungeonScreenBase::cDungeonScreenBase() : cGameWindow("dungeon_screen.xml") { };

void cDungeonScreenBase::set_ids() {
    m_AllowFoodBtn_id    = get_id("AllowFoodButton");
    m_Arena_id           = get_id("Arena");
    m_BackBtn_id         = get_id("BackButton");
    m_BrandSlaveBtn_id   = get_id("BrandSlaveButton");
    m_Brothel0_id        = get_id("Brothel0");
    m_Brothel1_id        = get_id("Brothel1");
    m_Brothel2_id        = get_id("Brothel2");
    m_Brothel3_id        = get_id("Brothel3");
    m_Brothel4_id        = get_id("Brothel4");
    m_Brothel5_id        = get_id("Brothel5");
    m_Brothel6_id        = get_id("Brothel6");
    m_Centre_id          = get_id("Centre");
    m_Clinic_id          = get_id("Clinic");
    m_DetailsBtn_id      = get_id("DetailsButton");
    m_DungeonHeader_id   = get_id("DungeonHeader");
    m_Farm_id            = get_id("Farm");
    m_GirlImage_id       = get_id("GirlImage");
    m_GirlList_id        = get_id("GirlList");
    m_Gold_id            = get_id("Gold");
    m_House_id           = get_id("House");
    m_InteractBtn_id     = get_id("InteractButton");
    m_InteractCount_id   = get_id("InteractCount");
    m_ReleaseBtn_id      = get_id("ReleaseButton");
    m_ReleaseTo_id       = get_id("ReleaseTo");
    m_RoomsFree_id       = get_id("RoomsFree");
    m_SelectAllCust_id   = get_id("SelectAllCust");
    m_SelectAllGirls_id  = get_id("SelectAllGirls");
    m_SellBtn_id         = get_id("SellButton");
    m_StopFeedingBtn_id  = get_id("StopFeedingButton");
    m_Studio_id          = get_id("Studio");
    m_TortureBtn_id      = get_id("TortureButton");

    setup_callbacks();
}

cGalleryScreenBase::cGalleryScreenBase() : cGameWindow("gallery_screen.xml") { };

void cGalleryScreenBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_Futa_id            = get_id("Futa");
    m_GirlImage_id       = get_id("GirlImage");
    m_ImageData_id       = get_id("ImageData");
    m_ImageList_id       = get_id("ImageList");
    m_ImageName_id       = get_id("ImageName");
    m_ImageSource_id     = get_id("ImageSource");
    m_NextBtn_id         = get_id("NextButton");
    m_Participants_id    = get_id("Participants");
    m_Pregnant_id        = get_id("Pregnant");
    m_PrevBtn_id         = get_id("PrevButton");
    m_TiedUp_id          = get_id("TiedUp");

    setup_callbacks();
}

cGameSetupBase::cGameSetupBase() : cInterfaceWindowXML("game_setup.xml") { };

void cGameSetupBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_LoadBtn_id         = get_id("LoadBtn");
    m_OkBtn_id           = get_id("OkButton");
    m_RevertBtn_id       = get_id("RevertButton");
    m_ScreenHeader_id    = get_id("ScreenHeader");
    m_SettingsList_id    = get_id("SettingsList");

    setup_callbacks();
}

cGangsScreenBase::cGangsScreenBase() : cInterfaceWindowXML("gangs_screen.xml") { };

void cGangsScreenBase::set_ids() {
    m_AutoBuyHealToggle_id = get_id("AutoBuyHealToggle");
    m_AutoBuyNetsToggle_id = get_id("AutoBuyNetsToggle");
    m_BackBtn_id         = get_id("BackButton");
    m_BuyHealPotBtn10_id = get_id("BuyHealPotButton10");
    m_BuyHealPotBtn20_id = get_id("BuyHealPotButton20");
    m_BuyHealPotBtn_id   = get_id("BuyHealPotButton");
    m_BuyNetsBtn10_id    = get_id("BuyNetsButton10");
    m_BuyNetsBtn20_id    = get_id("BuyNetsButton20");
    m_BuyNetsBtn_id      = get_id("BuyNetsButton");
    m_Catacombs_id       = get_id("Catacombs");
    m_ControlCatacombs_id = get_id("ControlCatacombs");
    m_FireArrow_id       = get_id("FireArrow");
    m_GangFireBtn_id     = get_id("GangFireButton");
    m_GangGetsBeast_id   = get_id("GangGetsBeast");
    m_GangGetsGirls_id   = get_id("GangGetsGirls");
    m_GangGetsItems_id   = get_id("GangGetsItems");
    m_GangHireBtn_id     = get_id("GangHireButton");
    m_GangList_id        = get_id("GangList");
    m_GirlsPercSlider_id = get_id("GirlsPercSlider");
    m_Gold_id            = get_id("Gold");
    m_HealPotDescription_id = get_id("HealPotDescription");
    m_HireArrow_id       = get_id("HireArrow");
    m_ItemsPercSlider_id = get_id("ItemsPercSlider");
    m_MissionDescription_id = get_id("MissionDescription");
    m_MissionList_id     = get_id("MissionList");
    m_NetDescription_id  = get_id("NetDescription");
    m_RecruitList_id     = get_id("RecruitList");
    m_TotalCost_id       = get_id("TotalCost");
    m_TxtControlledGangs_id = get_id("txtControlledGangs");
    m_TxtMissions_id     = get_id("txtMissions");
    m_WeaponDescription_id = get_id("WeaponDescription");
    m_WeaponUpBtn_id     = get_id("WeaponUpButton");

    setup_callbacks();
}

cGetInputBase::cGetInputBase() : cInterfaceWindowXML("getInput.xml") { };

void cGetInputBase::set_ids() {
    m_Cancel_id          = get_id("Cancel");
    m_Label_id           = get_id("Label");
    m_Ok_id              = get_id("Ok");
    m_TextField_id       = get_id("TextField");

    setup_callbacks();
}

cGirlDetailsScreenBase::cGirlDetailsScreenBase() : cGameWindow("girl_details_screen.xml") { };

void cGirlDetailsScreenBase::set_ids() {
    m_AccomSlider_id     = get_id("AccomSlider");
    m_AccomValue_id      = get_id("AccomValue");
    m_BackBtn_id         = get_id("BackButton");
    m_DayBtn_id          = get_id("DayButton");
    m_GalleryBtn_id      = get_id("GalleryButton");
    m_GirlDescription_id = get_id("GirlDescription");
    m_GirlImage_id       = get_id("GirlImage");
    m_GirlName_id        = get_id("GirlName");
    m_HousePercSlider_id = get_id("HousePercSlider");
    m_HousePercValue_id  = get_id("HousePercValue");
    m_InteractBtn_id     = get_id("InteractButton");
    m_InteractCount_id   = get_id("InteractCount");
    m_InventoryBtn_id    = get_id("InventoryButton");
    m_JobHeader_id       = get_id("JobHeader");
    m_JobList_id         = get_id("JobList");
    m_JobTypeHeader_id   = get_id("JobTypeHeader");
    m_JobTypeList_id     = get_id("JobTypeList");
    m_MoreBtn_id         = get_id("MoreButton");
    m_NextBtn_id         = get_id("NextButton");
    m_NightBtn_id        = get_id("NightButton");
    m_PrevBtn_id         = get_id("PrevButton");
    m_ReleaseDungeonBtn_id = get_id("ReleaseDungeonButton");
    m_SendDungeonBtn_id  = get_id("SendDungeonButton");
    m_TakeGoldBtn_id     = get_id("TakeGoldButton");
    m_TraitDescription_id = get_id("TraitDescription");
    m_TraitHeader_id     = get_id("TraitHeader");
    m_TraitInfoBtn_id    = get_id("TraitInfoBtn");
    m_TraitList_id       = get_id("TraitList");
    m_UseAntiPregToggle_id = get_id("UseAntiPregToggle");

    setup_callbacks();
}

cGirlManagementBaseBase::cGirlManagementBaseBase() : cInterfaceWindowXML("girl_management_base.xml") { };

void cGirlManagementBaseBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_CurrentBrothel_id  = get_id("CurrentBrothel");
    m_DayBtn_id          = get_id("DayButton");
    m_FireBtn_id         = get_id("FireButton");
    m_FreeSlaveBtn_id    = get_id("FreeSlaveButton");
    m_GirlDescription_id = get_id("GirlDescription");
    m_GirlImage_id       = get_id("GirlImage");
    m_Gold_id            = get_id("Gold");
    m_JobDescription_id  = get_id("JobDescription");
    m_JobHeader_id       = get_id("JobHeader");
    m_JobList_id         = get_id("JobList");
    m_JobTypeDescription_id = get_id("JobTypeDescription");
    m_JobTypeHeader_id   = get_id("JobTypeHeader");
    m_JobTypeList_id     = get_id("JobTypeList");
    m_NextBtn_id         = get_id("NextButton");
    m_NextWeek_id        = get_id("Next Week");
    m_NightBtn_id        = get_id("NightButton");
    m_PrevBtn_id         = get_id("PrevButton");
    m_SellSlaveBtn_id    = get_id("SellSlaveButton");
    m_TransferBtn_id     = get_id("TransferButton");
    m_ViewDetailsBtn_id  = get_id("ViewDetailsButton");

    setup_callbacks();
}

cInGameMenuBase::cInGameMenuBase() : cInterfaceWindowXML("in_game_menu.xml") { };

void cInGameMenuBase::set_ids() {
    m_ContinueBtn_id     = get_id("ContinueBtn");
    m_QuitBtn_id         = get_id("QuitBtn");
    m_SaveBtn_id         = get_id("SaveBtn");

    setup_callbacks();
}

cInfoTraitsScreenBase::cInfoTraitsScreenBase() : cInterfaceWindowXML("info_traits_screen.xml") { };

void cInfoTraitsScreenBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_DescrTxt_id        = get_id("DescrTxt");
    m_Description_id     = get_id("Description");
    m_EffectsList_id     = get_id("EffectsList");
    m_ExclusionList_id   = get_id("ExclusionList");
    m_ExclusionsTxt_id   = get_id("ExclusionsTxt");
    m_HelpLbl_id         = get_id("HelpLbl");
    m_HelpText_id        = get_id("HelpText");
    m_PropExplLbl_id     = get_id("PropExplLbl");
    m_PropExplanation_id = get_id("PropExplanation");
    m_PropertyList_id    = get_id("PropertyList");
    m_TraitEffectTxt_id  = get_id("TraitEffectTxt");
    m_TraitPropTxt_id    = get_id("TraitPropTxt");
    m_TraitsListTxt_id   = get_id("TraitsListTxt");
    m_TraitsList_id      = get_id("TraitsList");

    setup_callbacks();
}

cItemmanagementScreenBase::cItemmanagementScreenBase() : cInterfaceWindowXML("itemmanagement_screen.xml") { };

void cItemmanagementScreenBase::set_ids() {
    m_AutoUseItems_id    = get_id("AutoUseItems");
    m_BackBtn_id         = get_id("BackButton");
    m_Buy10LeftBtn_id    = get_id("Buy10LeftButton");
    m_Buy10RightBtn_id   = get_id("Buy10RightButton");
    m_CurrentBrothel_id  = get_id("CurrentBrothel");
    m_EquipLeftBtn_id    = get_id("EquipLeftButton");
    m_EquipRightBtn_id   = get_id("EquipRightButton");
    m_FilterList_id      = get_id("FilterList");
    m_IndicateLeft_id    = get_id("IndicateLeft");
    m_IndicateRight_id   = get_id("IndicateRight");
    m_ItemDesc_id        = get_id("ItemDesc");
    m_ItemsLeftHeader_id = get_id("ItemsLeftHeader");
    m_ItemsLeftList_id   = get_id("ItemsLeftList");
    m_ItemsRightHeader_id = get_id("ItemsRightHeader");
    m_ItemsRightList_id  = get_id("ItemsRightList");
    m_OwnersLeftDetails_id = get_id("OwnersLeftDetails");
    m_OwnersLeftHeader_id = get_id("OwnersLeftHeader");
    m_OwnersLeftList_id  = get_id("OwnersLeftList");
    m_OwnersRightDetails_id = get_id("OwnersRightDetails");
    m_OwnersRightHeader_id = get_id("OwnersRightHeader");
    m_OwnersRightList_id = get_id("OwnersRightList");
    m_PlayerGold_id      = get_id("PlayerGold");
    m_Sell10LeftBtn_id   = get_id("Sell10LeftButton");
    m_Sell10RightBtn_id  = get_id("Sell10RightButton");
    m_SellAllLeftBtn_id  = get_id("SellAllLeftButton");
    m_SellAllRightBtn_id = get_id("SellAllRightButton");
    m_ShiftHeader_id     = get_id("ShiftHeader");
    m_ShiftLeftBtn_id    = get_id("ShiftLeftButton");
    m_ShiftRightBtn_id   = get_id("ShiftRightButton");
    m_UnequipLeftBtn_id  = get_id("UnequipLeftButton");
    m_UnequipRightBtn_id = get_id("UnequipRightButton");

    setup_callbacks();
}

cMainMenuBase::cMainMenuBase() : cInterfaceWindowXML("main_menu.xml") { };

void cMainMenuBase::set_ids() {
    m_Background_id      = get_id("Background");
    m_Continue_id        = get_id("Continue");
    m_LoadGame_id        = get_id("Load Game");
    m_NewGame_id         = get_id("New Game");
    m_QuitGame_id        = get_id("Quit Game");
    m_ReadMe_id          = get_id("ReadMe");
    m_Settings_id        = get_id("Settings");
    m_Version_id         = get_id("Version");
    m_VisitUs_id         = get_id("VisitUs");
    m_WhoreMaster_id     = get_id("WhoreMaster");

    setup_callbacks();
}

cMayorScreenBase::cMayorScreenBase() : cInterfaceWindowXML("mayor_screen.xml") { };

void cMayorScreenBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_BribeBtn_id        = get_id("BribeButton");
    m_MayorDetails_id    = get_id("MayorDetails");
    m_MayorIcon_id       = get_id("MayorIcon");
    m_ScreenHeader_id    = get_id("ScreenHeader");

    setup_callbacks();
}

cMovieMakerScreenBase::cMovieMakerScreenBase() : cInterfaceWindowXML("movie_maker_screen.xml") { };

void cMovieMakerScreenBase::set_ids() {
    m_AddScene_id        = get_id("AddScene");
    m_AutoCreateMovies_id = get_id("AutoCreateMovies");
    m_BackBtn_id         = get_id("BackButton");
    m_MakeThisMovie_id   = get_id("MakeThisMovie");
    m_MarketResearch_id  = get_id("MarketResearch");
    m_MovieDetailTxt_id  = get_id("MovieDetail_txt");
    m_MovieDetails_id    = get_id("MovieDetails");
    m_MovieNameTxt_id    = get_id("MovieName_txt");
    m_MovieName_id       = get_id("MovieName");
    m_Movie_id           = get_id("Movie");
    m_PredictedAudience_id = get_id("PredictedAudience");
    m_Prediction_id      = get_id("Prediction");
    m_ReleaseMovieBtn_id = get_id("ReleaseMovieButton");
    m_RemoveScene_id     = get_id("RemoveScene");
    m_ScenesList_id      = get_id("ScenesList");
    m_Scenes_id          = get_id("Scenes");
    m_ScrapScene_id      = get_id("ScrapScene");

    setup_callbacks();
}

cMovieMarketScreenBase::cMovieMarketScreenBase() : cInterfaceWindowXML("movie_market_screen.xml") { };

void cMovieMarketScreenBase::set_ids() {
    m_AdCampaign_id      = get_id("AdCampaign");
    m_BackBtn_id         = get_id("BackButton");
    m_DecreaseTicketPrice_id = get_id("DecreaseTicketPrice");
    m_GroupsList_id      = get_id("GroupsList");
    m_IncreaseTicketPrice_id = get_id("IncreaseTicketPrice");
    m_MovieTxt_id        = get_id("Movie_txt");
    m_MoviesList_id      = get_id("MoviesList");
    m_PredictionList_id  = get_id("PredictionList");
    m_Prediction_id      = get_id("Prediction");
    m_PromoTxt_id        = get_id("Promo_txt");
    m_SurveyTxt_id       = get_id("Survey_txt");
    m_Survey_id          = get_id("Survey");
    m_TargetGroups_id    = get_id("Target_Groups");
    m_TicketPriceTxt_id  = get_id("TicketPrice_txt");

    setup_callbacks();
}

cPlayerOfficeScreenBase::cPlayerOfficeScreenBase() : cGameWindow("player_office_screen.xml") { };

void cPlayerOfficeScreenBase::set_ids() {
    m_Background_id      = get_id("Background");
    m_BuildingList_id    = get_id("BuildingList");
    m_BuyInteract10_id   = get_id("BuyInteract10");
    m_BuyInteract_id     = get_id("BuyInteract");
    m_D_id               = get_id("d");
    m_DateLbl_id         = get_id("DateLbl");
    m_Dungeon_id         = get_id("Dungeon");
    m_GangManagement_id  = get_id("Gang Management");
    m_InfoLbl_id         = get_id("InfoLbl");
    m_InteractText_id    = get_id("InteractText");
    m_InventoryBtn_id    = get_id("InventoryButton");
    m_Menu_id            = get_id("Menu");
    m_NextWeek_id        = get_id("Next Week");
    m_Settings_id        = get_id("Settings");
    m_TurnSummary_id     = get_id("Turn Summary");
    m_VisitBuildings_id  = get_id("Visit Buildings");
    m_VisitTown_id       = get_id("Visit Town");

    setup_callbacks();
}

cPopupMessageBase::cPopupMessageBase() : cInterfaceWindowXML("popup_message.xml") { };

void cPopupMessageBase::set_ids() {


    setup_callbacks();
}

cPreparingGameScreenBase::cPreparingGameScreenBase() : cGameWindow("preparing_game_screen.xml") { };

void cPreparingGameScreenBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_Background_id      = get_id("Background");
    m_Text1_id           = get_id("Text1");
    m_Text2_id           = get_id("Text2");
    m_Text3_id           = get_id("Text3");

    setup_callbacks();
}

cPrisonScreenBase::cPrisonScreenBase() : cGameWindow("prison_screen.xml") { };

void cPrisonScreenBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_GirlDescription_id = get_id("GirlDescription");
    m_PrisonIcon_id      = get_id("PrisonIcon");
    m_PrisonList_id      = get_id("PrisonList");
    m_ReleaseBtn_id      = get_id("ReleaseButton");
    m_ScreenHeader_id    = get_id("ScreenHeader");
    m_ShowMoreBtn_id     = get_id("ShowMoreButton");

    setup_callbacks();
}

cPropertyManagementBase::cPropertyManagementBase() : cInterfaceWindowXML("property_management.xml") { };

void cPropertyManagementBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_PArName_id         = get_id("P_Ar_Name");
    m_PB0AdvAmnt_id      = get_id("P_B0_AdvAmnt");
    m_PB0AdvSlid_id      = get_id("P_B0_AdvSlid");
    m_PB0Name_id         = get_id("P_B0_Name");
    m_PB0NoAnal_id       = get_id("P_B0_NoAnal");
    m_PB0NoBdsm_id       = get_id("P_B0_NoBDSM");
    m_PB0NoBeast_id      = get_id("P_B0_NoBeast");
    m_PB0NoGroup_id      = get_id("P_B0_NoGroup");
    m_PB0NoHandJob_id    = get_id("P_B0_NoHandJob");
    m_PB0NoLes_id        = get_id("P_B0_NoLes");
    m_PB0NoNorm_id       = get_id("P_B0_NoNorm");
    m_PB0NoOral_id       = get_id("P_B0_NoOral");
    m_PB0NoTitty_id      = get_id("P_B0_NoTitty");
    m_PB0PotA_id         = get_id("P_B0_Pot_A");
    m_PB0PotB_id         = get_id("P_B0_Pot_B");
    m_PB1AdvAmnt_id      = get_id("P_B1_AdvAmnt");
    m_PB1AdvSlid_id      = get_id("P_B1_AdvSlid");
    m_PB1Name_id         = get_id("P_B1_Name");
    m_PB1NoAnal_id       = get_id("P_B1_NoAnal");
    m_PB1NoBdsm_id       = get_id("P_B1_NoBDSM");
    m_PB1NoBeast_id      = get_id("P_B1_NoBeast");
    m_PB1NoGroup_id      = get_id("P_B1_NoGroup");
    m_PB1NoHandJob_id    = get_id("P_B1_NoHandJob");
    m_PB1NoLes_id        = get_id("P_B1_NoLes");
    m_PB1NoNorm_id       = get_id("P_B1_NoNorm");
    m_PB1NoOral_id       = get_id("P_B1_NoOral");
    m_PB1NoTitty_id      = get_id("P_B1_NoTitty");
    m_PB1PotA_id         = get_id("P_B1_Pot_A");
    m_PB1PotB_id         = get_id("P_B1_Pot_B");
    m_PB2AdvAmnt_id      = get_id("P_B2_AdvAmnt");
    m_PB2AdvSlid_id      = get_id("P_B2_AdvSlid");
    m_PB2Name_id         = get_id("P_B2_Name");
    m_PB2NoAnal_id       = get_id("P_B2_NoAnal");
    m_PB2NoBdsm_id       = get_id("P_B2_NoBDSM");
    m_PB2NoBeast_id      = get_id("P_B2_NoBeast");
    m_PB2NoGroup_id      = get_id("P_B2_NoGroup");
    m_PB2NoHandJob_id    = get_id("P_B2_NoHandJob");
    m_PB2NoLes_id        = get_id("P_B2_NoLes");
    m_PB2NoNorm_id       = get_id("P_B2_NoNorm");
    m_PB2NoOral_id       = get_id("P_B2_NoOral");
    m_PB2NoTitty_id      = get_id("P_B2_NoTitty");
    m_PB2PotA_id         = get_id("P_B2_Pot_A");
    m_PB2PotB_id         = get_id("P_B2_Pot_B");
    m_PB3AdvAmnt_id      = get_id("P_B3_AdvAmnt");
    m_PB3AdvSlid_id      = get_id("P_B3_AdvSlid");
    m_PB3Name_id         = get_id("P_B3_Name");
    m_PB3NoAnal_id       = get_id("P_B3_NoAnal");
    m_PB3NoBdsm_id       = get_id("P_B3_NoBDSM");
    m_PB3NoBeast_id      = get_id("P_B3_NoBeast");
    m_PB3NoGroup_id      = get_id("P_B3_NoGroup");
    m_PB3NoHandJob_id    = get_id("P_B3_NoHandJob");
    m_PB3NoLes_id        = get_id("P_B3_NoLes");
    m_PB3NoNorm_id       = get_id("P_B3_NoNorm");
    m_PB3NoOral_id       = get_id("P_B3_NoOral");
    m_PB3NoTitty_id      = get_id("P_B3_NoTitty");
    m_PB3PotA_id         = get_id("P_B3_Pot_A");
    m_PB3PotB_id         = get_id("P_B3_Pot_B");
    m_PB4AdvAmnt_id      = get_id("P_B4_AdvAmnt");
    m_PB4AdvSlid_id      = get_id("P_B4_AdvSlid");
    m_PB4Name_id         = get_id("P_B4_Name");
    m_PB4NoAnal_id       = get_id("P_B4_NoAnal");
    m_PB4NoBdsm_id       = get_id("P_B4_NoBDSM");
    m_PB4NoBeast_id      = get_id("P_B4_NoBeast");
    m_PB4NoGroup_id      = get_id("P_B4_NoGroup");
    m_PB4NoHandJob_id    = get_id("P_B4_NoHandJob");
    m_PB4NoLes_id        = get_id("P_B4_NoLes");
    m_PB4NoNorm_id       = get_id("P_B4_NoNorm");
    m_PB4NoOral_id       = get_id("P_B4_NoOral");
    m_PB4NoTitty_id      = get_id("P_B4_NoTitty");
    m_PB4PotA_id         = get_id("P_B4_Pot_A");
    m_PB4PotB_id         = get_id("P_B4_Pot_B");
    m_PB5AdvAmnt_id      = get_id("P_B5_AdvAmnt");
    m_PB5AdvSlid_id      = get_id("P_B5_AdvSlid");
    m_PB5Name_id         = get_id("P_B5_Name");
    m_PB5NoAnal_id       = get_id("P_B5_NoAnal");
    m_PB5NoBdsm_id       = get_id("P_B5_NoBDSM");
    m_PB5NoBeast_id      = get_id("P_B5_NoBeast");
    m_PB5NoGroup_id      = get_id("P_B5_NoGroup");
    m_PB5NoHandJob_id    = get_id("P_B5_NoHandJob");
    m_PB5NoLes_id        = get_id("P_B5_NoLes");
    m_PB5NoNorm_id       = get_id("P_B5_NoNorm");
    m_PB5NoOral_id       = get_id("P_B5_NoOral");
    m_PB5NoTitty_id      = get_id("P_B5_NoTitty");
    m_PB5PotA_id         = get_id("P_B5_Pot_A");
    m_PB5PotB_id         = get_id("P_B5_Pot_B");
    m_PB6AdvAmnt_id      = get_id("P_B6_AdvAmnt");
    m_PB6AdvSlid_id      = get_id("P_B6_AdvSlid");
    m_PB6Name_id         = get_id("P_B6_Name");
    m_PB6NoAnal_id       = get_id("P_B6_NoAnal");
    m_PB6NoBdsm_id       = get_id("P_B6_NoBDSM");
    m_PB6NoBeast_id      = get_id("P_B6_NoBeast");
    m_PB6NoGroup_id      = get_id("P_B6_NoGroup");
    m_PB6NoHandJob_id    = get_id("P_B6_NoHandJob");
    m_PB6NoLes_id        = get_id("P_B6_NoLes");
    m_PB6NoNorm_id       = get_id("P_B6_NoNorm");
    m_PB6NoOral_id       = get_id("P_B6_NoOral");
    m_PB6NoTitty_id      = get_id("P_B6_NoTitty");
    m_PB6PotA_id         = get_id("P_B6_Pot_A");
    m_PB6PotB_id         = get_id("P_B6_Pot_B");
    m_PCeName_id         = get_id("P_Ce_Name");
    m_PClName_id         = get_id("P_Cl_Name");
    m_PHoAdvAmnt_id      = get_id("P_Ho_AdvAmnt");
    m_PHoAdvSlid_id      = get_id("P_Ho_AdvSlid");
    m_PHoName_id         = get_id("P_Ho_Name");
    m_PHoNoAnal_id       = get_id("P_Ho_NoAnal");
    m_PHoNoBdsm_id       = get_id("P_Ho_NoBDSM");
    m_PHoNoBeast_id      = get_id("P_Ho_NoBeast");
    m_PHoNoGroup_id      = get_id("P_Ho_NoGroup");
    m_PHoNoHandJob_id    = get_id("P_Ho_NoHandJob");
    m_PHoNoLes_id        = get_id("P_Ho_NoLes");
    m_PHoNoNorm_id       = get_id("P_Ho_NoNorm");
    m_PHoNoOral_id       = get_id("P_Ho_NoOral");
    m_PHoNoTitty_id      = get_id("P_Ho_NoTitty");
    m_PHoPotA_id         = get_id("P_Ho_Pot_A");
    m_PHoPotB_id         = get_id("P_Ho_Pot_B");
    m_PStName_id         = get_id("P_St_Name");
    m_PotionCost_id      = get_id("PotionCost");

    setup_callbacks();
}

cSettingsBase::cSettingsBase() : cInterfaceWindowXML("settings.xml") { };

void cSettingsBase::set_ids() {
    m_BackBtn_id         = get_id("BackButton");
    m_CharactersLabel_id = get_id("CharactersLabel");
    m_Characters_id      = get_id("Characters");
    m_DefaultImagesLabel_id = get_id("DefaultImagesLabel");
    m_DefaultImages_id   = get_id("DefaultImages");
    m_DrawingsCb_id      = get_id("DrawingsCB");
    m_Fullscreen_id      = get_id("Fullscreen");
    m_HeightLabel_id     = get_id("HeightLabel");
    m_ItemsLabel_id      = get_id("ItemsLabel");
    m_Items_id           = get_id("Items");
    m_OkBtn_id           = get_id("OkButton");
    m_PhotosCb_id        = get_id("PhotosCB");
    m_PreferDefault_id   = get_id("PreferDefault");
    m_RenderingsCb_id    = get_id("RenderingsCB");
    m_RevertBtn_id       = get_id("RevertButton");
    m_SavesLabel_id      = get_id("SavesLabel");
    m_Saves_id           = get_id("Saves");
    m_ScreenHeader_id    = get_id("ScreenHeader");
    m_StyleLabel_id      = get_id("StyleLabel");
    m_ThemeLabel_id      = get_id("ThemeLabel");
    m_ThemeList_id       = get_id("ThemeList");
    m_WidthLabel_id      = get_id("WidthLabel");
    m_WindowHeight_id    = get_id("WindowHeight");
    m_WindowWidth_id     = get_id("WindowWidth");

    setup_callbacks();
}

cSlavemarketScreenBase::cSlavemarketScreenBase() : cGameWindow("slavemarket_screen.xml") { };

void cSlavemarketScreenBase::set_ids() {
    m_Arena_id           = get_id("Arena");
    m_BackBtn_id         = get_id("BackButton");
    m_Brothel0_id        = get_id("Brothel0");
    m_Brothel1_id        = get_id("Brothel1");
    m_Brothel2_id        = get_id("Brothel2");
    m_Brothel3_id        = get_id("Brothel3");
    m_Brothel4_id        = get_id("Brothel4");
    m_Brothel5_id        = get_id("Brothel5");
    m_Brothel6_id        = get_id("Brothel6");
    m_BuySlaveBtn_id     = get_id("BuySlaveButton");
    m_Centre_id          = get_id("Centre");
    m_Clinic_id          = get_id("Clinic");
    m_Dungeon_id         = get_id("Dungeon");
    m_Farm_id            = get_id("Farm");
    m_GirlDesc_id        = get_id("GirlDesc");
    m_GirlImage_id       = get_id("GirlImage");
    m_Gold_id            = get_id("Gold");
    m_House_id           = get_id("House");
    m_ReleaseTo_id       = get_id("ReleaseTo");
    m_RoomsFree_id       = get_id("RoomsFree");
    m_ShowMoreBtn_id     = get_id("ShowMoreButton");
    m_SlaveDetails_id    = get_id("SlaveDetails");
    m_SlaveList_id       = get_id("SlaveList");
    m_SlaveMarket_id     = get_id("SlaveMarket");
    m_Studio_id          = get_id("Studio");
    m_TraitListT_id      = get_id("TraitListT");

    setup_callbacks();
}

cTownScreenBase::cTownScreenBase() : cGameWindow("town_screen.xml") { };

void cTownScreenBase::set_ids() {
    m_Arena_id           = get_id("Arena");
    m_BackBtn_id         = get_id("BackButton");
    m_Bank_id            = get_id("Bank");
    m_Brothel0_id        = get_id("Brothel0");
    m_Brothel1_id        = get_id("Brothel1");
    m_Brothel2_id        = get_id("Brothel2");
    m_Brothel3_id        = get_id("Brothel3");
    m_Brothel4_id        = get_id("Brothel4");
    m_Brothel5_id        = get_id("Brothel5");
    m_Brothel6_id        = get_id("Brothel6");
    m_Centre_id          = get_id("Centre");
    m_Clinic_id          = get_id("Clinic");
    m_CurrentBrothel_id  = get_id("CurrentBrothel");
    m_Farm_id            = get_id("Farm");
    m_GirlImage_id       = get_id("GirlImage");
    m_House_id           = get_id("House");
    m_MayorsOffice_id    = get_id("MayorsOffice");
    m_NextWeek_id        = get_id("Next Week");
    m_Prison_id          = get_id("Prison");
    m_Shop_id            = get_id("Shop");
    m_SlaveMarket_id     = get_id("SlaveMarket");
    m_Studio_id          = get_id("Studio");
    m_TownMap_id         = get_id("TownMap");
    m_WalkBtn_id         = get_id("WalkButton");

    setup_callbacks();
}
