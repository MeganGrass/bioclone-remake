/*
*
*	Megan Grass
*	March 2, 2025
*
*/

#include "app.h"

const bool Global_Application::IsRoomOpen(void)
{
	if (b_FileOp.load() || Camera->b_ViewEditor) { return false; }

	return Room->IsOpen();
}

void Global_Application::CloseRDT(const bool b_AskSave)
{
	if (!IsRoomOpen()) { return; }

	if (b_AskSave && Room->IsOpen() && Window->Question(L"Do you want to save the current RDT file?"))
	{
		SaveRDT();
	}

	Camera->Reset();

	Room->Close();

	Player->CloseRoom();

	ResetLighting();
}

void Global_Application::OpenRDT(const std::filesystem::path Filename, std::uintmax_t iCut)
{
	b_FileOp.store(true);

	auto InitializeRoom = [&](const bool b_AskSave)
		{
			CloseRDT(b_AskSave);

			Camera->m_Cx = GTE->ToFloat(Room->Sca->GetHeader()->Cx);
			Camera->m_Cz = GTE->ToFloat(Room->Sca->GetHeader()->Cz);

			Camera->SetMeta(Room->m_Path, Room->m_Stage, Room->m_Room, Room->GetCameraCount(), Room->m_Game);
			Camera->SetImage(iCut);

			if (!Camera->b_ViewTopDown && !Camera->b_ViewEditor)
			{
				Camera->Set(Room->Rid->Get(iCut)->ViewR >> 7, Room->Rid->Get(iCut)->View_p, Room->Rid->Get(iCut)->View_r);
			}

			if (Player->b_ControllerMode)
			{
				m_PlayerState->Set(Idle);
			}
			else
			{
				Player->ResetClip();
			}

			Player->SetRoomAnimations(Room->Rbj);

			SetLighting();
		};

	if (!Filename.empty() && Standard_FileSystem().Exists(Filename) && Room->Open(Filename))
	{
		InitializeRoom(false);
		b_FileOp.store(false);
		return;
	}

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ Room->GameStrW().c_str() }, { L"*.rdt" }); Filename.has_value())
	{
		if (Room->Open(Filename.value()))
		{
			iCut = 0;
			InitializeRoom(true);
		}
	}

	CoUninitialize();

	b_FileOp.store(false);
}

void Global_Application::SaveRDT(void)
{
}

void Global_Application::OpenModel(std::shared_ptr<Resident_Evil_Model>& Model, const bool b_LinkRoom, const std::filesystem::path Filename)
{
	b_FileOp.store(true);

	auto InitializeModel = [&](const std::filesystem::path Filename)
		{
			bool b_ControllerMode = Model->b_ControllerMode;

			Model->Open(Filename);

			if (b_LinkRoom)
			{
				Model->SetRoomAnimations(Room->Rbj);
			}

			if (b_ControllerMode && Model->b_WeaponChange)
			{
				Model->b_ControllerMode = true;
				Model->b_WeaponChange = false;
				SetController(Model->b_ControllerMode);
			}
		};

	if (!Filename.empty() && Standard_FileSystem().Exists(Filename))
	{
		InitializeModel(Filename);
		b_FileOp.store(false);
		return;
	}

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename(
		{ L"All Types", L"3D Model", L"Player Model", L"Enemy Model", L"Weapon Model" },
		{ L"*.tmd;*.md1;*.md2;*.pld;*.emd;*.emw;*.plw", L"*.tmd;*.md1;*.md2", L"*.emd;*.pld", L"*.emd", L"*.emw;*.plw" }
	); Filename.has_value())
	{
		InitializeModel(Filename.value());
	}

	CoUninitialize();

	b_FileOp.store(false);
}

void Global_Application::OpenModelTexture(std::shared_ptr<Resident_Evil_Model>& Model, const std::filesystem::path Filename)
{
	b_FileOp.store(true);

	if (!Filename.empty() && Standard_FileSystem().Exists(Filename) && Model->OpenTexture(Filename))
	{
		b_FileOp.store(false);
		return;
	}

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename(
		{ L"All Types", L"Sony Texture Image", L"Bitmap Graphic", L"Portable Network Graphics", L"Joint Photographic Experts Group" },
		{ L"*.tim;*.bmp;*.png;*.jpg;*.jpeg", L"*.tim", L"*.bmp", L"*.png", L"*.jpg;*.jpeg" }
	); Filename.has_value())
	{
		Model->OpenTexture(Filename.value());
	}

	CoUninitialize();

	b_FileOp.store(false);
}

void Global_Application::SaveModelTexture(std::shared_ptr<Resident_Evil_Model>& Model)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename(
		{ L"All Types", L"Sony Texture Image", L"Bitmap Graphic", L"Portable Network Graphics", L"Joint Photographic Experts Group" },
		{ L"*.tim;*.bmp;*.png;*.jpg;*.jpeg", L"*.tim", L"*.bmp", L"*.png", L"*.jpg;*.jpeg" }
	); Filename.has_value())
	{
		String Extension = Filename.value().extension().string();

		std::filesystem::path Dir = Filename.value().parent_path();
		std::filesystem::path Stem = Filename.value().stem();

		if (Standard_String().ToUpper(Extension) == ".TIM")
		{
			Model->Texture()->SaveTIM(Filename.value());
		}
		if (Standard_String().ToUpper(Extension) == ".BMP")
		{
			if (Model->Texture()->GetPaletteCount())
			{
				for (uint16_t i = 0; i < Model->Texture()->GetPaletteCount(); i++)
				{
					std::filesystem::path PaletteFilename = Dir / Stem += L"_" + std::to_wstring(i) + L".bmp";
					Model->Texture()->SaveBMP(PaletteFilename, 0, i);
				}
			}
			else { Model->Texture()->SaveBMP(Filename.value()); }
		}
#ifdef LIB_PNG
		if (Standard_String().ToUpper(Extension) == ".PNG")
		{
			if (Model->Texture()->GetPaletteCount())
			{
				for (uint16_t i = 0; i < Model->Texture()->GetPaletteCount(); i++)
				{
					std::filesystem::path PaletteFilename = Dir / Stem += L"_" + std::to_wstring(i) + L".png";
					Model->Texture()->SavePNG(PaletteFilename, 0, i);
				}
			}
			else { Model->Texture()->SavePNG(Filename.value()); }
		}
#endif
#ifdef LIB_JPEG
		if ((Standard_String().ToUpper(Extension) == ".JPG" || Standard_String().ToUpper(Extension) == ".JPEG"))
		{
			if (Model->Texture()->GetPaletteCount())
			{
				for (uint16_t i = 0; i < Model->Texture()->GetPaletteCount(); i++)
				{
					std::filesystem::path PaletteFilename = Dir / Stem += L"_" + std::to_wstring(i) + L".jpg";
					Model->Texture()->SaveJPEG(PaletteFilename, 0, i);
				}
			}
			else { Model->Texture()->SaveJPEG(Filename.value()); }
		}
#endif
	}

	CoUninitialize();
}

void Global_Application::Screenshot(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename(
		{ L"All Types", L"Bitmap Graphic", L"Portable Network Graphics", L"Joint Photographic Experts Group" },
		{ L"*.bmp;*.png;*.jpg;*.jpeg", L"*.bmp", L"*.png", L"*.jpg;*.jpeg" }
	); Filename.has_value())
	{
		D3DXIMAGE_FILEFORMAT FileFormat = D3DXIFF_BMP;

		String Extension = Filename.value().extension().string();

		if (Standard_String().ToUpper(Extension) == ".PNG") { FileFormat = D3DXIFF_PNG; }
		if (Standard_String().ToUpper(Extension) == ".JPG" || Standard_String().ToUpper(Extension) == ".JPEG") { FileFormat = D3DXIFF_JPG; }

		Render->SaveTexture(m_RenderTexture.get(), FileFormat, true, Filename.value());
	}

	CoUninitialize();
}