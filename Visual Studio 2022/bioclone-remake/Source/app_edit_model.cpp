/*
*
*	Megan Grass
*	July 26, 2025
*
*/

#include "app.h"

void Global_Application::ModelEditor(void)
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File##ModelFileMenu"))
		{
			if (ImGui::MenuItem("Open##ModelFileMenu"))
			{
				FilePool.Enqueue([this]() { OpenPlayerModel(); });
			}
			if (ImGui::MenuItem("Save As##ModelFileMenu", NULL, false, false))
			{
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close##ModelFileMenu"))
			{
				Player->Close();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Assemble##ModelFileMenu", NULL, false, false))
			{
			}
			if (ImGui::MenuItem("Disassemble##ModelFileMenu", NULL, false, false))
			{
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Texture##ModelTextureMenu"))
		{
			if (ImGui::MenuItem("Open##ModelTextureMenu"))
			{
				FilePool.Enqueue([this]() { OpenPlayerTexture(); });
			}
			if (ImGui::MenuItem("Save As##ModelTextureMenu"))
			{
				FilePool.Enqueue([this]() { SavePlayerTexture(); });
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Object##ModelObjectMenu", false))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::CollapsingHeader("Type##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		bool b_Bio1Alpha = Player->GameType() & (AUG95 | OCT95);
		bool b_Bio1 = Player->GameType() & (BIO1);
		bool b_Bio2Nov96 = Player->GameType() & (BIO2NOV96);
		bool b_Bio2 = Player->GameType() & (BIO2);
		bool b_Bio3 = Player->GameType() & (BIO3);

		bool b_Bio1Enemy = false;

		if (ImGui::BeginTable("Table##ModelType", 2))
		{
			TooltipOnHover("Game Type on Open/Save");
			ImGui::TableNextColumn();
			if (ImGui::Checkbox(" Bio1 Alpha##ModelType", &b_Bio1Alpha)) { Player->SetGame(Video_Game::Resident_Evil_Aug_4_1995); }
			TooltipOnHover("Resident Evil (Aug/Oct 1995) Prototype");
			ImGui::TableNextColumn();
			if (ImGui::Checkbox(" Bio1 Enemy##ModelType", &b_Bio1Enemy)) {  }
			TooltipOnHover("Bio1 EMD is enemy type on Open\r\nAssume player type otherwise");
			ImGui::TableNextColumn();
			if (ImGui::Checkbox(" Bio1##ModelType", &b_Bio1)) { Player->SetGame(Video_Game::Resident_Evil); }
			TooltipOnHover("Resident Evil");
			if (ImGui::Checkbox(" Bio2 Nov '96##ModelType", &b_Bio2Nov96)) { Player->SetGame(Video_Game::Resident_Evil_2_Nov_6_1996); }
			TooltipOnHover("Resident Evil 2 (Nov 1996) Prototype");
			if (ImGui::Checkbox(" Bio2##ModelType", &b_Bio2)) { Player->SetGame(Video_Game::Resident_Evil_2); }
			TooltipOnHover("Resident Evil 2");
			if (ImGui::Checkbox(" Bio3##ModelType", &b_Bio3)) { Player->SetGame(Video_Game::Resident_Evil_3); }
			TooltipOnHover("Resident Evil 3");
			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Index##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		TooltipOnHover("Animation Type");

		bool b_Normal = Player->AnimIndex() == AnimationIndex::Normal;
		bool b_NormalEx0 = Player->AnimIndex() == AnimationIndex::NormalEx0;
		bool b_NormalEx1 = Player->AnimIndex() == AnimationIndex::NormalEx1;
		bool b_Damage = Player->AnimIndex() == AnimationIndex::Damage;
		bool b_Weapon = Player->AnimIndex() == AnimationIndex::Weapon;
		bool b_WeaponEx0 = Player->AnimIndex() == AnimationIndex::WeaponEx0;
		bool b_WeaponEx1 = Player->AnimIndex() == AnimationIndex::WeaponEx1;
		bool b_Room = Player->AnimIndex() == AnimationIndex::Room;

		if (ImGui::MenuItem(" Normal##ModelAnimationIndex", NULL, &b_Normal)) { Player->ResetClip(); Player->SetAnimIndex(AnimationIndex::Normal); }
		TooltipOnHover("Normal Animation");
		if (ImGui::MenuItem(" Normal Ex0##ModelAnimationIndex", NULL, &b_NormalEx0)) { Player->ResetClip(); Player->SetAnimIndex(AnimationIndex::NormalEx0); }
		TooltipOnHover("Normal Animation Extra\r\nEMD (Bio2/Bio3)");
		if (ImGui::MenuItem(" Normal Ex1##ModelAnimationIndex", NULL, &b_NormalEx1)) { Player->ResetClip(); Player->SetAnimIndex(AnimationIndex::NormalEx1); }
		TooltipOnHover("Normal Animation Extra\r\nEMD (Bio3)");
		if (ImGui::MenuItem(" Damage##ModelAnimationIndex", NULL, &b_Damage)) { Player->ResetClip(); Player->SetAnimIndex(AnimationIndex::Damage); }
		TooltipOnHover("Damage Animation\r\nEMD, for use with Player");
		if (ImGui::MenuItem(" Weapon##ModelAnimationIndex", NULL, &b_Weapon)) { Player->ResetClip(); Player->SetAnimIndex(AnimationIndex::Weapon); }
		TooltipOnHover("Weapon Animation\r\nEMW (Bio1) or PLW (Bio2/Bio3)");
		if (ImGui::MenuItem(" Weapon Ex0##ModelAnimationIndex", NULL, &b_WeaponEx0)) { Player->ResetClip(); Player->SetAnimIndex(AnimationIndex::WeaponEx0); }
		TooltipOnHover("Weapon Animation Ex0\r\nPLW/EMD (Bio3), unknown purpose");
		if (ImGui::MenuItem(" Weapon Ex1##ModelAnimationIndex", NULL, &b_WeaponEx1)) { Player->ResetClip(); Player->SetAnimIndex(AnimationIndex::WeaponEx1); }
		TooltipOnHover("Weapon Animation Ex1\r\nPLW/EMD (Bio3)");

		if (ImGui::BeginTable("Room##ModelAnimationIndexTable", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::MenuItem(" Room##ModelAnimationIndexTable", NULL, &b_Room)) { Player->ResetClip(); Player->SetAnimIndex(AnimationIndex::Room); }
			TooltipOnHover("Room Animation\r\nRDT");

			ImGui::TableNextColumn();
			ImGui::BeginDisabled(!IsRoomOpen() || !Player->iRoomMax);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##ModelAnimationIndex", ImGuiDataType_U64, &Player->iRoom, &Player->iRoomMin, &Player->iRoomMax))
			{
				Player->iRoom = std::clamp(Player->iRoom, (size_t)0, Player->iRoomMax);
				Player->Animation(AnimationIndex::Room) = Bio2->Rdt->Rbj->Data[Player->iRoom];
			}
			ScrollOnHover(&Player->iRoom, ImGuiDataType_U64, 1, Player->iRoomMin, Player->iRoomMax);
			ImGui::EndDisabled();
			TooltipOnHover("Room Animation Container ID");

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Render##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::MenuItem(" Dither##ModelRender", NULL, &Player->b_Dither);
		TooltipOnHover("Sony PlayStation (1994) Dithering Pixel Shader\r\n\r\nValid only when \"Textured\" is active");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		bool b_NoFilter = Player->m_TextureFilter == D3DTEXF_NONE || Player->m_TextureFilter == D3DTEXF_POINT;
		bool b_LinearFilter = Player->m_TextureFilter == D3DTEXF_LINEAR;
		bool b_AnisotropicFilter = Player->m_TextureFilter == D3DTEXF_ANISOTROPIC;

		if (ImGui::MenuItem(" Point##ModelRender", NULL, &b_NoFilter)) { Player->m_TextureFilter = D3DTEXF_POINT; }
		TooltipOnHover("Texture Filter");

		if (ImGui::MenuItem(" Linear##ModelRender", NULL, &b_LinearFilter)) { Player->m_TextureFilter = D3DTEXF_LINEAR; }
		TooltipOnHover("Texture Filter");

		if (ImGui::MenuItem(" Anisotropic##ModelRender", NULL, &b_AnisotropicFilter)) { Player->m_TextureFilter = D3DTEXF_ANISOTROPIC; }
		TooltipOnHover("Texture Filter");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::MenuItem(" Wireframe##ModelRender", NULL, &Player->b_DrawWireframe))
		{
			Player->b_DrawWireframe = Player->b_DrawWireframe ? true : false;
			Player->b_DrawSolidColor = false;
			Player->b_DrawTextured = false;
		}
		TooltipOnHover("Polygons will be drawn as wireframe");

		if (ImGui::MenuItem(" Solid##ModelRender", NULL, &Player->b_DrawSolidColor))
		{
			Player->b_DrawWireframe = false;
			Player->b_DrawSolidColor = Player->b_DrawSolidColor ? true : false;
			Player->b_DrawTextured = false;
		}
		TooltipOnHover("Polygons will be drawn as solid color");

		if (ImGui::MenuItem(" Textured##ModelRender", NULL, &Player->b_DrawTextured))
		{
			Player->b_DrawWireframe = false;
			Player->b_DrawSolidColor = false;
			Player->b_DrawTextured = Player->b_DrawTextured ? true : false;
		}
		TooltipOnHover("Polygons will be drawn as textured");

		ImGui::MenuItem(" Skeleton##ModelRender", NULL, &Player->b_DrawSkeleton);
		TooltipOnHover("Skeleton will be drawn");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::MenuItem(" Vertex Light##ModelRender", NULL, &b_PerVertexLighting)) { if (b_PerVertexLighting) { b_PerPixelLighting = false; } }
		TooltipOnHover("Per-Vertex Lighting");

		if (ImGui::MenuItem(" Pixel Light##ModelRender", NULL, &b_PerPixelLighting)) { if (b_PerPixelLighting) { b_PerVertexLighting = false; } }
		TooltipOnHover("Per-Pixel Lighting");
	}

	if (ImGui::CollapsingHeader("Object##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		TooltipOnHover("Model Objects");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::MenuItem(" All Objects##ModelObject", NULL, &Player->b_DrawAllObjects))
		{
			Player->b_DrawReference = false;
			Player->b_DrawSingleObject = false;
		}
		TooltipOnHover("Draw all objects\r\n\r\nKeyframes and skeleton are ignored\r\n\r\nOnly draw weapon when \"Weapon\" is active");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::BeginTable("ModelObjectPanel##ModelObject", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::MenuItem(" Object##ModelObject", NULL, &Player->b_DrawSingleObject))
			{
				Player->b_DrawReference = false;
				Player->b_DrawAllObjects = false;
			}
			TooltipOnHover(Standard_String().FormatCStyle("Draw object %d\r\n\r\nKeyframes and skeleton are ignored\r\n\r\nOnly draw weapon when \"Weapon\" is active", Player->iObject));

			ImGui::TableNextColumn();
			ImGui::BeginDisabled(!Player->b_DrawSingleObject);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##ModelObjectObject", ImGuiDataType_U64, &Player->iObject, &Player->iObjectMin, &Player->iObjectMax))
			{
				Player->iObject = std::clamp(Player->iObject, (size_t)0, Player->iObjectMax);
			}
			ScrollOnHover(&Player->iObject, ImGuiDataType_U64, 1, Player->iObjectMin, Player->iObjectMax);
			ImGui::EndDisabled();

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Hitbox##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::MenuItem(" View##ModelHitbox", NULL, &Player->b_DrawHitbox);
		TooltipOnHover("Draw the model's interactive/collision hitbox");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::BeginTable("Transform##ModelHitbox", 4))
		{
			ImGui::TableSetupColumn("Label##ModelHitbox", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("________").x);
			ImGui::TableSetupColumn("X##ModelHitbox", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("________").x);
			ImGui::TableSetupColumn("Y##ModelHitbox", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("________").x);
			ImGui::TableSetupColumn("Z##ModelHitbox");
			ImGui::TableNextRow();

			{
				ImGui::TableNextColumn(); ImGui::Text(" Width");
				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelHitboxWidth", ImGuiDataType_S32, &Player->Hitbox().w))
				{
					Player->Hitbox().w = std::clamp(Player->Hitbox().w, 0, 5400);
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
				if (ScrollOnHover(&Player->Hitbox().w, ImGuiDataType_S32, 32, 0, 5400))
				{
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();

				ImGui::TableNextColumn(); ImGui::Text(" Height");
				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelHitboxHeight", ImGuiDataType_S32, &Player->Hitbox().h)) { Player->Hitbox().h = std::clamp(Player->Hitbox().h, -5400, 0); }
				ScrollOnHover(&Player->Hitbox().h, ImGuiDataType_S32, 32, -5400, 0);
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();

				ImGui::TableNextColumn(); ImGui::Text(" Depth");
				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelHitboxDepth", ImGuiDataType_S32, &Player->Hitbox().d))
				{
					Player->Hitbox().d = std::clamp(Player->Hitbox().d, 0, 5400);
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
				if (ScrollOnHover(&Player->Hitbox().d, ImGuiDataType_S32, 32, 0, 5400))
				{
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();
			}

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Shadow##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::MenuItem(" View##ModelDraw", NULL, &Player->b_DrawShadow);
		TooltipOnHover("Draw the model's shadow");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::BeginTable("Transform##ModelShadow", 4))
		{
			ImGui::TableSetupColumn("Label##ModelShadow", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("________").x);
			ImGui::TableSetupColumn("X##ModelShadow", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("_________").x);
			ImGui::TableSetupColumn("Y##ModelShadow", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("_________").x);
			ImGui::TableSetupColumn("Z##ModelShadow");
			ImGui::TableNextRow();

			size_t PaletteCount = !Player->ModelDX9() || Player->ModelDX9()->Texture.empty() ? 0 : Player->ModelDX9()->Texture.size() - 1;
			ImGui::TableNextColumn(); ImGui::Text(" Palette");
			ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
			if (ImGui::InputScalar("##ModelShadowPalette", ImGuiDataType_U64, &Player->Shadow().TexID))
			{
				Player->Shadow().TexID = std::clamp(Player->Shadow().TexID, (size_t)0, PaletteCount);
				Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
			}
			if (ScrollOnHover(&Player->Shadow().TexID, ImGuiDataType_U64, 1, 0, PaletteCount))
			{
				Player->Shadow().TexID = std::clamp(Player->Shadow().TexID, (size_t)0, PaletteCount);
				Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
			}
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();

			static float Dummy = 0;
			float& TextureWidth = !Player->ModelDX9() ? Dummy : Player->ModelDX9()->TextureWidth;
			float& TextureHeight = !Player->ModelDX9() ? Dummy : Player->ModelDX9()->TextureHeight;

			{
				ImGui::TableNextColumn(); ImGui::Text(" X,Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelShadowX", ImGuiDataType_Float, &Player->Shadow().Rect.l))
				{
					Player->Shadow().Rect.l = std::clamp(Player->Shadow().Rect.l, 0.0f, TextureWidth);
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
				if (ScrollFloatOnHover(&Player->Shadow().Rect.l, ImGuiDataType_Float, 1.0, 0.0, TextureWidth))
				{
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelShadowY", ImGuiDataType_Float, &Player->Shadow().Rect.t))
				{
					Player->Shadow().Rect.t = std::clamp(Player->Shadow().Rect.t, 0.0f, TextureHeight);
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
				if (ScrollFloatOnHover(&Player->Shadow().Rect.t, ImGuiDataType_Float, 1.0, 0.0, TextureHeight))
				{
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);  ImGui::Text(" ");
			}

			{
				ImGui::TableNextColumn(); ImGui::Text(" W,H");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelShadowW", ImGuiDataType_Float, &Player->Shadow().Size.w))
				{
					Player->Shadow().Size.w = std::clamp(Player->Shadow().Size.w, 0.0f, TextureWidth);
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
				if (ScrollFloatOnHover(&Player->Shadow().Size.w, ImGuiDataType_Float, 1.0, 0.0, TextureWidth))
				{
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelShadowH", ImGuiDataType_Float, &Player->Shadow().Size.h))
				{
					Player->Shadow().Size.h = std::clamp(Player->Shadow().Size.h, 0.0f, TextureHeight);
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
				if (ScrollFloatOnHover(&Player->Shadow().Size.h, ImGuiDataType_Float, 1.0, 0.0, TextureHeight))
				{
					Player->SetShadow(Player->Shadow().TexID, (int16_t)Player->Shadow().Rect.l, (int16_t)Player->Shadow().Rect.t, (int16_t)Player->Shadow().Size.w, (int16_t)Player->Shadow().Size.h);
				}
			}

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Weapon##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::BeginDisabled(Player->WeaponModelDX9() ? Player->WeaponModelDX9()->Object.empty() : true);

		if (ImGui::BeginTable("Weapon##ModelWeapon", 2))
		{
			ImGui::TableNextColumn();
			ImGui::MenuItem(" Active##ModelWeapon", NULL, &Player->b_DrawWeapon);
			TooltipOnHover("Draw weapon model");

			ImGui::TableNextColumn();
			ImGui::BeginDisabled(!Player->b_DrawWeapon);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##ModelWeaponID", ImGuiDataType_U64, &Player->iWeaponObject, &Player->iWeaponObjectMin, &Player->iWeaponObjectMax))
			{
				Player->iWeaponObject = std::clamp(Player->iWeaponObject, (size_t)0, Player->iWeaponObjectMax);
			}
			TooltipOnHover("ID of model object to replace with weapon model");
			ScrollOnHover(&Player->iWeaponObject, ImGuiDataType_U64, 1, Player->iWeaponObjectMin, Player->iWeaponObjectMax);
			ImGui::EndDisabled();

			ImGui::EndTable();
		}

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		bool b_Value = Player->b_WeaponKickback.load();
		if (ImGui::MenuItem(" Kickback##ModelWeapon", NULL, &b_Value))
		{
			Player->b_WeaponKickback.store(b_Value);
		}
		TooltipOnHover("Position change on weapon discharge");

		ImGui::EndDisabled();
	}

	if (ImGui::CollapsingHeader("Health##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::BeginDisabled(!Player->b_ControllerMode);
		if (ImGui::BeginTable("ModelHealthPanel##ModelRenderHealth", 2))
		{
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Health").x);
			ImGui::Text(" Health");

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::SliderScalar("##ModelRenderHealth", ImGuiDataType_S32, &Player->iHealth, &Player->iHealthMin, &Player->iHealthMax);
			ScrollOnHover(&Player->iHealth, ImGuiDataType_S32, 1, Player->iHealthMin, Player->iHealthMax);

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Health").x);
			ImGui::Text(" Max");

			std::int32_t HealthMax = 32768;

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputScalar("##ModelRenderHealthMax", ImGuiDataType_S32, &Player->iHealthMax))
			{
				if (Player->iHealth > Player->iHealthMax) { Player->iHealth = Player->iHealthMax; }
			}
			if (ScrollOnHover(&Player->iHealthMax, ImGuiDataType_S32, 1, Player->iHealthMin, HealthMax))
			{
				if (Player->iHealth > Player->iHealthMax) { Player->iHealth = Player->iHealthMax; }
			}

			ImGui::EndTable();
		}
		ImGui::EndDisabled();
	}

	if (ImGui::CollapsingHeader("Transform##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		TooltipOnHover("Position, Rotation and Scale");

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
				if (ImGui::InputScalar("##ModelTransformPosX", ImGuiDataType_S32, &Player->EditorPosition().x))
				{
					Player->EditorPosition().x = std::clamp(Player->EditorPosition().x, -32767, 32767);
				}
				ScrollOnHover(&Player->EditorPosition().x, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformPosY", ImGuiDataType_S32, &Player->EditorPosition().y))
				{
					Player->EditorPosition().y = std::clamp(Player->EditorPosition().y, -32767, 32767);
				}
				ScrollOnHover(&Player->EditorPosition().y, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformPosZ", ImGuiDataType_S32, &Player->EditorPosition().z))
				{
					Player->EditorPosition().z = std::clamp(Player->EditorPosition().z, -32767, 32767);
				}
				ScrollOnHover(&Player->EditorPosition().z, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position Z");
			}

			{
				ImGui::TableNextColumn(); ImGui::Text(" Rot");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformRotX", ImGuiDataType_S32, &Player->EditorRotation().x))
				{
					Player->EditorRotation().x = std::clamp(Player->EditorRotation().x, -4096, 4096);
				}
				ScrollOnHover(&Player->EditorRotation().x, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformRotY", ImGuiDataType_S32, &Player->EditorRotation().y))
				{
					Player->EditorRotation().y = std::clamp(Player->EditorRotation().y, -4096, 4096);
				}
				ScrollOnHover(&Player->EditorRotation().y, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformRotZ", ImGuiDataType_S32, &Player->EditorRotation().z))
				{
					Player->EditorRotation().z = std::clamp(Player->EditorRotation().z, -4096, 4096);
				}
				ScrollOnHover(&Player->EditorRotation().z, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation Z");
			}

			{
				ImGui::TableNextColumn(); ImGui::Text(" Sx");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformScaleX", ImGuiDataType_S32, &Player->EditorScale().x))
				{
					Player->EditorScale().x = std::clamp(Player->EditorScale().x, 0, 32768);
				}
				ScrollOnHover(&Player->EditorScale().x, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformScaleY", ImGuiDataType_S32, &Player->EditorScale().y))
				{
					Player->EditorScale().y = std::clamp(Player->EditorScale().y, 0, 32768);
				}
				ScrollOnHover(&Player->EditorScale().y, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformScaleZ", ImGuiDataType_S32, &Player->EditorScale().z))
				{
					Player->EditorScale().z = std::clamp(Player->EditorScale().z, 0, 32768);
				}
				ScrollOnHover(&Player->EditorScale().z, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale Z");
			}

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Playback##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::BeginDisabled((!Player->ModelDX9() || !Player->WeaponModelDX9()));
		if (ImGui::MenuItem(" Controller##ModelPlayback", NULL, &Player->b_ControllerMode))
		{
			SetController(Player->b_ControllerMode);
		}
		TooltipOnHover("Controller input on/off\r\n\r\nWeapon must be open");
		ImGui::EndDisabled();

		ImGui::BeginDisabled(Player->b_ControllerMode);

		if (ImGui::MenuItem(" Root##ModelPlayback", NULL, &Player->b_DrawReference))
		{
			Player->b_DrawSingleObject = false;
			Player->b_DrawAllObjects = false;
		}
		TooltipOnHover("Ignore keyframes, use skeleton reference instead");

		ImGui::MenuItem(" Lock##ModelPlayback", NULL, &Player->b_LockPosition);
		TooltipOnHover("Lock model in position when drawing keyframes");

		bool b_Value = Player->b_Play.load();
		if (ImGui::MenuItem(" Play##ModelPlayback", NULL, &b_Value)) { Player->b_Play.store(!Player->b_Play.load()); }
		TooltipOnHover("Play/Pause clip playback");

		b_Value = Player->b_Loop.load();
		if (ImGui::MenuItem(" Loop##ModelPlayback", NULL, &b_Value)) { Player->b_Loop.store(!Player->b_Loop.load()); }
		TooltipOnHover("Loop clip playback");

		ImGui::EndDisabled();

		if (ImGui::BeginTable("Lerp##ModelPlayback", 2))
		{
			ImGui::TableNextColumn();
			b_Value = Player->b_LerpKeyframes.load();
			if (ImGui::MenuItem(" Lerp##ModelPlayback", NULL, &b_Value)) { Player->b_LerpKeyframes.store(!Player->b_LerpKeyframes.load()); }

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderFloat("##ModelPlaybackLerp", &Player->m_LerpValue, 0.0f, 1.0f))
			{
				Player->m_LerpValue = std::round(Player->m_LerpValue * 20.0f) / 20.0f;
			}
			ScrollFloatOnHover(&Player->m_LerpValue, ImGuiDataType_Float, 0.05f, 0.0f, 1.0f);

			ImGui::EndTable();
		}

		ImGui::BeginDisabled(Player->b_ControllerMode);
		if (ImGui::BeginTable("Clip##ModelPlayback", 2))
		{
			ImGui::TableNextColumn(); ImGui::Text(" Clip");

			size_t ClipCount = Player->Animation(Player->AnimIndex())->GetClipCount();
			size_t iClipMin = 0;
			size_t iClipMax = ClipCount ? ClipCount - 1 : 0;

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			ImGui::SliderScalar("##ModelPlaybackClip", ImGuiDataType_U64, &Player->iClip, &iClipMin, &iClipMax);
			ScrollOnHover(&Player->iClip, ImGuiDataType_U64, 1, iClipMin, iClipMax);

			ImGui::EndTable();
		}
		ImGui::EndDisabled();

		if (ImGui::BeginTable("Keyframe##ModelPlayback", 2))
		{
			ImGui::TableNextColumn(); ImGui::Text(" Keyframe");

			size_t FrameCount = Player->Animation(Player->AnimIndex())->GetFrameCount(Player->iClip);
			size_t iFrameMin = 0;
			size_t iFrameMax = FrameCount ? FrameCount - 1 : 0;
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			ImGui::SliderScalar("##ModelPlaybackKeyframe", ImGuiDataType_U64, &Player->iFrame, &iFrameMin, &iFrameMax);
			ScrollOnHover(&Player->iFrame, ImGuiDataType_U64, 1, iFrameMin, iFrameMax);

			ImGui::EndTable();
		}
	}
}