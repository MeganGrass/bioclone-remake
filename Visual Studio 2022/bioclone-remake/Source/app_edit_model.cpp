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
				OpenPlayerModel();
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
				OpenPlayerTexture();
			}
			if (ImGui::MenuItem("Save As##ModelTextureMenu"))
			{
				SavePlayerTexture();
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

		if (ImGui::MenuItem(" Normal##ModelAnimationIndex", NULL, &b_Normal)) { Player->ResetClip(); Player->AnimIndex(AnimationIndex::Normal); }
		TooltipOnHover("Normal Animation");
		if (ImGui::MenuItem(" Normal Ex0##ModelAnimationIndex", NULL, &b_NormalEx0)) { Player->ResetClip(); Player->AnimIndex(AnimationIndex::NormalEx0); }
		TooltipOnHover("Normal Animation Extra\r\nEMD (Bio2/Bio3)");
		if (ImGui::MenuItem(" Normal Ex1##ModelAnimationIndex", NULL, &b_NormalEx1)) { Player->ResetClip(); Player->AnimIndex(AnimationIndex::NormalEx1); }
		TooltipOnHover("Normal Animation Extra\r\nEMD (Bio3)");
		if (ImGui::MenuItem(" Damage##ModelAnimationIndex", NULL, &b_Damage)) { Player->ResetClip(); Player->AnimIndex(AnimationIndex::Damage); }
		TooltipOnHover("Damage Animation\r\nEMD, for use with Player");
		if (ImGui::MenuItem(" Weapon##ModelAnimationIndex", NULL, &b_Weapon)) { Player->ResetClip(); Player->AnimIndex(AnimationIndex::Weapon); }
		TooltipOnHover("Weapon Animation\r\nEMW (Bio1) or PLW (Bio2/Bio3)");
		if (ImGui::MenuItem(" Weapon Ex0##ModelAnimationIndex", NULL, &b_WeaponEx0)) { Player->ResetClip(); Player->AnimIndex(AnimationIndex::WeaponEx0); }
		TooltipOnHover("Weapon Animation Ex0\r\nPLW/EMD (Bio3), unknown purpose");
		if (ImGui::MenuItem(" Weapon Ex1##ModelAnimationIndex", NULL, &b_WeaponEx1)) { Player->ResetClip(); Player->AnimIndex(AnimationIndex::WeaponEx1); }
		TooltipOnHover("Weapon Animation Ex1\r\nPLW/EMD (Bio3)");

		if (ImGui::BeginTable("Room##ModelAnimationIndexTable", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::MenuItem(" Room##ModelAnimationIndexTable", NULL, &b_Room)) { Player->ResetClip(); Player->AnimIndex(AnimationIndex::Room); }
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
		TooltipOnHover("Sony PlayStation (1994) Dithering Pixel Shader");

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
			Player->b_DrawWireframe = true;
			Player->b_DrawSolidColor = false;
			Player->b_DrawTextured = false;
		}
		TooltipOnHover("Polygons will be drawn as wireframe");

		if (ImGui::MenuItem(" Solid##ModelRender", NULL, &Player->b_DrawSolidColor))
		{
			Player->b_DrawWireframe = false;
			Player->b_DrawSolidColor = true;
			Player->b_DrawTextured = false;
		}
		TooltipOnHover("Polygons will be drawn as solid color");

		if (ImGui::MenuItem(" Textured##ModelRender", NULL, &Player->b_DrawTextured))
		{
			Player->b_DrawWireframe = false;
			Player->b_DrawSolidColor = false;
			Player->b_DrawTextured = true;
		}
		TooltipOnHover("Polygons will be drawn as textured");

		ImGui::MenuItem(" Skeleton##ModelRender", NULL, &Player->b_DrawSkeletonMesh);
		TooltipOnHover("Skeleton mesh will be drawn");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);
	}

	if (ImGui::CollapsingHeader("Draw##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		ImGui::MenuItem(" Hitbox##ModelDraw", NULL, &Player->b_DrawHitbox);
		TooltipOnHover("Draw the model's interactive/collision hitbox");

		if (ImGui::MenuItem(" All Objects##ModelDraw", NULL, &Player->b_DrawAllObjects))
		{
			Player->b_DrawReference = false;
			Player->b_DrawSingleObject = false;
		}
		TooltipOnHover("Ignore keyframes and skeleton and draw all objects\r\nOnly draw weapon when \"Weapon\" is active");

		if (ImGui::BeginTable("ModelDrawPanel##ModelDraw", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::MenuItem(" Object##ModelDraw", NULL, &Player->b_DrawSingleObject))
			{
				Player->b_DrawReference = false;
				Player->b_DrawAllObjects = false;
			}
			TooltipOnHover(Standard_String().FormatCStyle("Ignore keyframes and skeleton and draw all object %d\r\nOnly draw weapon when \"Weapon\" is active", Player->iObject));

			ImGui::TableNextColumn();
			ImGui::BeginDisabled(!Player->b_DrawSingleObject);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##ModelDrawObject", ImGuiDataType_U64, &Player->iObject, &Player->iObjectMin, &Player->iObjectMax))
			{
				Player->iObject = std::clamp(Player->iObject, (size_t)0, Player->iObjectMax);
			}
			ScrollOnHover(&Player->iObject, ImGuiDataType_U64, 1, Player->iObjectMin, Player->iObjectMax);
			ImGui::EndDisabled();

			ImGui::EndTable();
		}

		ImGui::BeginDisabled(Player->WeaponModelDX9() ? Player->WeaponModelDX9()->Object.empty() : true);
		if (ImGui::BeginTable("Weapon##ModelDrawWeapon", 2))
		{
			ImGui::TableNextColumn();
			ImGui::MenuItem(" Weapon##ModelDraw", NULL, &Player->b_DrawWeapon);
			TooltipOnHover("Draw weapon model");

			ImGui::TableNextColumn();
			ImGui::BeginDisabled(!Player->b_DrawWeapon);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##ModelDrawWeaponID", ImGuiDataType_U64, &Player->iWeaponObject, &Player->iWeaponObjectMin, &Player->iWeaponObjectMax))
			{
				Player->iWeaponObject = std::clamp(Player->iWeaponObject, (size_t)0, Player->iWeaponObjectMax);
			}
			TooltipOnHover("ID of model object to replace with weapon model");
			ScrollOnHover(&Player->iWeaponObject, ImGuiDataType_U64, 1, Player->iWeaponObjectMin, Player->iWeaponObjectMax);
			ImGui::EndDisabled();

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
		if (ImGui::MenuItem(" Root##ModelPlayback", NULL, &Player->b_DrawReference))
		{
			Player->b_DrawSingleObject = false;
			Player->b_DrawAllObjects = false;
		}
		TooltipOnHover("Ignore keyframes, use skeleton reference instead");

		ImGui::MenuItem(" Lock##ModelPlayback", NULL, &Player->b_LockPosition);
		TooltipOnHover("Lock model in position when drawing keyframes");

		ImGui::MenuItem(" Play##ModelPlayback", NULL, &Player->b_Play);
		TooltipOnHover("Play/Pause clip playback");

		ImGui::MenuItem(" Loop##ModelPlayback", NULL, &Player->b_Loop);
		TooltipOnHover("Loop clip playback");

		if (ImGui::BeginTable("Lerp##ModelPlayback", 2))
		{
			ImGui::TableNextColumn(); ImGui::MenuItem(" Lerp##ModelPlayback", NULL, &Player->b_LerpKeyframes);

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderFloat("##ModelPlaybackLerp", &Player->m_LerpValue, 0.0f, 1.0f))
			{
				Player->m_LerpValue = std::round(Player->m_LerpValue * 20.0f) / 20.0f;
			}
			ScrollFloatOnHover(&Player->m_LerpValue, ImGuiDataType_Float, 0.05f, 0.0f, 1.0f);

			ImGui::EndTable();
		}

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