/*
*
*	Megan Grass
*	March 2, 2025
*
*/

#pragma once

#include <resource.h>

#include <std_window.h>

#include <bio_gamepad.h>
#include <bio_geometry.h>
#include <bio_camera.h>
#include <bio_room.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_stdlib.h>
#include <imgui_memory_editor.h>

typedef class Global_Application Global;

extern std::unique_ptr<Global_Application> G;

enum class PanelType : std::int32_t
{
	Pers3D = (1 << 0),
	Top2D = (1 << 1),
	CharacterModel = (1 << 2),
	RoomModel = (1 << 3),
	Effect = (1 << 4),
	Script = (1 << 5),
};

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

	PanelType m_PanelType;

	Standard_Thread_Pool MainOp;

	std::function<void()> Modal;
	std::atomic<bool> b_ModalOp;

	Standard_Thread_Pool FileOp;
	std::atomic<bool> b_FileOp;

	std::filesystem::path m_RoomcutNameID;
	float m_RoomcutProgress;

	std::unique_ptr<Resident_Evil_Gamepad> Gamepad;

	static std::shared_ptr<Resident_Evil_Model> Model;

	std::filesystem::path m_BootPlayerFilename;
	std::filesystem::path m_BootWeaponFilename;
	std::shared_ptr<Resident_Evil_Model> Player;
	std::unique_ptr<StateMachineType> m_PlayerState;

	std::unique_ptr<Resident_Evil_Geometry> Geometry;

	std::unique_ptr<Resident_Evil_Camera> Camera;

	std::shared_ptr<Resident_Evil_Room> Room;

	std::shared_ptr<Sony_PlayStation_GTE> GTE;

	std::unique_ptr<ImGuiContext, decltype(&ImGui::DestroyContext)> Context;

	void InitConfig(void);
	void OpenConfig(void);
	void SaveConfig(void);
	const std::filesystem::path GetConfigFilename(void) const { return Window->GetUserDocumentsDir() / VER_INTERNAL_NAME_STR / L"config.ini"; }
	const std::filesystem::path GetImGuiConfigFilename(void) const { return Window->GetUserDocumentsDir() / VER_INTERNAL_NAME_STR / L"imgui.ini"; }

	void Tooltip(String Tip);
	void TooltipOnHover(String Tip);
	const bool ScrollOnHover(void* Input, ImGuiDataType DataType, std::uintmax_t Step, std::uintmax_t Min, std::uintmax_t Max, std::function<void(void)> OnComplete = [&]() {}) const;
	const bool ScrollFloatOnHover(void* Input, ImGuiDataType DataType, double Step, double Min, double Max, std::function<void(void)> OnComplete = [&]() {}) const;
	const bool ScrollComboOnHover(String ID, void* Input, ImGuiDataType DataType, std::uintmax_t Step, std::uintmax_t Min, std::uintmax_t Max, std::function<void(void)> OnComplete = [&]() {}) const;
	void DrawHorizontalLine(float HorizontalIndent, float VerticalIndent, float Thickness, float Red, float Green, float Blue, float Alpha = 1.0f);
	void DrawVerticalLine(float HorizontalIndent, float VerticalIndent, float Thickness, float Red, float Green, float Blue, float Alpha = 1.0f);

	void RoomcutExtract(std::filesystem::path Filename);
	void RoomcutModal(void);
	void OnRoomcutComplete(std::filesystem::path Directory);

	const bool IsRoomOpen(void);
	void CloseRDT(const bool b_AskSave = false);
	void OpenRDT(const std::filesystem::path Filename = L"", std::uintmax_t iCut = 0);
	void SaveRDT(void);
	void OpenModel(std::shared_ptr<Resident_Evil_Model>& Model, const bool b_LinkRoom, const std::filesystem::path Filename = L"");
	void OpenModelTexture(std::shared_ptr<Resident_Evil_Model>& Model, const std::filesystem::path Filename = L"");
	void SaveModelTexture(std::shared_ptr<Resident_Evil_Model>& Model);
	void Screenshot(void);

	void CollisionEditor(void);
	void ModelEditor(const bool b_RoomModel);
	void EffectEditor(void);

	void InitPlayerStateBio1(std::uintmax_t iWeapon);
	void InitPlayerStateBio2Nov96(std::uintmax_t iWeapon);
	void InitPlayerStateBio2(std::uintmax_t iWeapon);

	void InitPlayerState(std::shared_ptr<Resident_Evil_Model> Model, std::unique_ptr<StateMachineType>& State);
	void ControllerInput(std::unique_ptr<StateMachineType>& State);

	void DrawBackground(void);
	void DrawCamera(void);
	void DrawSprite(void);
	void DrawSwitch(void);
	void DrawCollision(void);
	void DrawBlock(void);
	void DrawFloor(void);
	void DrawEffect(std::size_t iElement, VECTOR2 Position, const float Scale = 1.0f / (ONE / 32));
	void ResetLighting(void) const;
	void SetLighting(void);
	void Collision(ModelType ModelType, VECTOR2& Position, SIZEVECTOR Hitbox);
	void CameraSwitch(VECTOR2& Position, SIZEVECTOR Hitbox);

	void MainMenu(void);
	void Options(void);
	void ControllerMapping(void);
	void RenderWindow(void);
	void SetCenterPanel(PanelType Type);
	void LeftPanel(ImVec2 Position, ImVec2 Size);
	void CenterPanel(ImVec2 Position, ImVec2 Size);
	void RightPanel(ImVec2 Position, ImVec2 Size);
	void MouseCamera(VECTOR2& Eye, VECTOR2& At, bool b_Editor = true);
	const bool IsPanelType(PanelType Type) const { return (static_cast<std::int32_t>(m_PanelType) & static_cast<std::int32_t>(Type)) != 0; }

	void About(void) const;
	void Controls(void) const;
	void SetMaxRenderSize(const uint32_t MaxWidth, const uint32_t MaxHeight);
	void RenderScene(void);
	void Draw(void);
	void Input(void);
	void SetController(const bool b_OnOff);
	void InitRender(const uint32_t Width, const uint32_t Height);
	void InitGame(void);
	void InitImGuiColor(void);
	void InitImGui(void);
	void InitWin32(HINSTANCE hInstance);
	void DragAndDrop(StrVecW Files) const;
	void Commandline(StrVecW Args);
	void Shutdown(void);

	Global_Application(Global_Application&&) = delete;
	Global_Application& operator = (Global_Application&&) = delete;

	Global_Application(const Global_Application&) = delete;
	Global_Application& operator = (const Global_Application&) = delete;

public:

	bool b_Active;

	bool b_SetMaxRenderSize;

	std::unique_ptr<Standard_Window> Window;

	std::shared_ptr<Standard_DirectX_9> Render;

	Global_Application(void) :
		Context(nullptr, &ImGui::DestroyContext),
		Window{ std::make_unique<Standard_Window>() },
		Render{ std::make_shared<Standard_DirectX_9>() },
		GTE{ std::make_shared<Sony_PlayStation_GTE>() },
		Gamepad{ std::make_unique<Resident_Evil_Gamepad>() },
		Player{ std::make_shared<Resident_Evil_Model>() },
		Room{ std::make_shared<Resident_Evil_Room>() },
		m_PanelType(PanelType::Pers3D),
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
		m_RoomcutProgress(0.0f),
		b_Active(true)
	{
		b_ModalOp.store(false);
		b_FileOp.store(false);
		MainOp.InitPool(1);
		FileOp.InitPool(1);
		Camera = std::make_unique<Resident_Evil_Camera>(Render, GTE);
		Geometry = std::make_unique<Resident_Evil_Geometry>(Render, GTE);
		Modal = [&]() {};
	}
	~Global_Application(void) = default;

	int Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
};