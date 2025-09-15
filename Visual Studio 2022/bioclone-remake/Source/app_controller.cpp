/*
*
*	Megan Grass
*	August 15, 2025
*
*/

#include "app.h"

static constexpr Resident_Evil_Key NONE = Resident_Evil_Key::NONE;
static constexpr Resident_Evil_Key UP = Resident_Evil_Key::UP;
static constexpr Resident_Evil_Key RIGHT = Resident_Evil_Key::RIGHT;
static constexpr Resident_Evil_Key DOWN = Resident_Evil_Key::DOWN;
static constexpr Resident_Evil_Key LEFT = Resident_Evil_Key::LEFT;
static constexpr Resident_Evil_Key AIM_UP = Resident_Evil_Key::AIM_UP;
static constexpr Resident_Evil_Key AIM_DOWN = Resident_Evil_Key::AIM_DOWN;
static constexpr Resident_Evil_Key AIM_FIRE = Resident_Evil_Key::AIM_FIRE;
static constexpr Resident_Evil_Key INSPECT = Resident_Evil_Key::INSPECT;
static constexpr Resident_Evil_Key AIM = Resident_Evil_Key::AIM;
static constexpr Resident_Evil_Key RUN = Resident_Evil_Key::RUN;
static constexpr Resident_Evil_Key PREV = Resident_Evil_Key::PREV;
static constexpr Resident_Evil_Key NEXT = Resident_Evil_Key::NEXT;
static constexpr Resident_Evil_Key CONFIRM = Resident_Evil_Key::CONFIRM;
static constexpr Resident_Evil_Key CANCEL = Resident_Evil_Key::CANCEL;
static constexpr Resident_Evil_Key STATUS = Resident_Evil_Key::STATUS;
static constexpr Resident_Evil_Key NOTHING = Resident_Evil_Key::NOTHING;

void Global_Application::InitCommonStateBio1(std::uintmax_t iWeapon)
{
	Idle_Turn = StateType(AnimationIndex::Normal, AnimStateCustom::Idle_Turn, "Idle_Turn");

	Quick_Turn = StateType(AnimationIndex::Normal, AnimStateCustom::Quick_Turn, "Quick_Turn");
	Quick_Reload = StateType(AnimationIndex::Normal, AnimStateCustom::Quick_Reload, "Quick_Reload");

	Walk_Backward = StateType(AnimationIndex::Normal, AnimStatePlayerBio1::Walk_Backward, "Walk_Backward");
	Walk_Backward_Startled = StateType(AnimationIndex::Normal, AnimStatePlayerBio1::Walk_Backward_Startled, "Walk_Backward_Startled");
	Walk_Backward_Caution = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Walk_Backward_Caution");

	Inspect_Kneel = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Inspect_Kneel, "Inspect_Kneel");

	Push_Begin = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Push_Begin");
	Push = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Push");

	Damage_Front_Minor = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Damage_Front_Minor");
	Damage_Back = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Damage_Back");
	Damage_Front = StateType(AnimationIndex::Normal, AnimStatePlayerBio1::Damage_Front, "Damage_Front");

	Death = StateType(AnimationIndex::Normal, AnimStatePlayerBio1::Death, "Death");
}

void Global_Application::InitWeaponStateBio1(std::uintmax_t iWeapon)
{
	Idle = StateType(AnimationIndex::Normal, AnimStatePlayerBio1::Idle, "Idle");
	Idle_Pose_Begin = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Idle_Pose_Begin, "Idle_Pose_Begin");
	Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Idle_Pose, "Idle_Pose");
	Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Idle_Pose_Extra");
	Idle_Caution = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Idle_Caution");
	Idle_Danger = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Idle_Danger");

	Walk_Forward = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Walk_Forward, "Walk_Forward");
	Walk_Forward_Caution = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Walk_Forward_Caution");
	Walk_Forward_Danger = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Walk_Forward_Danger");

	Run = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Run, "Run");
	Run_Caution = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Run_Caution");
	Run_Danger = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Run_Danger");

	Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Begin");
	Aim_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Upward_Begin");
	Aim_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Downward_Begin");

	Aim = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim");
	Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Upward");
	Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Downward");

	Fire_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Begin");
	Fire_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_Begin");
	Fire_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_Begin");

	Fire_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_End");
	Fire_Upward_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_End");
	Fire_Downward_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_End");

	Fire_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Empty");
	Fire_Upward_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_Empty");
	Fire_Downward_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_Empty");

	Fire = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire");
	Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward");
	Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward");

	Fire_Blocked = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Blocked");

	Reload = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Reload");

	if (iWeapon == 0x00 || iWeapon == 0x0B)
	{
	}
	else if (iWeapon == 0x01)
	{
		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Aim_Begin, "Aim_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Aim_Downward, "Aim_Downward");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW01Bio1::Fire_Downward, "Fire_Downward");
	}
	else if (iWeapon == 0x05)
	{
		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Aim_Begin, "Aim_Begin");
		Aim_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Aim_Upward_Begin, "Aim_Upward_Begin");
		Aim_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Aim_Downward_Begin, "Aim_Downward_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Aim_Downward, "Aim_Downward");

		Fire_Begin = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Fire_Begin, "Fire_Begin");
		Fire_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Fire_Upward_Begin, "Fire_Upward_Begin");
		Fire_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Fire_Downward_Begin, "Fire_Downward_Begin");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW05Bio1::Fire_Downward, "Fire_Downward");
	}
	else
	{
		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Aim_Begin, "Aim_Begin");
		Aim_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Aim_Upward_Begin, "Aim_Upward_Begin");
		Aim_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Aim_Downward_Begin, "Aim_Downward_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Aim_Downward, "Aim_Downward");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Fire_Downward, "Fire_Downward");

		Reload = StateType(AnimationIndex::Weapon, AnimStateW02Bio1::Reload, "Reload");
	}

	if (iWeapon == 0x07)
	{
		Reload = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Reload");
	}
}

void Global_Application::InitCommonStateBio2Nov96(std::uintmax_t iWeapon)
{
	Idle_Turn = StateType(AnimationIndex::Normal, AnimStateCustom::Idle_Turn, "Idle_Turn");

	Quick_Turn = StateType(AnimationIndex::Normal, AnimStateCustom::Quick_Turn, "Quick_Turn");
	Quick_Reload = StateType(AnimationIndex::Normal, AnimStateCustom::Quick_Reload, "Quick_Reload");

	Walk_Backward = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Walk_Backward, "Walk_Backward");
	Walk_Backward_Startled = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Walk_Backward_Startled, "Walk_Backward_Startled");
	Walk_Backward_Caution = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Walk_Backward_Caution");

	Inspect_Kneel = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Inspect_Kneel, "Inspect_Kneel");

	Push_Begin = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Push_Begin, "Push_Begin");
	Push = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Push, "Push");

	Damage_Front_Minor = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Damage_Front_Minor, "Damage_Front_Minor");
	Damage_Back = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Damage_Back, "Damage_Back");
	Damage_Front = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Damage_Front, "Damage_Front");

	Death = StateType(AnimationIndex::Normal, AnimStatePlayerBio2Nov96::Death, "Death");
}

void Global_Application::InitWeaponStateBio2Nov96(std::uintmax_t iWeapon)
{
	Idle = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Idle");
	Idle_Pose_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Idle_Pose_Begin");
	Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Idle_Pose");
	Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Idle_Pose_Extra");
	Idle_Caution = StateType(AnimationIndex::Weapon, AnimStatePlayerBio2Nov96::Idle_Caution, "Idle_Caution");
	Idle_Danger = StateType(AnimationIndex::Weapon, AnimStatePlayerBio2Nov96::Idle_Danger, "Idle_Danger");

	Walk_Forward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Walk_Forward");
	Walk_Forward_Caution = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Walk_Forward_Caution");
	Walk_Forward_Danger = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Walk_Forward_Danger");

	Run = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Run");
	Run_Caution = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Run_Caution");
	Run_Danger = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Run_Danger");

	Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Begin");
	Aim_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Upward_Begin");
	Aim_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Downward_Begin");

	Aim = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim");
	Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Upward");
	Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Downward");

	Fire_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Begin");
	Fire_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_Begin");
	Fire_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_Begin");

	Fire_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_End");
	Fire_Upward_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_End");
	Fire_Downward_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_End");

	Fire_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Empty");
	Fire_Upward_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_Empty");
	Fire_Downward_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_Empty");

	Fire = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire");
	Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward");
	Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward");

	Fire_Blocked = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Blocked");

	Reload = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Reload");

	if (iWeapon == 0x00)
	{
		Idle = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Idle_Pose, "Idle_Pose");
	}
	else if (iWeapon == 0x02 || iWeapon == 0x04 || iWeapon == 0x06 || iWeapon == 0x0E || iWeapon == 0x0F || iWeapon == 0x012)
	{
		Idle = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Idle_Pose, "Idle_Pose");
		Idle_Pose_Begin = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Idle_Pose_Begin, "Idle_Pose_Begin");
		Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Idle_Pose, "Idle_Pose");
		Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateW00Bio2Nov96::Idle_Pose_Extra, "Idle_Pose_Extra");

		Run = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Run, "Run");

		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Aim_Begin, "Aim_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Aim_Downward, "Aim_Downward");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2Nov96::Fire_Downward, "Fire_Downward");
	}
	else if (iWeapon == 0x01)
	{
		Idle = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Idle, "Idle_Pose");
		Idle_Pose_Begin = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Idle_Pose_Begin, "Idle_Pose_Begin");
		Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Idle_Pose, "Idle_Pose");
		Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateW01Bio2Nov96::Idle_Pose_Extra, "Idle_Pose_Extra");

		Walk_Forward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Walk_Forward, "Walk_Forward");

		Run = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Run, "Run");

		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Aim_Begin, "Aim_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Aim_Downward, "Aim_Downward");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2Nov96::Fire_Downward, "Fire_Downward");
	}
	else if (iWeapon == 0x03 || iWeapon == 0x05 || iWeapon == 0x07 || iWeapon == 0x08 || iWeapon == 0x0D)
	{
		Idle = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Idle, "Idle_Pose");
		Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Idle_Pose, "Idle_Pose");
		Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateW03Bio2Nov96::Idle_Pose_Extra, "Idle_Pose_Extra");

		Walk_Forward = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Walk_Forward, "Walk_Forward");

		Run = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Run, "Run");

		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Aim_Begin, "Aim_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Aim_Downward, "Aim_Downward");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Fire_Downward, "Fire_Downward");

		Reload = StateType(AnimationIndex::Weapon, AnimStateW03Bio2Nov96::Reload, "Reload");
	}
	else if (iWeapon == 0x09 || iWeapon == 0x0A || iWeapon == 0x0B)
	{
		Idle = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Idle, "Idle_Pose");
		Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Idle_Pose, "Idle_Pose");
		Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateW09Bio2Nov96::Idle_Pose_Extra, "Idle_Pose_Extra");

		Walk_Forward = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Walk_Forward, "Walk_Forward");

		Run = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Run, "Run");

		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Aim_Begin, "Aim_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Aim_Downward, "Aim_Downward");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW09Bio2Nov96::Fire_Downward, "Fire_Downward");
	}
	else if (iWeapon == 0x0C || iWeapon == 0x13)
	{
		Idle = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Idle, "Idle_Pose");
		Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Idle_Pose, "Idle_Pose");
		Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateW0CBio2Nov96::Idle_Pose_Extra, "Idle_Pose_Extra");

		Walk_Forward = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Walk_Forward, "Walk_Forward");

		Run = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Run, "Run");

		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Aim_Begin, "Aim_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Aim_Downward, "Aim_Downward");

		Fire_Empty = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Fire_Empty, "Fire_Empty");
		Fire_Upward_Empty = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Fire_Upward_Empty, "Fire_Upward_Empty");
		Fire_Downward_Empty = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Fire_Downward_Empty, "Fire_Downward_Empty");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW0CBio2Nov96::Fire_Downward, "Fire_Downward");
	}
	else if (iWeapon == 0x10 || iWeapon == 0x11 || iWeapon == 0x14)
	{
		Idle = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Idle_Pose, "Idle_Pose");
		Idle_Pose_Begin = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Idle_Pose_Begin, "Idle_Pose_Begin");
		Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Idle_Pose, "Idle_Pose");
		Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateW10Bio2Nov96::Idle_Pose_Extra, "Idle_Pose_Extra");

		Run = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Run, "Run");

		Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Aim_Begin, "Aim_Begin");

		Aim = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Aim_Downward, "Aim_Downward");

		Fire_Empty = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Fire_Empty, "Fire_Empty");
		Fire_Upward_Empty = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Fire_Upward_Empty, "Fire_Upward_Empty");
		Fire_Downward_Empty = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Fire_Downward_Empty, "Fire_Downward_Empty");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW10Bio2Nov96::Fire_Downward, "Fire_Downward");
	}
}

void Global_Application::InitCommonStateBio2(std::uintmax_t iWeapon)
{
	Idle_Turn = StateType(AnimationIndex::Normal, AnimStateCustom::Idle_Turn, "Idle_Turn");

	Quick_Turn = StateType(AnimationIndex::Normal, AnimStateCustom::Quick_Turn, "Quick_Turn");
	Quick_Reload = StateType(AnimationIndex::Normal, AnimStateCustom::Quick_Reload, "Quick_Reload");

	Walk_Backward = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward, "Walk_Backward");
	Walk_Backward_Startled = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward_Startled, "Walk_Backward_Startled");
	Walk_Backward_Caution = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward_Caution, "Walk_Backward_Caution");

	Inspect_Kneel = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Inspect_Kneel, "Inspect_Kneel");

	Push_Begin = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Push_Begin, "Push_Begin");
	Push = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Push, "Push");

	Damage_Front_Minor = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Damage_Front_Minor, "Damage_Front_Minor");
	Damage_Back = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Damage_Back, "Damage_Back");
	Damage_Front = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Damage_Front, "Damage_Front");

	Death = StateType(AnimationIndex::Normal, AnimStatePlayerBio2::Death, "Death");
}

void Global_Application::InitWeaponStateBio2(std::uintmax_t iWeapon)
{
	Idle = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Idle, "Idle");
	Idle_Pose_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Idle_Pose_Begin");
	Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Idle_Pose");
	Idle_Pose_Extra = StateType(AnimationIndex::Normal, AnimStateCustom::Dummy, "Idle_Pose_Extra");
	Idle_Caution = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Idle_Caution, "Idle_Caution");
	Idle_Danger = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Idle_Danger, "Idle_Danger");

	Walk_Forward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Walk_Forward, "Walk_Forward");
	Walk_Forward_Caution = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Walk_Forward_Caution, "Walk_Forward_Caution");
	Walk_Forward_Danger = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Walk_Forward_Danger, "Walk_Forward_Danger");

	Run = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Run, "Run");
	Run_Caution = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Run_Caution, "Run_Caution");
	Run_Danger = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Run_Danger, "Run_Danger");

	Aim_Begin = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Aim_Begin, "Aim_Begin");
	Aim_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Upward_Begin");
	Aim_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Downward_Begin");

	Aim = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim");
	Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Upward");
	Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Aim_Downward");

	Fire_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Begin");
	Fire_Upward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_Begin");
	Fire_Downward_Begin = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_Begin");

	Fire_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_End");
	Fire_Upward_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_End");
	Fire_Downward_End = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_End");

	Fire_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Empty");
	Fire_Upward_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward_Empty");
	Fire_Downward_Empty = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward_Empty");

	Fire = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire");
	Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Upward");
	Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Downward");

	Fire_Blocked = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Fire_Blocked");

	Reload = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Reload");

	if (iWeapon == 0x00)
	{
	}
	else if (iWeapon == 0x01)
	{
		Aim = StateType(AnimationIndex::Weapon, AnimStateW01Bio2::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2::Aim_Downward, "Aim_Downward");

		Fire_Blocked = StateType(AnimationIndex::Weapon, AnimStateW01Bio2::Fire_Blocked, "Fire_Blocked");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW01Bio2::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW01Bio2::Fire_Downward, "Fire_Downward");
	}
	else if (iWeapon == 0x0E)
	{
		Aim = StateType(AnimationIndex::Weapon, AnimStateW0EBio2::Aim, "Aim");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW0EBio2::Fire, "Fire");

		Reload = StateType(AnimationIndex::Weapon, AnimStateW0EBio2::Reload, "Reload");
	}
	else if (iWeapon == 0x0F || iWeapon == 0x10)
	{
		Aim = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Aim_Downward, "Aim_Downward");

		Fire_Empty = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Fire_Empty, "Fire_Empty");
		Fire_Upward_Empty = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Fire_Upward_Empty, "Fire_Upward_Empty");
		Fire_Downward_Empty = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Fire_Downward_Empty, "Fire_Downward_Empty");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Fire_Downward, "Fire_Downward");

		Reload = StateType(AnimationIndex::Weapon, AnimStateW0FBio2::Reload, "Reload");
	}
	else if (iWeapon == 0x11)
	{
		Aim = StateType(AnimationIndex::Weapon, AnimStateW11Bio2::Aim, "Aim");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW11Bio2::Fire, "Fire");
	}
	else if (iWeapon == 0x12)
	{
		Aim = StateType(AnimationIndex::Weapon, AnimStateW12Bio2::Aim, "Aim");

		Fire_Begin = StateType(AnimationIndex::Weapon, AnimStateW12Bio2::Fire_Begin, "Fire_Begin");
		Fire_End = StateType(AnimationIndex::Weapon, AnimStateW12Bio2::Fire_End, "Fire_End");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW12Bio2::Fire, "Fire");
	}
	else
	{
		Aim = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Aim, "Aim");
		Aim_Upward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Aim_Upward, "Aim_Upward");
		Aim_Downward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Aim_Downward, "Aim_Downward");

		Fire = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Fire, "Fire");
		Fire_Upward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Fire_Upward, "Fire_Upward");
		Fire_Downward = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Fire_Downward, "Fire_Downward");

		Reload = StateType(AnimationIndex::Weapon, AnimStateW00Bio2::Reload, "Reload");
	}

	if (iWeapon == 0x09 || iWeapon == 0x0A || iWeapon == 0x0B)
	{
		Reload = StateType(AnimationIndex::Weapon, AnimStateCustom::Dummy, "Reload");
	}

	if (iWeapon == 0x13)
	{
		Idle_Pose_Begin = StateType(AnimationIndex::Weapon, AnimStateW13Bio2::Idle_Pose_Begin, "Idle_Pose_Begin");
		Idle_Pose = StateType(AnimationIndex::Weapon, AnimStateW13Bio2::Idle_Pose, "Idle_Pose");
	}
}

void Global_Application::InitPlayerState(std::shared_ptr<Resident_Evil_Model> Model, std::unique_ptr<StateMachineType>& State)
{
	if (State)
	{
		State->Reset();
		State.reset();
	}

	if (!Model)
	{
		return;
	}

	if (Model->ModelGame() & (AUG95 | OCT95 | BIO1) && Model->WeaponModelGame() & (AUG95 | OCT95 | BIO1))
	{
		InitCommonStateBio1(Model->m_WeaponID);
		InitWeaponStateBio1(Model->m_WeaponID);
	}
	else if (Model->ModelGame() & (BIO2NOV96) && Model->WeaponModelGame() & (BIO2NOV96))
	{
		InitCommonStateBio2Nov96(Model->m_WeaponID);
		InitWeaponStateBio2Nov96(Model->m_WeaponID);
	}
	else if (Model->ModelGame() & (BIO2TRIAL | BIO2) && Model->WeaponModelGame() & (BIO2TRIAL | BIO2))
	{
		InitCommonStateBio2(Model->m_WeaponID);
		InitWeaponStateBio2(Model->m_WeaponID);
	}
	else
	{
		return;
	}

	State = std::make_unique<StateMachineType>(Model);

	// Idle
	{
		State->Add(Idle,
			[&]()
			{
				if ((State->KeyState() & LEFT) != Resident_Evil_Key{} || (State->KeyState() & RIGHT) != Resident_Evil_Key{})
				{
					State->Set(Idle_Turn);
				}
				else if (State->Model()->IsHealthDanger())
				{
					State->Init(Idle_Danger, 0, false, false, true);
				}
				else if (State->Model()->IsHealthCaution())
				{
					State->Init(Idle_Caution, 0, false, false, true);
				}
				else
				{
					State->Init(Idle, 0, false, false, true);
				}
			},
			[&]()
			{
				if ((State->KeyState() & AIM) != Resident_Evil_Key{})
				{
					State->Set(Aim_Begin);
				}
				else if ((State->KeyState() & DOWN) != Resident_Evil_Key{})
				{
					State->Set(Walk_Backward);
				}
				else if ((State->KeyState() & RUN) != Resident_Evil_Key{})
				{
					State->Set(Run);
				}
				else if ((State->KeyState() & UP) != Resident_Evil_Key{} && Walk_Forward.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
				{
					State->Set(Walk_Forward);
				}
				else if ((State->KeyState() & RIGHT) != Resident_Evil_Key{} || (State->KeyState() & LEFT) != Resident_Evil_Key{})
				{
					State->Set(Idle_Turn);
				}
			},
			[&]()
			{
			}
		);
	}

	// Idle-Turn
	{
		State->Add(Idle_Turn,
			[&]()
			{
				if (State->Model()->IsHealthDanger() || State->Model()->IsHealthCaution())
				{
					State->Init(Walk_Backward_Caution, 0, false, false, true);
				}
				else
				{
					State->Init(Walk_Backward, 0, false, false, true);
				}
				State->Model()->b_IdleTurn.store(true);
			},
			[&]()
			{
				if ((State->KeyState() & AIM) != Resident_Evil_Key{})
				{
					State->Set(Aim_Begin);
				}
				else if ((State->KeyState() & DOWN) != Resident_Evil_Key{})
				{
					State->Set(Walk_Backward);
				}
				else if ((State->KeyState() & UP) != Resident_Evil_Key{} && Walk_Forward.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
				{
					State->Set(Walk_Forward);
				}
				else if ((State->KeyState() & LEFT) != Resident_Evil_Key{} || (State->KeyState() & RIGHT) != Resident_Evil_Key{})
				{
				}
				else
				{
					State->Set(Idle);
				}
			},
			[&]()
			{
				State->Model()->b_IdleTurn.store(false);
			}
		);
	}

	// Quick-Turn
	{
		State->Add(Quick_Turn,
			[&]()
			{
				if (State->Model()->IsHealthDanger() || State->Model()->IsHealthCaution())
				{
					State->Init(Walk_Backward_Caution, 0, false, false, false);
				}
				else
				{
					State->Init(Walk_Backward, 0, false, false, false);
				}
				State->Model()->b_QuickTurn.store(true);
				State->Model()->m_QuickTurnRotation.store(2048);
			},
			[&]()
			{
				if (State->Model()->m_QuickTurnRotation.load())
				{
					State->Model()->m_QuickTurnRotation.fetch_sub(64);
					if (State->Model()->b_EditorMode)
					{
						State->Model()->EditorRotation().y += 64;
						State->Model()->ClampRotation(State->Model()->EditorRotation());
					}
					else
					{
						State->Model()->Rotation().y += 64;
						State->Model()->ClampRotation(State->Model()->Rotation());
					}
				}
				else if (State->Model()->m_QuickTurnRotation.load() <= 0)
				{
					State->Model()->b_QuickTurn.store(false);
					State->Model()->m_QuickTurnRotation.store(0);
					State->Set(Idle);
				}
			},
			[&]()
			{
			}
		);
	}

	// Quick-Reload
	{
		State->Add(Quick_Reload,
			[&]()
			{
				State->Init(Reload, 0, true, false, false);
			},
			[&]()
			{
				if (State->Model()->b_PlayAllFrames.load())
				{
					State->Model()->b_PlayInReverse.store(false);
				}
				else
				{
					State->Set(Aim);
				}
			},
			[&]()
			{
			}
		);
	}

	// Walk Backward
	{
		State->Add(Walk_Backward,
			[&]()
			{
				if (State->Model()->IsHealthDanger() || State->Model()->IsHealthCaution())
				{
					State->Init(Walk_Backward_Caution, 0, false, false, true);
				}
				else
				{
					State->Init(Walk_Backward, 0, false, false, true);
				}
			},
			[&]()
			{
				if ((State->KeyState() & AIM) != Resident_Evil_Key{})
				{
					State->Set(Aim_Begin);
				}
				else if ((State->KeyState() & RUN) != Resident_Evil_Key{})
				{
					State->Set(Quick_Turn);
				}
				else if (!((State->KeyState() & DOWN) != Resident_Evil_Key{}))
				{
					State->Set(Idle);
				}
			},
			[&]()
			{
			}
		);
	}

	// Walk Forward
	{
		State->Add(Walk_Forward,
			[&]()
			{
				if (State->Model()->IsHealthDanger())
				{
					State->Init(Walk_Forward_Danger, 0, false, false, true);
				}
				else if (State->Model()->IsHealthCaution())
				{
					State->Init(Walk_Forward_Caution, 0, false, false, true);
				}
				else
				{
					State->Init(Walk_Forward, 0, false, false, true);
				}
			},
			[&]()
			{
				if ((State->KeyState() & AIM) != Resident_Evil_Key{})
				{
					State->Set(Aim_Begin);
				}
				else if ((State->KeyState() & RUN) != Resident_Evil_Key{})
				{
					State->Set(Run);
				}
				else if (!((State->KeyState() & UP) != Resident_Evil_Key{}))
				{
					State->Set(Idle);
				}
			},
			[&]()
			{
			}
		);
	}

	// Run
	{
		State->Add(Run,
			[&]()
			{
				if (State->Model()->IsHealthDanger())
				{
					State->Init(Run_Danger, 0, false, false, true);
				}
				else if (State->Model()->IsHealthCaution())
				{
					State->Init(Run_Caution, 0, false, false, true);
				}
				else
				{
					State->Init(Run, 0, false, false, true);
				}
			},
			[&]()
			{
				if ((State->KeyState() & AIM) != Resident_Evil_Key{})
				{
					State->Set(Aim_Begin);
				}
				else if (!((State->KeyState() & RUN) != Resident_Evil_Key{}))
				{
					State->Set(Idle);
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim Begin
	{
		State->Add(Aim_Begin,
			[&]()
			{
				if (State->Prior() == Aim)
				{
					size_t m_FrameCount = State->Model()->Animation(Aim_Begin.GetIndex())->GetFrameCount(Aim_Begin.GetClip()) - 1;
					State->Init(Aim_Begin, m_FrameCount, true, true, false);
				}
				else
				{
					State->Init(Aim_Begin, 0, true, false, false);
				}
			},
			[&]()
			{
				const auto m_FrameCount = State->Model()->Animation(Aim_Begin.GetIndex())->GetFrameCount(Aim_Begin.GetClip());
				const auto b_Complete = (State->Model()->iFrame.load() >= (m_FrameCount ? m_FrameCount - 1 : 0));

				if (!((State->KeyState() & AIM) != Resident_Evil_Key{}))
				{
					if (State->Model()->b_PlayAllFrames.load())
					{
						State->Model()->b_PlayInReverse.store(true);
					}
					else
					{
						if (State->Model()->iFrame.load() == 0)
						{
							State->Set(Idle);
						}
						else
						{
							State->Model()->b_PlayAllFrames.store(true);
							State->Model()->b_PlayInReverse.store(true);
						}
					}
				}
				else if (State->Model()->b_PlayAllFrames.load())
				{
					State->Model()->b_PlayInReverse.store(false);
				}
				else if ((State->KeyState() & AIM) != Resident_Evil_Key{})
				{
					if (b_Complete)
					{
						State->Set(Aim);
					}
					else if (State->Model()->iFrame.load() == 0)
					{
						State->Init(Aim_Begin, 0, true, false, false);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim Upward Begin
	{
		State->Add(Aim_Upward_Begin,
			[&]()
			{
				if (State->Prior() == Aim_Upward || State->Prior() == Fire_Upward)
				{
					size_t m_FrameCount = State->Model()->Animation(Aim_Upward_Begin.GetIndex())->GetFrameCount(Aim_Upward_Begin.GetClip()) - 1;
					State->Init(Aim_Upward_Begin, m_FrameCount, true, true, false);
				}
				else
				{
					State->Init(Aim_Upward_Begin, 0, true, false, false);
				}
			},
			[&]()
			{
				const auto m_FrameCount = State->Model()->Animation(Aim_Upward_Begin.GetIndex())->GetFrameCount(Aim_Upward_Begin.GetClip());
				const auto b_Complete = (State->Model()->iFrame.load() >= (m_FrameCount ? m_FrameCount - 1 : 0));

				if (!((State->KeyState() & AIM) != Resident_Evil_Key{}) || !((State->KeyState() & UP) != Resident_Evil_Key{}))
				{
					if (State->Model()->b_PlayAllFrames.load())
					{
						State->Model()->b_PlayInReverse.store(true);
					}
					else
					{
						if (State->Model()->iFrame.load() == 0)
						{
							State->Set(Aim);
						}
						else
						{
							State->Model()->b_PlayAllFrames.store(true);
							State->Model()->b_PlayInReverse.store(true);
						}
					}
				}
				else if (State->Model()->b_PlayAllFrames.load())
				{
					State->Model()->b_PlayInReverse.store(false);
				}
				else if ((State->KeyState() & AIM) != Resident_Evil_Key{})
				{
					if (b_Complete)
					{
						State->Set(Aim_Upward);
					}
					else if (State->Model()->iFrame.load() == 0)
					{
						State->Init(Aim_Upward_Begin, 0, true, false, false);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim Downward Begin
	{
		State->Add(Aim_Downward_Begin,
			[&]()
			{
				if (State->Prior() == Aim_Downward || State->Prior() == Fire_Downward)
				{
					size_t m_FrameCount = State->Model()->Animation(Aim_Downward_Begin.GetIndex())->GetFrameCount(Aim_Downward_Begin.GetClip()) - 1;
					State->Init(Aim_Downward_Begin, m_FrameCount, true, true, false);
				}
				else
				{
					State->Init(Aim_Downward_Begin, 0, true, false, false);
				}
			},
			[&]()
			{
				const auto m_FrameCount = State->Model()->Animation(Aim_Downward_Begin.GetIndex())->GetFrameCount(Aim_Downward_Begin.GetClip());
				const auto b_Complete = (State->Model()->iFrame.load() >= (m_FrameCount ? m_FrameCount - 1 : 0));

				if (!((State->KeyState() & AIM) != Resident_Evil_Key{}) || !((State->KeyState() & DOWN) != Resident_Evil_Key{}))
				{
					if (State->Model()->b_PlayAllFrames.load())
					{
						State->Model()->b_PlayInReverse.store(true);
					}
					else
					{
						if (State->Model()->iFrame.load() == 0)
						{
							State->Set(Aim);
						}
						else
						{
							State->Model()->b_PlayAllFrames.store(true);
							State->Model()->b_PlayInReverse.store(true);
						}
					}
				}
				else if (State->Model()->b_PlayAllFrames.load())
				{
					State->Model()->b_PlayInReverse.store(false);
				}
				else if ((State->KeyState() & AIM) != Resident_Evil_Key{})
				{
					if (b_Complete)
					{
						State->Set(Aim_Downward);
					}
					else if (State->Model()->iFrame.load() == 0)
					{
						State->Init(Aim_Downward_Begin, 0, true, false, false);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim
	{
		State->Add(Aim,
			[&]()
			{
				State->Init(Aim, 0, false, false, false);
			},
			[&]()
			{
				if (!((State->KeyState() & AIM) != Resident_Evil_Key{}))
				{
					State->Set(Aim_Begin);
				}
				else if ((State->KeyState() & RUN) != Resident_Evil_Key{} && Reload.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
				{
					State->Set(Quick_Reload);
				}
				else if ((State->KeyState() & UP) != Resident_Evil_Key{} && Aim_Upward.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
				{
					if (Aim_Upward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Aim_Upward_Begin);
					}
					else
					{
						State->Set(Aim_Upward);
					}
				}
				else if ((State->KeyState() & DOWN) != Resident_Evil_Key{} && Aim_Downward.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
				{
					if (Aim_Downward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Aim_Downward_Begin);
					}
					else
					{
						State->Set(Aim_Downward);
					}
				}
				else if ((State->KeyState() & AIM_FIRE) != Resident_Evil_Key{})
				{
					if (Fire_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_Begin);
					}
					else
					{
						State->Set(Fire);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim Upward
	{
		State->Add(Aim_Upward,
			[&]()
			{
				State->Init(Aim_Upward, 0, false, false, false);
			},
			[&]()
			{
				if (!((State->KeyState() & UP) != Resident_Evil_Key{}) || !((State->KeyState() & AIM) != Resident_Evil_Key{}))
				{
					if (Aim_Upward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Aim_Upward_Begin);
					}
					else
					{
						State->Set(Aim);
					}
				}
				else if ((State->KeyState() & AIM_FIRE) != Resident_Evil_Key{})
				{
					if (Fire_Upward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_Upward_Begin);
					}
					else
					{
						State->Set(Fire_Upward);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim Downward
	{
		State->Add(Aim_Downward,
			[&]()
			{
				State->Init(Aim_Downward, 0, false, false, false);
			},
			[&]()
			{
				if (!((State->KeyState() & DOWN) != Resident_Evil_Key{}) || !((State->KeyState() & AIM) != Resident_Evil_Key{}))
				{
					if (Aim_Downward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Aim_Downward_Begin);
					}
					else
					{
						State->Set(Aim);
					}
				}
				else if ((State->KeyState() & AIM_FIRE) != Resident_Evil_Key{})
				{
					if (Fire_Downward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_Downward_Begin);
					}
					else
					{
						State->Set(Fire_Downward);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Begin
	{
		State->Add(Fire_Begin,
			[&]()
			{
				if (State->Prior() == Fire)
				{
					size_t m_FrameCount = State->Model()->Animation(Fire_Begin.GetIndex())->GetFrameCount(Fire_Begin.GetClip()) - 1;
					State->Init(Fire_Begin, m_FrameCount, true, true, false);
				}
				else
				{
					State->Init(Fire_Begin, 0, true, false, false);
				}
			},
			[&]()
			{
				const auto m_FrameCount = State->Model()->Animation(Fire_Begin.GetIndex())->GetFrameCount(Fire_Begin.GetClip());
				const auto b_Complete = (State->Model()->iFrame.load() >= (m_FrameCount ? m_FrameCount - 1 : 0));

				if (!State->Model()->b_PlayAllFrames.load())
				{
					if (b_Complete)
					{
						State->Set(Fire);
					}
					else if (State->Model()->iFrame.load() == 0)
					{
						State->Set(Aim);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Upward Begin
	{
		State->Add(Fire_Upward_Begin,
			[&]()
			{
				if (State->Prior() == Fire_Upward)
				{
					size_t m_FrameCount = State->Model()->Animation(Fire_Upward_Begin.GetIndex())->GetFrameCount(Fire_Upward_Begin.GetClip()) - 1;
					State->Init(Fire_Upward_Begin, m_FrameCount, true, true, false);
				}
				else
				{
					State->Init(Fire_Upward_Begin, 0, true, false, false);
				}
			},
			[&]()
			{
				const auto m_FrameCount = State->Model()->Animation(Fire_Upward_Begin.GetIndex())->GetFrameCount(Fire_Upward_Begin.GetClip());
				const auto b_Complete = (State->Model()->iFrame.load() >= (m_FrameCount ? m_FrameCount - 1 : 0));

				if (!State->Model()->b_PlayAllFrames.load())
				{
					if (b_Complete)
					{
						State->Set(Fire_Upward);
					}
					else if (State->Model()->iFrame.load() == 0)
					{
						State->Set(Aim_Upward);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Downward Begin
	{
		State->Add(Fire_Downward_Begin,
			[&]()
			{
				if (State->Prior() == Fire_Downward)
				{
					size_t m_FrameCount = State->Model()->Animation(Fire_Downward_Begin.GetIndex())->GetFrameCount(Fire_Downward_Begin.GetClip()) - 1;
					State->Init(Fire_Downward_Begin, m_FrameCount, true, true, false);
				}
				else
				{
					State->Init(Fire_Downward_Begin, 0, true, false, false);
				}
			},
			[&]()
			{
				const auto m_FrameCount = State->Model()->Animation(Fire_Downward_Begin.GetIndex())->GetFrameCount(Fire_Downward_Begin.GetClip());
				const auto b_Complete = (State->Model()->iFrame.load() >= (m_FrameCount ? m_FrameCount - 1 : 0));

				if (!State->Model()->b_PlayAllFrames.load())
				{
					if (b_Complete)
					{
						State->Set(Fire_Downward);
					}
					else if (State->Model()->iFrame.load() == 0)
					{
						State->Set(Aim_Downward);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire End
	{
		State->Add(Fire_End,
			[&]()
			{
				State->Init(Fire_End, 0, true, false, false);
			},
			[&]()
			{
				if (!State->Model()->b_PlayAllFrames.load())
				{
					State->Set(Aim);
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Upward End
	{
		State->Add(Fire_Upward_End,
			[&]()
			{
				State->Init(Fire_Upward_End, 0, true, false, false);
			},
			[&]()
			{
				if (!State->Model()->b_PlayAllFrames.load())
				{
					State->Set(Aim_Upward);
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Downward End
	{
		State->Add(Fire_Downward_End,
			[&]()
			{
				State->Init(Fire_Downward_End, 0, true, false, false);
			},
			[&]()
			{
				if (!State->Model()->b_PlayAllFrames.load())
				{
					State->Set(Aim_Downward);
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire
	{
		State->Add(Fire,
			[&]()
			{
				State->Init(Fire, 0, true, false, false);
				State->Model()->b_WeaponKickbackComplete.store(false);
			},
			[&]()
			{
				if (State->Model()->b_PlayAllFrames.load())
				{
					State->Model()->b_WeaponKickbackComplete.store(true);
				}
				else if (Fire_End.GetClip() != std::to_underlying(AnimStateCustom::Dummy) || Fire_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
				{
					if ((State->KeyState() & UP) != Resident_Evil_Key{} || (State->KeyState() & DOWN) != Resident_Evil_Key{})
					{
						if (Fire_End.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
						{
							State->Set(Fire_End);
						}
						else if (Fire_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
						{
							State->Set(Fire_Begin);
						}
					}
					else if ((State->KeyState() & AIM_FIRE) != Resident_Evil_Key{})
					{
						State->Set(Fire);
						State->Model()->b_Loop.store(true);
					}
					else if (Fire_End.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_End);
					}
					else if (Fire_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_Begin);
					}
				}
				else
				{
					State->Set(Aim);
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Upward
	{
		State->Add(Fire_Upward,
			[&]()
			{
				State->Init(Fire_Upward, 0, true, false, false);
				State->Model()->b_WeaponKickbackComplete.store(false);
			},
			[&]()
			{
				if (State->Model()->b_PlayAllFrames.load())
				{
					State->Model()->b_WeaponKickbackComplete.store(true);
				}
				else if (Fire_Upward_End.GetClip() != std::to_underlying(AnimStateCustom::Dummy) || Fire_Upward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
				{
					if ((State->KeyState() & UP) != Resident_Evil_Key{} && (State->KeyState() & AIM_FIRE) != Resident_Evil_Key{})
					{
						State->Set(Fire_Upward);
						State->Model()->b_Loop.store(true);
					}
					else if (Fire_Upward_End.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_Upward_End);
					}
					else if (Fire_Upward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_Upward_Begin);
					}
				}
				else
				{
					State->Set(Aim_Upward);
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Downward
	{
		State->Add(Fire_Downward,
			[&]()
			{
				State->Init(Fire_Downward, 0, true, false, false);
				State->Model()->b_WeaponKickbackComplete.store(false);
			},
			[&]()
			{
				if (State->Model()->b_PlayAllFrames.load())
				{
					State->Model()->b_WeaponKickbackComplete.store(true);
				}
				else if (Fire_Downward_End.GetClip() != std::to_underlying(AnimStateCustom::Dummy) || Fire_Downward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
				{
					if ((State->KeyState() & DOWN) != Resident_Evil_Key{} && (State->KeyState() & AIM_FIRE) != Resident_Evil_Key{})
					{
						State->Set(Fire_Downward);
						State->Model()->b_Loop.store(true);
					}
					else if (Fire_Downward_End.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_Downward_End);
					}
					else if (Fire_Downward_Begin.GetClip() != std::to_underlying(AnimStateCustom::Dummy))
					{
						State->Set(Fire_Downward_Begin);
					}
				}
				else
				{
					State->Set(Aim_Downward);
				}
			},
			[&]()
			{
			}
		);
	}

	State->Set(Idle);
}

void Global_Application::ControllerInput(std::unique_ptr<StateMachineType>& State)
{
	if (!State || !State->Model()->b_ControllerMode) { return; }

	Resident_Evil_Key m_KeyState = NONE;

	if (!Gamepad->PollState())
	{
		State->Update(m_KeyState, false);
		return;
	}

	VECTOR2& Rotation = State->Model()->b_EditorMode ? State->Model()->EditorRotation() : State->Model()->Rotation();

	bool b_QuickTurn = (State->Current() == Quick_Turn);
	bool b_Reloading = (State->Current() == Reload || State->Current() == Quick_Reload);
	bool b_AimBegin = (State->Current() == Aim_Begin || (State->Current() == Aim_Upward_Begin) || (State->Current() == Aim_Downward_Begin));
	bool b_Aiming = (State->Current() == Aim) || (State->Current() == Aim_Upward) || (State->Current() == Aim_Downward);
	bool b_Firing = (State->Current() == Fire) || (State->Current() == Fire_Upward) || (State->Current() == Fire_Downward);

	bool b_Up = Gamepad->IsPressed(Gamepad->Map().Up);
	bool b_Right = Gamepad->IsPressed(Gamepad->Map().Right);
	bool b_Down = Gamepad->IsPressed(Gamepad->Map().Down);
	bool b_Left = Gamepad->IsPressed(Gamepad->Map().Left);
	bool b_Cross = Gamepad->IsPressed(Gamepad->Map().Cross);
	bool b_Square = Gamepad->IsPressed(Gamepad->Map().Square);
	bool b_R1 = Gamepad->IsPressed(Gamepad->Map().R1);

	if (Aim_Begin.GetClip() == std::to_underlying(AnimStateCustom::Dummy))
	{
		b_R1 = false;
	}

	if (b_Right) { m_KeyState |= RIGHT; }
	else if (b_Left) { m_KeyState |= LEFT; }
	if (b_Up) { m_KeyState |= UP; }
	else if (b_Down) { m_KeyState |= DOWN; }

	if (b_Right || b_Left)
	{
		if (b_Reloading)
		{
		}

		else if (b_AimBegin || b_Aiming || b_Firing) { Rotation.y += (b_Right ? 8 : -8); }

		else { Rotation.y += (b_Right ? 16 : -16); }

		State->Model()->ClampRotation(Rotation);
	}

	if (b_R1)
	{
		m_KeyState |= AIM;

		if (b_Aiming || b_Firing)
		{
			if (b_Cross) { m_KeyState |= AIM_FIRE; }
			else if (b_Square) { m_KeyState |= RUN; }
		}
	}
	else if (b_Up || b_Down)
	{
		if (State->Model()->ModelGame() & (AUG95 | OCT95 | BIO1 | BIO2NOV96))
		{
			if (b_Up && !b_Square && Walk_Forward.GetClip() == std::to_underlying(AnimStateCustom::Dummy)) // bio2 nov '96 is such a mess...
			{
				m_KeyState = NONE;
			}

			static float WalkSpeed = 0.2150f;
			float RunSpeed = WalkSpeed * 1.46f;

			if (State->Model()->ModelGame() & BIO2NOV96)
			{
				RunSpeed = WalkSpeed * 1.58f;
			}

			auto CalcSpeed = [](Resident_Evil_Model& Model, const StateType& State, float Distance) -> std::int16_t
				{
					if (State.GetClip() == std::to_underlying(AnimStateCustom::Dummy))
					{
						return 0;
					}

					std::size_t nFrames = Model.Animation(State.GetIndex())->GetFrameCount(State.GetClip());

					if (nFrames) { nFrames -= 1; }

					if (nFrames <= 0)
					{
						return static_cast<std::int16_t>(Distance * 4096.0f / 60.0f);
					}

					float Speed = (Distance / nFrames) * 4096.0f;

					return static_cast<std::int16_t>(std::round(Speed));
				};

			if (b_Up || b_Down)
			{
				std::int16_t Speed = CalcSpeed(*State->Model(), Walk_Forward, WalkSpeed);
				State->Model()->Speed().x += (b_Up ? -Speed : Speed);
			}

			if (b_Square)
			{
				if (b_Up || b_Down)
				{
					std::int16_t Speed = CalcSpeed(*State->Model(), Run, RunSpeed);
					State->Model()->Speed().x += (b_Up ? -Speed : Speed);
				}

				m_KeyState |= RUN;
			}
		}

		else if (b_Square)
		{
			m_KeyState |= RUN;
		}
	}

	bool b_UpdateTransform = !(m_KeyState == NONE || State->Model()->b_EditorMode || State->Model()->b_IdleTurn.load() || b_AimBegin || b_Aiming || b_QuickTurn || b_Reloading);

	if (b_Firing && (!State->Model()->b_WeaponKickback.load() || State->Model()->b_WeaponKickbackComplete.load()))
	{
		b_UpdateTransform = false;
	}

	State->Update(m_KeyState, b_UpdateTransform);
}