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
				FileOp.Enqueue([&]() { OpenRDT(); });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create##FileMenu", "CTRL+N"))
			{
				// FileOp.Enqueue([&]() { CreateRDT(); });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save As##FileMenu", "CTRL+S"))
			{
				FileOp.Enqueue([&]() { SaveRDT(); });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close##FileMenu", NULL))
			{
				FileOp.Enqueue([&]() { CloseRDT(true); });
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Roomcut##FileMenu", NULL))
			{
				FileOp.Enqueue([&]()
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
				FileOp.Enqueue([&]() { Screenshot(); });
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
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Setup##MenuWindow"))
		{
			if (ImGui::MenuItem("Controller Map##ViewMenu", NULL))
			{
				Modal = [&]() { ControllerMapping(); };
			}
			ImGui::Separator();
			ImGui::MenuItem("Window Options##ViewMenu", NULL, &b_ViewWindowOptions);
			ImGui::Separator();
			if (ImGui::MenuItem("Data Directory##SetupMenu"))
			{
				b_ViewScenarioSetup = !b_ViewScenarioSetup;
			}
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

	DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

	{
		static constexpr std::array<const char*, 6> PostProcessingShaders =
		{
			"None",
			"Daguerreotype",
			"Grayscale",
			"Sepia",
			"Posterization",
			"Auto HDR"
		};

		if (ImGui::BeginCombo("Post Processing##OptionsWindow", PostProcessingShaders[Render->iPostProcessor], ImGuiComboFlags_HeightLarge))
		{
			for (size_t i = 0; i < std::size(PostProcessingShaders); i++)
			{
				bool b_IsSelected = (Render->iPostProcessor == i);

				if (ImGui::Selectable(PostProcessingShaders[i], b_IsSelected))
				{
					Render->iPostProcessor = i;
				}

				if (b_IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (Render->iPostProcessor == 1)
		{
			static float min = 0.0f;
			static float max = 1.0f;

			ImGui::SliderScalar("Intensity##OptionsWindow", ImGuiDataType_Float, &Render->m_VignetteIntensity, &min, &max, "%.2f");
			ScrollFloatOnHover(&Render->m_VignetteIntensity, ImGuiDataType_Float, 0.01f, 0.0f, 1.0f);

			ImGui::SliderScalar("Contrast##OptionsWindow", ImGuiDataType_Float, &Render->m_Contrast, &min, &max, "%.2f");
			ScrollFloatOnHover(&Render->m_Contrast, ImGuiDataType_Float, 0.01f, 0.0f, 1.0f);
		}
		else if (Render->iPostProcessor == 2 || Render->iPostProcessor == 3)
		{
			static float min = 0.0f;
			static float max = 1.0f;

			ImGui::SliderScalar("Intensity##OptionsWindow", ImGuiDataType_Float, &Render->m_VignetteIntensity, &min, &max, "%.2f");
			ScrollFloatOnHover(&Render->m_VignetteIntensity, ImGuiDataType_Float, 0.01f, 0.0f, 1.0f);

			ImGui::SliderScalar("Vignette##OptionsWindow", ImGuiDataType_Float, &Render->m_VignetteRadius, &min, &max, "%.2f");
			ScrollFloatOnHover(&Render->m_VignetteRadius, ImGuiDataType_Float, 0.01f, 0.0f, 1.0f);
		}
		else if (Render->iPostProcessor == 4)
		{
			static float min = 0.0f;
			static float max = 256.0f;
			ImGui::SliderScalar("Level##OptionsWindow", ImGuiDataType_Float, &Render->m_Posterization, &min, &max, "%.0f");
			ScrollFloatOnHover(&Render->m_Posterization, ImGuiDataType_Float, 1.0f, 1.0f, 256.0f);
		}
		else if (Render->iPostProcessor == 5)
		{
			static float PaperWhitemin = 80.0f;
			static float PaperWhitemax = 2000.0f;

			static float StandardWhitemin = 80.0f;
			static float StandardWhitemax = 120.0f;

			static float Displaymin = 80.0f;
			static float Displaymax = 2000.0f;

			static float Alphamin = 0.0f;
			static float Alphamax = 0.95f;

			static float Shouldermin = 1.0f;
			static float Shouldermax = 10.0f;

			if (ImGui::SliderScalar("Paper White (nits)", ImGuiDataType_Float, &Render->m_PaperWhiteNits, &PaperWhitemin, &PaperWhitemax, "%.0f"))
			{
				Render->m_PaperWhiteNits = std::clamp(Render->m_PaperWhiteNits, Render->m_StandardWhiteNits, Render->m_DisplayMaxNits);
			}
			if (ScrollFloatOnHover(&Render->m_PaperWhiteNits, ImGuiDataType_Float, 1.0f, PaperWhitemin, PaperWhitemax))
			{
				Render->m_PaperWhiteNits = std::clamp(Render->m_PaperWhiteNits, Render->m_StandardWhiteNits, Render->m_DisplayMaxNits);
			}

			if (ImGui::SliderScalar("SDR White (nits)", ImGuiDataType_Float, &Render->m_StandardWhiteNits, &StandardWhitemin, &StandardWhitemax, "%.0f"))
			{
				if (Render->m_StandardWhiteNits > Render->m_PaperWhiteNits) { Render->m_StandardWhiteNits = Render->m_PaperWhiteNits; }
			}
			if (ScrollFloatOnHover(&Render->m_StandardWhiteNits, ImGuiDataType_Float, 1.0f, StandardWhitemin, StandardWhitemax))
			{
				if (Render->m_StandardWhiteNits > Render->m_PaperWhiteNits) { Render->m_StandardWhiteNits = Render->m_PaperWhiteNits; }
			}

			if (ImGui::SliderScalar("Display Max (nits)", ImGuiDataType_Float, &Render->m_DisplayMaxNits, &Displaymin, &Displaymax, "%.0f"))
			{
				if (Render->m_DisplayMaxNits < Render->m_PaperWhiteNits) { Render->m_PaperWhiteNits = Render->m_DisplayMaxNits; }
			}
			if (ScrollFloatOnHover(&Render->m_DisplayMaxNits, ImGuiDataType_Float, 1.0f, Displaymin, Displaymax))
			{
				if (Render->m_DisplayMaxNits < Render->m_PaperWhiteNits) { Render->m_PaperWhiteNits = Render->m_DisplayMaxNits; }
			}

			ImGui::SliderScalar("Shoulder Start", ImGuiDataType_Float, &Render->m_ShoulderAlpha, &Alphamin, &Alphamax, "%.2f");
			ScrollFloatOnHover(&Render->m_ShoulderAlpha, ImGuiDataType_Float, 0.01f, Alphamin, Alphamax);

			ImGui::SliderScalar("Shoulder Power", ImGuiDataType_Float, &Render->m_ShoulderPOW, &Shouldermin, &Shouldermax, "%.2f");
			ScrollFloatOnHover(&Render->m_ShoulderPOW, ImGuiDataType_Float, 0.05f, Shouldermin, Shouldermax);
		}
	}

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::ScenarioDirectory(void)
{
	if (!b_ViewScenarioSetup) { return; }

	ImGui::Begin("Scenario Directories##ScenarioSetup", &b_ViewScenarioSetup, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	{
		static char buf1[4096] = "";

		std::memcpy(buf1, m_DataPath.string().c_str(), std::min<std::size_t>(m_DataPath.string().size(), sizeof(buf1) - 1));

		ImGui::SetNextItemWidth(900.0f);

		if (ImGui::InputText("Data", buf1, IM_ARRAYSIZE(buf1)))
		{
			m_DataPath = buf1;
		}

		ImGui::SameLine();

		if (ImGui::Button("...##Data"))
		{
			if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

			if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
			{
				m_DataPath = Directory.value();
			}

			CoUninitialize();
		}
	}

	{
		static char buf1[4096] = "";

		std::memcpy(buf1, m_PlayerPath.string().c_str(), std::min<std::size_t>(m_PlayerPath.string().size(), sizeof(buf1) - 1));

		ImGui::SetNextItemWidth(900.0f);

		if (ImGui::InputText("Player", buf1, IM_ARRAYSIZE(buf1)))
		{
			m_PlayerPath = buf1;
		}

		ImGui::SameLine();

		if (ImGui::Button("...##Player"))
		{
			if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

			if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
			{
				m_PlayerPath = Directory.value();
			}

			CoUninitialize();
		}
	}

	{
		static char buf2[4096] = "";

		std::memcpy(buf2, m_EnemyPath.string().c_str(), std::min<std::size_t>(m_EnemyPath.string().size(), sizeof(buf2) - 1));

		ImGui::SetNextItemWidth(900.0f);

		if (ImGui::InputText("Enemy", buf2, IM_ARRAYSIZE(buf2)))
		{
			m_EnemyPath = buf2;
		}

		ImGui::SameLine();

		if (ImGui::Button("...##Enemy"))
		{
			if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

			if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
			{
				m_EnemyPath = Directory.value();
			}

			CoUninitialize();
		}
	}

	{
		static char buf3[4096] = "";

		std::memcpy(buf3, m_DoorPath.string().c_str(), std::min<std::size_t>(m_DoorPath.string().size(), sizeof(buf3) - 1));

		ImGui::SetNextItemWidth(900.0f);

		if (ImGui::InputText("Door", buf3, IM_ARRAYSIZE(buf3)))
		{
			m_DoorPath = buf3;
		}

		ImGui::SameLine();

		if (ImGui::Button("...##Door"))
		{
			if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

			if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
			{
				m_DoorPath = Directory.value();
			}

			CoUninitialize();
		}
	}

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::ControllerMapping(void)
{
	std::function<void(const std::string&)> SetMapping = [&](const std::string& Button)
		{
			auto This = std::shared_ptr<Global_Application>(this, [](Global_Application*) {});

			auto ThreadWork = std::thread([This, Button]()
				{
					This->b_ModalOp.store(true);

					std::function<void(bool&)> Callback = [&](bool& b_Status) -> void { This->b_ModalOp.store(b_Status); };

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
				b_ModalOp.store(false);
				Modal = []() {};
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

		ImGui::EndPopup();
	}
}

void Global_Application::RenderWindow(void)
{
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);

	ImVec2 ContentSize = ImGui::GetContentRegionAvail();
	ImVec2 WindowSize = ImVec2(m_RenderWidth * m_RenderZoom, m_RenderHeight * m_RenderZoom);

	ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
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

			if (G->IsPanelType(PanelType::Pers3D) && G->Render->iPostProcessor == 1)
			{
				G->Render->Device()->SetPixelShaderConstantF(2, &G->Render->m_VignetteIntensity, 1);
				G->Render->Device()->SetPixelShaderConstantF(3, &G->Render->m_Contrast, 1);
			}
			else if (G->IsPanelType(PanelType::Pers3D) && (G->Render->iPostProcessor == 2 || G->Render->iPostProcessor == 3))
			{
				G->Render->Device()->SetPixelShaderConstantF(2, &G->Render->m_VignetteIntensity, 1);
				G->Render->Device()->SetPixelShaderConstantF(3, &G->Render->m_VignetteRadius, 1);
			}
			else if (G->IsPanelType(PanelType::Pers3D) && G->Render->iPostProcessor == 4)
			{
				G->Render->Device()->SetPixelShaderConstantF(2, &G->Render->m_Posterization, 1);
			}
			else if (G->IsPanelType(PanelType::Pers3D) && G->Render->iPostProcessor == 5)
			{
				G->Render->Device()->SetPixelShaderConstantF(2, &G->Render->m_PaperWhiteNits, 1);
				G->Render->Device()->SetPixelShaderConstantF(3, &G->Render->m_StandardWhiteNits, 1);
				G->Render->Device()->SetPixelShaderConstantF(4, &G->Render->m_DisplayMaxNits, 1);
				G->Render->Device()->SetPixelShaderConstantF(5, &G->Render->m_ShoulderAlpha, 1);
				G->Render->Device()->SetPixelShaderConstantF(6, &G->Render->m_ShoulderPOW, 1);
			}

			G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

			G->Render->Device()->SetRenderState(D3DRS_WRAP0, 0);

			G->Render->TextureFiltering(D3DTEXF_NONE);

			G->Render->SetVertexShader(D3DFVF_VERTCT);

			if (G->IsPanelType(PanelType::Pers3D))
			{
				G->Render->Device()->SetPixelShader(G->Render->PostProcessor[G->Render->iPostProcessor]);
			}
			else
			{
				G->Render->Device()->SetPixelShader(G->Render->PassthroughPixelShader.get());
			}
		}, nullptr);

	ImVec2 uv0 = ImVec2(0.0f, static_cast<float>(m_RenderHeight) / static_cast<float>(m_RenderDesc.Height));
	ImVec2 uv1 = ImVec2(static_cast<float>(m_RenderWidth) / static_cast<float>(m_RenderDesc.Width), 0.0f);

	ImGui::Image((ImTextureID)(intptr_t)m_RenderTexture.get(), ImVec2(m_RenderWidth * m_RenderZoom, m_RenderHeight * m_RenderZoom), uv0, uv1);

	ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

	if (!Camera->b_ViewTopDown && !Camera->b_ViewEditor && ImGui::IsItemHovered())
	{
		m_MouseX = (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / m_RenderZoom;
		m_MouseY = (ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) / m_RenderZoom;

		if (m_MouseX >= 0 && m_MouseX < m_RenderWidth && m_MouseY >= 0 && m_MouseY < m_RenderHeight)
		{
			m_MouseScaledX = m_MouseX * (Camera->m_NativeWidth / static_cast<float>(m_RenderWidth));
			m_MouseScaledY = m_MouseY * (Camera->m_NativeHeight / static_cast<float>(m_RenderHeight));

			// adjust for direct-x 9 half texel
#if MSTD_DX9
			m_MouseScaledX -= 0.5f;
			m_MouseScaledY -= 0.5f;
#endif

			m_MouseScaledX = std::clamp(m_MouseScaledX, 0.0f, Camera->NativeWidth());
			m_MouseScaledY = std::clamp(m_MouseScaledY, 0.0f, Camera->NativeHeight());

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
			Camera->SetTopDown();
		}
		else if (Window->Device()->GetMouseDeltaX())
		{
			Camera->m_Cx += Window->Device()->GetMouseDeltaX() * -0.25f;
			Camera->SetTopDown();
		}
		else if (Window->Device()->GetMouseDeltaZ())
		{
			Camera->m_Cz += Window->Device()->GetMouseDeltaZ() * -0.25f;
			Camera->SetTopDown();
		}
	}
	else if (Camera->b_ViewEditor && ImGui::IsItemHovered())
	{
		/*m_MouseX = (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / m_RenderZoom;
		m_MouseY = (ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) / m_RenderZoom;

		if (m_MouseX >= 0 && m_MouseX < m_RenderWidth && m_MouseY >= 0 && m_MouseY < m_RenderHeight)
		{
			m_MouseScaledX = m_MouseX * (Camera->m_NativeWidth / static_cast<float>(m_RenderWidth));
			m_MouseScaledY = m_MouseY * (Camera->m_NativeHeight / static_cast<float>(m_RenderHeight));

#if MSTD_DX9
			m_MouseScaledX -= 0.5f;
			m_MouseScaledY -= 0.5f;
#endif

			m_MouseScaledX = std::clamp(m_MouseScaledX, 0.0f, Camera->NativeWidth());
			m_MouseScaledY = std::clamp(m_MouseScaledY, 0.0f, Camera->NativeHeight());
		}*/

		MouseCamera(Camera->m_EditorEye, Camera->m_EditorAt, Camera->b_ViewEditor);
	}

	ImGui::EndChild();
}

void Global_Application::SetCenterPanel(PanelType Type)
{
	static constexpr std::uint32_t PANEL_PERS3D = std::to_underlying(PanelType::Pers3D);
	static constexpr std::uint32_t PANEL_TOP2D = std::to_underlying(PanelType::Top2D);
	static constexpr std::uint32_t PANEL_CHARMODEL = std::to_underlying(PanelType::CharacterModel);
	static constexpr std::uint32_t PANEL_ROOMMODEL = std::to_underlying(PanelType::RoomModel);
	static constexpr std::uint32_t PANEL_EFFECT = std::to_underlying(PanelType::Effect);
	static constexpr std::uint32_t PANEL_SCRIPT = std::to_underlying(PanelType::Script);

	m_PanelType = Type;

	if (b_FileOp.load()) { return; }

	if (std::to_underlying(Type) & PANEL_PERS3D)
	{
		if (Camera->b_ViewTopDown || Camera->b_ViewEditor)
		{
			Camera->b_ViewTopDown = false;
			Camera->b_ViewEditor = false;

			Player->b_Active.store(true);
			Player->b_EditorMode = false;

			Room->ResetEditor();

			Camera->Set(Camera->m_FOV, Camera->m_Eye, Camera->m_At);
		}
	}

	if (std::to_underlying(Type) & PANEL_TOP2D)
	{
		if (!Camera->b_ViewTopDown)
		{
			Camera->b_ViewTopDown = true;
			Camera->b_ViewEditor = false;

			Player->b_Active.store(true);
			Player->b_EditorMode = false;

			Room->ResetEditor();

			Camera->SetTopDown();
		}
	}

	if (std::to_underlying(Type) & PANEL_CHARMODEL)
	{
		if (!Player->b_EditorMode)
		{
			Camera->b_ViewTopDown = false;
			Camera->b_ViewEditor = true;

			Player->b_Active.store(true);
			Player->b_EditorMode = true;

			Room->ResetEditor();

			Camera->SetEditor(Camera->m_EditorFOV, Camera->m_EditorEye, Camera->m_EditorAt);
		}
	}

	if (std::to_underlying(Type) & PANEL_ROOMMODEL)
	{
		if (!Room->b_EditModel)
		{
			Camera->b_ViewTopDown = false;
			Camera->b_ViewEditor = true;

			Player->b_Active.store(false);
			Player->b_EditorMode = false;

			Room->SetEditor(Room_Editor_Type::MODEL);

			Camera->SetEditor(Camera->m_EditorFOV, Camera->m_EditorEye, Camera->m_EditorAt);
		}
	}

	if (std::to_underlying(Type) & PANEL_EFFECT)
	{
		if (!Room->b_EditEffect)
		{
			Camera->b_ViewTopDown = false;
			Camera->b_ViewEditor = true;

			Player->b_Active.store(false);
			Player->b_EditorMode = false;

			Room->SetEditor(Room_Editor_Type::ESP);

			Camera->SetEditor(Camera->m_EditorFOV, Camera->m_EditorEye, Camera->m_EditorAt);
		}
	}

}

void Global_Application::LeftPanel(ImVec2 Position, ImVec2 Size)
{
	ImGui::GetStyle().WindowBorderSize = 2.0f;
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, 1);

	ImGui::SetNextWindowPos(Position);
	ImGui::SetNextWindowSize(Size);

	ImGui::Begin("Window##LeftPanel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

	ImGui::Text(" Stage: %X", Room->m_Stage);
	ImGui::Text(" Room: %02X", Room->m_Room);
	ImGui::Text(" Disk: %d", Room->m_Disk);
	ImGui::Text(" Cut: %d / %d", Camera->m_Cut, Camera->m_CutMax ? Camera->m_CutMax - 1 : 0);

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

	ImGui::Text(" AOT: %s", Sce->AotStr.c_str());

	DrawHorizontalLine(8.0f, 12.0f, 2.0f, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b);

	ImGui::BeginDisabled(Camera->b_ViewEditor);
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
			SetCenterPanel(PanelType::Pers3D);
			RenderWindow();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("2D Layout##CenterPanel"))
		{
			SetCenterPanel(PanelType::Top2D);
			RenderWindow();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Model##CenterPanel"))
		{
			SetCenterPanel(PanelType::CharacterModel);
			RenderWindow();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Item##CenterPanel"))
		{
			SetCenterPanel(PanelType::RoomModel);
			RenderWindow();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Effect##CenterPanel"))
		{
			SetCenterPanel(PanelType::Effect);
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

	if (Camera->b_ViewEditor) { WindowFlags |= ImGuiWindowFlags_MenuBar; }

	ImGui::Begin("Window##RightPanel", nullptr, WindowFlags);

	if (IsPanelType(PanelType::CharacterModel) || IsPanelType(PanelType::RoomModel))
	{
		ModelEditor(IsPanelType(PanelType::RoomModel));
		ImGui::End();
		return;
	}

	if (IsPanelType(PanelType::Effect))
	{
		EffectEditor();
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Type##RightPanel", ImGuiTreeNodeFlags_None))
	{
		bool b_Bio1Aug95 = Room->GameType() & (AUG95);
		bool b_Bio1Oct95 = Room->GameType() & (OCT95);
		bool b_Bio1 = Room->GameType() & (BIO1);
		bool b_Bio2Nov96 = Room->GameType() & (BIO2NOV96);
		bool b_Bio2 = Room->GameType() & (BIO2);
		bool b_Bio3 = Room->GameType() & (BIO3);

		if (ImGui::BeginTable("Table##RoomType", 1))
		{
			TooltipOnHover("Room Game Type on Open/Save");
			ImGui::TableNextColumn();
			if (ImGui::Checkbox(" Bio1 Aug '95##RoomType", &b_Bio1Aug95)) { Room->SetGame(Video_Game::Resident_Evil_Aug_4_1995); }
			TooltipOnHover("Resident Evil (Aug 1995) Alpha");
			if (ImGui::Checkbox(" Bio1 Oct '95##RoomType", &b_Bio1Oct95)) { Room->SetGame(Video_Game::Resident_Evil_Oct_4_1995); }
			TooltipOnHover("Resident Evil (Oct 1995) Alpha");
			if (ImGui::Checkbox(" Bio1##RoomType", &b_Bio1)) { Room->SetGame(Video_Game::Resident_Evil); }
			TooltipOnHover("Resident Evil");
			if (ImGui::Checkbox(" Bio2 Nov '96##RoomType", &b_Bio2Nov96)) { Room->SetGame(Video_Game::Resident_Evil_2_Nov_6_1996); }
			TooltipOnHover("Resident Evil 2 (Nov 1996) Prototype");
			if (ImGui::Checkbox(" Bio2##RoomType", &b_Bio2)) { Room->SetGame(Video_Game::Resident_Evil_2); }
			TooltipOnHover("Resident Evil 2");

			ImGui::BeginDisabled(true);
			if (ImGui::Checkbox(" Bio3##RoomType", &b_Bio3)) { Room->SetGame(Video_Game::Resident_Evil_3); }
			TooltipOnHover("Resident Evil 3");
			ImGui::EndDisabled();

			ImGui::EndTable();
		}
	}

	ScenarioEditor();

	if (ImGui::CollapsingHeader("Collision##RightPanel", ImGuiTreeNodeFlags_None))
	{
		ImGui::BeginDisabled(!Geometry->b_DrawCollision);
		CollisionEditor();
		ImGui::EndDisabled();
	}

	SceModelEditor();

	ImGui::End();
}

void Global_Application::MouseCamera(VECTOR2& Eye, VECTOR2& At, bool b_Editor)
{
	static bool b_MouseCamera = false;
	static float m_Distance = 1.0f;
	static float m_Yaw = 0.0f;
	static float m_Pitch = 0.0f;

	ImVec2 m_MouseDelta = ImGui::GetIO().MouseDelta;
	bool m_MouseLeft = ImGui::IsMouseDown(ImGuiMouseButton_Left);
	bool m_MouseMiddle = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
	bool m_KeyShift = ImGui::GetIO().KeyShift;

	if ((IsPanelType(PanelType::CharacterModel) || IsPanelType(PanelType::RoomModel)) && !Model)
	{
		b_MouseCamera = false;
		return;
	}

	// reset
	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		m_Yaw = 0.0f;
		m_Pitch = 0.0f;

		if (b_Editor)
		{
			Camera->SetEditor(Camera->m_EditorFOV, { 5400, -1800, 0 }, { 0, -1800 , 0 });
		}
		else
		{
			Camera->Set(Camera->m_FOV, { -16000, -7200, -16000 }, { 0, 7200, 0 });
		}
	}

	if (b_Editor)
	{
		if (Eye.x != Camera->m_EditorEye.x || Eye.y != Camera->m_EditorEye.y || Eye.z != Camera->m_EditorEye.z || At.x != Camera->m_EditorAt.x || At.y != Camera->m_EditorAt.y || At.z != Camera->m_EditorAt.z)
		{
			b_MouseCamera = false;
		}
	}
	else
	{
		if (Eye.x != Camera->m_Eye.x || Eye.y != Camera->m_Eye.y || Eye.z != Camera->m_Eye.z || At.x != Camera->m_At.x || At.y != Camera->m_At.y || At.z != Camera->m_At.z)
		{
			b_MouseCamera = false;
		}
	}

	if (!b_MouseCamera)
	{
		float vx = static_cast<float>(Eye.x - At.x);
		float vy = static_cast<float>(Eye.y - At.y);
		float vz = static_cast<float>(Eye.z - At.z);

		m_Distance = max(1.0f, std::sqrt(vx * vx + vy * vy + vz * vz));

		m_Yaw = std::atan2(vz, vx);

		m_Pitch = std::asin(std::clamp(vy / m_Distance, -1.0f, 1.0f));

		b_MouseCamera = true;
	}

	// zoom
	if (ImGui::GetIO().MouseWheel != 0.0f)
	{
		float m_Wheel = std::clamp(ImGui::GetIO().MouseWheel, -10.0f, 10.0f);
		float m_ZoomFactor = std::pow(1.0f - (m_Wheel * 0.02048f), 1.0f);
		m_Distance = std::clamp(m_Distance * m_ZoomFactor, 1024.0f, 25600.0f);
	}

	// pan
	if ((m_MouseMiddle || (m_KeyShift && m_MouseLeft)) && (m_MouseDelta.x != 0.0f || m_MouseDelta.y != 0.0f))
	{
		float cp = std::cos(m_Pitch);
		float sp = std::sin(m_Pitch);
		float cy = std::cos(m_Yaw);
		float sy = std::sin(m_Yaw);

		float m_DirX = cy * cp;
		float m_DirY = sp;
		float m_DirZ = sy * cp;

		float m_RightX = m_DirZ;
		float m_RightY = 0.0f;
		float m_RightZ = -m_DirX;

		float m_Length = max(0.0001f, std::sqrt(m_RightX * m_RightX + m_RightZ * m_RightZ));
		m_RightX /= m_Length;
		m_RightZ /= m_Length;

		float m_UpX = m_RightY * m_DirZ - 0.0f * m_DirY;
		float m_UpY = m_RightZ * m_DirX - m_RightX * m_DirZ;
		float m_UpZ = 0.0f;

		m_UpX = 0.0f;
		m_UpY = 1.0f;
		m_UpZ = 0.0f;

		float m_PanSpeed = m_Distance * 0.0015f;

		float m_DeltaX = -m_MouseDelta.x * m_PanSpeed;
		float m_DeltaY = m_MouseDelta.y * m_PanSpeed;

		float m_PanX = m_RightX * m_DeltaX + m_UpX * m_DeltaY;
		float m_PanY = m_RightY * m_DeltaX + m_UpY * m_DeltaY;
		float m_PanZ = m_RightZ * m_DeltaX + m_UpZ * m_DeltaY;

		At.x += static_cast<std::int32_t>(m_PanX);
		At.y += static_cast<std::int32_t>(m_PanY);
		At.z += static_cast<std::int32_t>(m_PanZ);
	}

	// orbit
	else if (m_MouseLeft && !m_KeyShift && (m_MouseDelta.x != 0.0f || m_MouseDelta.y != 0.0f))
	{
		const float m_OrbitSpeed = 0.005f;
		const float m_PitchClamp = 1.55f;	// prevent gimbal lock
		m_Yaw -= m_MouseDelta.x * m_OrbitSpeed;
		m_Pitch -= m_MouseDelta.y * m_OrbitSpeed;
		m_Pitch = std::clamp(m_Pitch, -m_PitchClamp, m_PitchClamp);
	}

	{
		float cp = std::cos(m_Pitch);
		float sp = std::sin(m_Pitch);
		float cy = std::cos(m_Yaw);
		float sy = std::sin(m_Yaw);

		float m_DirX = cy * cp;
		float m_DirY = sp;
		float m_DirZ = sy * cp;

		Eye.x = At.x + static_cast<std::int32_t>(m_DirX * m_Distance);
		Eye.y = At.y + static_cast<std::int32_t>(m_DirY * m_Distance);
		Eye.z = At.z + static_cast<std::int32_t>(m_DirZ * m_Distance);
	}

	if (b_Editor)
	{
		Camera->SetEditor(Camera->m_EditorFOV, Eye, At);
	}
	else
	{
		Camera->Set(Camera->m_FOV, Eye, At);
	}
}