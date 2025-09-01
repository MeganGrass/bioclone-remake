/*
*
*	Megan Grass
*	March 2, 2025
*
*/

#include "app.h"

std::unique_ptr<Global_Application> G = std::make_unique<Global_Application>();

void Global_Application::About(void) const
{
	Standard_String Str;

	String AboutStr = Str.FormatCStyle("%ws - %ws", VER_INTERNAL_NAME_STR, VER_FILE_DESCRIPTION_STR);
	AboutStr += Str.FormatCStyle("\r\nv%ws %s %s Megan Grass", VER_PRODUCT_VERSION_STR, __DATE__, __TIME__);
	AboutStr += Str.FormatCStyle("\r\n\r\n<a href=\"https://github.com/MeganGrass/%ws\">https://github.com/megangrass/%ws</a>", VER_INTERNAL_NAME_STR, VER_INTERNAL_NAME_STR);

	AboutStr += Str.FormatCStyle("\r\n\r\nConfiguration:\r\n<a href=\"%ws\">%ws</a>", GetConfigFilename().wstring().c_str(), GetConfigFilename().wstring().c_str());
	AboutStr += Str.FormatCStyle("\r\n<a href=\"%ws\">%ws</a>", GetImGuiConfigFilename().wstring().c_str(), GetImGuiConfigFilename().wstring().c_str());

	AboutStr += Str.FormatCStyle("\r\n\r\nimgui: <a href=\"https://github.com/ocornut/imgui\">https://github.com/ocornut/imgui</a>");
	AboutStr += Str.FormatCStyle("\r\n\r\nlibjpeg-turbo: <a href=\"https://github.com/libjpeg-turbo/libjpeg-turbo\">https://github.com/libjpeg-turbo/libjpeg-turbo</a>");
	AboutStr += Str.FormatCStyle("\r\n\r\nlibpng: <a href=\"https://github.com/pnggroup/libpng\">https://github.com/pnggroup/libpng</a>");
	AboutStr += Str.FormatCStyle("\r\n\r\nzlib: <a href=\"https://github.com/madler/zlib\">https://github.com/madler/zlib</a>");

	Window->MessageModal(L"About", L"", Str.GetWide(AboutStr));
}

void Global_Application::Controls(void) const
{
	Standard_String Str;

	String ControlsStr = Str.FormatCStyle("CTRL+O -- Open RDT");
	ControlsStr += Str.FormatCStyle("\r\nCTRL+N -- New RDT");
	ControlsStr += Str.FormatCStyle("\r\nCTRL+S -- Save RDT");
	ControlsStr += Str.FormatCStyle("\r\nLEFT/RIGHT -- View previous/next camera");
	ControlsStr += Str.FormatCStyle("\r\nCTRL+LEFT/RIGHT -- View previous/next animation");
	ControlsStr += Str.FormatCStyle("\r\nCTRL+MOUSEWHEEL -- Adjust render zoom");
	ControlsStr += Str.FormatCStyle("\r\nF11 -- Enter/Exit fullscreen mode");
	ControlsStr += Str.FormatCStyle("\r\nCTRL+? -- About this application");
	ControlsStr += Str.FormatCStyle("\r\nESC -- Exit Application");

	Window->MessageModal(L"Controls", L"", Str.GetWide(ControlsStr));
}

void Global_Application::SetMaxRenderSize(uint32_t MaxWidth, uint32_t MaxHeight)
{
	b_SetMaxRenderSize = false;

	if (b_IgnoreAutoResize) { return; }

	m_RequestWidth = min(MaxWidth, (MaxWidth / static_cast<uint32_t>(Camera->m_NativeWidth)) * static_cast<uint32_t>(Camera->m_NativeWidth));
	m_RequestHeight = min(MaxHeight, (MaxHeight / static_cast<uint32_t>(Camera->m_NativeHeight)) * static_cast<uint32_t>(Camera->m_NativeHeight));

	Camera->SetOrtho(static_cast<float>(m_RequestWidth), static_cast<float>(m_RequestHeight));

	Camera->m_BackgroundVert.reset(Render->CreateVec4t(Camera->GetImageVert()));

	b_RequestRenderResize = true;
}

void Global_Application::RenderScene(void)
{
	if (b_RequestRenderResize)
	{
		b_RequestRenderResize = false;
		InitRender(m_RequestWidth, m_RequestHeight);
	}

	static IDirect3DSurface9* OriginalSurface = nullptr;
	static IDirect3DSurface9* TextureSurface = nullptr;

	{
		Render->Device()->GetRenderTarget(0, &OriginalSurface);
		Render->Device()->SetRenderTarget(0, m_RenderSurface.get());

		Render->Device()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(GetRValue(Window->GetColor()), GetGValue(Window->GetColor()), GetBValue(Window->GetColor())), 1.0f, 0);
		Render->Device()->BeginScene();
	}

	{
		Render->Device()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		Render->Device()->SetRenderState(D3DRS_CLIPPING, FALSE);

		Render->SetPSXLightToggle(false, true);

		if (Render->b_ViewGrid) { Render->DrawGrid(); }

		if (Render->b_ViewAxis) { Render->DrawAxis(); }

		DrawBackground();

		DrawSprite();

		DrawCamera();

		DrawSwitch();

		DrawCollision();

		DrawBlock();

		DrawFloor();

		Player->Routine();
	}

	{
		Render->Device()->EndScene();
		Render->Device()->PresentEx(NULL, NULL, NULL, NULL, NULL);

		m_RenderTexture->GetSurfaceLevel(0, &TextureSurface);
		Render->Device()->StretchRect(m_RenderSurface.get(), nullptr, TextureSurface, nullptr, D3DTEXF_NONE);

		Render->Device()->SetRenderTarget(0, OriginalSurface);

		OriginalSurface->Release();
		TextureSurface->Release();
	}
}

void Global_Application::Draw(void)
{
	if (Render->b_DeviceWasReset)
	{
		Render->b_DeviceWasReset = false;

		ImGui_ImplDX9_InvalidateDeviceObjects();
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	if (b_RequestFontChange)
	{
		b_RequestFontChange = false;

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF(Window->FontList()[Window->FontIndex()].string().c_str(), Window->FontSize());
		io.FontDefault = io.Fonts->Fonts.back();
		io.Fonts->Build();

		ImGui_ImplDX9_InvalidateDeviceObjects();
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	{
		//bool show_demo_window = true;
		//ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7.4f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 7.4f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 7.4f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.4f);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 7.4f);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 7.4f);
		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 7.4f);

		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));

		ImGui::GetStyle().WindowBorderSize = 0.0f;
		ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);

		MainMenu();

		float WindowWidth = ImGui::GetIO().DisplaySize.x - 4.0f;
		float WindowHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeightWithSpacing() - 2.0f;
		static constexpr float LeftPanelWidth = 200.0f;
		static constexpr float RightPanelWidth = 420.0f;
		float CenterPanelWidth = max(0.0f, WindowWidth - LeftPanelWidth - RightPanelWidth);

		if (b_SetMaxRenderSize)
		{
			SetMaxRenderSize(static_cast<uint32_t>(CenterPanelWidth), static_cast<uint32_t>(WindowHeight));
		}

		LeftPanel(ImVec2(2.0f, ImGui::GetFrameHeightWithSpacing()), ImVec2(LeftPanelWidth, WindowHeight));
		CenterPanel(ImVec2(LeftPanelWidth + 2.0f, ImGui::GetFrameHeightWithSpacing()), ImVec2(CenterPanelWidth, WindowHeight));
		RightPanel(ImVec2(LeftPanelWidth + CenterPanelWidth + 2.0f, ImGui::GetFrameHeightWithSpacing()), ImVec2(RightPanelWidth, WindowHeight));

		Options();

		Modal();

		ImGui::PopStyleColor();

		ImGui::PopStyleVar(7);
	}
	ImGui::EndFrame();

	{
		Render->Device()->SetRenderState(D3DRS_ZENABLE, FALSE);
		Render->Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Render->Device()->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		Render->Device()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(GetRValue(Window->GetColor()), GetGValue(Window->GetColor()), GetBValue(Window->GetColor())), 1.0f, 0);
		Render->Device()->BeginScene();

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		Render->Device()->EndScene();
		Render->Device()->PresentEx(NULL, NULL, NULL, NULL, NULL);
	}
}

void Global_Application::Input(void)
{
	if (!ImGui::GetKeyData(ImGuiKey_Escape)->DownDuration)
	{
		PostMessage(Window->Get(), WM_CLOSE, 0, 0);
		return;
	}

	if (!ImGui::GetKeyData(ImGuiKey_F11)->DownDuration)
	{
		b_RequestFullscreen = true;
	}

	if (!ImGui::GetKeyData(ImGuiKey_DownArrow)->DownDuration)
	{
	}

	if (!ImGui::GetKeyData(ImGuiKey_UpArrow)->DownDuration)
	{
	}

	if (!ImGui::GetKeyData(ImGuiKey_LeftArrow)->DownDuration)
	{
		if (ImGui::GetIO().KeyCtrl && Player->Animation(Player->AnimIndex())->IsOpen())
		{
			Player->iClip--;
			Player->iFrame = 0;
		}
		else if (!Player->b_EditorMode)
		{
			switch (Game)
			{
			case Video_Game::Resident_Evil_2_Trial:
			case Video_Game::Resident_Evil_2:
				if (Bio2->Rdt->IsOpen())
				{
					uint8_t i = Camera->SetImage(--Camera->Cut);
					Camera->Set(Bio2->Rdt->Rid->Get(i)->ViewR >> 7, Bio2->Rdt->Rid->Get(i)->View_p, Bio2->Rdt->Rid->Get(i)->View_r);
					SetLighting();
				}
				break;
			}
		}
	}

	if (!ImGui::GetKeyData(ImGuiKey_RightArrow)->DownDuration)
	{
		if (ImGui::GetIO().KeyCtrl && Player->Animation(Player->AnimIndex())->IsOpen())
		{
			Player->iClip++;
			Player->iFrame = 0;
		}
		else if (!Player->b_EditorMode)
		{
			switch (Game)
			{
			case Video_Game::Resident_Evil_2_Trial:
			case Video_Game::Resident_Evil_2:
				if (Bio2->Rdt->IsOpen())
				{
					uint8_t i = Camera->SetImage(++Camera->Cut);
					Camera->Set(Bio2->Rdt->Rid->Get(i)->ViewR >> 7, Bio2->Rdt->Rid->Get(i)->View_p, Bio2->Rdt->Rid->Get(i)->View_r);
					SetLighting();
				}
				break;
			}
		}
	}
}

void Global_Application::SetController(bool b_OnOff)
{
	Player->b_ControllerMode = b_OnOff;

	if (!Player->b_ControllerMode)
	{
		Player->Controller = [&]() {};
		return;
	}

	Player->b_Play.store(true);
	Player->b_LockPosition = true;
	Player->b_DrawReference = false;

	if (Player->ModelGame() & (AUG95 | OCT95 | BIO1) && Player->WeaponModelGame() & (AUG95 | OCT95 | BIO1))
	{
		InitPlayerStateBio1(Player, m_PlayerState);
	}
	else if (Player->ModelGame() & BIO2NOV96 && Player->WeaponModelGame() & BIO2NOV96)
	{
		InitPlayerStateBio2Nov96(Player, m_PlayerState);
	}
	else if (Player->ModelGame() & (BIO2TRIAL | BIO2) && Player->WeaponModelGame() & (BIO2TRIAL | BIO2))
	{
		InitPlayerStateBio2(Player, m_PlayerState);
	}

	Player->Controller = [this]() -> void
		{
			if (Player->ModelGame() & (AUG95 | OCT95 | BIO1) && Player->WeaponModelGame() & (AUG95 | OCT95 | BIO1))
			{
				ControllerInputBio1(m_PlayerState);
			}
			else if (Player->ModelGame() & BIO2NOV96 && Player->WeaponModelGame() & BIO2NOV96)
			{
				ControllerInputBio2Nov96(m_PlayerState);
			}
			else if (Player->ModelGame() & (BIO2TRIAL | BIO2) && Player->WeaponModelGame() & (BIO2TRIAL | BIO2))
			{
				ControllerInputBio2(m_PlayerState);
			}
		};
}

void Global_Application::DragAndDrop(StrVecW Files) const
{
	/*for (std::size_t i = 0; i < Files.size(); i++)
	{
		StringW FileExtension = FS.GetFileExtension(Files[i]).wstring();

		Str.ToUpper(FileExtension);

		if ((FileExtension == L".TIM") || (FileExtension == L".BS"))
		{
			Open(Files[i]);
		}
	}*/

	//if (!Files.empty()) { TextureIO(Files[0], ImageIO::All); }
}

void Global_Application::Commandline(StrVecW Args)
{
	for (std::size_t i = 0; i < Args.size(); i++)
	{
		if (Standard_String().ToUpper(Args[i]) == L"-FILE")
		{
			if ((i + 1 <= Args.size()) && (Standard_FileSystem().Exists(Args[i + 1])))
			{
				//b_OpenLastFileOnBoot = true;
				//m_Filename = Args[i + 1];
			}
		}
	}

	if (!Args.empty() && Standard_FileSystem().Exists(Args[0]))
	{
		//b_OpenLastFileOnBoot = true;
		//m_Filename = Args[0];
	}
}

void Global_Application::InitWin32(HINSTANCE hInstance)
{
	extern LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
	extern LRESULT CALLBACK RenderProc(HWND, UINT, WPARAM, LPARAM);

	int DefaultWidth = 1920;
	int DefaultHeight = 1080;

	if (b_BootMaximized)
	{
		m_BootWidth = DefaultWidth;
		m_BootHeight = DefaultHeight;
	}

	if (b_BootFullscreen)
	{
		b_BootMaximized = false;
		m_BootWidth = DefaultWidth;
		m_BootHeight = DefaultHeight;
	}

	if ((!m_BootWidth) || (m_BootWidth < DefaultWidth) || (!m_BootHeight) || (m_BootHeight < DefaultHeight))
	{
		m_BootWidth = DefaultWidth;
		m_BootHeight = DefaultHeight;
	}

	Window->PresetStyle(WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	Window->PresetStyleEx(WS_EX_ACCEPTFILES | WS_EX_APPWINDOW);
	Window->SetCaptionName(VER_PRODUCT_NAME_STR);
	Window->PresetClassName(VER_INTERNAL_NAME_STR);
	Window->SetIcon(hInstance, IDI_WINDOW);
	Window->SetIconSmall(hInstance, IDI_WINDOW);
	Window->Create(m_BootWidth, m_BootHeight, hInstance, SW_HIDE, WindowProc);

	Render->Initialize(Window->CreateChild(0, 0, m_BootWidth, m_BootHeight, hInstance, SW_SHOW, RenderProc, NULL, NULL), m_BootWidth, m_BootHeight, false);

	Window->SetTimer(60);

	while (!Render->NormalState()) { Window->SleepTimer(); }

	if (b_BootMaximized)
	{
		ShowWindow(Window->Get(), SW_SHOWMAXIMIZED);

		RECT Rect = Window->GetRect();
		WINDOWPOS WindowPos{};
		WindowPos.hwnd = Window->Get();
		WindowPos.hwndInsertAfter = nullptr;
		WindowPos.x = Rect.left;
		WindowPos.y = Rect.top;
		WindowPos.cx = (Rect.right - Rect.left);
		WindowPos.cy = (Rect.bottom - Rect.top);
		WindowPos.flags = SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_DEFERERASE | SWP_ASYNCWINDOWPOS;
		SendMessage(Render->Window()->Get(), WM_WINDOWPOSCHANGED, 0, (LPARAM)&WindowPos);
	}

	else if (b_BootFullscreen)
	{
		Window->AutoFullscreen();
	}

	else { ShowWindow(Window->Get(), SW_SHOWDEFAULT); }
}

void Global_Application::InitImGuiColor(void)
{
	m_BorderColor.r = static_cast<float>(GetRValue(Window->GetBorderColor())) / 255.0f;
	m_BorderColor.g = static_cast<float>(GetGValue(Window->GetBorderColor())) / 255.0f;
	m_BorderColor.b = static_cast<float>(GetBValue(Window->GetBorderColor())) / 255.0f;

	m_CaptionColor.r = static_cast<float>(GetRValue(Window->GetCaptionColor())) / 255.0f;
	m_CaptionColor.g = static_cast<float>(GetGValue(Window->GetCaptionColor())) / 255.0f;
	m_CaptionColor.b = static_cast<float>(GetBValue(Window->GetCaptionColor())) / 255.0f;

	m_WindowColor.r = static_cast<float>(GetRValue(Window->GetColor())) / 255.0f;
	m_WindowColor.g = static_cast<float>(GetGValue(Window->GetColor())) / 255.0f;
	m_WindowColor.b = static_cast<float>(GetBValue(Window->GetColor())) / 255.0f;

	m_FontColor.r = static_cast<float>(GetRValue(Window->GetTextColor())) / 255.0f;
	m_FontColor.g = static_cast<float>(GetGValue(Window->GetTextColor())) / 255.0f;
	m_FontColor.b = static_cast<float>(GetBValue(Window->GetTextColor())) / 255.0f;

	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(m_FontColor.r, m_FontColor.g, m_FontColor.b, 1.0f);

	ImGui::GetStyle().Colors[ImGuiCol_TextDisabled] = ImVec4(m_FontColor.r * 0.6f, m_FontColor.g * 0.6f, m_FontColor.b * 0.6f, 1.0f);

	ImGui::GetStyle().Colors[ImGuiCol_PopupBg] = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];

	ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg] = ImVec4(m_CaptionColor.r, m_CaptionColor.g, m_CaptionColor.b, 1.0f);

	ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(m_CaptionColor.r, m_CaptionColor.g, m_CaptionColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImVec4(m_CaptionColor.r, m_CaptionColor.g, m_CaptionColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(m_CaptionColor.r, m_CaptionColor.g, m_CaptionColor.b, 1.0f);

	ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(m_CaptionColor.r, m_CaptionColor.g, m_CaptionColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(m_BorderColor.r * 2.0f, m_BorderColor.g * 2.0f, m_BorderColor.b * 2.0f, 1.0f);

	ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(m_CaptionColor.r, m_CaptionColor.g, m_CaptionColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(m_BorderColor.r * 2.0f, m_BorderColor.g * 2.0f, m_BorderColor.b * 2.0f, 1.0f);

	ImGui::GetStyle().Colors[ImGuiCol_Tab] = ImVec4(m_CaptionColor.r, m_CaptionColor.g, m_CaptionColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = ImVec4(m_BorderColor.r * 2.0f, m_BorderColor.g * 2.0f, m_BorderColor.b * 2.0f, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_TabSelected] = ImVec4(m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_TabSelectedOverline] = ImVec4(1, 0, 0, 0);

	ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(m_CaptionColor.r, m_CaptionColor.g, m_CaptionColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(m_BorderColor.r * 2.0f, m_BorderColor.g * 2.0f, m_BorderColor.b * 2.0f, 1.0f);
}

void Global_Application::InitImGui(void)
{
	m_ConfigStr = GetImGuiConfigFilename().string();

	IMGUI_CHECKVERSION();

	Context.reset(ImGui::CreateContext());

	InitImGuiColor();

	ImGui::GetIO().IniFilename = m_ConfigStr.c_str();
	ImGui::GetIO().Fonts->Clear();
	ImGui::GetIO().Fonts->AddFontFromFileTTF(Window->GetFont().string().c_str(), Window->FontSize());
	ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->Fonts.back();

	ImGui_ImplWin32_Init(Render->Window()->Get());
	ImGui_ImplDX9_Init(Render->Device());
}

void Global_Application::InitRender(uint32_t Width, uint32_t Height)
{
	m_RequestWidth = m_RenderWidth = Width;
	m_RequestHeight = m_RenderHeight = Height;

	m_RenderTexture.reset(Render->CreateTexture(static_cast<std::uint16_t>(m_RenderWidth), static_cast<std::uint16_t>(m_RenderHeight), D3DUSAGE_RENDERTARGET));
	m_RenderTexture->GetLevelDesc(0, &m_RenderDesc);

	m_RenderSurface.reset(Render->CreateRenderSurface(m_RenderWidth, m_RenderHeight, D3DFMT_A8R8G8B8));

	Render->CreateAxisGrid();
}

void Global_Application::InitGame(void)
{
	{
		Geometry->Init();
	}
	{
		Camera->Str.hWnd = Window->Get();

		Camera->SetOrtho(static_cast<float>(m_RenderWidth), static_cast<float>(m_RenderHeight));

		Camera->m_BackgroundVert.reset(Render->CreateVec4t(Camera->GetImageVert()));

		Camera->Set(Camera->m_FOV, Camera->m_Eye, Camera->m_At);
	}
	{
		ResetLighting();
	}

	{
		Player->PlatformSetup(Window->Get(), GTE, Render, Camera->b_HorzFlip, Camera->b_VertFlip);

		Player->Hitbox().w = 450;
		Player->Hitbox().h = -1530;
		Player->Hitbox().d = 450;

		Player->iHealth = Player->iHealthMax = 200;

		Player->Routine = [this]() -> void
			{
				Player->Controller();

				Collision(Player->ModelType(), Player->Position(), Player->Hitbox());

				CameraSwitch(Player->Position(), Player->Hitbox());

				if (Player->b_DrawHitbox) { Geometry->DrawCylinder(Player->HitboxShape(), Player->b_EditorMode ? Player->EditorRotation() : Player->Rotation(), 0x00C5C5C5, true); }

				Render->SetPSXLightToggle(b_PerPixelLighting, !b_PerPixelLighting && !b_PerVertexLighting ? true : false);

				Player->Draw();

				Render->SetPSXLightToggle(true, false);

				Player->DrawShadow();
			};

		if (Standard_FileSystem().Exists(m_BootPlayerFilename))
		{
			Player->Open(m_BootPlayerFilename);
		}

		if (Standard_FileSystem().Exists(m_BootWeaponFilename) && Player->ModelDX9())
		{
			Player->Open(m_BootWeaponFilename);
		}

		if (Player->ModelDX9() && Player->WeaponModelDX9())
		{
			SetController(true);
		}
	}
}

void Global_Application::Shutdown(void)
{
	if (b_Shutdown) { return; }

	SaveConfig();

	b_Shutdown = true;

	Modal = []() {};

	if (b_ControllerMapping.load() || b_RoomcutExtraction.load())
	{
		b_ControllerMapping.store(false);
		b_RoomcutExtraction.store(false);

		auto StartTime = std::chrono::steady_clock::now();

		while (true)
		{
			std::this_thread::yield();
			if (std::chrono::steady_clock::now() - StartTime > std::chrono::seconds(1)) { break; }
		}
	}

	auto StartTime = std::chrono::steady_clock::now();

	while (!ThreadPool.Stop())
	{
		std::this_thread::yield();
		if (std::chrono::steady_clock::now() - StartTime > std::chrono::seconds(1)) { break; }
	}

	Render->b_ViewGrid = false;

	Render->b_ViewAxis = false;

	Player->Shutdown();

	Geometry->Shutdown();

	Camera->Shutdown();

	CloseRDT();

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();

	Render->Shutdown();

	DestroyWindow(Window->Get());
}

int Global_Application::Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	InitConfig();
	Commandline(Window->GetCommandline(lpCmdLine));
	InitWin32(hInstance);
	InitImGui();
	InitRender(m_RenderWidth, m_RenderHeight);
	InitGame();

	MSG msg{ Window->Get(), 0U, 0U, 0, 0U, { 0, 0 } };
	
	while (b_Active)
	{
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if (b_RequestFullscreen)
		{
			b_RequestFullscreen = false;
			Window->AutoFullscreen();
		}

		if (!Window->GetDroppedFiles().empty()) { DragAndDrop(Window->GetDroppedFiles()); Window->GetDroppedFiles().clear(); }

		Window->ChronoTimerInit(59.94);

		ThreadPool.Enqueue([this]()
			{
				if (!b_Active || Window->IsMinimized() || !Render->NormalState() || !Context) { return; }

				Input();

				RenderScene();

				Draw();
			});

		Window->ChronoTimerSleep();
	}

	Shutdown();

	return (int)msg.wParam;
}