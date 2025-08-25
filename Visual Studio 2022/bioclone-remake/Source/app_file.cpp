/*
*
*	Megan Grass
*	March 2, 2025
*
*/

#include "app.h"

bool Global_Application::IsRoomOpen(void)
{
	if (b_RoomFileOp) { return false; }
	switch (Game)
	{
	case Video_Game::Resident_Evil_2_Nov_6_1996:
	case Video_Game::Resident_Evil_2_Trial:
	case Video_Game::Resident_Evil_2: return Bio2->Rdt->IsOpen();
	}
	return false;
}

void Global_Application::CloseRDT(void)
{
	if (!IsRoomOpen()) { return; }

	switch (Game)
	{
	case Video_Game::Resident_Evil_2_Nov_6_1996:
	case Video_Game::Resident_Evil_2_Trial:
	case Video_Game::Resident_Evil_2: if (Bio2->Rdt->IsOpen() && Window->Question(L"Do you want to save the current RDT file?")) { SaveRDT(); } break;
	}

	Camera->Reset();

	switch (Game)
	{
	case Video_Game::Resident_Evil_2_Nov_6_1996:
	case Video_Game::Resident_Evil_2_Trial:
	case Video_Game::Resident_Evil_2: Bio2->Rdt->Close(); break;
	}

	Stage = 0;
	Room = 0;
	Disk = 0;

	Geometry->iObjectMax = 0;

	Player->CloseRoom();

	ResetLighting();
}

void Global_Application::OpenRDT(void)
{
	CloseRDT();

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ GameStrW().c_str()}, {L"*.rdt"}); Filename.has_value())
	{
		switch (Game)
		{
		case Video_Game::Resident_Evil_2_Nov_6_1996:
		case Video_Game::Resident_Evil_2_Trial:
		case Video_Game::Resident_Evil_2:
			if (Bio2->Rdt->Open(Filename.value()))
			{
				Stage = Bio2->Rdt->Stage;
				Room = Bio2->Rdt->Room;
				Disk = Bio2->Rdt->Disk;

				Camera->b_ViewTopDown = false;
				Camera->b_ViewModelEdit = false;

				Camera->m_Cx = GTE->ToFloat(Bio2->Rdt->Sca->GetHeader()->Cx);
				Camera->m_Cz = GTE->ToFloat(Bio2->Rdt->Sca->GetHeader()->Cz);

				Camera->SetMeta(Bio2->Rdt->m_Path, Bio2->Rdt->Stage, Bio2->Rdt->Room, Bio2->Rdt->GetCameraCount());
				Camera->SetImage(0);
				Camera->Set(Bio2->Rdt->Rid->Get(0)->ViewR >> 7, Bio2->Rdt->Rid->Get(0)->View_p, Bio2->Rdt->Rid->Get(0)->View_r);

				Geometry->iObjectMax = Bio2->Rdt->Sca->Count() ? Bio2->Rdt->Sca->Count() - 1 : 0;

				SetLighting();
			}
			break;
		}
	}

	CoUninitialize();

	SetRoomAnimations();
}

void Global_Application::SaveRDT(void)
{
}

void Global_Application::SetRoomAnimations(void)
{
	switch (Game)
	{
	case Video_Game::Resident_Evil_2_Nov_6_1996:
	case Video_Game::Resident_Evil_2_Trial:
	case Video_Game::Resident_Evil_2:
		if (Bio2->Rdt->IsOpen()) { Rbj = Bio2->Rdt->Rbj; } break;
	}

	Player->SetRoomAnimations(Rbj);
}

void Global_Application::OpenPlayerModel(std::filesystem::path Filename)
{
	if (!Filename.empty())
	{
		Player->Open(Filename);
		SetRoomAnimations();
		return;
	}

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename(
		{ L"All Types", L"3D Model", L"Player Model", L"Enemy Model", L"Weapon Model" },
		{ L"*.tmd;*.md1;*.md2;*.pld;*.emd;*.emw;*.plw", L"*.tmd;*.md1;*.md2", L"*.emd;*.pld", L"*.emd", L"*.emw;*.plw" }
	); Filename.has_value())
	{
		Player->Open(Filename.value());
		SetRoomAnimations();
	}

	CoUninitialize();
}

void Global_Application::OpenPlayerTexture(std::filesystem::path Filename)
{
	if (!Filename.empty())
	{
		Player->OpenTexture(Filename);
		return;
	}

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename(
		{ L"All Types", L"Sony Texture Image", L"Bitmap Graphic", L"Portable Network Graphics", L"Joint Photographic Experts Group" },
		{ L"*.tim;*.bmp;*.png;*.jpg;*.jpeg", L"*.tim", L"*.bmp", L"*.png", L"*.jpg;*.jpeg" }
	); Filename.has_value())
	{
		Player->OpenTexture(Filename.value());
	}

	CoUninitialize();
}

void Global_Application::SavePlayerTexture(void)
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
			Player->Texture()->SaveTIM(Filename.value());
		}
		if (Standard_String().ToUpper(Extension) == ".BMP")
		{
			if (Player->Texture()->GetPaletteCount())
			{
				for (uint16_t i = 0; i < Player->Texture()->GetPaletteCount(); i++)
				{
					std::filesystem::path PaletteFilename = Dir / Stem += L"_" + std::to_wstring(i) + L".bmp";
					Player->Texture()->SaveBMP(PaletteFilename, 0, i);
				}
			}
			else { Player->Texture()->SaveBMP(Filename.value()); }
		}
#ifdef LIB_PNG
		if (Standard_String().ToUpper(Extension) == ".PNG")
		{
			if (Player->Texture()->GetPaletteCount())
			{
				for (uint16_t i = 0; i < Player->Texture()->GetPaletteCount(); i++)
				{
					std::filesystem::path PaletteFilename = Dir / Stem += L"_" + std::to_wstring(i) + L".png";
					Player->Texture()->SavePNG(PaletteFilename, 0, i);
				}
			}
			else { Player->Texture()->SavePNG(Filename.value()); }
		}
#endif
#ifdef LIB_JPEG
		if ((Standard_String().ToUpper(Extension) == ".JPG" || Standard_String().ToUpper(Extension) == ".JPEG"))
		{
			if (Player->Texture()->GetPaletteCount())
			{
				for (uint16_t i = 0; i < Player->Texture()->GetPaletteCount(); i++)
				{
					std::filesystem::path PaletteFilename = Dir / Stem += L"_" + std::to_wstring(i) + L".jpg";
					Player->Texture()->SaveJPEG(PaletteFilename, 0, i);
				}
			}
			else { Player->Texture()->SaveJPEG(Filename.value()); }
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