/*
*
*	Megan Grass
*	March 2, 2025
*
*/

#include "app.h"

void Global_Application::MainMenu(void)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File##MenuWindow"))
		{
			if (ImGui::MenuItem("Open##FileMenu", "CTRL+O"))
			{
				OpenRDT();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create##FileMenu", "CTRL+N"))
			{
				//
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save As##FileMenu", "CTRL+S"))
			{
				SaveRDT();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close##FileMenu", NULL))
			{
				CloseRDT();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Screenshot##FileMenu", NULL))
			{
				Screenshot();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit##FileMenu", "ESC"))
			{
				b_ForceShutdown = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View##MenuWindow"))
		{
			if (ImGui::MenuItem("Fullscreen##ViewMenu", "F11")) { Window->AutoFullscreen(); }
			ImGui::Separator();
			ImGui::MenuItem("Window Options##ViewMenu", NULL, &b_ViewWindowOptions);
			ImGui::EndMenu();
		}

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("?").x - ImGui::GetFontSize() / 2);

		if (ImGui::BeginMenu("?##MenuWindow"))
		{
			if (ImGui::MenuItem("Controls##HelpMenu", NULL))
			{
				Controls();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("About##HelpMenu", "ALT+?"))
			{
				About();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void Global_Application::Options(void)
{
	if (!b_ViewWindowOptions) { return; }

	ImGui::Begin("Window Options##OptionsWindow", &b_ViewWindowOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	if (ImGui::BeginCombo("Font##OptionsWindow", Window->FontStems()[Window->FontIndex()].c_str(), ImGuiComboFlags_HeightLarge))
	{
		for (std::size_t i = 0; i < Window->FontStems().size(); i++)
		{
			bool b_IsSelected = (Window->FontIndex() == i);

			if (ImGui::Selectable(Window->FontStems()[i].c_str(), b_IsSelected))
			{
				Window->SetFont(Window->FontList()[Window->FontIndex() = i], Window->FontSize());
				b_RequestFontChange = true;
			}

			if (b_IsSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ScrollComboOnHover("Font##OptionsWindow", &Window->FontIndex(), ImGuiDataType_U64, 1, 0, Window->FontStems().size(),
		[&]() {
			Window->SetFont(Window->FontList()[Window->FontIndex()], Window->FontSize());
			b_RequestFontChange = true;
		});

	{
		float OldFontSize = Window->FontSize();

		if (ImGui::ArrowButton("FontSizeSub##OptionsWindow", ImGuiDir_Left))
		{
			--Window->FontSize();
		}
		TooltipOnHover("Decrease the font size");

		ImGui::SameLine();

		if (ImGui::ArrowButton("FontSizeAdd##OptionsWindow", ImGuiDir_Right))
		{
			++Window->FontSize();
		}
		TooltipOnHover("Increase the font size");

		ImGui::SameLine();
		ImGui::Text("Font Size: %.1f", OldFontSize);

		Window->FontSize() = std::clamp(Window->FontSize(), Window->FontSizeMin(), Window->FontSizeMax());

		if (OldFontSize != Window->FontSize())
		{
			b_RequestFontChange = true;
		}
	}

	{
		auto OldRed = GetRValue(Window->GetColor());
		auto OldGreen = GetGValue(Window->GetColor());
		auto OldBlue = GetBValue(Window->GetColor());

		ImVec4 ColorF = ImVec4(
			GetRValue(Window->GetColor()) / 255.0f,
			GetGValue(Window->GetColor()) / 255.0f,
			GetBValue(Window->GetColor()) / 255.0f,
			GetAValue(Window->GetColor()) / 255.0f);

		ImGui::ColorEdit3(Str.FormatCStyle("Options##WindowColor").c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
		ImGui::SameLine(); ImGui::Text("Window Color");

		COLORREF Color = RGB(
			BYTE(ColorF.x * 255.0f),
			BYTE(ColorF.y * 255.0f),
			BYTE(ColorF.z * 255.0f));

		auto Red = GetRValue(Color);
		auto Green = GetGValue(Color);
		auto Blue = GetBValue(Color);

		if ((Red != OldRed) || (Green != OldGreen) || (Blue != OldBlue))
		{
			Window->SetColor(GetRValue(Color), GetGValue(Color), GetBValue(Color), true);
		}
	}

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::RenderWindow(void)
{
	if (!m_RenderTexture) { return; }

	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);

	ImVec2 ContentSize = ImGui::GetContentRegionAvail();
	ImVec2 WindowSize = ImVec2(m_RenderWidth * m_RenderZoom, m_RenderHeight * m_RenderZoom);

	ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoSavedSettings;
	if (WindowSize.x > ContentSize.x) WindowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
	if (WindowSize.y > ContentSize.y) WindowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

	ImGui::SetNextWindowContentSize(WindowSize);

	ImGui::BeginChild("Texture##RenderWindow", ContentSize, false, WindowFlags);

	ImGui::GetWindowDrawList()->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd)
		{
			float m_TextureWidth = (float)(G->m_RenderDesc.Width);
			float m_TextureHeight = (float)(G->m_RenderDesc.Height);

			G->Render->Device()->SetPixelShaderConstantF(0, &m_TextureWidth, 1);
			G->Render->Device()->SetPixelShaderConstantF(1, &m_TextureHeight, 1);

			G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

			G->Render->Device()->SetRenderState(D3DRS_WRAP0, 0);

			G->Render->TextureFiltering(D3DTEXF_NONE);

			G->Render->SetVertexShader(D3DFVF_VERTCT);

			G->Render->Device()->SetPixelShader(G->Render->PassthroughPixelShader.get());
		}, nullptr);

	ImVec2 uv0 = ImVec2(0.0f, (float)(m_RenderHeight) / (float)(m_RenderDesc.Height));
	ImVec2 uv1 = ImVec2((float)(m_RenderWidth) / (float)(m_RenderDesc.Width), 0.0f);

	ImGui::Image((ImTextureID)(intptr_t)m_RenderTexture.get(), ImVec2(m_RenderWidth * m_RenderZoom, m_RenderHeight * m_RenderZoom), uv0, uv1);

	ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

	if (!Camera->b_ViewTopDown && ImGui::IsItemHovered())
	{
		m_MouseX = (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / m_RenderZoom;
		m_MouseY = (ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) / m_RenderZoom;

		if (m_MouseX >= 0 && m_MouseX < m_RenderWidth && m_MouseY >= 0 && m_MouseY < m_RenderHeight)
		{
			m_MouseScaledX = m_MouseX * (Camera->m_NativeWidth / (float)(m_RenderWidth));
			m_MouseScaledY = m_MouseY * (Camera->m_NativeHeight / (float)(m_RenderHeight));

			ImGui::BeginTooltip();
			ImGui::Text("%.f, %.f", m_MouseScaledX, m_MouseScaledY);
			ImGui::EndTooltip();
		}

		if (ImGui::GetIO().KeyCtrl && Window->Device()->GetMouseDeltaZ())
		{
			float RenderZoom = m_RenderZoom;

			m_RenderZoom += Window->Device()->GetMouseDeltaZ() * 0.25f;
			m_RenderZoom = std::clamp(m_RenderZoom, m_RenderZoomMin, m_RenderZoomMax);

			ImGui::SetScrollX((ImGui::GetScrollX() + ImGui::GetMousePos().x - ImGui::GetWindowPos().x) * (m_RenderZoom / RenderZoom) - (ImGui::GetMousePos().x - ImGui::GetWindowPos().x));
			ImGui::SetScrollY((ImGui::GetScrollY() + ImGui::GetMousePos().y - ImGui::GetWindowPos().y) * (m_RenderZoom / RenderZoom) - (ImGui::GetMousePos().y - ImGui::GetWindowPos().y));
		}
	}

	ImGui::EndChild();
}

void Global_Application::LeftPanel(ImVec2 Position, ImVec2 Size)
{
	ImGui::GetStyle().WindowBorderSize = 2.0f;
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(m_BorderRed, m_BorderGreen, m_BorderBlue, 1);

	ImGui::SetNextWindowPos(Position);
	ImGui::SetNextWindowSize(Size);

	ImGui::Begin("Window##LeftPanel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

	ImGui::Text(" Stage: %X", Stage);
	ImGui::Text(" Room: %02X", Room);
	ImGui::Text(" Disk: %d", Disk);
	ImGui::Text(" Cut: %d / %d", Camera->Cut, Camera->CutMax ? Camera->CutMax - 1 : 0);

	ImGui::Text(" Anim: %d / %d", Player->iClip, Player->Animation(Player->AnimIndex())->IsOpen() ? Player->Animation(Player->AnimIndex())->GetClipCount() - 1 : 0);

	DrawVerticalLine(8.0f, 12.0f, 2.0f, m_BorderRed, m_BorderGreen, m_BorderBlue);

	ImGui::BeginDisabled(Camera->b_ViewModelEdit);
	{
		ImGui::BeginDisabled(Camera->b_ViewTopDown); ImGui::MenuItem(" Background##LeftPanel", NULL, &Camera->b_ViewBackground); ImGui::EndDisabled(); TooltipOnHover("Prerendered Background Image");
		ImGui::MenuItem(" Camera##LeftPanel", NULL, &Camera->b_DrawLine); TooltipOnHover("Camera Delta Line");
		ImGui::BeginDisabled(Camera->b_ViewTopDown); ImGui::MenuItem(" Sprite##LeftPanel", NULL, &Camera->b_ViewSprite); ImGui::EndDisabled(); TooltipOnHover("Camera Sprite Masks");
		ImGui::MenuItem(" Switch##LeftPanel", NULL, &Camera->b_DrawSwitch); TooltipOnHover("Camera Switch Vectors");
		//ImGui::MenuItem("Light##LeftPanel", NULL, NULL);
		ImGui::MenuItem(" Collision##LeftPanel", NULL, &Geometry->b_DrawCollision); TooltipOnHover("Collision Geometry");
		ImGui::MenuItem(" Block##LeftPanel", NULL, &Geometry->b_DrawBlock); TooltipOnHover("Enemy Navigation Mesh");
		ImGui::MenuItem(" Floor##LeftPanel", NULL, &Geometry->b_DrawFloor); TooltipOnHover("Floor Sound Mesh");
	}
	ImGui::EndDisabled();

	DrawVerticalLine(8.0f, 12.0f, 2.0f, m_BorderRed, m_BorderGreen, m_BorderBlue);

	ImGui::MenuItem(" Grid##LeftPanel", NULL, &Render->b_ViewGrid); TooltipOnHover("Grid");

	ImGui::MenuItem(" Axis##LeftPanel", NULL, &Render->b_ViewAxis); TooltipOnHover("Axis");

	ImGui::End();
}

void Global_Application::CenterPanel(ImVec2 Position, ImVec2 Size)
{
	ImGui::GetStyle().WindowBorderSize = 2.0f;
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(m_BorderRed, m_BorderGreen, m_BorderBlue, 1);

	ImGui::SetNextWindowPos(Position);
	ImGui::SetNextWindowSize(Size);

	ImGui::Begin("Window##CenterPanel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

	if (ImGui::BeginTabBar("Tab##CenterPanel", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("3D Layout##CenterPanel"))
		{
			if (Camera->b_ViewTopDown || Camera->b_ViewModelEdit)
			{
				Camera->b_ViewTopDown = false;
				Camera->b_ViewModelEdit = false;
				Player->b_EditorMode = false;
				Camera->Set(Camera->m_FOV, Camera->m_Eye, Camera->m_At);
			}
			RenderWindow();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("2D Layout##CenterPanel"))
		{
			if (!Camera->b_ViewTopDown)
			{
				Camera->b_ViewTopDown = true;
				Camera->b_ViewModelEdit = false;
				Player->b_EditorMode = false;
				Camera->SetTopDownPerspective();
			}
			RenderWindow();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Model##CenterPanel"))
		{
			if (!Camera->b_ViewModelEdit)
			{
				Camera->b_ViewTopDown = false;
				Camera->b_ViewModelEdit = true;
				Player->b_EditorMode = true;
				Camera->Set(Camera->m_ModelFOV, Camera->m_ModelEye, Camera->m_ModelAt);
			}
			RenderWindow();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Global_Application::RightPanel(ImVec2 Position, ImVec2 Size)
{
	ImGui::GetStyle().WindowBorderSize = 2.0f;
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(m_BorderRed, m_BorderGreen, m_BorderBlue, 1);

	ImGui::SetNextWindowPos(Position);
	ImGui::SetNextWindowSize(Size);

	ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
	if (Camera->b_ViewModelEdit) { WindowFlags |= ImGuiWindowFlags_MenuBar; }

	ImGui::Begin("Window##RightPanel", nullptr, WindowFlags);

	if (Camera->b_ViewModelEdit)
	{
		ModelEditor();
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Player##RightPanel", ImGuiTreeNodeFlags_DefaultOpen))
	{
		TooltipOnHover("World Position and Rotation");
		if (ImGui::BeginTable("Transform##PlayerRightPanel", 4))
		{
			ImGui::TableSetupColumn("Label##PlayerRightPanel", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("____").x);
			ImGui::TableSetupColumn("X##PlayerRightPanel");
			ImGui::TableSetupColumn("Y##PlayerRightPanel");
			ImGui::TableSetupColumn("Z##PlayerRightPanel");
			ImGui::TableNextRow();

			{
				ImGui::TableNextColumn(); ImGui::Text(" Pos");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelPosX", ImGuiDataType_S32, &Player->Position().x))
				{
					Player->Position().x = std::clamp(Player->Position().x, -32767, 32767);
				}
				ScrollOnHover(&Player->Position().x, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelPosY", ImGuiDataType_S32, &Player->Position().y))
				{
					Player->Position().y = std::clamp(Player->Position().y, -32767, 32767);
				}
				ScrollOnHover(&Player->Position().y, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelPosZ", ImGuiDataType_S32, &Player->Position().z))
				{
					Player->Position().z = std::clamp(Player->Position().z, -32767, 32767);
				}
				ScrollOnHover(&Player->Position().z, ImGuiDataType_S32, 32, -32767, 32767);
				TooltipOnHover("Position Z");
			}

			{
				ImGui::TableNextColumn(); ImGui::Text(" Rot");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelRotX", ImGuiDataType_S32, &Player->Rotation().x))
				{
					Player->Rotation().x = std::clamp(Player->Rotation().x, -4096, 4096);
				}
				ScrollOnHover(&Player->Rotation().x, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelRotY", ImGuiDataType_S32, &Player->Rotation().y))
				{
					Player->Rotation().y = std::clamp(Player->Rotation().y, -4096, 4096);
				}
				ScrollOnHover(&Player->Rotation().y, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelRotZ", ImGuiDataType_S32, &Player->Rotation().z))
				{
					Player->Rotation().z = std::clamp(Player->Rotation().z, -4096, 4096);
				}
				ScrollOnHover(&Player->Rotation().z, ImGuiDataType_S32, 32, -4096, 4096);
				TooltipOnHover("Rotation Z");
			}

			{
				ImGui::TableNextColumn(); ImGui::Text(" Sx");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelScaleX", ImGuiDataType_S32, &Player->Scale().x))
				{
					Player->Scale().x = std::clamp(Player->Scale().x, 0, 32768);
				}
				ScrollOnHover(&Player->Scale().x, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale X");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelScaleY", ImGuiDataType_S32, &Player->Scale().y))
				{
					Player->Scale().y = std::clamp(Player->Scale().y, 0, 32768);
				}
				ScrollOnHover(&Player->Scale().y, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale Y");

				ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
				if (ImGui::InputScalar("##PlayerRightPanelScaleZ", ImGuiDataType_S32, &Player->Scale().z))
				{
					Player->Scale().z = std::clamp(Player->Scale().z, 0, 32768);
				}
				ScrollOnHover(&Player->Scale().z, ImGuiDataType_S32, 512, 0, 32768);
				TooltipOnHover("Scale Z");
			}

			ImGui::EndTable();
		}
	}

	ImGui::End();
}