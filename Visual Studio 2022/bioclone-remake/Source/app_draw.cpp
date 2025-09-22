/*
*
*	Megan Grass
*	July 10, 2025
*
*/

#include "app.h"

void Global_Application::DrawBackground(void)
{
	if (!IsRoomOpen() || Camera->b_ViewTopDown || !Camera->b_ViewBackground || !Camera->m_Background) { return; }

#if MSTD_DX9
	Render->SetDepthScale(1.0f, 1.0f);

	Render->AlphaBlending(FALSE, D3DBLEND_ZERO, D3DBLEND_ZERO);

	Render->AlphaTesting(FALSE, 0x00, D3DCMP_NEVER);

	Render->DrawVec4t(
		Camera->m_BackgroundVert.get(), Camera->m_Background.get(), Render->PassthroughPixelShader.get(),
		Camera->m_TexWidth, Camera->m_TexHeight,
		(D3DXMATRIX*)Camera->Orthogonal.get());
#endif
}

void Global_Application::DrawCamera(void)
{
	if (!IsRoomOpen() || !Camera->b_DrawLine) { return; }

	std::vector<vec3> Shape;

	std::int32_t CrossSize = 900;
	std::int32_t ArrowSize = 900;

	vec3 Delta = vec3(GTE->ToFloat(Camera->m_At.x - Camera->m_Eye.x), 0.0f, GTE->ToFloat(Camera->m_At.z - Camera->m_Eye.z));
	vec3 DeltaNormal = vec3(Delta.x, Delta.y, Delta.z).Normalize();

	vec3 Perpendicular0 = vec3(-DeltaNormal.z, 0.0f, DeltaNormal.x).Normalize() * (GTE->ToFloat(ArrowSize) * 0.5f);
	vec3 Perpendicular1 = vec3(DeltaNormal.z, 0.0f, -DeltaNormal.x).Normalize() * (GTE->ToFloat(ArrowSize) * 0.5f);

	vec3 Arrow = vec3(
		GTE->ToFloat(Camera->m_At.x) - DeltaNormal.x * GTE->ToFloat(ArrowSize),
		GTE->ToFloat(Camera->m_At.y) - DeltaNormal.y * GTE->ToFloat(ArrowSize),
		GTE->ToFloat(Camera->m_At.z) - DeltaNormal.z * GTE->ToFloat(ArrowSize));

	{
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_Eye.x - CrossSize), 0.0f, GTE->ToFloat(Camera->m_Eye.z)));
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_Eye.x + CrossSize), 0.0f, GTE->ToFloat(Camera->m_Eye.z)));
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_Eye.x), 0.0f, GTE->ToFloat(Camera->m_Eye.z)));
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_Eye.x), 0.0f, GTE->ToFloat(Camera->m_Eye.z)));
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_Eye.x), 0.0f, GTE->ToFloat(Camera->m_Eye.z - CrossSize)));
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_Eye.x), 0.0f, GTE->ToFloat(Camera->m_Eye.z + CrossSize)));
	}

	{
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_Eye.x), 0.0f, GTE->ToFloat(Camera->m_Eye.z)));
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_At.x), 0.0f, GTE->ToFloat(Camera->m_At.z)));
	}

	{
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_At.x), 0.0f, GTE->ToFloat(Camera->m_At.z)));
		Shape.push_back(vec3(Arrow.x + Perpendicular0.x, 0.0f, Arrow.z + Perpendicular0.z));
		Shape.push_back(vec3(GTE->ToFloat(Camera->m_At.x), 0.0f, GTE->ToFloat(Camera->m_At.z)));
		Shape.push_back(vec3(Arrow.x + Perpendicular1.x, 0.0f, Arrow.z + Perpendicular1.z));
	}

#if MSTD_DX9
	std::unique_ptr<IDirect3DVertexBuffer9, IDirect3DDelete9<IDirect3DVertexBuffer9>> Vertices;
	Vertices.reset(Render->CreateVec3c(Shape, 0x00FF0000));

	Render->SetWorld(Standard_Matrix());

	Render->DrawVec3c(Vertices.get(), nullptr, nullptr, D3DFILL_WIREFRAME, D3DPT_LINELIST);

	Render->ResetWorld();
#endif
}

void Global_Application::DrawSprite(void)
{
	if (!IsRoomOpen() || Camera->b_ViewTopDown || !Camera->b_ViewSprite) { return; }

	{
		Render->SetDepthScale(-1.013f, -2048.000f);

		Render->AlphaBlending(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);

		Render->AlphaTesting(TRUE, 0xFF, D3DCMP_GREATEREQUAL);
	}

	for (std::size_t i = 0; i < Room->Pri[Camera->m_Cut]->Count(); i++)
	{
		for (std::size_t x = 0; x < Room->Pri[Camera->m_Cut]->Get(i)->Sprite.size(); x++)
		{
			auto& Sprite = Room->Pri[Camera->m_Cut]->Get(i)->Sprite[x];
			auto& Group = Room->Pri[Camera->m_Cut]->Get(i)->Group;

			float X = ((float)Sprite.x + (float)Group.OfsX) * Camera->m_OrthoScaleX;
			float Y = ((float)Sprite.y + (float)Group.OfsY) * Camera->m_OrthoScaleY;
			float W = (float)Sprite.w * Camera->m_OrthoScaleX;
			float H = (float)Sprite.h * Camera->m_OrthoScaleY;

#if MSTD_DX9
			X -= 0.5f;
			Y += 0.5f;

			float OffsetU = 0.5f / Camera->m_TexSprWidth;
			float OffsetV = 0.5f / Camera->m_TexSprHeight;
#else
			float OffsetU = 0.0f;
			float OffsetV = 0.0f;
#endif

			float U0 = ((float)Sprite.u + OffsetU) / Camera->m_TexSprWidth;
			float V0 = ((float)Sprite.v + OffsetV) / Camera->m_TexSprHeight;
			float U1 = ((float)Sprite.u + (float)Sprite.w - OffsetU) / Camera->m_TexSprWidth;
			float V1 = ((float)Sprite.v + (float)Sprite.h - OffsetV) / Camera->m_TexSprHeight;

			float Z = (float)Sprite.otz;

			std::vector<vec4t> Vector = {
				{ vec4{ X,		Y + H,	Z, 1.0f }, vec2{ U0, V1 } },
				{ vec4{ X,		Y,		Z, 1.0f }, vec2{ U0, V0 } },
				{ vec4{ X + W,	Y + H,	Z, 1.0f }, vec2{ U1, V1 } },
				{ vec4{ X + W,	Y,		Z, 1.0f }, vec2{ U1, V0 } } };

#if MSTD_DX9
			std::unique_ptr<IDirect3DVertexBuffer9, IDirect3DDelete9<IDirect3DVertexBuffer9>> Vertices;
			Vertices.reset(Render->CreateVec4t(Vector));

			Render->DrawVec4t(
				Vertices.get(), Camera->m_Sprite.get(), Render->PassthroughPixelShader.get(),
				Camera->m_TexSprWidth, Camera->m_TexSprHeight,
				(D3DXMATRIX*)Camera->Orthogonal.get());
#endif
		}
	}

	{
		Render->SetDepthScale(0.0f, 0.0f);

		Render->AlphaBlending(FALSE, D3DBLEND_ZERO, D3DBLEND_ZERO);

		Render->AlphaTesting(FALSE, 0x00, D3DCMP_NEVER);
	}
}

void Global_Application::DrawSwitch(void)
{
	if (!IsRoomOpen() || !Camera->b_DrawSwitch) { return; }

	for (std::size_t i = 0; i < Room->Rvd->Count(); i++)
	{
		if (Room->Rvd->Get(i)->Fcut == Camera->m_Cut)
		{
			Geometry->Draw4p(Room->Rvd->Get(i)->Xz, 0, D3DCOLOR_ARGB(255, 0, 197, 0));
		}
	}
}

void Global_Application::DrawCollision(void)
{
	if (!IsRoomOpen() || !Geometry->b_DrawCollision) { return; }

	for (std::size_t i = 0; i < Room->Sca->Count(); i++)
	{
		SHAPEVECTOR Vec = Room->Sca->GetShapeVector(i);

		bool b_Shape = Geometry->b_ShapeCollision ? (Geometry->iObject == i) ? true : Geometry->b_ShapeCollisionAll : Geometry->b_ShapeCollisionAll;

		bool b_Solid = Geometry->b_SolidCollision ? (Geometry->iObject == i) ? true : Geometry->b_SolidCollisionAll : Geometry->b_SolidCollisionAll;

		DWORD Color = Geometry->b_HighlightCollision ? (Geometry->iObject == i) ? 0x00FF0000 : 0x00C5C5C5 : 0x00C5C5C5;

		if (!b_Shape) { Vec.h = Vec.y; }

		switch (Room->Sca->Get(i)->Id.Bits.Shape)
		{
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Box): Geometry->DrawBox(Vec, {}, Color, b_Solid); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Naname_a): Geometry->DrawDiagonal(Vec, {}, Color, b_Solid, Shape_Type::Diagonal_A); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Naname_b): Geometry->DrawDiagonal(Vec, {}, Color, b_Solid, Shape_Type::Diagonal_B); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Naname_c): Geometry->DrawDiagonal(Vec, {}, Color, b_Solid, Shape_Type::Diagonal_C); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Naname_d): Geometry->DrawDiagonal(Vec, {}, Color, b_Solid, Shape_Type::Diagonal_D); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Hishi): Geometry->DrawRhombus(Vec, {}, Color, b_Solid); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Circle):
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Koban_x):
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Koban_z): Geometry->DrawCylinder(Vec, {}, Color, b_Solid); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Slope):
			switch (Room->Sca->GetSlopeHypotenuse(i))
			{
			case Resident_Evil_2_Slope_Hypotenuse::Type_A: Geometry->DrawTriangle(Vec, {}, Color, b_Solid, Shape_Type::Slope_A); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_B: Geometry->DrawTriangle(Vec, {}, Color, b_Solid, Shape_Type::Slope_B); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_C: Geometry->DrawTriangle(Vec, {}, Color, b_Solid, Shape_Type::Slope_C); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_D: Geometry->DrawTriangle(Vec, {}, Color, b_Solid, Shape_Type::Slope_D); break;
			}
			break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Box_3):
			switch (Room->Sca->GetSlopeHypotenuse(i))
			{
			case Resident_Evil_2_Slope_Hypotenuse::Type_A: Geometry->DrawTriangle(Vec, {}, Color, b_Solid, Shape_Type::Slope_A); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_B: Geometry->DrawTriangle(Vec, {}, Color, b_Solid, Shape_Type::Slope_B); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_C: Geometry->DrawTriangle(Vec, {}, Color, b_Solid, Shape_Type::Slope_C); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_D: Geometry->DrawTriangle(Vec, {}, Color, b_Solid, Shape_Type::Slope_D); break;
			}
			break;
		}
	}
}

void Global_Application::DrawBlock(void)
{
	if (!IsRoomOpen() || !Geometry->b_DrawBlock) { return; }

	for (std::size_t i = 0; i < Room->Blk->Count(); i++)
	{
		std::int16_t WW = Room->Blk->Get(i)->Size_x - Room->Blk->Get(i)->Pos_x;
		std::int16_t DD = Room->Blk->Get(i)->Size_z - Room->Blk->Get(i)->Pos_z;

		Geometry->DrawBox({ Room->Blk->Get(i)->Pos_x, 0, Room->Blk->Get(i)->Pos_z, WW, 0, DD }, { }, 0x00FFFF00, false);
	}
}

void Global_Application::DrawFloor(void)
{
	if (!IsRoomOpen() || !Geometry->b_DrawFloor) { return; }

	for (std::size_t i = 0; i < Room->Flr->Count(); i++)
	{
		std::int16_t XX = Room->Flr->Get(i)->X;
		std::int16_t ZZ = Room->Flr->Get(i)->Z;
		std::uint16_t WW = Room->Flr->Get(i)->W;
		std::uint16_t DD = Room->Flr->Get(i)->D;

		Geometry->DrawBox({ XX, 0, ZZ, WW, 0, DD }, { }, 0x0000FFFF, false);
	}
}

void Global_Application::DrawEffect(std::size_t iElement, VECTOR2 Position, const float Scale)
{
	if (b_FileOp.load()) { return; }

	if (iElement >= Room->Esp->Data().size() || Room->Esp->Get(iElement).Id == 0xFF) { return; }

#ifdef MSTD_DX9
	const float TexelOffset = 0.5f;

	//Render->AlphaBlending(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
	//Render->AlphaTesting(TRUE, 0xFF, D3DCMP_GREATEREQUAL);

	Render->ResetWorld();
#else
	const float TexelOffset = 0.0f;
#endif

	vec3 World = vec3(GTE->ToFloat(Position.x), GTE->ToFloat(Position.y), GTE->ToFloat(Position.z));

	vec3 Side{};

	vec3 Billboard{};

	Camera->GetBillboard(World, Side, Billboard);

	Side.Scale(-1.0f);

	const auto& Effect = Room->Esp->Get(iElement);

	const auto& SSeq = Effect.SSequence[Room->Esp->iSSeq];

	for (std::size_t i = 0; i < SSeq.nSpr; i++)
	{
		if (!SSeq.Time || SSeq.Time == 0xFF) { continue; }

		const auto& Group = Effect.SpriteGp[SSeq.pGp + i];

		const float Width = static_cast<float>(SSeq.Width);
		const float Height = static_cast<float>(SSeq.Height);

		const float U0 = (static_cast<float>(Group.U) + TexelOffset) / Effect.TextureWidth;
		const float V0 = (static_cast<float>(Group.V) + TexelOffset) / Effect.TextureHeight;
		const float U1 = (static_cast<float>(Group.U) + Width - TexelOffset) / Effect.TextureWidth;
		const float V1 = (static_cast<float>(Group.V) + Height - TexelOffset) / Effect.TextureHeight;

		float Left = 0;
		float Top = 0;
		if (Room->Esp->GameType() & (AUG95 | OCT95 | BIO1))
		{
			Left = static_cast<float>(-Group.OfsX / 8) * Scale;
			Top = static_cast<float>(-Group.OfsY / 8) * Scale;
		}
		else
		{
			Left = static_cast<float>(-SSeq.Hotx + Group.OfsX) * Scale;
			Top = static_cast<float>(-SSeq.Hoty + Group.OfsY) * Scale;
		}
		const float Right = (Left + Width * Scale);
		const float Bottom = (Top + Height * Scale);

		std::vector<vec3t> Vector = {
			{ { World + Side * Left + Billboard * Bottom }, { U0, V1 } },
			{ { World + Side * Left + Billboard * Top }, { U0, V0 } },
			{ { World + Side * Right + Billboard * Bottom }, { U1, V1 } },
			{ { World + Side * Right + Billboard * Top }, { U1, V0 } },
		};

#ifdef MSTD_DX9
		std::unique_ptr<IDirect3DVertexBuffer9, IDirect3DDelete9<IDirect3DVertexBuffer9>> Vertices;

		Vertices.reset(Render->CreateVec3t(Vector));

		Render->DrawVec3t(Vertices.get(), nullptr, Effect.Texture[Room->iEffectClutID].get(), Render->PS1DitherPixelShader.get(), Effect.TextureWidth, Effect.TextureHeight);
#endif
	}

	{
#ifdef MSTD_DX9
		//Render->AlphaBlending(FALSE, D3DBLEND_ZERO, D3DBLEND_ZERO);
		//Render->AlphaTesting(FALSE, 0x00, D3DCMP_NEVER);
#endif
	}

	if (!Room->Esp->b_Play.load()) { return; }

	if (Room->Esp->m_FrameCounter % 2 == 0)
	{
		Room->Esp->iTime.fetch_add(1);
		if (Room->Esp->iTime.load() >= SSeq.Time || SSeq.Time == 0xFF)
		{
			Room->Esp->iTime.store(0);
			Room->Esp->iSSeq.fetch_add(1);
			Room->Esp->m_FrameCounter = 0;
		}
	}

	Room->Esp->m_FrameCounter++;

	const size_t m_FrameCount = Effect.SSequence.empty() ? 0 : Effect.SSequence.size() - 1;

	const bool b_FirstFrame = (Room->Esp->iSSeq.load() == 0);
	const bool b_LastFrame = (Room->Esp->iSSeq.load() >= m_FrameCount);

	if (Room->Esp->b_Loop.load())
	{
		if (!Room->Esp->b_PlayInReverse.load())
		{
			if (b_LastFrame)
			{
				Room->Esp->iTime.store(0);
				Room->Esp->iSSeq.store(0);
				Room->Esp->m_FrameCounter = 0;
			}
		}
		else
		{
			if (b_FirstFrame)
			{
				Room->Esp->iTime.store(0);
				Room->Esp->iSSeq.store(m_FrameCount);
				Room->Esp->m_FrameCounter = 0;
			}
		}
	}

	else if (b_LastFrame) { Room->Esp->iSSeq.store(m_FrameCount); Room->Esp->m_FrameCounter = 0; }
}

void Global_Application::ResetLighting(void) const
{
	Render->SetPSXLightMag(0);

	Render->SetPSXLightAmbient({ 64, 64, 64 });

	for (std::size_t i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			Render->SetPSXLight(i, 0, { 128, 128, 128 }, { 5400, -1800, 0 }, 7500);
		}
		else
		{
			Render->SetPSXLight(i, 0, { 0, 0, 0 }, { 0, 0, 0 }, 0);
		}
	}

	Render->UpdateLightConstants();
}

void Global_Application::SetLighting(void)
{
	if (!IsRoomOpen()) { ResetLighting(); return; }

	uintmax_t Cut = Camera->m_Cut;

	Render->SetPSXLightMag(Room->Lit->Get(Cut)->Mag);

	Render->SetPSXLightAmbient({ (float)Room->Lit->Get(Cut)->Ambient.r, (float)Room->Lit->Get(Cut)->Ambient.g, (float)Room->Lit->Get(Cut)->Ambient.b });

	for (std::size_t i = 0; i < 3; i++)
	{
		Render->SetPSXLight(i,
			Room->Lit->Get(Cut)->Mode[i],
			{ (float)Room->Lit->Get(Cut)->Col[i].r, (float)Room->Lit->Get(Cut)->Col[i].g, (float)Room->Lit->Get(Cut)->Col[i].b },
			{ (float)Room->Lit->Get(Cut)->Pos[i].x, (float)Room->Lit->Get(Cut)->Pos[i].y, (float)Room->Lit->Get(Cut)->Pos[i].z },
			Room->Lit->Get(Cut)->L[i]);
	}

	Render->UpdateLightConstants();
}

void Global_Application::Collision(ModelType ModelType, VECTOR2& Position, SIZEVECTOR Hitbox)
{
	if (!IsRoomOpen() || !Geometry->b_CollisionDetection) { return; }

	for (std::size_t i = 0; i < Room->Sca->Count(); i++)
	{
		if (std::to_underlying(ModelType) & std::to_underlying(ModelType::Object) && !Room->Sca->Get(i)->Id.Bits.bit7) { continue; }
		if (std::to_underlying(ModelType) & std::to_underlying(ModelType::Player) && !Room->Sca->Get(i)->Id.Bits.bit8) { continue; }
		if (std::to_underlying(ModelType) & std::to_underlying(ModelType::SubPlayer) && !Room->Sca->Get(i)->Id.Bits.bit8) { continue; }
		if (std::to_underlying(ModelType) & std::to_underlying(ModelType::Enemy) && !Room->Sca->Get(i)->Id.Bits.bit3) { continue; }

		switch (Room->Sca->Get(i)->Id.Bits.Shape)
		{
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Box): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Rectangle); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Naname_a): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Diagonal_A); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Naname_b): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Diagonal_B); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Naname_c): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Diagonal_C); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Naname_d): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Diagonal_D); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Hishi): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Rhombus); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Circle): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Circle); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Koban_x): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::OblongX); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Koban_z): Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::OblongZ); break;
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Slope):
		case std::to_underlying(Resident_Evil_2_Collision_Shape::Box_3):
			switch (Room->Sca->GetSlopeHypotenuse(i))
			{
			case Resident_Evil_2_Slope_Hypotenuse::Type_A: Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Slope_A); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_B: Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Slope_B); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_C: Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Slope_C); break;
			case Resident_Evil_2_Slope_Hypotenuse::Type_D: Geometry->Collision(Position, Hitbox, Room->Sca->GetShapeVector(i), Shape_Type::Slope_D); break;
			}
			break;
		}
	}
}

void Global_Application::CameraSwitch(VECTOR2& Position, SIZEVECTOR Hitbox)
{
	if (!IsRoomOpen() || !Geometry->b_SwitchDetection) { return; }

	auto& Data = Room->Rvd->data();

	bool b_FirstInstance = false;

	for (std::size_t i = 0; i < Data.size(); i++)
	{
		if (!Data[i].Be_flg || (Data[i].Fcut != Camera->m_Cut)) { continue; }

		if (Data[i].Tcut == 0 && !b_FirstInstance) { b_FirstInstance = true; continue; }

		if (Data[i].Fcut == Camera->m_Cut)
		{
			if (Geometry->CameraSwitch(Position, Data[i].Xz))
			{
				uint8_t Cut = Data[i].Tcut;
				Camera->SetImage(Cut);
				Camera->Set(Room->Rid->Get(Cut)->ViewR >> 7, Room->Rid->Get(Cut)->View_p, Room->Rid->Get(Cut)->View_r);
				SetLighting();
				return;
			}
		}
	}
}