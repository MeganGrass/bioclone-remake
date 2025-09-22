/*
*
*	Megan Grass
*	September 17, 2025
*
*/

#include "app.h"

void Global_Application::EffectEditor(void)
{
	if (b_FileOp.load()) { return; }

	Room->iEffectMax = Room->Esp->Data().empty() ? 0 : Room->Esp->Data().size() - 1;
	std::uintmax_t& iEffect = Room->iEffect, iEffectMin = Room->iEffectMin, iEffectMax = Room->iEffectMax;

	if (Room->Esp->Data().empty()) { ImGui::Text(" No ESP data available"); return; }

	auto& Effect = Room->Esp->Get(iEffect);

	Room->iEffectSSeqMax = Effect.SSequence.empty() ? 0 : Effect.SSequence.size() - 1;
	std::uintmax_t& iEffectSSeq = Room->iEffectSSeq, iEffectSSeqMin = Room->iEffectSSeqMin, iEffectSSeqMax = Room->iEffectSSeqMax;

	Room->iEffectGpMax = Effect.SpriteGp.empty() ? 0 : Effect.SpriteGp.size() - 1;
	std::uintmax_t& iEffectGp = Room->iEffectGp, iEffectGpMin = Room->iEffectGpMin, iEffectGpMax = Room->iEffectGpMax;

	Room->iEffectMSeqMax = Effect.MSequence.empty() ? 0 : Effect.MSequence.size() - 1;
	std::uintmax_t& iEffectMSeq = Room->iEffectMSeq, iEffectMSeqMin = Room->iEffectMSeqMin, iEffectMSeqMax = Room->iEffectMSeqMax;

	Room->iEffectMSeqIDMax = Effect.MSequence.empty() ? 0 : Effect.MSequence[iEffectMSeq].empty() ? 0 : Effect.MSequence[iEffectMSeq].size() - 1;
	std::uintmax_t& iEffectMSeqID = Room->iEffectMSeqID, iEffectMSeqIDMin = Room->iEffectMSeqIDMin, iEffectMSeqIDMax = Room->iEffectMSeqIDMax;

	Room->iEffectClutIDMax = Effect.Texture.empty() ? 0 : Effect.Texture.size() - 1;
	std::uintmax_t& iEffectClutID = Room->iEffectClutID, iEffectClutIDMin = Room->iEffectClutIDMin, iEffectClutIDMax = Room->iEffectClutIDMax;

	if (ImGui::BeginTable("EffectID##EffectEditor", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text(" ID 0x%02X", Effect.Id);

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
		if (ImGui::SliderScalar("##EffectIndex", ImGuiDataType_U64, &iEffect, &iEffectMin, &iEffectMax))
		{
			Room->Esp->iSSeq = 0;
			Room->Esp->iTime = 0;
			Room->Esp->m_FrameCounter = 0;
			iEffectSSeq = 0;
			iEffectGp = 0;
			iEffectMSeq = 0;
			iEffectMSeqID = 0;
			iEffectClutID = 0;
		}
		ScrollOnHover(&iEffect, ImGuiDataType_U64, 1, iEffectMin, iEffectMax, [&]()
			{
				Room->Esp->iSSeq = 0;
				Room->Esp->iTime = 0;
				Room->Esp->m_FrameCounter = 0;
				iEffectSSeq = 0;
				iEffectGp = 0;
				iEffectMSeq = 0;
				iEffectMSeqID = 0;
				iEffectClutID = 0;
			}
		);

		ImGui::EndTable();

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);
	}

	if (Effect.Id != 0xFF && ImGui::CollapsingHeader("Texture##EffectEditor", ImGuiTreeNodeFlags_None))
	{
		if (ImGui::BeginTable("Texture##EffectEditor", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text(" Palette");

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##EffectClutIDIndex", ImGuiDataType_U64, &iEffectClutID, &iEffectClutIDMin, &iEffectClutIDMax))
			{
			}
			ScrollOnHover(&iEffectClutID, ImGuiDataType_U64, 1, iEffectClutIDMin, iEffectClutIDMax, [&]()
				{
				}
			);

			ImGui::EndTable();

			DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);
		}
	}

	if (Effect.Id != 0xFF && ImGui::CollapsingHeader("Sprite Sequence##EffectEditor", ImGuiTreeNodeFlags_None))
	{
		if (ImGui::BeginTable("EffectSSeq##EffectEditorTable", 2))
		{
			{
				ImGui::TableNextColumn();
				ImGui::Text(" ID");

				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
				if (ImGui::SliderScalar("##EffectSSeqIndex", ImGuiDataType_U64, &iEffectSSeq, &iEffectSSeqMin, &iEffectSSeqMax))
				{
				}
				ScrollOnHover(&iEffectSSeq, ImGuiDataType_U64, 1, iEffectSSeqMin, iEffectSSeqMax, [&]()
					{
					});
			}

			{
				ImGui::TableNextColumn();
				ImGui::Text(" pGp %d", Effect.SSequence[iEffectSSeq].pGp);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" nSprite %d", Effect.SSequence[iEffectSSeq].nSpr);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Time %d", Effect.SSequence[iEffectSSeq].Time);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Width %d", Effect.SSequence[iEffectSSeq].Width);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Height %d", Effect.SSequence[iEffectSSeq].Height);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Hotx %d", Effect.SSequence[iEffectSSeq].Hotx);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Hoty %d", Effect.SSequence[iEffectSSeq].Hoty);
				ImGui::TableNextColumn();
			}

			ImGui::EndTable();

			DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);
		}
	}

	if (Effect.Id != 0xFF && ImGui::CollapsingHeader("Group##EffectEditor", ImGuiTreeNodeFlags_None))
	{
		if (ImGui::BeginTable("EffectGroup##EffectEditor", 2))
		{
			{
				ImGui::TableNextColumn();
				ImGui::Text(" Group");

				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
				if (ImGui::SliderScalar("##EffectGpIndex", ImGuiDataType_U64, &iEffectGp, &iEffectGpMin, &iEffectGpMax))
				{
				}
				ScrollOnHover(&iEffectGp, ImGuiDataType_U64, 1, iEffectGpMin, iEffectGpMax, [&]()
					{
					});
			}

			{
				ImGui::TableNextColumn();
				ImGui::Text(" U %d", Effect.SpriteGp[iEffectGp].U);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" V %d", Effect.SpriteGp[iEffectGp].V);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" OfsX %d", Effect.SpriteGp[iEffectGp].OfsX);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" OfsY %d", Effect.SpriteGp[iEffectGp].OfsY);
				ImGui::TableNextColumn();
			}

			ImGui::EndTable();

			DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);
		}
	}

	if (Effect.Id != 0xFF && ImGui::CollapsingHeader("Motion Sequence##EffectEditor", ImGuiTreeNodeFlags_None))
	{
		if (ImGui::BeginTable("EffectMSeq##EffectEditor", 2))
		{
			{
				ImGui::TableNextColumn();
				ImGui::Text(" ID");

				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
				if (ImGui::SliderScalar("##EffectMSeqIndex", ImGuiDataType_U64, &iEffectMSeq, &iEffectMSeqMin, &iEffectMSeqMax))
				{
					iEffectMSeqID = 0;
				}
				ScrollOnHover(&iEffectMSeq, ImGuiDataType_U64, 1, iEffectMSeqMin, iEffectMSeqMax, [&]()
					{
						iEffectMSeqID = 0;
					});

				ImGui::TableNextColumn();
				ImGui::Text(" Index");

				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
				if (ImGui::SliderScalar("##EffectMSeqIDIndex", ImGuiDataType_U64, &iEffectMSeqID, &iEffectMSeqIDMin, &iEffectMSeqIDMax))
				{
				}
				ScrollOnHover(&iEffectMSeqID, ImGuiDataType_U64, 1, iEffectMSeqIDMin, iEffectMSeqIDMax, [&]()
					{
					});
			}

			if (Effect.MSequence[iEffectMSeq].size())
			{
				ImGui::TableNextColumn();
				ImGui::Text(" Routine0 %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Routine0);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Routine1 %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Routine1);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Free0 %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Free0);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Free1 %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Free1);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Transx %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Transx);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Transy %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Transy);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Add_x %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Add_x);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Add_y %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Add_y);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Add_z %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Add_z);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Free2 %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Free2);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Speed_x %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Speed_x);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Speed_y %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Speed_y);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Speed_z %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Speed_z);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Free3 %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Free3);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Free4 %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Free4);
				ImGui::TableNextColumn();

				ImGui::TableNextColumn();
				ImGui::Text(" Free5 %d", Effect.MSequence[iEffectMSeq][iEffectMSeqID].Free5);
				ImGui::TableNextColumn();
			}
			else
			{
				ImGui::TableNextColumn();
				ImGui::Text(" empty data");
				ImGui::TableNextColumn();
			}

			ImGui::EndTable();

			DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);
		}
	}

	if (Effect.Id != 0xFF && ImGui::CollapsingHeader("Transform##EffectEditor", ImGuiTreeNodeFlags_None))
	{
		TooltipOnHover("Position and Scale");

		if (ImGui::BeginTable("Transform##ModelTransform", 4))
		{
			ImGui::TableSetupColumn("Label##ModelTransform", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("____").x);
			ImGui::TableSetupColumn("X##ModelTransform");
			ImGui::TableSetupColumn("Y##ModelTransform");
			ImGui::TableSetupColumn("Z##ModelTransform");
			ImGui::TableNextRow();

			{
				ImGui::TableNextColumn(); ImGui::Text(" Pos");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformPosX", ImGuiDataType_S32, &Effect.Position.x))
				{
					Effect.Position.x = std::clamp(Effect.Position.x, -32767, 32767);
				}
				ScrollOnHover(&Effect.Position.x, ImGuiDataType_S32, 32, -32767, 32767, [&]()
					{
						Effect.Position.x = std::clamp(Effect.Position.x, -32767, 32767);
					});
				TooltipOnHover("Position X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformPosY", ImGuiDataType_S32, &Effect.Position.y))
				{
					Effect.Position.y = std::clamp(Effect.Position.y, -32767, 32767);
				}
				ScrollOnHover(&Effect.Position.y, ImGuiDataType_S32, 32, -32767, 32767, [&]()
					{
						Effect.Position.y = std::clamp(Effect.Position.y, -32767, 32767);
					});
				TooltipOnHover("Position Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformPosZ", ImGuiDataType_S32, &Effect.Position.z))
				{
					Effect.Position.z = std::clamp(Effect.Position.z, -32767, 32767);
				}
				ScrollOnHover(&Effect.Position.z, ImGuiDataType_S32, 32, -32767, 32767, [&]()
					{
						Effect.Position.z = std::clamp(Effect.Position.z, -32767, 32767);
					});
				TooltipOnHover("Position Z");
			}

			ImGui::EndTable();
		}
	}

	if (Effect.Id != 0xFF && ImGui::CollapsingHeader("Playback##EffectEditor", ImGuiTreeNodeFlags_None))
	{
		bool b_Value = Room->Esp->b_Play.load();
		if (ImGui::MenuItem(" Play##EffectEditor", NULL, &b_Value)) { Room->Esp->b_Play.store(!Room->Esp->b_Play.load()); }
		TooltipOnHover("Play/Pause sequence playback");

		b_Value = Room->Esp->b_Loop.load();
		if (ImGui::MenuItem(" Loop##EffectEditor", NULL, &b_Value)) { Room->Esp->b_Loop.store(!Room->Esp->b_Loop.load()); }
		TooltipOnHover("Loop sequence playback");

		if (ImGui::BeginTable("Keyframe##EffectEditor", 2))
		{
			ImGui::TableNextColumn(); ImGui::Text(" Keyframe");

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			ImGui::SliderScalar("##EffectEditorKeyFrame", ImGuiDataType_U64, &Room->Esp->iSSeq, &iEffectSSeqMin, &iEffectSSeqMax);
			ScrollOnHover(&Room->Esp->iSSeq, ImGuiDataType_U64, 1, iEffectSSeqMin, iEffectSSeqMax);

			ImGui::EndTable();
		}
	}

}