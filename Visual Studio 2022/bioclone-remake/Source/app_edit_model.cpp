/*
*
*	Megan Grass
*	July 26, 2025
*
*/

#include "app.h"

std::shared_ptr<Resident_Evil_Model> Global_Application::Model = std::shared_ptr<Resident_Evil_Model>();

void Global_Application::ModelEditor(const bool b_RoomModel)
{
	if (b_FileOp.load()) { return; }

	if (!b_RoomModel) { Model = Player; }

	if (b_RoomModel)
	{
		if (Model == Player) { Model.reset(); }

		Room->iItemMax = (Room->Item.empty() ? 0 : Room->Item.size() - 1);
		Room->iObjectMax = (Room->Object.empty() ? 0 : Room->Object.size() - 1);

		bool& b_Item = Room->b_EditorItem;
		bool& b_Object = Room->b_EditorObject;
		std::uintmax_t& iItem = Room->iItem, iItemMin = Room->iItemMin, iItemMax = Room->iItemMax;
		std::uintmax_t& iObject = Room->iObject, iObjectMin = Room->iObjectMin, iObjectMax = Room->iObjectMax;

		if (ImGui::BeginTable("Item##RoomModelType", 2))
		{
			{
				ImGui::BeginDisabled(Room->Item.empty() || (GameType() & (AUG95 | OCT95 | BIO1)) == 0);
				ImGui::TableNextColumn();
				if (ImGui::MenuItem(" Item##ItemModelIndex", NULL, &b_Item))
				{
					Room->b_EditorObject = false;
					if (b_Item) { Model = Room->Item[iItem]; } else { Model.reset(); }
				}

				ImGui::TableNextColumn();
				if (ImGui::SliderScalar("##ItemModelIndex", ImGuiDataType_U64, &iItem, &iItemMin, &iItemMax))
				{
					if (b_Item) { Model = Room->Item[iItem]; } else { Model.reset(); }
				}
				ScrollOnHover(&iItem, ImGuiDataType_U64, 1, iItemMin, iItemMax, [&]() { if (b_Item) { Model = Room->Item[iItem]; } });
				ImGui::EndDisabled();
			}

			{
				ImGui::BeginDisabled(Room->Object.empty());
				ImGui::TableNextColumn();
				if (ImGui::MenuItem(" Object##ObjectModelIndex", NULL, &b_Object))
				{
					Room->b_EditorItem = false;
					if (b_Object) { Model = Room->Object[iObject]; } else { Model.reset(); }
				}

				ImGui::TableNextColumn();
				if (ImGui::SliderScalar("##ObjectModelIndex", ImGuiDataType_U64, &iObject, &iObjectMin, &iObjectMax))
				{
					if (b_Object) { Model = Room->Object[iObject]; } else { Model.reset(); }
				}
				ScrollOnHover(&iObject, ImGuiDataType_U64, 1, iObjectMin, iObjectMax, [&]() { if (b_Object) { Model = Room->Object[iObject]; } });
				ImGui::EndDisabled();
			}

			ImGui::EndTable();
		}
	}

	if (Room->b_EditModel && !Model.get())
	{
		return;
	}

	if (Room->b_EditModel && !Model->ModelDX9())
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);
		ImGui::TextWrapped(" Empty Slot");
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File##ModelFileMenu"))
		{
			if (ImGui::MenuItem("Open##ModelFileMenu"))
			{
				FileOp.Enqueue([this]() { OpenModel(Model, (Model == Player)); });
			}
			if (ImGui::MenuItem("Save As##ModelFileMenu", NULL, false, false))
			{
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close##ModelFileMenu", NULL, false, (Model == Player)))
			{
				Model->Close();
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
				FileOp.Enqueue([this]() { OpenModelTexture(Model); });
			}
			if (ImGui::MenuItem("Save As##ModelTextureMenu"))
			{
				FileOp.Enqueue([this]() { SaveModelTexture(Model); });
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
		bool b_Bio1Alpha = Model->GameType() & (AUG95 | OCT95);
		bool b_Bio1 = Model->GameType() & (BIO1);
		bool b_Bio2Nov96 = Model->GameType() & (BIO2NOV96);
		bool b_Bio2 = Model->GameType() & (BIO2);
		bool b_Bio3 = Model->GameType() & (BIO3);

		if (ImGui::BeginTable("Table##ModelType", 2))
		{
			TooltipOnHover("Model Game Type on Open/Save");
			ImGui::TableNextColumn();
			if (ImGui::Checkbox(" Bio1 Alpha##ModelType", &b_Bio1Alpha)) { Model->SetGame(Video_Game::Resident_Evil_Aug_4_1995); }
			TooltipOnHover("Resident Evil (Aug/Oct 1995) Prototype");
			ImGui::TableNextColumn();

			ImGui::BeginDisabled(true);
			if (ImGui::Checkbox(" Bio1 Enemy##ModelType", &Model->b_Bio1Enemy)) {  }
			TooltipOnHover("Bio1 EMD is enemy type on Open\r\nAssume player type otherwise");
			ImGui::EndDisabled();

			ImGui::TableNextColumn();
			if (ImGui::Checkbox(" Bio1##ModelType", &b_Bio1)) { Model->SetGame(Video_Game::Resident_Evil); }
			TooltipOnHover("Resident Evil");
			if (ImGui::Checkbox(" Bio2 Nov '96##ModelType", &b_Bio2Nov96)) { Model->SetGame(Video_Game::Resident_Evil_2_Nov_6_1996); }
			TooltipOnHover("Resident Evil 2 (Nov 1996) Prototype");
			if (ImGui::Checkbox(" Bio2##ModelType", &b_Bio2)) { Model->SetGame(Video_Game::Resident_Evil_2); }
			TooltipOnHover("Resident Evil 2");
			if (ImGui::Checkbox(" Bio3##ModelType", &b_Bio3)) { Model->SetGame(Video_Game::Resident_Evil_3); }
			TooltipOnHover("Resident Evil 3");
			ImGui::EndTable();
		}
	}

	if (!b_RoomModel && ImGui::CollapsingHeader("Index##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		TooltipOnHover("Animation Type");

		bool b_Normal = Model->AnimIndex() == AnimationIndex::Normal;
		bool b_NormalEx0 = Model->AnimIndex() == AnimationIndex::NormalEx0;
		bool b_NormalEx1 = Model->AnimIndex() == AnimationIndex::NormalEx1;
		bool b_Damage = Model->AnimIndex() == AnimationIndex::Damage;
		bool b_Weapon = Model->AnimIndex() == AnimationIndex::Weapon;
		bool b_WeaponEx0 = Model->AnimIndex() == AnimationIndex::WeaponEx0;
		bool b_WeaponEx1 = Model->AnimIndex() == AnimationIndex::WeaponEx1;
		bool b_Room = Model->AnimIndex() == AnimationIndex::Room;

		if (ImGui::MenuItem(" Normal##ModelAnimationIndex", NULL, &b_Normal)) { Model->ResetClip(); Model->SetAnimIndex(AnimationIndex::Normal); }
		TooltipOnHover("Normal Animation");
		if (ImGui::MenuItem(" Normal Ex0##ModelAnimationIndex", NULL, &b_NormalEx0)) { Model->ResetClip(); Model->SetAnimIndex(AnimationIndex::NormalEx0); }
		TooltipOnHover("Normal Animation Extra\r\nEMD (Bio2/Bio3)");
		if (ImGui::MenuItem(" Normal Ex1##ModelAnimationIndex", NULL, &b_NormalEx1)) { Model->ResetClip(); Model->SetAnimIndex(AnimationIndex::NormalEx1); }
		TooltipOnHover("Normal Animation Extra\r\nEMD (Bio3)");
		if (ImGui::MenuItem(" Damage##ModelAnimationIndex", NULL, &b_Damage)) { Model->ResetClip(); Model->SetAnimIndex(AnimationIndex::Damage); }
		TooltipOnHover("Damage Animation\r\nEMD, for use with Player");
		if (ImGui::MenuItem(" Weapon##ModelAnimationIndex", NULL, &b_Weapon)) { Model->ResetClip(); Model->SetAnimIndex(AnimationIndex::Weapon); }
		TooltipOnHover("Weapon Animation\r\nEMW (Bio1) or PLW (Bio2/Bio3)");
		if (ImGui::MenuItem(" Weapon Ex0##ModelAnimationIndex", NULL, &b_WeaponEx0)) { Model->ResetClip(); Model->SetAnimIndex(AnimationIndex::WeaponEx0); }
		TooltipOnHover("Weapon Animation Ex0\r\nPLW/EMD (Bio3), unknown purpose");
		if (ImGui::MenuItem(" Weapon Ex1##ModelAnimationIndex", NULL, &b_WeaponEx1)) { Model->ResetClip(); Model->SetAnimIndex(AnimationIndex::WeaponEx1); }
		TooltipOnHover("Weapon Animation Ex1\r\nPLW/EMD (Bio3)");

		if (ImGui::BeginTable("Room##ModelAnimationIndexTable", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::MenuItem(" Room##ModelAnimationIndexTable", NULL, &b_Room)) { Model->ResetClip(); Model->SetAnimIndex(AnimationIndex::Room); }
			TooltipOnHover("Room Animation\r\nRDT");

			ImGui::TableNextColumn();
			ImGui::BeginDisabled(!IsRoomOpen() || Room->Rbj->Data.empty());
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##ModelAnimationIndex", ImGuiDataType_U64, &Model->iRoom, &Model->iRoomMin, &Model->iRoomMax))
			{
				Model->iRoom = std::clamp(Model->iRoom, (size_t)0, Model->iRoomMax);
				Model->Animation(AnimationIndex::Room) = Room->Rbj->Data[Model->iRoom];
			}
			ScrollOnHover(&Model->iRoom, ImGuiDataType_U64, 1, Model->iRoomMin, Model->iRoomMax);
			ImGui::EndDisabled();
			TooltipOnHover("Room Animation Container ID");

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Render##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::MenuItem(" Dither##ModelRender", NULL, &Model->b_Dither);
		TooltipOnHover("Sony PlayStation (1994) Dithering Pixel Shader\r\n\r\nValid only when \"Textured\" is active");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		bool b_NoFilter = Model->m_TextureFilter == D3DTEXF_NONE || Model->m_TextureFilter == D3DTEXF_POINT;
		bool b_LinearFilter = Model->m_TextureFilter == D3DTEXF_LINEAR;
		bool b_AnisotropicFilter = Model->m_TextureFilter == D3DTEXF_ANISOTROPIC;

		if (ImGui::MenuItem(" Point##ModelRender", NULL, &b_NoFilter)) { Model->m_TextureFilter = D3DTEXF_POINT; }
		TooltipOnHover("Texture Filter");

		if (ImGui::MenuItem(" Linear##ModelRender", NULL, &b_LinearFilter)) { Model->m_TextureFilter = D3DTEXF_LINEAR; }
		TooltipOnHover("Texture Filter");

		if (ImGui::MenuItem(" Anisotropic##ModelRender", NULL, &b_AnisotropicFilter)) { Model->m_TextureFilter = D3DTEXF_ANISOTROPIC; }
		TooltipOnHover("Texture Filter");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::MenuItem(" Wireframe##ModelRender", NULL, &Model->b_DrawWireframe))
		{
			Model->b_DrawWireframe = Model->b_DrawWireframe ? true : false;
			Model->b_DrawSolidColor = false;
			Model->b_DrawTextured = false;
		}
		TooltipOnHover("Polygons will be drawn as wireframe");

		if (ImGui::MenuItem(" Solid##ModelRender", NULL, &Model->b_DrawSolidColor))
		{
			Model->b_DrawWireframe = false;
			Model->b_DrawSolidColor = Model->b_DrawSolidColor ? true : false;
			Model->b_DrawTextured = false;
		}
		TooltipOnHover("Polygons will be drawn as solid color");

		if (ImGui::MenuItem(" Textured##ModelRender", NULL, &Model->b_DrawTextured))
		{
			Model->b_DrawWireframe = false;
			Model->b_DrawSolidColor = false;
			Model->b_DrawTextured = Model->b_DrawTextured ? true : false;
		}
		TooltipOnHover("Polygons will be drawn as textured");

		ImGui::MenuItem(" Skeleton##ModelRender", NULL, &Model->b_DrawSkeleton);
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

		if (ImGui::MenuItem(" All Objects##ModelObject", NULL, &Model->b_DrawAllObjects))
		{
			Model->b_DrawReference = false;
			Model->b_DrawSingleObject = false;
		}
		TooltipOnHover("Draw all objects\r\n\r\nKeyframes and skeleton are ignored\r\n\r\nOnly draw weapon when \"Weapon\" is active");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::BeginTable("ModelObjectPanel##ModelObject", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::MenuItem(" Object##ModelObject", NULL, &Model->b_DrawSingleObject))
			{
				Model->b_DrawReference = false;
				Model->b_DrawAllObjects = false;
			}
			TooltipOnHover(Standard_String().FormatCStyle("Draw object %d\r\n\r\nKeyframes and skeleton are ignored\r\n\r\nOnly draw weapon when \"Weapon\" is active", Model->iObject));

			ImGui::TableNextColumn();
			ImGui::BeginDisabled(!Model->b_DrawSingleObject);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##ModelObjectObject", ImGuiDataType_U64, &Model->iObject, &Model->iObjectMin, &Model->iObjectMax))
			{
				Model->iObject = std::clamp(Model->iObject, (size_t)0, Model->iObjectMax);
			}
			ScrollOnHover(&Model->iObject, ImGuiDataType_U64, 1, Model->iObjectMin, Model->iObjectMax);
			ImGui::EndDisabled();

			ImGui::EndTable();
		}
	}

	if (!b_RoomModel && ImGui::CollapsingHeader("Hitbox##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::MenuItem(" View##ModelHitbox", NULL, &Model->b_DrawHitbox);
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
				if (ImGui::InputScalar("##ModelHitboxWidth", ImGuiDataType_S32, &Model->Hitbox().w))
				{
					Model->Hitbox().w = std::clamp(Model->Hitbox().w, 0, 5400);
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
				if (ScrollOnHover(&Model->Hitbox().w, ImGuiDataType_S32, 32, 0, 5400))
				{
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();

				ImGui::TableNextColumn(); ImGui::Text(" Height");
				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelHitboxHeight", ImGuiDataType_S32, &Model->Hitbox().h)) { Model->Hitbox().h = std::clamp(Model->Hitbox().h, -5400, 0); }
				ScrollOnHover(&Model->Hitbox().h, ImGuiDataType_S32, 32, -5400, 0);
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();

				ImGui::TableNextColumn(); ImGui::Text(" Depth");
				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelHitboxDepth", ImGuiDataType_S32, &Model->Hitbox().d))
				{
					Model->Hitbox().d = std::clamp(Model->Hitbox().d, 0, 5400);
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
				if (ScrollOnHover(&Model->Hitbox().d, ImGuiDataType_S32, 32, 0, 5400))
				{
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
				ImGui::TableNextColumn();
				ImGui::TableNextColumn();
			}

			ImGui::EndTable();
		}
	}

	if (!b_RoomModel && ImGui::CollapsingHeader("Shadow##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::MenuItem(" View##ModelDraw", NULL, &Model->b_DrawShadow);
		TooltipOnHover("Draw the model's shadow");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::BeginTable("Transform##ModelShadow", 4))
		{
			ImGui::TableSetupColumn("Label##ModelShadow", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("________").x);
			ImGui::TableSetupColumn("X##ModelShadow", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("_________").x);
			ImGui::TableSetupColumn("Y##ModelShadow", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("_________").x);
			ImGui::TableSetupColumn("Z##ModelShadow");
			ImGui::TableNextRow();

			size_t PaletteCount = !Model->ModelDX9() || Model->ModelDX9()->Texture.empty() ? 0 : Model->ModelDX9()->Texture.size() - 1;
			ImGui::TableNextColumn(); ImGui::Text(" Palette");
			ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
			if (ImGui::InputScalar("##ModelShadowPalette", ImGuiDataType_U64, &Model->Shadow().TexID))
			{
				Model->Shadow().TexID = std::clamp(Model->Shadow().TexID, (size_t)0, PaletteCount);
				Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
			}
			if (ScrollOnHover(&Model->Shadow().TexID, ImGuiDataType_U64, 1, 0, PaletteCount))
			{
				Model->Shadow().TexID = std::clamp(Model->Shadow().TexID, (size_t)0, PaletteCount);
				Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
			}
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();

			static float Dummy = 0;
			float& TextureWidth = !Model->ModelDX9() ? Dummy : Model->ModelDX9()->TextureWidth;
			float& TextureHeight = !Model->ModelDX9() ? Dummy : Model->ModelDX9()->TextureHeight;

			{
				ImGui::TableNextColumn(); ImGui::Text(" X,Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelShadowX", ImGuiDataType_Float, &Model->Shadow().Rect.l))
				{
					Model->Shadow().Rect.l = std::clamp(Model->Shadow().Rect.l, 0.0f, TextureWidth);
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
				if (ScrollFloatOnHover(&Model->Shadow().Rect.l, ImGuiDataType_Float, 1.0, 0.0, TextureWidth))
				{
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelShadowY", ImGuiDataType_Float, &Model->Shadow().Rect.t))
				{
					Model->Shadow().Rect.t = std::clamp(Model->Shadow().Rect.t, 0.0f, TextureHeight);
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
				if (ScrollFloatOnHover(&Model->Shadow().Rect.t, ImGuiDataType_Float, 1.0, 0.0, TextureHeight))
				{
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);  ImGui::Text(" ");
			}

			{
				ImGui::TableNextColumn(); ImGui::Text(" W,H");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelShadowW", ImGuiDataType_Float, &Model->Shadow().Size.w))
				{
					Model->Shadow().Size.w = std::clamp(Model->Shadow().Size.w, 0.0f, TextureWidth);
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
				if (ScrollFloatOnHover(&Model->Shadow().Size.w, ImGuiDataType_Float, 1.0, 0.0, TextureWidth))
				{
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelShadowH", ImGuiDataType_Float, &Model->Shadow().Size.h))
				{
					Model->Shadow().Size.h = std::clamp(Model->Shadow().Size.h, 0.0f, TextureHeight);
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
				if (ScrollFloatOnHover(&Model->Shadow().Size.h, ImGuiDataType_Float, 1.0, 0.0, TextureHeight))
				{
					Model->SetShadow(Model->Shadow().TexID, (int16_t)Model->Shadow().Rect.l, (int16_t)Model->Shadow().Rect.t, (int16_t)Model->Shadow().Size.w, (int16_t)Model->Shadow().Size.h);
				}
			}

			ImGui::EndTable();
		}
	}

	if (!b_RoomModel && ImGui::CollapsingHeader("Weapon##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::BeginDisabled(Model->WeaponModelDX9() ? Model->WeaponModelDX9()->Object.empty() : true);

		if (ImGui::BeginTable("Weapon##ModelWeapon", 2))
		{
			ImGui::TableNextColumn();
			ImGui::MenuItem(" Active##ModelWeapon", NULL, &Model->b_DrawWeapon);
			TooltipOnHover("Draw weapon model");

			ImGui::TableNextColumn();
			ImGui::BeginDisabled(!Model->b_DrawWeapon);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderScalar("##ModelWeaponID", ImGuiDataType_U64, &Model->iWeaponObject, &Model->iWeaponObjectMin, &Model->iWeaponObjectMax))
			{
				Model->iWeaponObject = std::clamp(Model->iWeaponObject, (size_t)0, Model->iWeaponObjectMax);
			}
			TooltipOnHover("ID of model object to replace with weapon model");
			ScrollOnHover(&Model->iWeaponObject, ImGuiDataType_U64, 1, Model->iWeaponObjectMin, Model->iWeaponObjectMax);
			ImGui::EndDisabled();

			ImGui::EndTable();
		}

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		bool b_Value = Model->b_WeaponKickback.load();
		if (ImGui::MenuItem(" Kickback##ModelWeapon", NULL, &b_Value))
		{
			Model->b_WeaponKickback.store(b_Value);
		}
		TooltipOnHover("Position change on weapon discharge");

		ImGui::EndDisabled();
	}

	if (!b_RoomModel && ImGui::CollapsingHeader("Health##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::BeginDisabled(!Model->b_ControllerMode);
		if (ImGui::BeginTable("ModelHealthPanel##ModelRenderHealth", 2))
		{
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Health").x);
			ImGui::Text(" Health");

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::SliderScalar("##ModelRenderHealth", ImGuiDataType_S32, &Model->iHealth, &Model->iHealthMin, &Model->iHealthMax);
			ScrollOnHover(&Model->iHealth, ImGuiDataType_S32, 1, Model->iHealthMin, Model->iHealthMax);

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Health").x);
			ImGui::Text(" Max");

			std::int32_t HealthMax = 32768;

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputScalar("##ModelRenderHealthMax", ImGuiDataType_S32, &Model->iHealthMax))
			{
				if (Model->iHealth > Model->iHealthMax) { Model->iHealth = Model->iHealthMax; }
			}
			if (ScrollOnHover(&Model->iHealthMax, ImGuiDataType_S32, 1, Model->iHealthMin, HealthMax))
			{
				if (Model->iHealth > Model->iHealthMax) { Model->iHealth = Model->iHealthMax; }
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
				if (ImGui::InputScalar("##ModelTransformPosX", ImGuiDataType_S32, &Model->EditorPosition().x))
				{
					Model->EditorPosition().x = std::clamp(Model->EditorPosition().x, -32767, 32767);
				}
				ScrollOnHover(&Model->EditorPosition().x, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformPosY", ImGuiDataType_S32, &Model->EditorPosition().y))
				{
					Model->EditorPosition().y = std::clamp(Model->EditorPosition().y, -32767, 32767);
				}
				ScrollOnHover(&Model->EditorPosition().y, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformPosZ", ImGuiDataType_S32, &Model->EditorPosition().z))
				{
					Model->EditorPosition().z = std::clamp(Model->EditorPosition().z, -32767, 32767);
				}
				ScrollOnHover(&Model->EditorPosition().z, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position Z");
			}

			{
				ImGui::TableNextColumn(); ImGui::Text(" Rot");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformRotX", ImGuiDataType_S32, &Model->EditorRotation().x))
				{
					Model->EditorRotation().x = std::clamp(Model->EditorRotation().x, -4096, 4096);
				}
				ScrollOnHover(&Model->EditorRotation().x, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformRotY", ImGuiDataType_S32, &Model->EditorRotation().y))
				{
					Model->EditorRotation().y = std::clamp(Model->EditorRotation().y, -4096, 4096);
				}
				ScrollOnHover(&Model->EditorRotation().y, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformRotZ", ImGuiDataType_S32, &Model->EditorRotation().z))
				{
					Model->EditorRotation().z = std::clamp(Model->EditorRotation().z, -4096, 4096);
				}
				ScrollOnHover(&Model->EditorRotation().z, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation Z");
			}

			{
				ImGui::TableNextColumn(); ImGui::Text(" Sx");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformScaleX", ImGuiDataType_S32, &Model->EditorScale().x))
				{
					Model->EditorScale().x = std::clamp(Model->EditorScale().x, 0, 32768);
				}
				ScrollOnHover(&Model->EditorScale().x, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformScaleY", ImGuiDataType_S32, &Model->EditorScale().y))
				{
					Model->EditorScale().y = std::clamp(Model->EditorScale().y, 0, 32768);
				}
				ScrollOnHover(&Model->EditorScale().y, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##ModelTransformScaleZ", ImGuiDataType_S32, &Model->EditorScale().z))
				{
					Model->EditorScale().z = std::clamp(Model->EditorScale().z, 0, 32768);
				}
				ScrollOnHover(&Model->EditorScale().z, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale Z");
			}

			ImGui::EndTable();
		}
	}

	if (!b_RoomModel && ImGui::CollapsingHeader("Playback##ModelEditor", ImGuiTreeNodeFlags_None))
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::BeginDisabled((!Model->ModelDX9() || !Model->WeaponModelDX9()));
		if (ImGui::MenuItem(" Controller##ModelPlayback", NULL, &Model->b_ControllerMode))
		{
			SetController(Model->b_ControllerMode);
		}
		TooltipOnHover("Controller input on/off\r\n\r\nWeapon must be open");
		ImGui::EndDisabled();

		ImGui::BeginDisabled(Model->b_ControllerMode);

		if (ImGui::MenuItem(" Root##ModelPlayback", NULL, &Model->b_DrawReference))
		{
			Model->b_DrawSingleObject = false;
			Model->b_DrawAllObjects = false;
		}
		TooltipOnHover("Ignore keyframes, use skeleton reference instead");

		ImGui::MenuItem(" Lock##ModelPlayback", NULL, &Model->b_LockPosition);
		TooltipOnHover("Lock model in position when drawing keyframes");

		bool b_Value = Model->b_Play.load();
		if (ImGui::MenuItem(" Play##ModelPlayback", NULL, &b_Value)) { Model->b_Play.store(!Model->b_Play.load()); }
		TooltipOnHover("Play/Pause clip playback");

		b_Value = Model->b_Loop.load();
		if (ImGui::MenuItem(" Loop##ModelPlayback", NULL, &b_Value)) { Model->b_Loop.store(!Model->b_Loop.load()); }
		TooltipOnHover("Loop clip playback");

		ImGui::EndDisabled();

		if (ImGui::BeginTable("Lerp##ModelPlayback", 2))
		{
			ImGui::TableNextColumn();
			b_Value = Model->b_LerpKeyframes.load();
			if (ImGui::MenuItem(" Lerp##ModelPlayback", NULL, &b_Value)) { Model->b_LerpKeyframes.store(!Model->b_LerpKeyframes.load()); }

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			if (ImGui::SliderFloat("##ModelPlaybackLerp", &Model->m_LerpValue, 0.0f, 1.0f))
			{
				Model->m_LerpValue = std::round(Model->m_LerpValue * 20.0f) / 20.0f;
			}
			ScrollFloatOnHover(&Model->m_LerpValue, ImGuiDataType_Float, 0.05f, 0.0f, 1.0f);

			ImGui::EndTable();
		}

		ImGui::BeginDisabled(Model->b_ControllerMode);
		if (ImGui::BeginTable("Clip##ModelPlayback", 2))
		{
			ImGui::TableNextColumn(); ImGui::Text(" Clip");

			size_t ClipCount = Model->Animation(Model->AnimIndex())->GetClipCount();
			size_t iClipMin = 0;
			size_t iClipMax = ClipCount ? ClipCount - 1 : 0;

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			ImGui::SliderScalar("##ModelPlaybackClip", ImGuiDataType_U64, &Model->iClip, &iClipMin, &iClipMax);
			ScrollOnHover(&Model->iClip, ImGuiDataType_U64, 1, iClipMin, iClipMax);

			ImGui::EndTable();
		}
		ImGui::EndDisabled();

		if (ImGui::BeginTable("Keyframe##ModelPlayback", 2))
		{
			ImGui::TableNextColumn(); ImGui::Text(" Keyframe");

			size_t FrameCount = Model->Animation(Model->AnimIndex())->GetFrameCount(Model->iClip);
			size_t iFrameMin = 0;
			size_t iFrameMax = FrameCount ? FrameCount - 1 : 0;
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
			ImGui::SliderScalar("##ModelPlaybackKeyframe", ImGuiDataType_U64, &Model->iFrame, &iFrameMin, &iFrameMax);
			ScrollOnHover(&Model->iFrame, ImGuiDataType_U64, 1, iFrameMin, iFrameMax);

			ImGui::EndTable();
		}
	}
}