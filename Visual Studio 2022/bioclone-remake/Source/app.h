/*
*
*	Megan Grass
*	March 2, 2025
*
*/

#pragma once

#include <resource.h>

#include <std_window.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_stdlib.h>

#include <bio_gamepad.h>

#include <bio_geometry.h>

#include <bio_camera.h>

#include <bio1.h>

#include <bio2.h>

#include <bio2_nov96.h>

#include <bio3.h>

#include <bio_cdx.h>

#include <bio_disk.h>

typedef class Global_Application Global;

extern std::unique_ptr<Global_Application> G;

class Global_Application final :
	public Resident_Evil_Common {
private:

	String m_ConfigStr;

	colorvec m_BorderColor, m_CaptionColor, m_WindowColor, m_FontColor;

	std::unique_ptr<IDirect3DTexture9, IDirect3DDelete9<IDirect3DTexture9>> m_RenderTexture;
	std::unique_ptr<IDirect3DSurface9, IDirect3DDelete9<IDirect3DSurface9>> m_RenderSurface;
	D3DSURFACE_DESC m_RenderDesc;

	std::uint32_t m_RenderWidth, m_RenderHeight, m_RequestWidth, m_RequestHeight;

	float m_RenderZoom, m_RenderZoomMin, m_RenderZoomMax;

	double m_TargetFPS;

	float m_MouseX, m_MouseY;
	float m_MouseScaledX, m_MouseScaledY;

	int m_BootWidth, m_BootHeight;
	bool b_BootMaximized, b_BootFullscreen;

	bool b_RequestFullscreen;

	bool b_RequestFontChange;

	bool b_RequestRenderResize;
	bool b_IgnoreAutoResize;

	bool b_ViewWindowOptions;
	bool b_ShowFPS;

	bool b_Shutdown;

	bool b_PerVertexLighting;
	bool b_PerPixelLighting;

	std::function<void()> Modal;

	Standard_Thread_Pool ThreadPool;

	Standard_Thread_Pool FilePool;
	bool b_RoomFileOp;

	std::atomic<bool> b_RoomcutExtraction;
	std::filesystem::path m_RoomcutNameID;
	float m_RoomcutProgress;

	std::atomic<bool> b_ControllerMapping;

	std::unique_ptr<Resident_Evil_Gamepad> Gamepad;

	std::unique_ptr<Resident_Evil> Bio1;
	std::unique_ptr<Resident_Evil_2_Nov96> Bio2Nov96;
	std::unique_ptr<Resident_Evil_2> Bio2;
	std::unique_ptr<Resident_Evil_3> Bio3;
	std::unique_ptr<CDX_File_Container> Cdx;
	std::unique_ptr<Capcom_Disk> Exe;

	std::shared_ptr<Resident_Evil_Animation> Rbj;

	std::unique_ptr<Resident_Evil_Model> Player;
	std::filesystem::path m_BootPlayerFilename;
	std::filesystem::path m_BootWeaponFilename;

	std::unique_ptr<ImGuiContext, decltype(&ImGui::DestroyContext)> Context;

	void InitConfig(void);
	void OpenConfig(void);
	const std::filesystem::path GetConfigFilename(void) const { return Window->GetUserDocumentsDir() / VER_INTERNAL_NAME_STR / L"config.ini"; }
	const std::filesystem::path GetImGuiConfigFilename(void) const { return Window->GetUserDocumentsDir() / VER_INTERNAL_NAME_STR / L"imgui.ini"; }

	void Tooltip(String Tip);
	void TooltipOnHover(String Tip);
	bool ScrollOnHover(void* Input, ImGuiDataType DataType, std::uintmax_t Step, std::uintmax_t Min, std::uintmax_t Max, std::function<void(void)> OnComplete = [&]() {}) const;
	bool ScrollFloatOnHover(void* Input, ImGuiDataType DataType, double Step, double Min, double Max, std::function<void(void)> OnComplete = [&]() {}) const;
	bool ScrollComboOnHover(String ID, void* Input, ImGuiDataType DataType, std::uintmax_t Step, std::uintmax_t Min, std::uintmax_t Max, std::function<void(void)> OnComplete = [&]() {}) const;
	void DrawHorizontalLine(float HorizontalIndent, float VerticalIndent, float Thickness, float Red, float Green, float Blue, float Alpha = 1.0f);
	void DrawVerticalLine(float HorizontalIndent, float VerticalIndent, float Thickness, float Red, float Green, float Blue, float Alpha = 1.0f);

	bool IsRoomOpen(void);
	void CloseRDT(void);
	void OpenRDT(void);
	void SaveRDT(void);
	void SetRoomAnimations(void);
	void OpenPlayerModel(std::filesystem::path Filename = L"");
	void OpenPlayerTexture(std::filesystem::path Filename = L"");
	void SavePlayerTexture(void);
	void Screenshot(void);

	void RoomcutExtract(std::filesystem::path Filename);
	void RoomcutModal(void);
	void OnRoomcutComplete(std::filesystem::path Directory);

	void CollisionEditor(void);
	void ModelEditor(void);

	void ControllerMapping(void);
	void ControllerInput(std::unique_ptr<Resident_Evil_Model>& Model);

	void DrawBackground(void);
	void DrawCamera(void);
	void DrawSprite(void);
	void DrawSwitch(void);
	void DrawCollision(void);
	void DrawBlock(void);
	void DrawFloor(void);
	void ResetLighting(void) const;
	void SetLighting(void);
	void Collision(ModelType ModelType, VECTOR2& Position, SIZEVECTOR Hitbox);
	void CameraSwitch(VECTOR2& Position, SIZEVECTOR Hitbox);

	void MainMenu(void);
	void Options(void);
	void RenderWindow(void);
	void LeftPanel(ImVec2 Position, ImVec2 Size);
	void CenterPanel(ImVec2 Position, ImVec2 Size);
	void RightPanel(ImVec2 Position, ImVec2 Size);

	void SetMaxRenderSize(uint32_t MaxWidth, uint32_t MaxHeight);
	void RenderScene(void);
	void Draw(void);
	void Input(void);
	void InitRender(uint32_t Width, uint32_t Height);
	void InitGame(void);
	void InitImGuiColor(void);
	void InitImGui(void);
	void InitWin32(HINSTANCE hInstance);
	void DragAndDrop(StrVecW Files) const;
	void Commandline(StrVecW Args);

	Global_Application(Global_Application&&) = delete;
	Global_Application& operator = (Global_Application&&) = delete;

	Global_Application(const Global_Application&) = delete;
	Global_Application& operator = (const Global_Application&) = delete;

public:

	bool b_Active;

	bool b_SetMaxRenderSize;

	std::unique_ptr<Standard_Window> Window;

	std::shared_ptr<Standard_DirectX_9> Render;

	std::shared_ptr<Sony_PlayStation_GTE> GTE;

	std::unique_ptr<Resident_Evil_Camera> Camera;

	std::unique_ptr<Resident_Evil_Geometry> Geometry;

	Global_Application(void) :
		Context(nullptr, &ImGui::DestroyContext),
		Window(std::make_unique<Standard_Window>()),
		Render(std::make_shared<Standard_DirectX_9>()),
		GTE{ std::make_shared<Sony_PlayStation_GTE>() },
		Gamepad(std::make_unique<Resident_Evil_Gamepad>()),
		Bio1{ std::make_unique<Resident_Evil>() },
		Bio2Nov96{ std::make_unique<Resident_Evil_2_Nov96>() },
		Bio2{ std::make_unique<Resident_Evil_2>() },
		Bio3{ std::make_unique<Resident_Evil_3>() },
		Cdx{ std::make_unique<CDX_File_Container>() },
		Exe{ std::make_unique<Capcom_Disk>() },
		Player{ std::make_unique<Resident_Evil_Model>() },
		m_ConfigStr(),
		m_BorderColor{},
		m_CaptionColor{},
		m_WindowColor{},
		m_FontColor{},
		m_RenderDesc(),
		m_RenderWidth(1280),
		m_RenderHeight(960),
		m_RequestWidth(1280),
		m_RequestHeight(960),
		m_RenderZoom(1.0f),
		m_RenderZoomMin(1.0f),
		m_RenderZoomMax(128.0f),
		m_TargetFPS(60.0),
		m_MouseX(0.0f),
		m_MouseY(0.0f),
		m_MouseScaledX(0.0f),
		m_MouseScaledY(0.0f),
		m_BootWidth(1920),
		m_BootHeight(1080),
		b_BootMaximized(false),
		b_BootFullscreen(false),
		b_RequestFullscreen(false),
		b_RequestFontChange(false),
		b_RequestRenderResize(false),
		b_IgnoreAutoResize(false),
		b_SetMaxRenderSize(false),
		b_ViewWindowOptions(false),
		b_ShowFPS(false),
		b_Shutdown(false),
		b_PerVertexLighting(false),
		b_PerPixelLighting(false),
		b_RoomFileOp(false),
		m_RoomcutProgress(0.0f),
		b_RoomcutExtraction(false),
		b_ControllerMapping(false),
		b_Active(true)
	{
		ThreadPool.InitPool(1);
		FilePool.InitPool(1);
		Game = Video_Game::Resident_Evil_2;
		Camera = std::make_unique<Resident_Evil_Camera>(Render, GTE);
		Geometry = std::make_unique<Resident_Evil_Geometry>(Render, GTE);
		Modal = [&]() {};
	}
	~Global_Application(void) = default;

	void About(void) const;

	void Controls(void) const;

	void Shutdown(void);

	void SaveConfig(void);

	int Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
};