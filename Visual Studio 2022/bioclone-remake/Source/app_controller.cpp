/*
*
*	Megan Grass
*	August 15, 2025
*
*/

#include "app.h"

void Global_Application::ControllerMapping(void)
{
	std::function<void(const std::string&)> SetMapping = [&](const std::string& Button)
		{
			auto This = std::shared_ptr<Global_Application>(this, [](Global_Application*) {});

			auto ThreadWork = std::thread([This, Button]()
				{
					This->b_ControllerMapping.store(true);

					std::function<void(bool&)> Callback = [&](bool& b_Status) -> void { This->b_ControllerMapping.store(b_Status); };

					This->Gamepad->SetMapping(Button, Callback);
				});

			ThreadWork.detach();
		};

	auto GetButtonStr = [this](const std::string& ButtonStr)
		{
			if (Gamepad->GetTriggerMapL(ButtonStr)) { return Standard_String().FormatCStyle("L##%s", ButtonStr.c_str()); }
			if (Gamepad->GetTriggerMapR(ButtonStr)) { return Standard_String().FormatCStyle("R##%s", ButtonStr.c_str()); }

			uint16_t Button = Gamepad->GetButtonMapBit(ButtonStr);
			if (Button == 0xFFFF) { return Standard_String().FormatCStyle("---##%s", ButtonStr.c_str()); }
			return Standard_String().FormatCStyle("%d##%s", Button, ButtonStr.c_str());
		};

	if (!ImGui::IsPopupOpen("Controller##ControllerMapping"))
	{
		ImGui::OpenPopup("Controller##ControllerMapping");
	}

	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) / 2, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) / 3));

	if (ImGui::BeginPopupModal("Controller##ControllerMapping", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		if (ImGui::BeginTable("MapPanel##ControllerMapping", 2))
		{
			ImGui::TableNextColumn(); ImGui::Text("Aim"); ImGui::TableNextColumn();
			if (ImGui::Button(GetButtonStr("Aim").c_str(), ImVec2(100, ImGui::GetFrameHeight()))) { SetMapping("Aim"); }

			ImGui::TableNextColumn(); ImGui::Text("Cancel"); ImGui::TableNextColumn();
			if (ImGui::Button(GetButtonStr("Cancel").c_str(), ImVec2(100, ImGui::GetFrameHeight()))) { SetMapping("Cancel"); }

			ImGui::TableNextColumn(); ImGui::Text("Action"); ImGui::TableNextColumn();
			if (ImGui::Button(GetButtonStr("Action").c_str(), ImVec2(100, ImGui::GetFrameHeight()))) { SetMapping("Action"); }

			ImGui::TableNextColumn(); ImGui::Text("Run"); ImGui::TableNextColumn();
			if (ImGui::Button(GetButtonStr("Run").c_str(), ImVec2(100, ImGui::GetFrameHeight()))) { SetMapping("Run"); }

			ImGui::TableNextColumn(); ImGui::Text("Forward"); ImGui::TableNextColumn();
			if (ImGui::Button(GetButtonStr("Forward").c_str(), ImVec2(100, ImGui::GetFrameHeight()))) { SetMapping("Forward"); }

			ImGui::TableNextColumn(); ImGui::Text("Backward"); ImGui::TableNextColumn();
			if (ImGui::Button(GetButtonStr("Backward").c_str(), ImVec2(100, ImGui::GetFrameHeight()))) { SetMapping("Backward"); }

			ImGui::TableNextColumn(); ImGui::Text("Left"); ImGui::TableNextColumn();
			if (ImGui::Button(GetButtonStr("Left").c_str(), ImVec2(100, ImGui::GetFrameHeight()))) { SetMapping("Left"); }

			ImGui::TableNextColumn(); ImGui::Text("Right"); ImGui::TableNextColumn();
			if (ImGui::Button(GetButtonStr("Right").c_str(), ImVec2(100, ImGui::GetFrameHeight()))) { SetMapping("Right"); }

			ImGui::EndTable();
		}

		{
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200) / 2);
			if (ImGui::Button("OK##ControllerMapping", ImVec2(200, 40)))
			{
				b_ControllerMapping.store(false);
				Modal = []() {};
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

		ImGui::EndPopup();
	}
}

void Global_Application::ControllerInput(VECTOR2& Rotation)
{
	if (!Player->b_ControllerMode) { return; }

	if (Player->b_QuickTurn.load())
	{
		if (Player->m_QuickTurnRotation.load() <= 0)
		{
			Player->b_QuickTurn.store(false);
			Player->m_QuickTurnRotation.store(0);
			Player->SetState(Bio2PlayerState::Idle, AnimationIndex::Weapon, Player->iFrame.load(), false, true);
		}
		else
		{
			Player->m_QuickTurnRotation -= 64;
			Rotation.y += 64;
			Player->b_EditorMode ? Player->ClampEditorRotation() : Player->ClampRotation();
			return;
		}
	}

	bool b_AimBegin = (Player->State() == Bio2PlayerState::Aim_Begin);
	bool b_Firing = (Player->State() == Bio2PlayerState::Fire) || (Player->State() == Bio2PlayerState::Fire_Upward) || (Player->State() == Bio2PlayerState::Fire_Downward);
	bool b_Aiming = (Player->State() == Bio2PlayerState::Aim) || (Player->State() == Bio2PlayerState::Aim_Upward) || (Player->State() == Bio2PlayerState::Aim_Downward);

	bool b_Running = (Player->State() == Bio2PlayerState::Run) || (Player->State() == Bio2PlayerState::Run_Caution) || (Player->State() == Bio2PlayerState::Run_Danger);

	if (!Gamepad->XInput()->PollState())
	{
		if (b_Firing)
		{
			Player->SetNextState(Bio2PlayerState::Aim_Begin, AnimationIndex::Weapon);
		}
		else if (b_Aiming)
		{
			Player->b_Reverse.store(true);
			Player->b_PlayAllFrames.store(true);
			Player->b_WaitComplete.store(true);
			Player->SetState(Bio2PlayerState::Aim_Begin, AnimationIndex::Weapon, 100, true, false);
		}
		else if (b_AimBegin)
		{
			Player->b_Reverse.store(true);
			Player->b_PlayAllFrames.store(true);
			Player->b_WaitComplete.store(true);
			Player->SetNextState(Bio2PlayerState::Idle, AnimationIndex::Weapon);
		}
		else
		{
			Player->SetState(Bio2PlayerState::Idle, AnimationIndex::Weapon, Player->iFrame.load(), false, true);
		}
		return;
	}

	b_Aiming |= b_Firing;

	std::uint16_t Button = Gamepad->XInput()->Gamepad().wButtons;
	bool b_TriggerL = Gamepad->XInput()->Gamepad().bLeftTrigger > 0;
	bool b_TriggerR = Gamepad->XInput()->Gamepad().bRightTrigger > 0;

	bool b_R1 = Gamepad->Map().R1.Button == 0xFFFF ? false : Button & Gamepad->Map().R1.Button;
	Gamepad->Map().R1.IsLeftTrigger ? b_R1 |= b_TriggerL : 0;
	Gamepad->Map().R1.IsRightTrigger ? b_R1 |= b_TriggerR : 0;

	bool b_Circle = (Button & Gamepad->Map().Circle.Button);
	bool b_Cross = (Button & Gamepad->Map().Cross.Button);
	bool b_Square = (Button & Gamepad->Map().Square.Button);

	bool b_Up = (Button & Gamepad->Map().Up.Button);
	bool b_Right = (Button & Gamepad->Map().Right.Button);
	bool b_Down = (Button & Gamepad->Map().Down.Button);
	bool b_Left = (Button & Gamepad->Map().Left.Button);

	bool b_IdleTurn = false;

	if (b_Right || b_Left)
	{
		if (b_AimBegin || b_Aiming || b_Firing) { Rotation.y += (b_Right ? 12 : -12); }

		else if (b_Running) { Rotation.y += (b_Right ? 14 : -14); }

		else { Rotation.y += (b_Right ? 12 : -12); }

		Player->ClampRotation();
	}

	if (b_R1)
	{
		if (b_AimBegin || b_Firing)
		{
		}
		else if (b_Cross && (b_Aiming || b_Firing))
		{
			Player->SetState(Bio2PlayerState::Fire, AnimationIndex::Weapon, Player->iFrame.load(), true, true);
			Player->b_PlayAllFrames.store(true);
			Player->b_WaitComplete.store(true);
		}
		else if (b_Aiming)
		{
			Player->SetState(Bio2PlayerState::Aim, AnimationIndex::Weapon, Player->iFrame.load(), false, true);
		}
		else if (!b_Aiming || !b_AimBegin)
		{
			Player->SetState(Bio2PlayerState::Aim_Begin, AnimationIndex::Weapon, 0, true, false);
			Player->SetNextState(Bio2PlayerState::Aim, AnimationIndex::Weapon);
		}
	}
	else if (b_Up)
	{
		if (b_Square)
		{
			Player->ResetFrame(Bio2PlayerState::Run);
			Player->SetState(Bio2PlayerState::Run, AnimationIndex::Weapon, Player->iFrame.load(), false, true);
		}
		else
		{
			Player->ResetFrame(Bio2PlayerState::Walk_Forward);
			Player->SetState(Bio2PlayerState::Walk_Forward, AnimationIndex::Weapon, Player->iFrame.load(), false, true);
		}
	}
	else if (b_Down)
	{
		Player->ResetFrame(Bio2PlayerState::Walk_Backward);
		Player->SetState(Bio2PlayerState::Walk_Backward, AnimationIndex::Normal, Player->iFrame.load(), false, true);

		if (b_Square)
		{
			Player->b_QuickTurn.store(true);
			Player->m_QuickTurnRotation.store(2048);
			return;
		}
	}
	else
	{
		if (b_Right || b_Left)
		{
			Player->ResetFrame(Bio2PlayerState::Idle);
			Player->SetState(Bio2PlayerState::Idle, AnimationIndex::Weapon, Player->iFrame.load(), false, true);
			b_IdleTurn = true;
		}
	}

	if (!b_AimBegin && !b_Aiming)
	{
		SVECTOR2 Delta{};

		std::int16_t X = 0, Z = 0;

		if (!Player->iFrame) { Player->Speed() = {}; }

		if (Player->iFrame >= Player->Animation(Player->AnimIndex())->Clip[Player->iClip.load()].size())
		{
			Player->iFrame.store(min(Player->iFrame.load(), Player->Animation(Player->AnimIndex())->GetFrameCount(Player->iClip.load()) - 1));
		}

		auto Speed = Player->Animation(Player->AnimIndex())->Clip[Player->iClip.load()][Player->iFrame.load()].Speed;

		if (b_IdleTurn)
		{
			Delta.x = Delta.y = Delta.z = 0;
		}
		else
		{
			Delta.x = Speed.x - Player->Speed().x;
			Delta.y = Speed.y - Player->Speed().y;
			Delta.z = Speed.z - Player->Speed().z;
		}

		Player->Speed() = { Speed.x, Speed.y, Speed.z };

		Player->AddSpeedXZ(Rotation.y, (SVECTOR*)&Delta, X, Z);

		if (!Player->b_EditorMode)
		{
			Player->Position().x += X;
			Player->Position().y += Delta.y;
			Player->Position().z += Z;
		}

		if (Player->Position().x >= 32767) { Player->Position().x = -Player->Position().x; }
		else if (Player->Position().x <= -32767) { Player->Position().x = 32767; }

		if (Player->Position().z >= 32767) { Player->Position().z = -Player->Position().z; }
		else if (Player->Position().z <= -32767) { Player->Position().z = 32767; }
	}

}