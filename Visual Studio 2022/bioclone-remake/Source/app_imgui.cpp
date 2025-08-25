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
				FilePool.Enqueue([&]() { b_RoomFileOp = true; OpenRDT(); b_RoomFileOp = false; });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create##FileMenu", "CTRL+N"))
			{
				// FilePool.Enqueue([&]() { b_RoomFileOp = true; CreateRDT(); b_RoomFileOp = false; });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save As##FileMenu", "CTRL+S"))
			{
				FilePool.Enqueue([&]() { b_RoomFileOp = true; SaveRDT(); b_RoomFileOp = false; });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close##FileMenu", NULL))
			{
				FilePool.Enqueue([&]() { CloseRDT(); });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Roomcut##FileMenu", NULL))
			{
				FilePool.Enqueue([&]()
					{
						if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

						if (auto Filename = Window->GetOpenFilename({ L"roomcut.bin" }, { L"roomcut.bin" }); Filename.has_value())
						{
							RoomcutExtract(Filename.value());
						}

						CoUninitialize();
					});
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Screenshot##FileMenu", NULL))
			{
				FilePool.Enqueue([&]() { Screenshot(); });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit##FileMenu", "ESC"))
			{
				b_Active = false;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View##MenuWindow"))
		{
			if (ImGui::MenuItem("Fullscreen##ViewMenu", "F11")) { b_RequestFullscreen = true; }
			ImGui::Separator();
			if (ImGui::MenuItem("Controller Map##ViewMenu", NULL))
			{
				Modal = [&]() { ControllerMapping(); };
			}
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

	{
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

		if (ImGui::SliderScalar("Font Size##OptionsWindow", ImGuiDataType_Float, &Window->FontSize(), &Window->FontSizeMin(), &Window->FontSizeMax()))
		{
			Window->FontSize() = std::roundf(Window->FontSize());
			Window->SetFont(Window->FontList()[Window->FontIndex()], Window->FontSize());
			b_RequestFontChange = true;
		}

		ScrollFloatOnHover(&Window->FontSize(), ImGuiDataType_Float, 1.0, Window->FontSizeMin(), Window->FontSizeMax(),
			[&]() {
				Window->SetFont(Window->FontList()[Window->FontIndex()], Window->FontSize());
				b_RequestFontChange = true;
			});
	}

	DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

	{
		if (ImGui::BeginTable("Color##OptionsWindow", 4))
		{
			ImGui::TableNextColumn();
			{
				if (ImGui::ColorEdit3("Border Color##OptionsWindow", (float*)&m_BorderColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions))
				{
					Window->SetBorderColor(RGB(m_BorderColor.r * 255.0f, m_BorderColor.g * 255.0f, m_BorderColor.b * 255.0f));
					InitImGuiColor();
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2.0f);
				ImGui::Text("Border");
			}

			//DrawVerticalLine(8.0f, 0.5f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

			ImGui::TableNextColumn();
			{
				if (ImGui::ColorEdit3("Caption Color##OptionsWindow", (float*)&m_CaptionColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions))
				{
					Window->SetCaptionColor(RGB(m_CaptionColor.r * 255.0f, m_CaptionColor.g * 255.0f, m_CaptionColor.b * 255.0f));
					InitImGuiColor();
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2.0f);
				ImGui::Text("Caption");
			}

			//DrawVerticalLine(8.0f, 0.5f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

			ImGui::TableNextColumn();
			{
				if (ImGui::ColorEdit3("Window Color##OptionsWindow", (float*)&m_WindowColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions))
				{
					Window->SetColor(BYTE(m_WindowColor.r * 255.0f), BYTE(m_WindowColor.g * 255.0f), BYTE(m_WindowColor.b * 255.0f), true);
					InitImGuiColor();
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2.0f);
				ImGui::Text("Window");
			}

			//DrawVerticalLine(8.0f, 0.5f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

			ImGui::TableNextColumn();
			{
				if (ImGui::ColorEdit3("Font Color##OptionsWindow", (float*)&m_FontColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions))
				{
					Window->SetTextColor(RGB(m_FontColor.r * 255.0f, m_FontColor.g * 255.0f, m_FontColor.b * 255.0f));
					InitImGuiColor();
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2.0f);
				ImGui::Text("Font");
			}

			ImGui::EndTable();
		}
	}

	DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

	{
		if (ImGui::BeginTable("Render##OptionsWindow", 3))
		{
			ImGui::TableNextColumn(); ImGui::Text(" Width");

			ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
			ImGui::InputScalar("##RenderSceneWidth", ImGuiDataType_U32, &m_RequestWidth);
			TooltipOnHover("Render Scene Width");

			//DrawVerticalLine(8.0f, 0.5f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

			ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
			ImGui::Checkbox(" Ignore Resize##RenderSceneHeight", &b_IgnoreAutoResize);
			TooltipOnHover("Do not resize render window on fullscreen/maximize/restore");

			ImGui::TableNextColumn(); ImGui::Text(" Height");

			ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
			ImGui::InputScalar("##RenderSceneHeight", ImGuiDataType_U32, &m_RequestHeight);
			TooltipOnHover("Render Scene Height");

			if ((m_RequestWidth != m_RenderWidth) || (m_RequestHeight != m_RenderHeight))
			{
				m_RenderWidth = std::clamp(m_RenderWidth, 1u, 65535u);
				m_RenderHeight = std::clamp(m_RenderHeight, 1u, 65535u);
				b_RequestRenderResize = true;
			}

			ImGui::TableNextColumn(); ImGui::SetNextItemWidth(ImGui::CalcTextSize("_______").x);
			ImGui::Checkbox(" Show FPS##RenderSceneHeight", &b_ShowFPS);
			TooltipOnHover("Display the frames per second");

			ImGui::EndTable();
		}
	}

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::RenderWindow(void)
{
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

			G->Render->SetPSXLightToggle(false, true);

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

			// adjust for direct-x 9 half texel
			m_MouseScaledX -= 0.5f;
			m_MouseScaledY -= 0.5f;

			m_MouseScaledX = std::clamp(m_MouseScaledX, 0.0f, (float)Camera->m_NativeWidth);
			m_MouseScaledY = std::clamp(m_MouseScaledY, 0.0f, (float)Camera->m_NativeHeight);

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
	else if (Camera->b_ViewTopDown && ImGui::IsItemHovered())
	{
		if (ImGui::GetIO().KeyCtrl && Window->Device()->GetMouseDeltaZ())
		{
			Camera->m_Cy += Window->Device()->GetMouseDeltaZ() * 2.5f;
			Camera->SetTopDownPerspective();
		}
		else if (Window->Device()->GetMouseDeltaX())
		{
			Camera->m_Cx += Window->Device()->GetMouseDeltaX() * -0.25f;
			Camera->SetTopDownPerspective();
		}
		else if (Window->Device()->GetMouseDeltaZ())
		{
			Camera->m_Cz += Window->Device()->GetMouseDeltaZ() * -0.25f;
			Camera->SetTopDownPerspective();
		}
	}

	ImGui::EndChild();
}

void Global_Application::LeftPanel(ImVec2 Position, ImVec2 Size)
{
	ImGui::GetStyle().WindowBorderSize = 2.0f;
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, 1);

	ImGui::SetNextWindowPos(Position);
	ImGui::SetNextWindowSize(Size);

	ImGui::Begin("Window##LeftPanel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

	ImGui::Text(" Stage: %X", Stage);
	ImGui::Text(" Room: %02X", Room);
	ImGui::Text(" Disk: %d", Disk);
	ImGui::Text(" Cut: %d / %d", Camera->Cut, Camera->CutMax ? Camera->CutMax - 1 : 0);

	DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

	ImGui::BeginDisabled(Player->Filename().empty());
	ImGui::Text(" %ws", Player->Filename().empty() ? L"N/A" : Player->Filename().stem().wstring().c_str());
	ImGui::EndDisabled();

	ImGui::BeginDisabled(Player->WeaponFilename().empty());
	ImGui::Text(" %ws", Player->WeaponFilename().empty() ? L"N/A" : Player->WeaponFilename().stem().wstring().c_str());
	ImGui::EndDisabled();

	ImGui::BeginDisabled(!Player->Animation(Player->AnimIndex())->IsOpen());
	ImGui::Text(" Anim: %d / %d", Player->iClip.load(), Player->Animation(Player->AnimIndex())->IsOpen() ? Player->Animation(Player->AnimIndex())->GetClipCount() - 1 : 0);
	ImGui::EndDisabled();

	DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

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

	DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

	ImGui::MenuItem(" Grid##LeftPanel", NULL, &Render->b_ViewGrid); TooltipOnHover("Grid");

	ImGui::MenuItem(" Axis##LeftPanel", NULL, &Render->b_ViewAxis); TooltipOnHover("Axis");

	if (b_ShowFPS)
	{
		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::Text("%.3f %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	ImGui::End();
}

void Global_Application::CenterPanel(ImVec2 Position, ImVec2 Size)
{
	ImGui::GetStyle().WindowBorderSize = 2.0f;
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, 1);

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
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, 1);

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

	if (ImGui::CollapsingHeader("Collision##RightPanel", ImGuiTreeNodeFlags_None))
	{
		ImGui::BeginDisabled(!Geometry->b_DrawCollision);
		CollisionEditor();
		ImGui::EndDisabled();
	}

	if (ImGui::CollapsingHeader("Player##RightPanel", ImGuiTreeNodeFlags_None))
	{
		TooltipOnHover("Position, Rotation and Scale");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::BeginDisabled((Player->ModelGame() & BIO2) == 0 || (Player->WeaponModelGame() & BIO2) == 0);	// temp
		ImGui::BeginDisabled((!Player->ModelDX9() || !Player->WeaponModelDX9()) && Player->Animation(AnimationIndex::Weapon)->Clip.empty());
		if (ImGui::MenuItem(" Controller##ModelEditor", NULL, &Player->b_ControllerMode))
		{
			if (Player->b_ControllerMode) { Player->b_LockPosition = true; }
		}
		TooltipOnHover("Controller input on/off\r\n\r\nWeapon must be open");
		ImGui::EndDisabled();
		ImGui::EndDisabled();

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		ImGui::MenuItem(" Collision##ModelEditor", NULL, &Geometry->b_CollisionDetection);
		TooltipOnHover("Collision detection on/off");

		ImGui::MenuItem(" Cam Switch##ModelEditor", NULL, &Geometry->b_SwitchDetection);
		TooltipOnHover("Camera switch detection on/off");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

		if (ImGui::MenuItem(" Vertex Light##ModelRender", NULL, &b_PerVertexLighting)) { if (b_PerVertexLighting) { b_PerPixelLighting = false; } }
		TooltipOnHover("Per-Vertex Lighting");

		if (ImGui::MenuItem(" Pixel Light##ModelRender", NULL, &b_PerPixelLighting)) { if (b_PerPixelLighting) { b_PerVertexLighting = false; } }
		TooltipOnHover("Per-Pixel Lighting");

		DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

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