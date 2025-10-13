/*
*
*	Megan Grass
*	October 12, 2025
*
*/

#include "app.h"

static constexpr std::array<const char*, 25> GameFlags =
{
	"System_flg",       // scalar
	"Status_flg",       // scalar
	"Stop_flg",         // scalar
	"Scenario_flg",     // [8]
	"Common_flg",       // [8]
	"Room_flg",         // [2]
	"Enemy_flg",        // [8]
	"Enemy_flg2",       // [8]
	"Item_flg",         // [8]
	"Map_flg",          // [4]
	"Use_flg",          // [4]
	"Mess_flg",         // scalar
	"Room_enemy_flg",   // scalar
	"Pri_be_flg",       // [16][4] -> 64
	"Zapping_flg",      // [2]
	"Rbj_set_flg",      // scalar
	"Key_flg",          // [2]
	"Map_c_flg",        // [2]
	"Map_i_flg",        // scalar
	"Item_flg2",        // [4]
	"Map_o_flg",        // scalar
	"ExData0",          // [8]
	"ExData1",          // [4]
	"ExData2",          // [8]
	"ExData3"           // [25]
};

static constexpr std::array<std::uint32_t, 25> GameFlagCount =
{
	1,   // System_flg
	1,   // Status_flg
	1,   // Stop_flg
	8,   // Scenario_flg
	8,   // Common_flg
	2,   // Room_flg
	8,   // Enemy_flg
	8,   // Enemy_flg2
	8,   // Item_flg
	4,   // Map_flg
	4,   // Use_flg
	1,   // Mess_flg
	1,   // Room_enemy_flg
	64,  // Pri_be_flg (16 * 4)
	2,   // Zapping_flg
	1,   // Rbj_set_flg
	2,   // Key_flg
	2,   // Map_c_flg
	1,   // Map_i_flg
	4,   // Item_flg2
	1,   // Map_o_flg
	8,   // ExData0
	4,   // ExData1
	8,   // ExData2
	25   // ExData3
};

void Global_Application::ScenarioEditor(void)
{
	if ((Room->Game() & BIO2) && ImGui::CollapsingHeader("Scenario##RightPanel", ImGuiTreeNodeFlags_None))	// TEMP: Bio2 only
	{
		{
			static int SelectedGameFlag = 0;
			static int SelectedElement = 0;

			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Pri_be_flg").x * 2.0f);
			if (ImGui::BeginCombo("##GameFlagSelect", GameFlags[SelectedGameFlag], ImGuiComboFlags_HeightLargest))
			{
				for (int i = 0; i < (int)GameFlags.size(); ++i)
				{
					bool b_Selected = (SelectedGameFlag == i);
					if (ImGui::Selectable(GameFlags[i], b_Selected))
					{
						SelectedGameFlag = i;
						SelectedElement = 0; // reset element index when switching group
					}
					if (b_Selected) ImGui::SetItemDefaultFocus();
					ImGui::SameLine();
					ImGui::TextDisabled("(%u)", GameFlagCount[i]);
				}
				ImGui::EndCombo();
			}

			ScrollComboOnHover("##GameFlagSelect", &SelectedGameFlag, ImGuiDataType_S32, 1, 0, 24, [&]() { SelectedElement = 0; });

			std::uint32_t totalElems = GameFlagCount[SelectedGameFlag];
			if (SelectedElement >= (int)totalElems) SelectedElement = 0;

			auto GetFlagBasePtr = [&]() -> std::uint32_t*
				{
					switch (SelectedGameFlag)
					{
					case 0:  return &Sce->System_flg;
					case 1:  return &Sce->Status_flg;
					case 2:  return &Sce->Stop_flg;
					case 3:  return Sce->SaveData.Scenario_flg;       // [8]
					case 4:  return Sce->SaveData.Common_flg;         // [8]
					case 5:  return Sce->SaveData.Room_flg;           // [2]
					case 6:  return Sce->SaveData.Enemy_flg;          // [8]
					case 7:  return Sce->SaveData.Enemy_flg2;         // [8]
					case 8:  return Sce->SaveData.Item_flg;           // [8]
					case 9:  return Sce->SaveData.Map_flg;            // [4]
					case 10: return Sce->Use_flg;            // [4]
					case 11: return &Sce->Mess_flg;
					case 12: return &Sce->Room_enemy_flg;
					case 13: return &Sce->SaveData.Pri_be_flg[0][0];  // [16][4] flattened
					case 14: return Sce->SaveData.Zapping_flg;        // [2]
					case 15: return &Sce->Rbj_set_flg;
					case 16: return Sce->SaveData.Key_flg;            // [2]
					case 17: return Sce->SaveData.Map_c_flg;          // [2]
					case 18: return &Sce->SaveData.Map_i_flg;
					case 19: return Sce->SaveData.Item_flg2;          // [4]
					case 20: return &Sce->SaveData.Map_o_flg;
					case 21: return Sce->SaveData.ExData0;            // [8]
					case 22: return Sce->SaveData.ExData1;            // [4]
					case 23: return Sce->SaveData.ExData2;            // [8]
					case 24: return Sce->SaveData.ExData3;            // [25]
					default: return nullptr;
					}
				};

			std::uint32_t* base = GetFlagBasePtr();
			if (!base)
			{
				ImGui::TextDisabled("Unavailable");
				return;
			}

			if (totalElems > 1)
			{
				ImGui::SetNextItemWidth(ImGui::CalcTextSize("Element 000").x * 1.3f);
				ImGui::SliderInt("Element##FlagElemIndex", &SelectedElement, 0, (int)totalElems == 1 ? 0 : (int)totalElems - 1);
				ScrollOnHover(&SelectedElement, ImGuiDataType_S32, 1, 0, (int)totalElems == 1 ? 0 : (int)totalElems - 1);
				TooltipOnHover("Index within this flag group");
			}

			std::uint32_t& Value = base[SelectedElement];

			ImGui::Text("0x%08X", Value);

			DrawHorizontalLine(4.0f, 8.0f, 1.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

			ImGui::BeginGroup();
			for (int Bit = 0; Bit < 32; ++Bit)
			{
				bool BitSet = (Value & (1u << Bit)) != 0;
				if (ImGui::Checkbox(("##FlagBit" + std::to_string(Bit)).c_str(), &BitSet))
				{
					if (BitSet) Value |= (1u << Bit);
					else Value &= ~(1u << Bit);
				}
				if ((Bit % 8) != 7) ImGui::SameLine();
			}
			ImGui::EndGroup();

			ImGui::Spacing();	if (ImGui::Button("All##FlagSetAll")) { Value = 0xFFFFFFFFu; }
			ImGui::SameLine();	if (ImGui::Button("None##FlagClearAll")) { Value = 0; }
			ImGui::SameLine();	if (ImGui::Button("Invert##FlagInvert")) { Value ^= 0xFFFFFFFFu; }
		}

		DrawHorizontalLine(4.0f, 8.0f, 1.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		{
			bool b_Process = b_ScriptOp.load();
			bool b_DrawSceAot = b_DrawAot.load();

			if (ImGui::Checkbox("Process##ScenarioPanel", &b_Process))
			{
				b_ScriptOp.store(b_Process);

				if (b_ScriptOp.load())
				{
					if (Room->GameType() & BIO2)
					{
						Room->Script->Initialize = [this]() -> void
							{
								Resident_Evil_2_Bytecode().SceSchedulerSet();
							};

						Room->Script->Routine = [this]() -> void
							{
								Resident_Evil_2_Bytecode().SceScheduler();
							};

						Room->Script->Initialize();
					}
				}
				else
				{
					Room->Script->Routine = []() -> void {};
				}

				Room->Script->Initialize = []() -> void {};
			}
			TooltipOnHover("Script bytecode will be parsed and executed");

			if (ImGui::Checkbox("Draw AOT##ScenarioPanel", &b_DrawSceAot))
			{
				b_DrawAot.store(b_DrawSceAot);
			}
			TooltipOnHover("Interactive hotspots will be drawn");
		}

		DrawHorizontalLine(4.0f, 8.0f, 1.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		{
			bool b_FontJapanese = G->Sce->System_flg & SYS_LANG_MAIN;
			bool b_FontOther = G->Sce->System_flg & SYS_LANG_SUB;

			if (ImGui::Checkbox("Japanese##ScenarioPanel", &b_FontJapanese))
			{
				if (!(G->Sce->System_flg & SYS_LANG_MAIN))
				{
					G->Sce->System_flg |= SYS_LANG_MAIN;
					G->Sce->System_flg &= ~SYS_LANG_SUB;
				}
				else
				{
					G->Sce->System_flg &= ~SYS_LANG_MAIN;
					G->Sce->System_flg |= SYS_LANG_SUB;
				}
			}
			TooltipOnHover("Messages will be in Japanese (Main)");

			ImGui::SameLine(); if (ImGui::Checkbox("Other##ScenarioPanel", &b_FontOther))
			{
				if (!(G->Sce->System_flg & SYS_LANG_SUB))
				{
					G->Sce->System_flg &= ~SYS_LANG_MAIN;
					G->Sce->System_flg |= SYS_LANG_SUB;
				}
				else
				{
					G->Sce->System_flg |= SYS_LANG_MAIN;
					G->Sce->System_flg &= ~SYS_LANG_SUB;
				}
			}
			TooltipOnHover("Messages will be in English/other (Sub)");
		}
	}
}