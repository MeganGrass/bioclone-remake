/*
*
*	Megan Grass
*	March 2, 2025
*
*/

#include "app.h"

#include <std_text.h>


void Global_Application::InitConfig(void)
{
	if (Standard_FileSystem().Exists(GetConfigFilename()))
	{
		OpenConfig();
	}
	else
	{
		SaveConfig();
	}
}

void Global_Application::OpenConfig(void)
{
	if (b_Shutdown) { return; }

	std::unique_ptr<StdText> Text = std::make_unique<StdText>(GetConfigFilename(), FileAccessMode::Read_Ex, TextFileBOM::UTF16_LE);
	if (!Text->IsOpen())
	{
		Standard_String(Window->Get()).Message("Error, could not open \"%ws\" app config", GetConfigFilename().filename().wstring().c_str());
		return;
	}

	for (std::size_t i = 0; i < Text->GetLineCount(); i++)
	{
		StrVecW Args = Text->GetArgsW(i);

		if (!Args.empty() && (Args.size() > 1))
		{
			if (Standard_String().ToUpper(Args[0]) == L"M_BORDERCOLOR")
			{
				DWORD Color = std::stoul(Args[1], nullptr, 16);
				Window->GetBorderColor() = D3DCOLOR_ARGB(0, GetRValue(Color), GetGValue(Color), GetBValue(Color));
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_CAPTIONCOLOR")
			{
				DWORD Color = std::stoul(Args[1], nullptr, 16);
				Window->GetCaptionColor() = D3DCOLOR_ARGB(0, GetRValue(Color), GetGValue(Color), GetBValue(Color));
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_WINDOWCOLOR")
			{
				DWORD Color = std::stoul(Args[1], nullptr, 16);
				Window->GetColor() = D3DCOLOR_ARGB(0, GetRValue(Color), GetGValue(Color), GetBValue(Color));
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_FONTCOLOR")
			{
				DWORD Color = std::stoul(Args[1], nullptr, 16);
				Window->GetTextColor() = D3DCOLOR_ARGB(0, GetRValue(Color), GetGValue(Color), GetBValue(Color));
			}
			if (Standard_String().ToUpper(Args[0]) == L"M_FONT")
			{
				Window->SetFont(Args[1], Window->FontSize());
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_FONTSIZE")
			{
				Window->FontSize() = std::stof(Args[1]);
				Window->FontSize() = std::clamp(Window->FontSize(), Window->FontSizeMin(), Window->FontSizeMax());
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_WINDOWWIDTH")
			{
				m_BootWidth = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_WINDOWHEIGHT")
			{
				m_BootHeight = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_RENDERWIDTH")
			{
				m_RenderWidth = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_RENDERHEIGHT")
			{
				m_RenderHeight = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MAXIMIZED")
			{
				b_BootMaximized = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_FULLSCREEN")
			{
				b_BootFullscreen = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_IGNOREAUTORESIZE")
			{
				b_IgnoreAutoResize = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_SHOWFPS")
			{
				b_ShowFPS = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWWINDOWOPTIONS")
			{
				b_ViewWindowOptions = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_PERVERTEXLIGHTING")
			{
				b_PerVertexLighting = b_PerPixelLighting ? false : std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_PERPIXELLIGHTING")
			{
				b_PerPixelLighting = b_PerVertexLighting ? false : std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWGRID")
			{
				Render->b_ViewGrid = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWAXIS")
			{
				Render->b_ViewAxis = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_HORZFLIP")
			{
				Camera->b_HorzFlip = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VERTFLIP")
			{
				Camera->b_VertFlip = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWBACKGROUND")
			{
				Camera->b_ViewBackground = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_CAMERALINE")
			{
				Camera->b_DrawLine = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWSPRITE")
			{
				Camera->b_ViewSprite = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWSWITCH")
			{
				Camera->b_DrawSwitch = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWCOLLISION")
			{
				Geometry->b_DrawCollision = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWBLOCK")
			{
				Geometry->b_DrawBlock = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_VIEWFLOOR")
			{
				Geometry->b_DrawFloor = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_SOLIDCOLLISION")
			{
				Geometry->b_SolidCollision = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_SOLIDCOLLISIONALL")
			{
				Geometry->b_SolidCollisionAll = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_SHAPECOLLISION")
			{
				Geometry->b_ShapeCollision = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_SHAPECOLLISIONALL")
			{
				Geometry->b_ShapeCollisionAll = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_HIGHLIGHTCOLLISION")
			{
				Geometry->b_HighlightCollision = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELPLAY")
			{
				Player->b_Play = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELLOOP")
			{
				Player->b_Loop = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELLOCKPOSITION")
			{
				Player->b_LockPosition = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELDRAWWIREFRAME")
			{
				Player->b_DrawWireframe = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELDRAWSOLIDCOLOR")
			{
				Player->b_DrawSolidColor = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELDRAWTEXTURED")
			{
				Player->b_DrawTextured = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELDRAWSKELETON")
			{
				Player->b_DrawSkeleton = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELDRAWREFERENCE")
			{
				Player->b_DrawReference = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELDRAWHITBOX")
			{
				Player->b_DrawHitbox = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELDRAWSHADOW")
			{
				Player->b_DrawShadow = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELTEXTUREDITHER")
			{
				Player->b_Dither = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_MODELTEXTUREFILTER")
			{
				Player->m_TextureFilter = static_cast<D3DTEXTUREFILTERTYPE>(std::stoi(Args[1]));
				Player->m_TextureFilter = std::clamp(Player->m_TextureFilter, D3DTEXF_NONE, D3DTEXF_ANISOTROPIC);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_MODELGAMETYPE")
			{
				uint32_t Type = std::stoul(Args[1], nullptr, 16);
				switch (Type)
				{
				case std::to_underlying(Video_Game::Resident_Evil_Aug_4_1995): Player->SetGame(Video_Game::Resident_Evil_Aug_4_1995); break;
				case std::to_underlying(Video_Game::Resident_Evil_Oct_4_1995): Player->SetGame(Video_Game::Resident_Evil_Oct_4_1995); break;
				case std::to_underlying(Video_Game::Resident_Evil): Player->SetGame(Video_Game::Resident_Evil); break;
				case std::to_underlying(Video_Game::Resident_Evil_2_Nov_6_1996): Player->SetGame(Video_Game::Resident_Evil_2_Nov_6_1996); break;
				case std::to_underlying(Video_Game::Resident_Evil_3): Player->SetGame(Video_Game::Resident_Evil_3); break;
				default: Player->SetGame(Video_Game::Resident_Evil_2); break;
				}
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_ROOMGAMETYPE")
			{
				uint32_t Type = std::stoul(Args[1], nullptr, 16);
				switch (Type)
				{
				case std::to_underlying(Video_Game::Resident_Evil_Aug_4_1995): Room->SetGame(Video_Game::Resident_Evil_Aug_4_1995); break;
				case std::to_underlying(Video_Game::Resident_Evil_Oct_4_1995): Room->SetGame(Video_Game::Resident_Evil_Oct_4_1995); break;
				case std::to_underlying(Video_Game::Resident_Evil): Room->SetGame(Video_Game::Resident_Evil); break;
				case std::to_underlying(Video_Game::Resident_Evil_2_Nov_6_1996): Room->SetGame(Video_Game::Resident_Evil_2_Nov_6_1996); break;
				case std::to_underlying(Video_Game::Resident_Evil_3): Room->SetGame(Video_Game::Resident_Evil_3); break;
				default: Room->SetGame(Video_Game::Resident_Evil_2); break;
				}
			}

			if (Standard_String().ToUpper(Args[0]) == L"B_MODELLERP")
			{
				Player->b_LerpKeyframes = std::stoi(Args[1]);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_MODELLERPVALUE")
			{
				Player->m_LerpValue = std::stof(Args[1]);
				Player->m_LerpValue = std::round(Player->m_LerpValue * 20.0f) / 20.0f;
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_MODELFILENAME")
			{
				m_BootPlayerFilename = Args[1];
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_MODELWEAPONFILENAME")
			{
				m_BootWeaponFilename = Args[1];
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_DATAPATH")
			{
				m_DataPath = Args[1];
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_PLAYERPATH")
			{
				m_PlayerPath = Args[1];
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_ENEMYPATH")
			{
				m_EnemyPath = Args[1];
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_DOORPATH")
			{
				m_DoorPath = Args[1];
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_AIM")
			{
				std::int32_t Value = std::stoi(Args[1]);
				Gamepad->Map().R1.Button = Value == 0xFFFF ? 0xFFFF : (1 << Value);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_CANCEL")
			{
				std::int32_t Value = std::stoi(Args[1]);
				Gamepad->Map().Circle.Button = Value == 0xFFFF ? 0xFFFF : (1 << Value);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_ACTION")
			{
				std::int32_t Value = std::stoi(Args[1]);
				Gamepad->Map().Cross.Button = Value == 0xFFFF ? 0xFFFF : (1 << Value);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_RUN")
			{
				std::int32_t Value = std::stoi(Args[1]);
				Gamepad->Map().Square.Button = Value == 0xFFFF ? 0xFFFF : (1 << Value);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_FORWARD")
			{
				std::int32_t Value = std::stoi(Args[1]);
				Gamepad->Map().Up.Button = Value == 0xFFFF ? 0xFFFF : (1 << Value);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_BACKWARD")
			{
				std::int32_t Value = std::stoi(Args[1]);
				Gamepad->Map().Down.Button = Value == 0xFFFF ? 0xFFFF : (1 << Value);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_LEFT")
			{
				std::int32_t Value = std::stoi(Args[1]);
				Gamepad->Map().Left.Button = Value == 0xFFFF ? 0xFFFF : (1 << Value);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_RIGHT")
			{
				std::int32_t Value = std::stoi(Args[1]);
				Gamepad->Map().Right.Button = Value == 0xFFFF ? 0xFFFF : (1 << Value);
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_AIMSHOULDER")
			{
				Gamepad->Map().R1.IsLeftTrigger = std::stoi(Args[1]) == 1;
				Gamepad->Map().R1.IsRightTrigger = std::stoi(Args[1]) == 2;
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_CANCELSHOULDER")
			{
				Gamepad->Map().Circle.IsLeftTrigger = std::stoi(Args[1]) == 1;
				Gamepad->Map().Circle.IsRightTrigger = std::stoi(Args[1]) == 2;
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_ACTIONSHOULDER")
			{
				Gamepad->Map().Cross.IsLeftTrigger = std::stoi(Args[1]) == 1;
				Gamepad->Map().Cross.IsRightTrigger = std::stoi(Args[1]) == 2;
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_RUNSHOULDER")
			{
				Gamepad->Map().Square.IsLeftTrigger = std::stoi(Args[1]) == 1;
				Gamepad->Map().Square.IsRightTrigger = std::stoi(Args[1]) == 2;
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_FORWARDSHOULDER")
			{
				Gamepad->Map().Up.IsLeftTrigger = std::stoi(Args[1]) == 1;
				Gamepad->Map().Up.IsRightTrigger = std::stoi(Args[1]) == 2;
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_BACKWARDSHOULDER")
			{
				Gamepad->Map().Down.IsLeftTrigger = std::stoi(Args[1]) == 1;
				Gamepad->Map().Down.IsRightTrigger = std::stoi(Args[1]) == 2;
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_LEFTSHOULDER")
			{
				Gamepad->Map().Left.IsLeftTrigger = std::stoi(Args[1]) == 1;
				Gamepad->Map().Left.IsRightTrigger = std::stoi(Args[1]) == 2;
			}

			if (Standard_String().ToUpper(Args[0]) == L"M_RIGHTSHOULDER")
			{
				Gamepad->Map().Right.IsLeftTrigger = std::stoi(Args[1]) == 1;
				Gamepad->Map().Right.IsRightTrigger = std::stoi(Args[1]) == 2;
			}
		}
	}
}

void Global_Application::SaveConfig(void)
{
	if (b_Shutdown) { return; }

	std::unique_ptr<StdText> Text = std::make_unique<StdText>(GetConfigFilename(), FileAccessMode::Write_Ex, TextFileBOM::UTF16_LE);
	if (!Text->IsOpen())
	{
		Standard_String(Window->Get()).Message("Error, could not create \"%ws\" app config", GetConfigFilename().filename().wstring().c_str());
		return;
	}

	Text->AddLine(L"app\t\"%ws\"\r", Window->GetModuleStr().c_str());

	Text->AddLine(L"m_BorderColor\t0x%08x\r", D3DCOLOR_ARGB(0, GetRValue(Window->GetBorderColor()), GetGValue(Window->GetBorderColor()), GetBValue(Window->GetBorderColor())));
	Text->AddLine(L"m_CaptionColor\t0x%08x\r", D3DCOLOR_ARGB(0, GetRValue(Window->GetCaptionColor()), GetGValue(Window->GetCaptionColor()), GetBValue(Window->GetCaptionColor())));
	Text->AddLine(L"m_WindowColor\t0x%08x\r", D3DCOLOR_ARGB(0, GetRValue(Window->GetColor()), GetGValue(Window->GetColor()), GetBValue(Window->GetColor())));
	Text->AddLine(L"m_FontColor\t0x%08x\r", D3DCOLOR_ARGB(0, GetRValue(Window->GetTextColor()), GetGValue(Window->GetTextColor()), GetBValue(Window->GetTextColor())));
	Text->AddLine(L"m_Font\t\t%ws\r", Window->GetFont().stem().wstring().c_str());
	Text->AddLine(L"m_FontSize\t%.0f\r", Window->FontSize());
	Text->AddLine(L"m_WindowWidth\t%d\r", (Window->GetRect().right - Window->GetRect().left));
	Text->AddLine(L"m_WindowHeight\t%d\r", (Window->GetRect().bottom - Window->GetRect().top));
	Text->AddLine(L"m_RenderWidth\t%d\r", m_RenderWidth);
	Text->AddLine(L"m_RenderHeight\t%d\r", m_RenderHeight);
	Text->AddLine(L"b_Maximized\t%d\r", Window->IsMaximized());
	Text->AddLine(L"b_Fullscreen\t%d\r", Window->IsFullscreen());
	Text->AddLine(L"b_IgnoreAutoResize\t%d\r", b_IgnoreAutoResize);
	Text->AddLine(L"b_ShowFPS\t%d\r", b_ShowFPS);

	Text->AddLine(L"m_Aim\t%d\r", Gamepad->GetButtonMapBit("Aim"));
	Text->AddLine(L"m_Cancel\t%d\r", Gamepad->GetButtonMapBit("Cancel"));
	Text->AddLine(L"m_Action\t%d\r", Gamepad->GetButtonMapBit("Action"));
	Text->AddLine(L"m_Run\t%d\r", Gamepad->GetButtonMapBit("Run"));
	Text->AddLine(L"m_Forward\t%d\r", Gamepad->GetButtonMapBit("Forward"));
	Text->AddLine(L"m_Backward\t%d\r", Gamepad->GetButtonMapBit("Backward"));
	Text->AddLine(L"m_Left\t%d\r", Gamepad->GetButtonMapBit("Left"));
	Text->AddLine(L"m_Right\t%d\r", Gamepad->GetButtonMapBit("Right"));
	Text->AddLine(L"m_AimShoulder\t%d\r", Gamepad->Map().R1.IsLeftTrigger ? 1 : Gamepad->Map().R1.IsRightTrigger ? 2 : 0);
	Text->AddLine(L"m_CancelShoulder\t%d\r", Gamepad->Map().Circle.IsLeftTrigger ? 1 : Gamepad->Map().Circle.IsRightTrigger ? 2 : 0);
	Text->AddLine(L"m_ActionShoulder\t%d\r", Gamepad->Map().Cross.IsLeftTrigger ? 1 : Gamepad->Map().Cross.IsRightTrigger ? 2 : 0);
	Text->AddLine(L"m_RunShoulder\t%d\r", Gamepad->Map().Square.IsLeftTrigger ? 1 : Gamepad->Map().Square.IsRightTrigger ? 2 : 0);
	Text->AddLine(L"m_ForwardShoulder\t%d\r", Gamepad->Map().Up.IsLeftTrigger ? 1 : Gamepad->Map().Up.IsRightTrigger ? 2 : 0);
	Text->AddLine(L"m_BackwardShoulder\t%d\r", Gamepad->Map().Down.IsLeftTrigger ? 1 : Gamepad->Map().Down.IsRightTrigger ? 2 : 0);
	Text->AddLine(L"m_LeftShoulder\t%d\r", Gamepad->Map().Left.IsLeftTrigger ? 1 : Gamepad->Map().Left.IsRightTrigger ? 2 : 0);
	Text->AddLine(L"m_RightShoulder\t%d\r", Gamepad->Map().Right.IsLeftTrigger ? 1 : Gamepad->Map().Right.IsRightTrigger ? 2 : 0);

	Text->AddLine(L"b_ViewWindowOptions\t%d\r", b_ViewWindowOptions);

	Text->AddLine(L"b_PerVertexLighting\t%d\r", b_PerVertexLighting);
	Text->AddLine(L"b_PerPixelLighting\t%d\r", b_PerPixelLighting);

	Text->AddLine(L"b_ViewGrid\t%d\r", Render->b_ViewGrid);
	Text->AddLine(L"b_ViewAxis\t%d\r", Render->b_ViewAxis);

	Text->AddLine(L"b_HorzFlip\t%d\r", Camera->b_HorzFlip);
	Text->AddLine(L"b_VertFlip\t%d\r", Camera->b_VertFlip);
	Text->AddLine(L"b_ViewBackground\t%d\r", Camera->b_ViewBackground);
	Text->AddLine(L"b_CameraLine\t%d\r", Camera->b_DrawLine);
	Text->AddLine(L"b_ViewSprite\t%d\r", Camera->b_ViewSprite);
	Text->AddLine(L"b_ViewSwitch\t%d\r", Camera->b_DrawSwitch);
	Text->AddLine(L"b_ViewCollision\t%d\r", Geometry->b_DrawCollision);
	Text->AddLine(L"b_ViewBlock\t%d\r", Geometry->b_DrawBlock);
	Text->AddLine(L"b_ViewFloor\t%d\r", Geometry->b_DrawFloor);

	Text->AddLine(L"b_SolidCollision\t%d\r", Geometry->b_SolidCollision);
	Text->AddLine(L"b_SolidCollisionAll\t%d\r", Geometry->b_SolidCollisionAll);
	Text->AddLine(L"b_ShapeCollision\t%d\r", Geometry->b_ShapeCollision);
	Text->AddLine(L"b_ShapeCollisionAll\t%d\r", Geometry->b_ShapeCollisionAll);
	Text->AddLine(L"b_HighlightCollision\t%d\r", Geometry->b_HighlightCollision);
	Text->AddLine(L"b_CollisionDetection\t%d\r", Geometry->b_CollisionDetection);

	Text->AddLine(L"m_ModelGameType\t0x%x\r", Player->GameType());
	Text->AddLine(L"b_ModelTextureDither\t%d\r", Player->b_Dither);
	Text->AddLine(L"m_ModelTextureFilter\t%d\r", Player->m_TextureFilter);
	Text->AddLine(L"b_ModelDrawWireframe\t%d\r", Player->b_DrawWireframe);
	Text->AddLine(L"b_ModelDrawSolidColor\t%d\r", Player->b_DrawSolidColor);
	Text->AddLine(L"b_ModelDrawTextured\t%d\r", Player->b_DrawTextured);
	Text->AddLine(L"b_ModelDrawSkeleton\t%d\r", Player->b_DrawSkeleton);
	Text->AddLine(L"b_ModelDrawHitbox\t%d\r", Player->b_DrawHitbox);
	Text->AddLine(L"b_ModelDrawShadow\t%d\r", Player->b_DrawShadow);
	Text->AddLine(L"b_ModelDrawReference\t%d\r", Player->b_DrawReference);
	Text->AddLine(L"b_ModelLockPosition\t%d\r", Player->b_LockPosition);
	Text->AddLine(L"b_ModelPlay\t%d\r", Player->b_Play.load());
	Text->AddLine(L"b_ModelLoop\t%d\r", Player->b_Loop.load());
	Text->AddLine(L"b_ModelLerp\t%d\r", Player->b_LerpKeyframes.load());
	Text->AddLine(L"m_ModelLerpValue\t%f\r", Player->m_LerpValue);
	Text->AddLine(L"m_ModelFilename\t\"%ws\"\r", Player->Filename().wstring().c_str());
	Text->AddLine(L"m_ModelWeaponFilename\t\"%ws\"\r", Player->WeaponFilename().wstring().c_str());

	Text->AddLine(L"m_DataPath\t\"%ws\"\r", m_DataPath.wstring().c_str());
	Text->AddLine(L"m_PlayerPath\t\"%ws\"\r", m_PlayerPath.wstring().c_str());
	Text->AddLine(L"m_EnemyPath\t\"%ws\"\r", m_EnemyPath.wstring().c_str());
	Text->AddLine(L"m_DoorPath\t\"%ws\"\r", m_DoorPath.wstring().c_str());

	Text->AddLine(L"m_RoomGameType\t0x%x\r", Room->GameType());

	Text->FlushUTF16();
}