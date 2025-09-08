/*
*
*	Megan Grass
*	August 14, 2025
*
*/

#include "app.h"

#include <bio2.h>

void Global_Application::RoomcutExtract(std::filesystem::path Filename)
{
	auto This = std::shared_ptr<Global_Application>(this, [](Global_Application*) {});

	auto ThreadWork = std::thread([This, Filename]()
		{
			This->m_RoomcutProgress = 0.0f;
			This->b_RoomcutExtraction.store(true);

			std::function<void(float, bool&, std::filesystem::path)> Callback =
				[&](float Progress, bool& b_Status, std::filesystem::path CurrentID) -> void
				{
					This->m_RoomcutProgress = Progress;
					b_Status = This->b_RoomcutExtraction.load();
					This->m_RoomcutNameID = CurrentID;
				};

			std::function<void(std::filesystem::path&)> OnSearchComplete =
				[&](std::filesystem::path Directory) -> void
				{
					This->OnRoomcutComplete(Directory);
				};

			Resident_Evil_2().DisassembleRoomCut(Filename, Callback, OnSearchComplete);
		});

	Modal = [This]() -> void { This->RoomcutModal(); };

	ThreadWork.detach();
}

void Global_Application::RoomcutModal(void)
{
	if (!ImGui::IsPopupOpen("roomcut.bin##RoomcutModal"))
	{
		ImGui::OpenPopup("roomcut.bin##RoomcutModal");
	}

	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) / 2, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) / 1.5f));

	if (ImGui::BeginPopupModal("roomcut.bin##RoomcutModal", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		if (m_RoomcutProgress >= 1.0f)
		{
			b_RoomcutExtraction.store(false);
			Modal = []() {};
			ImGui::CloseCurrentPopup();
		}

		{
			float TextWidth = ImGui::CalcTextSize(m_RoomcutNameID.string().c_str()).x;
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - TextWidth) / 2.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);

			ImGui::Text(Standard_String().FormatCStyle("%s", m_RoomcutNameID.string().c_str()).c_str());
		}

		ImGui::ProgressBar(m_RoomcutProgress, ImVec2(0.0f, 0.0f));

		float WindowWidth = ImGui::GetWindowSize().x;
		float ButtonWidth = ImGui::CalcTextSize("Cancel##RoomcutModal").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float ButtonHeight = ImGui::GetFrameHeight() * 1.25f;
		ImGui::SetCursorPosX((WindowWidth - ButtonWidth) / 2.0f);

		if (ImGui::Button("Cancel##RoomcutModal", ImVec2(ButtonWidth, ButtonHeight)))
		{
			b_RoomcutExtraction.store(false);
			Modal = []() {};
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

		ImGui::EndPopup();
	}
}

void Global_Application::OnRoomcutComplete(std::filesystem::path Directory)
{
	b_RoomcutExtraction = false;

	Modal = []() {};

	ShellExecuteW(nullptr, L"open", (LPCWSTR)Directory.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}