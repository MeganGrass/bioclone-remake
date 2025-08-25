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

void Global_Application::ControllerInput(std::unique_ptr<Resident_Evil_Model>& Model)
{
	if (!Model->b_ControllerMode) { return; }

	VECTOR2& Rotation = Model->b_EditorMode ? Model->EditorRotation() : Model->Rotation();

	if (Model->b_QuickTurn.load())
	{
		if (Model->m_QuickTurnRotation.load() <= 0)
		{
			Model->b_QuickTurn.store(false);
			Model->m_QuickTurnRotation.store(0);
			Model->ResetFrame(Bio2PlayerState::Idle);
			Model->SetState(Bio2PlayerState::Idle, AnimationIndex::Weapon, Model->iFrame.load(), false, true);
			return;
		}
		else
		{
			Model->m_QuickTurnRotation -= 64;
			Rotation.y += 64;
			Model->ClampRotation(Rotation);
			return;
		}
	}

	bool b_AimBegin = (Model->State() == Bio2PlayerState::Aim_Begin);
	bool b_Firing = (Model->State() == Bio2PlayerState::Fire) || (Model->State() == Bio2PlayerState::Fire_Upward) || (Model->State() == Bio2PlayerState::Fire_Downward);
	bool b_Aiming = (Model->State() == Bio2PlayerState::Aim) || (Model->State() == Bio2PlayerState::Aim_Upward) || (Model->State() == Bio2PlayerState::Aim_Downward);
	bool b_Running = (Model->PriorState() == Bio2PlayerState::Run) || (Model->PriorState() == Bio2PlayerState::Run_Caution) || (Model->PriorState() == Bio2PlayerState::Run_Danger);

	if (!Gamepad->PollState())
	{
		if (b_AimBegin || b_Aiming || b_Firing || b_Running)
		{
			Model->RecoverState(b_AimBegin, b_Aiming, b_Firing, b_Running);
		}
		else
		{
			Model->ResetFrame(Bio2PlayerState::Idle);
			Model->SetState(Bio2PlayerState::Idle, AnimationIndex::Weapon, Model->iFrame.load(), false, true);
		}
		return;
	}

	bool b_Up = Gamepad->IsPressed(Gamepad->Map().Up);
	bool b_Right = Gamepad->IsPressed(Gamepad->Map().Right);
	bool b_Down = Gamepad->IsPressed(Gamepad->Map().Down);
	bool b_Left = Gamepad->IsPressed(Gamepad->Map().Left);

	if (b_Right || b_Left)
	{
		if (b_AimBegin || b_Aiming || b_Firing) { Rotation.y += (b_Right ? 12 : -12); }

		else { Rotation.y += (b_Right ? 16 : -16); }

		Model->ClampRotation(Rotation);
	}

	if (Model->b_IdleTurn.load() && (b_Up || b_Down))
	{
		Model->ResetFrame(Bio2PlayerState::Idle);
		Model->SetState(Bio2PlayerState::Idle, AnimationIndex::Weapon, 0, false, true);
		Model->b_IdleTurn.store(false);
	}

	if (Gamepad->IsPressed(Gamepad->Map().R1))
	{
		bool b_Cross = Gamepad->IsPressed(Gamepad->Map().Cross);

		if (b_AimBegin || b_Firing)
		{
		}
		else if (b_Aiming && b_Cross && b_Up)
		{
			Model->SetState(Bio2PlayerState::Fire_Upward, AnimationIndex::Weapon, Model->iFrame.load(), true, true);
			Model->b_PlayAllFrames.store(true);
		}
		else if (b_Aiming && b_Cross && b_Down)
		{
			Model->SetState(Bio2PlayerState::Fire_Downward, AnimationIndex::Weapon, Model->iFrame.load(), true, true);
			Model->b_PlayAllFrames.store(true);
		}
		else if (b_Aiming && b_Cross)
		{
			Model->SetState(Bio2PlayerState::Fire, AnimationIndex::Weapon, Model->iFrame.load(), true, true);
			Model->b_PlayAllFrames.store(true);
		}
		else if (b_Aiming && b_Up)
		{
			Model->SetState(Bio2PlayerState::Aim_Upward, AnimationIndex::Weapon, Model->iFrame.load(), true, true);
		}
		else if (b_Aiming && b_Down)
		{
			Model->SetState(Bio2PlayerState::Aim_Downward, AnimationIndex::Weapon, Model->iFrame.load(), true, true);
		}
		else if (b_Aiming)
		{
			Model->SetState(Bio2PlayerState::Aim, AnimationIndex::Weapon, Model->iFrame.load(), true, true);
		}
		else if (!b_Aiming || !b_AimBegin)
		{
			Model->SetState(Bio2PlayerState::Aim_Begin, AnimationIndex::Weapon, 0, true, false);
			Model->SetNextState(Bio2PlayerState::Aim, AnimationIndex::Weapon);
			Model->b_PlayAllFrames.store(true);
		}
	}
	else if (b_AimBegin || b_Aiming || b_Firing)
	{
		Model->RecoverState(b_AimBegin, b_Aiming, b_Firing, b_Running);
	}
	else if (b_Up)
	{
		if (Gamepad->IsPressed(Gamepad->Map().Square))
		{
			Model->ResetFrame(Bio2PlayerState::Run);
			Model->SetState(Bio2PlayerState::Run, AnimationIndex::Weapon, Model->iFrame.load(), false, true);
		}
		else
		{
			Model->ResetFrame(Bio2PlayerState::Walk_Forward);
			Model->SetState(Bio2PlayerState::Walk_Forward, AnimationIndex::Weapon, Model->iFrame.load(), false, true);
		}
	}
	else if (b_Running)
	{
		Model->RecoverState(b_AimBegin, b_Aiming, b_Firing, b_Running);
	}
	else if (b_Down)
	{
		Model->ResetFrame(Bio2PlayerState::Walk_Backward);
		Model->SetState(Bio2PlayerState::Walk_Backward, AnimationIndex::Normal, Model->iFrame.load(), false, true);

		if (Gamepad->IsPressed(Gamepad->Map().Square))
		{
			Model->b_QuickTurn.store(true);
			Model->m_QuickTurnRotation.store(2048);
			return;
		}
	}
	else if (b_Right || b_Left)
	{
		Model->ResetFrame(Bio2PlayerState::Walk_Backward);
		Model->SetState(Bio2PlayerState::Walk_Backward, AnimationIndex::Normal, Model->iFrame.load(), false, true);
		Model->b_IdleTurn.store(true);
	}

	if (!Model->b_IdleTurn.load() && !b_AimBegin && !b_Aiming && !Model->b_EditorMode)
	{
		if (b_Firing && !Model->b_WeaponKickback) { return; }

		auto& Animation = Model->Animation(Model->AnimIndex());

		auto iClip = min(Model->iClip.load(), Animation->GetClipCount() - 1);

		auto iFrame = min(Model->iFrame.load(), Animation->GetFrameCount(iClip) - 1);

		if (Animation->Clip.empty())
		{
			Model->ResetFrame(Bio2PlayerState::Idle);
			Model->SetState(Bio2PlayerState::Idle, AnimationIndex::Weapon, Model->iFrame.load(), false, true);
			return;
		}

		auto& Frame = Animation->Clip[iClip][iFrame];

		if (!iFrame) { Model->Speed() = {}; }

		SVECTOR2 Delta{ (Frame.Speed.x - Model->Speed().x), (Frame.Speed.y - Model->Speed().y), (Frame.Speed.z - Model->Speed().z) };

		Model->Speed() = { Frame.Speed.x, Frame.Speed.y, Frame.Speed.z };

		Model->AddSpeedXZ((SVECTOR*)&Delta);

		Model->ClampPosition(Model->Position());
	}

}