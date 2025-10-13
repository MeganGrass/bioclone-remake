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
#include <bio_message.h>

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

#pragma pack(push, 1)
struct Scenario
{
	struct AOT
	{
		UCHAR Id;						// Sce ID
		UCHAR Type;
		UCHAR nFloor;
		UCHAR Super;
		SHORT X;
		SHORT Z;
		USHORT W;
		USHORT D;
		SHORT Xz[4][2];
		USHORT Data0;
		USHORT Data1;
		USHORT Data2;
		SHORT Next_pos_x;				// X Position of Player in next RDT
		SHORT Next_pos_y;				// Y Position of Player in next RDT
		SHORT Next_pos_z;				// Z Position of Player in next RDT
		SHORT Next_cdir_y;				// Y Rotation of Player in next RDT
		UCHAR Next_stage;				// Numerical ID of Stage for next RDT file
		UCHAR Next_room;				// Numerical ID of Room for next RDT file
		UCHAR Next_cut;					// Numerical ID of Cut for next RDT file
		UCHAR Next_nfloor;				// Player distance from ground in next RDT (actual is nFloor * -1800)
		UCHAR Dtex_type;				// Numerical ID of DO2 file
		UCHAR Door_type;				// Numerical ID of DO2 animation
		UCHAR Knock_type;				// Numerical ID of VAG sound from VAB soundbank (always 0x00)
		UCHAR Key_id;					// G.Savegame.Key_flg bit (0x00 for unlocked door)
		UCHAR Key_type;					// Numerical ID of Item required to unlock door
		UCHAR Free;						// always 0x00
		USHORT iItem;					// Numerical ID of Item
		USHORT nItem;					// Numerical count of Item
		USHORT Item_flg;				// G.Savegame.Item_flg or G.Savegame.Item_flg2 Bit
		UCHAR Om_no;					// Numerical ID of MD1 model file (0xFF for none)
		UCHAR Action;					// G.Ob_model[Om_no].Free0
											// 0 = Stand to obtain
											// 1 = Kneel to obtain
											// 2 = G.Ob_model[Om_no].Be_flg = 0x80000000;
		UCHAR Rnd_flg;					// Something to do with G.Savegame.Item_flg/Item_flg2
		bool Is4P;						// if true, use SHORT Xz[4][2]
										// if false, use SHORT X, Z, W, D
	};

	struct Task
	{
		uint8_t Routine0;
		uint8_t Status;
		int8_t Sub_ctr;
		uint8_t Task_level;
		int8_t Ifel_ctr[4];
		int8_t Loop_ctr[4];
		int8_t Loop_if_class[4][4];
		uint8_t* Data;
		uint8_t* Lstack[4][4];
		uint8_t* Lbreak[4][4];
		int16_t Lcnt[4][4];
		uintptr_t Stack[4][8];	//uint32_t Stack[4][8];
		uintptr_t* pS_SP;		//uint32_t* pS_SP;
		uint8_t* Ret_addr[4];
		std::shared_ptr<Resident_Evil_Model> pWork;
		SVECTOR2 Speed{};
		SVECTOR2 DSpeed{};
		SVECTOR2 ASpeed{};
		SVECTOR2 ADSpeed{};
		uint32_t R_no_bak;
	};

	std::array<AOT, 32> Sce_aot{};

	std::array<Task, 14> Sce_task{};

	uint32_t Random_base{ 0 };
	uint8_t* pCScd{ 0 };
	std::shared_ptr<Resident_Evil_Model> pC_em{ nullptr };
	POLY_GT4* pMizu_div{ 0 };
	uint8_t Type{ 0 };
	uintmax_t Cut_old{ 0 };		// uint8_t Cut_old{ 0 };
	uint8_t C_id{ 0 };
	uint8_t C_model_type{ 0 };
	uint8_t C_kind{ 0 };
	uint8_t Mizu_div_max{ 0 };
	uint8_t Mizu_div_ctr{ 0 };
	uint8_t Rbj_reset_flg{ 0 };
	uint8_t Se_tmp0{ 0 };
	uint8_t Se_tmp1{ 0 };
	uint8_t Se_vol{ 0 };

	AOT* pCAot{ nullptr };

	int16_t Old_Fatari{};
	int16_t Old_Uatari{};

	struct SAVE_DATA
	{
		struct CARD
		{
			uint8_t magic[2];
			uint8_t type;
			uint8_t blockEntry;
			uint8_t title[64];
			uint8_t reserve[28];
			uint8_t clut[32];
			uint8_t icon[3][128];
		} Card_Head{};
		uint32_t Game_play_time{};
		uint32_t Front_play_time{};
		uint8_t Max_item_num{};
		uint8_t Stereo{};
		uint8_t Pl_id{};
		uint8_t Save_area{};
		uint8_t Vol_Se{};
		uint8_t Vol_Bgm{};
		uint8_t Key_idx{};
		uint8_t Pl_poison_down{};
		uint16_t Pl_poison_timer{};
		uint16_t Dummy16{};
		uint32_t Status_bak{};
		uint8_t Equip_id{};
		uint8_t Equip_no{};
		int16_t Pl_life{};
		uint16_t Zonbi_cnt{};
		uint16_t Cure_cnt{};
		uint16_t Save_cnt{};
		int16_t Pl_pos_x{};
		int16_t Pl_pos_y{};
		int16_t Pl_pos_z{};
		uint32_t Weapon_mugen{};
		uint16_t Bgm_tbl[142]{};
		int16_t F_atari{};
		int16_t U_atari{};
		int16_t Use_id{};
		int16_t Get_id{};
		int16_t Sce_work[8]{};
		int16_t DSce_work[4]{};
		int16_t Sce_tmp_work[8]{};
		int16_t Stage_no{};
		int16_t Room_no{};
		int16_t Cut_no{};
		int16_t Room_no_old{};
		int16_t Cut_no_old{};
		int16_t Sce_random{};
		int16_t Player_life{};
		int16_t Sce_timer{};
		int16_t Change_player{};
		int16_t Def_em_set{};
		int16_t Def_aot_set{};
		int16_t Count_down_timer{};
		uint32_t Scenario_flg[8]{};
		uint32_t Common_flg[8]{};
		uint32_t Room_flg[2]{};
		uint32_t Enemy_flg[8]{};
		uint32_t Enemy_flg2[8]{};
		uint32_t Item_flg[8]{};
		uint32_t Item_flg2[4]{};
		uint32_t Item_c_flg[7]{};
		uint32_t Map_o_flg{};
		uint32_t Map_flg[4]{};
		uint32_t Map_c_flg[2]{};
		uint32_t Map_i_flg{};
		uint32_t Pri_be_flg[16][4]{};
		uint32_t File_flg{};
		uint32_t Zapping_flg[2]{};
		uint32_t Key_flg[2]{};
		struct ITEM
		{
			uint8_t Id{};
			uint8_t Num{};
			uint8_t Size{};
			uint8_t dummy{};
		} Item_work[11]{};
		struct ITEM_BOX
		{
			uint8_t Id{};
			uint8_t Num{};
			uint8_t Size{};
			uint8_t dummy{};
		} Item_boxwork[64]{};
		uint8_t File_tag_bk[24]{};
		int16_t Dummy0{};
		uint8_t Dummy1[2]{};
		uint16_t Dummy[2]{};
		uint32_t ExData0[8]{};
		uint32_t ExData1[4]{};
		uint32_t ExData2[8]{};
		uint32_t ExData3[25]{};
	} SaveData{};

	uint32_t Random{};
	uint32_t System_flg{};
	uint32_t Status_flg{};
	uint32_t Stop_flg{};
	uint32_t Use_flg[4]{};
	uint32_t Mess_flg{};
	uint32_t Room_enemy_flg{};
	uint32_t Room_player_flg{};
	uint32_t Rbj_set_flg{};
	uint32_t nSce_aot{};
	uint16_t Get_item_id{};
	uint8_t Next_cut_no{};
	uint8_t Mirror_flg{};
	uint16_t Mirror_pos{};
	uint16_t Mirror_max{};
	uint16_t Mirror_min{};

	uint32_t Stop_bak{};

	std::shared_ptr<Resident_Evil_Model> pEm;

	Resident_Evil_2_RVD_Data* Cccut{ nullptr };
	Resident_Evil_2_RVD_Data* Cccut_next{ nullptr };

	// TEMP
	void NullScheduler(void);

	// Initialize Scheduler
	void SceSchedulerSet(void);

	// Run Scheduler
	void SceScheduler(void);

	// Process AOT (doors, items, triggers, etc)
	void ProcessAOT(void);

	// Last activated AOT trigger
	std::string AotStr = "";

};
#pragma pack(pop)

class Global_Application final :
	private Resident_Evil_Common {
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
	bool b_ViewScenarioSetup;
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

	Standard_Thread_Pool ScriptOp;
	std::atomic<bool> b_ScriptOp;
	std::atomic<bool> b_DrawAot;

	std::filesystem::path m_RoomcutNameID;
	float m_RoomcutProgress;

	std::unique_ptr<Resident_Evil_Gamepad> Gamepad;

	static std::shared_ptr<Resident_Evil_Model> Model;

	std::filesystem::path m_BootPlayerFilename;
	std::filesystem::path m_BootWeaponFilename;

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

	void CloseRDT(const bool b_AskSave = false);
	void OpenModel(std::shared_ptr<Resident_Evil_Model>& Model, const bool b_LinkRoom, const std::filesystem::path Filename = L"");
	void OpenModelTexture(std::shared_ptr<Resident_Evil_Model>& Model, const std::filesystem::path Filename = L"");
	void SaveModelTexture(std::shared_ptr<Resident_Evil_Model>& Model);
	void Screenshot(void);

	void CollisionEditor(void);
	void ScenarioEditor(void);
	void SceModelEditor(void);
	void ModelEditor(const bool b_RoomModel);
	void EffectEditor(void);

	void InitPlayerStateBio1(std::uintmax_t iWeapon);
	void InitPlayerStateBio2Nov96(std::uintmax_t iWeapon);
	void InitPlayerStateBio2(std::uintmax_t iWeapon);

	void InitPlayerState(std::shared_ptr<Resident_Evil_Model> Model, std::unique_ptr<StateMachineType>& State);
	void ControllerInput(std::unique_ptr<StateMachineType>& State);

	void DrawMessage(void);
	void DrawBackground(void);
	void DrawCamera(void);
	void DrawSprite(void);
	void DrawSwitch(void);
	void DrawCollision(void);
	void DrawBlock(void);
	void DrawFloor(void);
	void DrawScenario(void);
	void DrawEffect(std::size_t iElement, VECTOR2 Position, const float Scale = 1.0f / (ONE / 32));
	void Collision(ModelType ModelType, VECTOR2& Position, SIZEVECTOR Hitbox);
	void CameraSwitch(VECTOR2 Position);

	void MainMenu(void);
	void Options(void);
	void ScenarioDirectory(void);
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

	std::unique_ptr<Scenario> Sce;

	std::shared_ptr<Resident_Evil_Model> Player;
	std::shared_ptr<Resident_Evil_Model> SubPlayer;
	std::vector<std::shared_ptr<Resident_Evil_Model>> Enemy;
	std::vector<std::shared_ptr<Resident_Evil_Model>> Door;

	std::unique_ptr<Resident_Evil_Geometry> Geometry;

	std::unique_ptr<Resident_Evil_Message> Message;

	std::unique_ptr<Resident_Evil_Camera> Camera;

	std::shared_ptr<Resident_Evil_Room> Room;

	std::filesystem::path m_DataPath;
	std::filesystem::path m_PlayerPath;
	std::filesystem::path m_EnemyPath;
	std::filesystem::path m_DoorPath;

	std::atomic<bool> b_Pause;

	bool b_Active;

	bool b_SetMaxRenderSize;

	std::unique_ptr<Standard_Window> Window;

	std::shared_ptr<Standard_DirectX_9> Render;

	std::shared_ptr<Sony_PlayStation_GTE> GTE;

	const bool IsRoomOpen(void);
	void OpenRDT(const std::filesystem::path Filename = L"", std::uintmax_t iCut = 0);
	void SaveRDT(void);
	void SetLighting(void);
	void ResetLighting(void) const;

	Global_Application(void) :
		Context(nullptr, &ImGui::DestroyContext),
		Window{ std::make_unique<Standard_Window>() },
		Render{ std::make_shared<Standard_DirectX_9>() },
		GTE{ std::make_shared<Sony_PlayStation_GTE>() },
		Gamepad{ std::make_unique<Resident_Evil_Gamepad>() },
		Player{ std::make_shared<Resident_Evil_Model>() },
		SubPlayer{ std::make_shared<Resident_Evil_Model>() },
		Room{ std::make_shared<Resident_Evil_Room>() },
		Sce{ std::make_unique<Scenario>() },
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
		b_ViewScenarioSetup(false),
		b_ShowFPS(false),
		b_Shutdown(false),
		b_PerVertexLighting(false),
		b_PerPixelLighting(false),
		m_RoomcutProgress(0.0f),
		b_Active(true)
	{
		b_ModalOp.store(false);
		b_FileOp.store(false);
		b_ScriptOp.store(true);
		b_DrawAot.store(true);
		b_Pause.store(false);
		MainOp.InitPool(1);
		FileOp.InitPool(1);
		ScriptOp.InitPool(1);
		Camera = std::make_unique<Resident_Evil_Camera>(Render, GTE);
		Geometry = std::make_unique<Resident_Evil_Geometry>(Render, GTE);
		Message = std::make_unique<Resident_Evil_Message>(Render);
		Modal = [&]() {};
	}
	~Global_Application(void) = default;

	int Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
};