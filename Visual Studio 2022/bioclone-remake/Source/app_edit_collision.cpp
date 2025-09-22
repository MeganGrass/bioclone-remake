/*
*
*	Megan Grass
*	August 9, 2025
*
*/

#include "app.h"

void Global_Application::CollisionEditor(void)
{
	if (ImGui::MenuItem(" Shape##CollisionEditor", NULL, &Geometry->b_ShapeCollision)) { Geometry->b_ShapeCollisionAll = false; }
	TooltipOnHover("Draw selected as shape");

	if (ImGui::MenuItem(" Shape (all)##CollisionEditor", NULL, &Geometry->b_ShapeCollisionAll)) { Geometry->b_ShapeCollision = false; }
	TooltipOnHover("Draw all as shapes");

	if (ImGui::MenuItem(" Solid##CollisionEditor", NULL, &Geometry->b_SolidCollision)) { Geometry->b_SolidCollisionAll = false; }
	TooltipOnHover("Draw selected as solid color\r\nWireframe otherwise");

	if (ImGui::MenuItem(" Solid (all)##CollisionEditor", NULL, &Geometry->b_SolidCollisionAll)) { Geometry->b_SolidCollision = false; }
	TooltipOnHover("Draw all as solid color\r\nWireframe otherwise");

	if (ImGui::BeginTable("CollisionID##CollisionEditor", 2))
	{
		ImGui::TableNextColumn();
		ImGui::MenuItem(" Highlight##CollisionEditor", NULL, &Geometry->b_HighlightCollision);
		TooltipOnHover("Highlight selected in red\r\n");

		Geometry->iObjectMax = Room->Sca->Count() ? Room->Sca->Count() - 1 : 0;

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
		if (ImGui::SliderScalar("##CollisionEditorID", ImGuiDataType_U64, &Geometry->iObject, &Geometry->iObjectMin, &Geometry->iObjectMax))
		{
			Geometry->iObject = std::clamp(Geometry->iObject, static_cast<std::size_t>(0), Geometry->iObjectMax);
		}
		TooltipOnHover("Selected ID\r\n");
		ScrollOnHover(&Geometry->iObject, ImGuiDataType_U64, 1, Geometry->iObjectMin, Geometry->iObjectMax);

		ImGui::EndTable();
	}
}