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

void Global_Application::InitPlayerStateBio2(std::shared_ptr<Resident_Evil_Model> iModel, std::unique_ptr<StateMachineVariant>& iStateMachineVariant)
{
	iStateMachineVariant = std::make_unique<StateMachineVariant>(StateMachine<AnimStatePlayerBio2>(iModel));

	auto& State = std::get<StateMachine<AnimStatePlayerBio2>>(*iStateMachineVariant);

	// Idle-Turn
	{
		State.Add(AnimationIndex::Normal, AnimStatePlayerBio2::Idle_Turn,
			[&]()
			{
				if (State.Model()->IsHealthDanger() || State.Model()->IsHealthCaution())
				{
					State.Init(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward_Caution, 0, false, false, true);
				}
				else
				{
					State.Init(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward, 0, false, false, true);
				}
				State.Model()->b_IdleTurn.store(true);
			},
			[&]()
			{
				if ((State.KeyState() & AIM) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin);
				}
				else if ((State.KeyState() & DOWN) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward);
				}
				else if ((State.KeyState() & UP) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Walk_Forward);
				}
				else if ((State.KeyState() & LEFT) != Resident_Evil_Key{} || (State.KeyState() & RIGHT) != Resident_Evil_Key{})
				{
				}
				else
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle);
				}
			},
			[&]()
			{
				State.Model()->b_IdleTurn.store(false);
			}
		);
	}

	// Quick-Turn
	{
		State.Add(AnimationIndex::Normal, AnimStatePlayerBio2::Quick_Turn,
			[&]()
			{
				if (State.Model()->IsHealthDanger() || State.Model()->IsHealthCaution())
				{
					State.Init(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward_Caution, 0, false, false, false);
				}
				else
				{
					State.Init(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward, 0, false, false, false);
				}
				State.Model()->b_QuickTurn.store(true);
				State.Model()->m_QuickTurnRotation.store(2048);
			},
			[&]()
			{
				if (State.Model()->m_QuickTurnRotation.load())
				{
					State.Model()->m_QuickTurnRotation.fetch_sub(64);
					State.Model()->Rotation().y += 64;
					State.Model()->ClampRotation(State.Model()->Rotation());
				}
				else if (State.Model()->m_QuickTurnRotation.load() <= 0)
				{
					State.Model()->b_QuickTurn.store(false);
					State.Model()->m_QuickTurnRotation.store(0);
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle);
				}
			},
			[&]()
			{
			}
		);
	}

	// Quick-Reload
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Quick_Reload,
			[&]()
			{
				State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Reload, 0, true, false, false);
			},
			[&]()
			{
				if (State.Model()->b_PlayAllFrames.load())
				{
					State.Model()->b_PlayInReverse.store(false);
				}
				else
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
				}
			},
			[&]()
			{
			}
		);
	}

	// Walk Backward
	{
		State.Add(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward,
			[&]()
			{
				if (State.Model()->IsHealthDanger() || State.Model()->IsHealthCaution())
				{
					State.Init(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward_Caution, 0, false, false, true);
				}
				else
				{
					State.Init(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward, 0, false, false, true);
				}
			},
			[&]()
			{
				if ((State.KeyState() & AIM) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin);
				}
				else if ((State.KeyState() & RUN) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Normal, AnimStatePlayerBio2::Quick_Turn);
				}
				else if (!((State.KeyState() & DOWN) != Resident_Evil_Key{}))
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle);
				}
			},
			[&]()
			{
			}
		);
	}

	// Walk Forward
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Walk_Forward,
			[&]()
			{
				if (State.Model()->IsHealthDanger())
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Walk_Forward_Danger, 0, false, false, true);
				}
				else if (State.Model()->IsHealthCaution())
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Walk_Forward_Caution, 0, false, false, true);
				}
				else
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Walk_Forward, 0, false, false, true);
				}
			},
			[&]()
			{
				if ((State.KeyState() & AIM) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin);
				}
				else if ((State.KeyState() & RUN) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Run);
				}
				else if (!((State.KeyState() & UP) != Resident_Evil_Key{}))
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle);
				}
			},
			[&]()
			{
			}
		);
	}

	// Run
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Run,
			[&]()
			{
				if (State.Model()->IsHealthDanger())
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Run_Danger, 0, false, false, true);
				}
				else if (State.Model()->IsHealthCaution())
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Run_Caution, 0, false, false, true);
				}
				else
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Run, 0, false, false, true);
				}
			},
			[&]()
			{
				if ((State.KeyState() & AIM) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin);
				}
				else if (!((State.KeyState() & RUN) != Resident_Evil_Key{}))
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle);
				}
			},
			[&]()
			{
			}
		);
	}

	// Idle
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle,
			[&]()
			{
				if ((State.KeyState() & LEFT) != Resident_Evil_Key{} || (State.KeyState() & RIGHT) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Normal, AnimStatePlayerBio2::Idle_Turn);
				}
				else if (State.Model()->IsHealthDanger())
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle_Danger, 0, false, false, true);
				}
				else if (State.Model()->IsHealthCaution())
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle_Caution, 0, false, false, true);
				}
				else
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle, 0, false, false, true);
				}
			},
			[&]()
			{
				if ((State.KeyState() & AIM) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin);
				}
				else if ((State.KeyState() & DOWN) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Normal, AnimStatePlayerBio2::Walk_Backward);
				}
				else if ((State.KeyState() & RUN) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Run);
				}
				else if ((State.KeyState() & UP) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Walk_Forward);
				}
				else if ((State.KeyState() & RIGHT) != Resident_Evil_Key{} || (State.KeyState() & LEFT) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Normal, AnimStatePlayerBio2::Idle_Turn);
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim Begin
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin,
			[&]()
			{
				if (State.Prior() == AnimStatePlayerBio2::Aim)
				{
					size_t m_FrameCount = State.Model()->Animation(AnimationIndex::Weapon)->GetFrameCount(std::to_underlying(AnimStatePlayerBio2::Aim_Begin)) - 1;
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin, m_FrameCount, true, true, false);
				}
				else
				{
					State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin, 0, true, false, false);
				}
			},
			[&]()
			{
				const auto m_FrameCount = State.Model()->Animation(State.Model()->AnimIndex())->GetFrameCount(State.Model()->iClip.load());
				const auto b_Complete = (State.Model()->iFrame.load() >= (m_FrameCount ? m_FrameCount - 1 : 0));

				if (!((State.KeyState() & AIM) != Resident_Evil_Key{}))
				{
					if (State.Model()->b_PlayAllFrames.load())
					{
						State.Model()->b_PlayInReverse.store(true);
					}
					else
					{
						if (State.Model()->iFrame.load() == 0)
						{
							State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle);
						}
						else
						{
							State.Model()->b_PlayAllFrames.store(true);
							State.Model()->b_PlayInReverse.store(true);
						}
					}
				}
				else if (State.Model()->b_PlayAllFrames.load())
				{
					State.Model()->b_PlayInReverse.store(false);
				}
				else if ((State.KeyState() & AIM) != Resident_Evil_Key{})
				{
					if (b_Complete)
					{
						State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
					}
					else if (State.Model()->iFrame.load() == 0)
					{
						State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin, 0, true, false, false);
					}
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire,
			[&]()
			{
				State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire, 0, true, false, false);
				State.Model()->b_WeaponKickbackComplete.store(false);
			},
			[&]()
			{
				if (State.Model()->b_PlayAllFrames.load())
				{
					State.Model()->b_PlayInReverse.store(false);
					State.Model()->b_WeaponKickbackComplete.store(true);
				}
				else
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim,
			[&]()
			{
				State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim, 0, false, false, false);
			},
			[&]()
			{
				if (!((State.KeyState() & AIM) != Resident_Evil_Key{}))
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Begin);
				}
				else if ((State.KeyState() & RUN) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Quick_Reload);
				}
				else if ((State.KeyState() & UP) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Upward);
				}
				else if ((State.KeyState() & DOWN) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Downward);
				}
				else if ((State.KeyState() & AIM_FIRE) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire);
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Upward
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire_Upward,
			[&]()
			{
				State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire_Upward, 0, true, false, false);
				State.Model()->b_WeaponKickbackComplete.store(false);
			},
			[&]()
			{
				if (State.Model()->b_PlayAllFrames.load())
				{
					State.Model()->b_PlayInReverse.store(false);
					State.Model()->b_WeaponKickbackComplete.store(true);
				}
				else if (!((State.KeyState() & UP) != Resident_Evil_Key{}) || (State.KeyState() & DOWN) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
				}
				else
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Upward);
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim Upward
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Upward,
			[&]()
			{
				State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Upward, 0, false, false, false);
			},
			[&]()
			{
				if (!((State.KeyState() & AIM) != Resident_Evil_Key{}))
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
				}
				else if (!((State.KeyState() & UP) != Resident_Evil_Key{}))
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
				}
				else if ((State.KeyState() & AIM_FIRE) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire_Upward);
				}
			},
			[&]()
			{
			}
		);
	}

	// Fire Downward
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire_Downward,
			[&]()
			{
				State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire_Downward, 0, true, false, false);
				State.Model()->b_WeaponKickbackComplete.store(false);
			},
			[&]()
			{
				if (State.Model()->b_PlayAllFrames.load())
				{
					State.Model()->b_PlayInReverse.store(false);
					State.Model()->b_WeaponKickbackComplete.store(true);
				}
				else if (!((State.KeyState() & DOWN) != Resident_Evil_Key{}) || (State.KeyState() & UP) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
				}
				else
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Downward);
				}
			},
			[&]()
			{
			}
		);
	}

	// Aim Downward
	{
		State.Add(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Downward,
			[&]()
			{
				State.Init(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim_Downward, 0, false, false, false);
			},
			[&]()
			{
				if (!((State.KeyState() & AIM) != Resident_Evil_Key{}))
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
				}
				else if (!((State.KeyState() & DOWN) != Resident_Evil_Key{}))
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Aim);
				}
				else if ((State.KeyState() & AIM_FIRE) != Resident_Evil_Key{})
				{
					State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Fire_Downward);
				}
			},
			[&]()
			{
			}
		);
	}

	State.Set(AnimationIndex::Weapon, AnimStatePlayerBio2::Idle);
}

void Global_Application::ControllerInputBio2(std::unique_ptr<StateMachineVariant>& iStateMachineVariant)
{
	if (!std::holds_alternative<StateMachine<AnimStatePlayerBio2>>(*iStateMachineVariant)) { return; }

	auto& State = std::get<StateMachine<AnimStatePlayerBio2>>(*iStateMachineVariant);

	if (!State.Model()->b_ControllerMode) { return; }

	Resident_Evil_Key m_KeyState = NONE;

	if (!Gamepad->PollState())
	{
		State.Update(m_KeyState, false);
		return;
	}

	VECTOR2& Rotation = State.Model()->b_EditorMode ? State.Model()->EditorRotation() : State.Model()->Rotation();

	bool b_AimBegin = (State.Current() == AnimStatePlayerBio2::Aim_Begin);
	bool b_QuickTurn = (State.Current() == AnimStatePlayerBio2::Quick_Turn);
	bool b_Reloading = (State.Current() == AnimStatePlayerBio2::Reload || State.Current() == AnimStatePlayerBio2::Quick_Reload);
	bool b_Aiming = (State.Current() == AnimStatePlayerBio2::Aim) || (State.Current() == AnimStatePlayerBio2::Aim_Upward) || (State.Current() == AnimStatePlayerBio2::Aim_Downward);
	bool b_Firing = (State.Current() == AnimStatePlayerBio2::Fire) || (State.Current() == AnimStatePlayerBio2::Fire_Upward) || (State.Current() == AnimStatePlayerBio2::Fire_Downward);

	bool b_Up = Gamepad->IsPressed(Gamepad->Map().Up);
	bool b_Right = Gamepad->IsPressed(Gamepad->Map().Right);
	bool b_Down = Gamepad->IsPressed(Gamepad->Map().Down);
	bool b_Left = Gamepad->IsPressed(Gamepad->Map().Left);
	bool b_Cross = Gamepad->IsPressed(Gamepad->Map().Cross);
	bool b_Square = Gamepad->IsPressed(Gamepad->Map().Square);
	bool b_R1 = Gamepad->IsPressed(Gamepad->Map().R1);

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

		State.Model()->ClampRotation(Rotation);
	}

	if (b_R1)
	{
		m_KeyState |= AIM;

		if (b_Aiming)
		{
			if (b_Cross) { m_KeyState |= AIM_FIRE; }
			else if (b_Square) { m_KeyState |= RUN; }
		}
	}
	else if (b_Up || b_Down)
	{
		if (b_Square)
		{
			m_KeyState |= RUN;
		}
	}

	bool b_UpdateTransform = !(m_KeyState == NONE || State.Model()->b_EditorMode || State.Model()->b_IdleTurn.load() || b_AimBegin || b_Aiming || b_QuickTurn || b_Reloading);

	if (b_Firing && (!State.Model()->b_WeaponKickback.load() || State.Model()->b_WeaponKickbackComplete.load()))
	{
		b_UpdateTransform = false;
	}

	State.Update(m_KeyState, b_UpdateTransform);
}