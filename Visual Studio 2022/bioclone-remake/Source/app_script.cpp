/*
*
*	Megan Grass
*	September 27, 2025
*
*/


#include "app.h"


struct ScriptFunction { uint32_t(*func)(Scenario::Task*); };
extern ScriptFunction Sce_jmp_tbl[];


struct ScenarioFunction { uint32_t(*func)(Scenario::AOT&); };
extern ScenarioFunction Sce_at_jump_tbl[];


namespace Resident_Evil_2_Instruction
{
	struct StdExCall : public Standard_Exception
	{
		using Standard_Exception::Exception;
	};

	enum : uint32_t
	{
		STATUS_READY = 0,
		STATUS_BUSY = 1
	};

	enum : uint32_t
	{
		EXEC_NORMAL = 1,
		EXEC_YIELD = 2
	};


	static uint32_t* Flg_addr(uint32_t Flag)
	{
		switch (Flag)
		{
		case FLAG_SYSTEM: return &G->Sce->System_flg;
		case FLAG_STATUS: return &G->Sce->Status_flg;
		case FLAG_STOP: return &G->Sce->Stop_flg;
		case FLAG_SCENARIO: return &G->Sce->SaveData.Scenario_flg[0];
		case FLAG_COMMON: return &G->Sce->SaveData.Common_flg[0];
		case FLAG_ROOM: return &G->Sce->SaveData.Room_flg[0];
		case FLAG_ENEMY: return &G->Sce->SaveData.Enemy_flg[0];
		case FLAG_ENEMY_2: return &G->Sce->SaveData.Enemy_flg2[0];
		case FLAG_ITEM: return &G->Sce->SaveData.Item_flg[0];
		case FLAG_MAP: return &G->Sce->SaveData.Map_flg[0];
		case FLAG_USE: return &G->Sce->Use_flg[0];
		case FLAG_MESSAGE: return &G->Sce->Mess_flg;
		case FLAG_ROOM_ENEMY: return &G->Sce->Room_enemy_flg;
		case FLAG_PRI_00: return &G->Sce->SaveData.Pri_be_flg[0][0];
		case FLAG_PRI_01: return &G->Sce->SaveData.Pri_be_flg[1][0];
		case FLAG_PRI_02: return &G->Sce->SaveData.Pri_be_flg[2][0];
		case FLAG_PRI_03: return &G->Sce->SaveData.Pri_be_flg[3][0];
		case FLAG_PRI_04: return &G->Sce->SaveData.Pri_be_flg[4][0];
		case FLAG_PRI_05: return &G->Sce->SaveData.Pri_be_flg[5][0];
		case FLAG_PRI_06: return &G->Sce->SaveData.Pri_be_flg[6][0];
		case FLAG_PRI_07: return &G->Sce->SaveData.Pri_be_flg[7][0];
		case FLAG_PRI_08: return &G->Sce->SaveData.Pri_be_flg[8][0];
		case FLAG_PRI_09: return &G->Sce->SaveData.Pri_be_flg[9][0];
		case FLAG_PRI_10: return &G->Sce->SaveData.Pri_be_flg[10][0];
		case FLAG_PRI_11: return &G->Sce->SaveData.Pri_be_flg[11][0];
		case FLAG_PRI_12: return &G->Sce->SaveData.Pri_be_flg[12][0];
		case FLAG_PRI_13: return &G->Sce->SaveData.Pri_be_flg[13][0];
		case FLAG_PRI_14: return &G->Sce->SaveData.Pri_be_flg[14][0];
		case FLAG_PRI_15: return &G->Sce->SaveData.Pri_be_flg[15][0];
		case FLAG_ZAPPING: return &G->Sce->SaveData.Zapping_flg[0];
		case FLAG_RBJ_SET: return &G->Sce->Rbj_set_flg;
		case FLAG_KEY: return &G->Sce->SaveData.Key_flg[0];
		case FLAG_MAP_C: return &G->Sce->SaveData.Map_c_flg[0];
		case FLAG_MAP_I: return &G->Sce->SaveData.Map_i_flg;
		case FLAG_ITEM_2: return &G->Sce->SaveData.Item_flg2[0];
		case FLAG_MAP_O: return &G->Sce->SaveData.Map_o_flg;
		case FLAG_EX_0: return &G->Sce->SaveData.ExData0[0];
		case FLAG_EX_1: return &G->Sce->SaveData.ExData1[0];
		case FLAG_EX_2: return &G->Sce->SaveData.ExData2[0];
		case FLAG_EX_3: return &G->Sce->SaveData.ExData3[0];
		}
		return nullptr;
	}

	static void Flg_on(uint32_t* p, uint32_t x)
	{
		p[x >> 5] = p[x >> 5] | (0x80000000U >> (x & 0x1F));
	}

	static void Flg_off(uint32_t* p, uint32_t x)
	{
		p[x >> 5] = p[x >> 5] & ~(0x80000000U >> (x & 0x1F));
	}

	static uint32_t Flg_ck(uint32_t* p, uint32_t x)
	{
		return p[x >> 5] & (0x80000000U >> (x & 0x1F));
	}


	static uint8_t Rnd(void)
	{
		uint32_t Bits = G->Sce->Random >> 7 & 0xFF;

		uint32_t Random = Bits + G->Sce->Random;

		G->Sce->Random = (Random & 0xFF) | (Bits << 8);

		return static_cast<uint8_t>(Random);
	}


	static Resident_Evil_2_RVD_Data* Ccut_search(uint32_t Fc) noexcept
	{
		static Resident_Evil_2_RVD_Data dummy{};

		if (G->Room->Rvd->data().empty()) { return &dummy; }

		const uint8_t Target = static_cast<uint8_t>(Fc & 0xFF);

		for (size_t i = 0; i < G->Room->Rvd->data().size(); ++i)
		{
			if (G->Room->Rvd->Get(i)->Fcut == Target)
			{
				return G->Room->Rvd->Get(i);
			}
		}

		return &dummy;
	}

	static void Cut_change(uint32_t Fc)
	{
		G->Sce->Next_cut_no = static_cast<uint8_t>(Fc);

		G->Sce->Cccut_next = Ccut_search(Fc);
	}

	static void Cut_chg_main(uintmax_t Cut_no)
	{
		//Main.Bg_chg_flg = 1;

		// G->Sce->Cut_old = G.SaveData.Cut_no;
		G->Sce->Cut_old = G->Camera->m_Cut;

		G->Sce->SaveData.Cut_no = static_cast<int16_t>(Cut_no);

		Cut_change(static_cast<uint32_t>(Cut_no));

		if (Cut_no != G->Camera->m_Cut && Cut_no < G->Camera->m_CutMax)
		{
			G->Camera->SetImage(Cut_no);

			G->Camera->Set(G->Room->Rid->Get(Cut_no)->ViewR >> 7, G->Room->Rid->Get(Cut_no)->View_p, G->Room->Rid->Get(Cut_no)->View_r);

			G->Room->Rvd->GetFrustum(Cut_no, G->Camera->m_Frustum);

			G->SetLighting();

			G->Sce->System_flg &= 0xdfffffff;
		}
	}


	static void Rot_vector(int16_t dir, SVECTOR* p, SVECTOR* q)
	{
		// Svec and Tmp_matrix should be global, probably used by enemy overlays

		SVECTOR Svec{};
		MATRIX Tmp_matrix{};

		Svec.vy = dir;

		G->GTE->RotMatrix((SVECTOR*)&Svec, (MATRIX*)&Tmp_matrix);
		G->GTE->ApplyMatrixSV((MATRIX*)&Tmp_matrix, (SVECTOR*)p, (SVECTOR*)q);
	}

	static void Rot_vector_super(int16_t dir, MATRIX* pM, SVECTOR* p, SVECTOR* q)
	{
		// Svec and Tmp_matrix should be global, probably used by enemy overlays

		SVECTOR Svec{};
		MATRIX Tmp_matrix{};

		G->GTE->RotMatrix((SVECTOR*)&Svec, (MATRIX*)&Tmp_matrix);
		G->GTE->MulMatrix2((MATRIX*)pM, (MATRIX*)&Tmp_matrix);
		G->GTE->ApplyMatrixSV((MATRIX*)&Tmp_matrix, (SVECTOR*)p, (SVECTOR*)q);
	}


	static int16_t Direction_ck(int16_t Hontai_x, int16_t Hontai_z, int16_t Target_x, int16_t Target_z)
	{
		int16_t Dir = 1024;

		int32_t XX = static_cast<int32_t>(Target_x) - static_cast<int32_t>(Hontai_x);

		if (0 < static_cast<int32_t>(Target_z) - static_cast<int32_t>(Hontai_z))
		{
			Dir = 3072;
		}

		int32_t ZZ = (static_cast<int32_t>(Target_z) - static_cast<int32_t>(Hontai_z)) * 4096;

		if (XX != 0)
		{
			if (XX == 0)
			{
				try
				{
					throw std::runtime_error("Resident Evil 2 Instruction: Direction_ck - Division by zero");
				}
				catch (...)
				{
					StdExCall().Exception(std::current_exception(), true, std::source_location::current());
				}
			}

			if ((XX == -1) && (ZZ == INT32_MIN))
			{
				try
				{
					throw std::runtime_error("Resident Evil 2 Instruction: Direction_ck - Overflow in negation");
				}
				catch (...)
				{
					StdExCall().Exception(std::current_exception(), true, std::source_location::current());
				}
			}

			//ZZ = catan(ZZ / XX);
			ZZ = G->GTE->ratan2(ZZ, XX);	// TODO: fix for accuracy

			int16_t Angle = 0;

			if (XX < 0)
			{
				Angle = 2048;
			}

			Dir = Angle - static_cast<int16_t>(ZZ) & 4095;
		}

		return Dir;
	}

	static void Goto00(std::shared_ptr<Resident_Evil_Model>& pEm, int32_t Vec_x, int32_t Vec_z, int16_t Add_dir)
	{
		uint32_t Step = static_cast<uint16_t>(Add_dir);

		uint32_t Direction = static_cast<uint16_t>(Direction_ck(static_cast<int16_t>(pEm->Position().x), static_cast<int16_t>(pEm->Position().z), static_cast<int16_t>(Vec_x), static_cast<int16_t>(Vec_z)));

		if (static_cast<int16_t>(Add_dir) < 0)
		{
			Step = static_cast<uint32_t>(-static_cast<int16_t>(Add_dir));

			Direction = (Direction + 2048) & 4095;
		}

		const uint32_t Current = static_cast<uint16_t>(pEm->Rotation().y);

		const uint32_t Angle = (Step + ((Direction - Current) & 4095)) & 4095;

		if (Angle < (Step * 2))
		{
			pEm->Rotation().y = static_cast<int16_t>(Direction);
		}
		else
		{
			int16_t sVar1 = static_cast<int16_t>(Current) - static_cast<int16_t>(Step);

			pEm->Rotation().y = sVar1;

			if (Angle < 2049)
			{
				pEm->Rotation().y = static_cast<int16_t>(static_cast<int32_t>(sVar1) + static_cast<int32_t>(Step) * 2);
			}
		}
	}

	static void Goto01(std::shared_ptr<Resident_Evil_Model>& pEm, int16_t Dir, int16_t Add_dir)
	{
		uint32_t Step = static_cast<uint16_t>(Add_dir);

		uint32_t Direction = static_cast<uint16_t>(Dir);

		if (static_cast<int16_t>(Add_dir) < 0)
		{
			Step = static_cast<uint32_t>(-static_cast<int16_t>(Add_dir));
			Direction = (Direction + 2048) & 4095;
		}

		const uint32_t Current = static_cast<uint16_t>(pEm->Rotation().y);

		const uint32_t Angle = (Step + ((Direction - Current) & 4095)) & 4095;

		if (Angle < (Step * 2))
		{
			pEm->Rotation().y = static_cast<int16_t>(Direction);
		}
		else
		{
			int16_t sVar1 = static_cast<int16_t>(Current) - static_cast<int16_t>(Step);

			pEm->Rotation().y = sVar1;

			if (Angle < 2049)
			{
				pEm->Rotation().y = static_cast<int16_t>(static_cast<int32_t>(sVar1) + static_cast<int32_t>(Step) * 2);
			}
		}
	}

	static int32_t Goto00_ck(std::shared_ptr<Resident_Evil_Model>& pEm, int32_t Vec_x, int32_t Vec_z, int16_t Add_dir)
	{
		const uint16_t Dir = Direction_ck(static_cast<int16_t>(pEm->Position().x), static_cast<int16_t>(pEm->Position().z), static_cast<int16_t>(Vec_x), static_cast<int16_t>(Vec_z));

		const uint32_t Angle = (static_cast<uint32_t>(static_cast<uint16_t>(Add_dir)) + (static_cast<uint32_t>(Dir) - static_cast<uint32_t>(static_cast<uint16_t>(pEm->Rotation().y)))) & 4095u;

		const int32_t Step = static_cast<int32_t>(Add_dir);

		int32_t Result = 0;

		if ((Step << 1) <= static_cast<int32_t>(Angle))
		{
			Result = Step;

			if (2048u < Angle)
			{
				Result = -Step;
			}
		}

		return Result;
	}

	static int32_t Goto01_ck(std::shared_ptr<Resident_Evil_Model>& pEm, int16_t Dir, int16_t Add_dir)
	{
		const uint32_t Angle = (static_cast<uint32_t>(static_cast<uint16_t>(Add_dir)) + (static_cast<uint32_t>(static_cast<uint16_t>(Dir)) -
			static_cast<uint32_t>(static_cast<uint16_t>(pEm->Rotation().y)))) & 4095u;

		const int32_t Step = static_cast<int32_t>(Add_dir);

		int32_t Result = 0;

		if ((Step << 1 <= static_cast<int32_t>(Angle)) && (Result = Step, 2048u < Angle))
		{
			Result = -Step;
		}

		return Result;
	}

	static uint8_t Cdir_ck(std::shared_ptr<Resident_Evil_Model>& pEm, std::shared_ptr<Resident_Evil_Model>& pT)
	{
		return ((pT->Rotation().y - pEm->Rotation().y) + 1024u & 4095) < 2048;
	}

	static uint8_t Cdir_ck2(std::shared_ptr<Resident_Evil_Model>& pEm, std::shared_ptr<Resident_Evil_Model>& pT)
	{
		int16_t Dir = Direction_ck(static_cast<int16_t>(pEm->Position().x), static_cast<int16_t>(pEm->Position().z), static_cast<int16_t>(pT->Position().x), static_cast<int16_t>(pT->Position().z));

		return ((pT->Rotation().y - static_cast<int32_t>(Dir)) + 1024u & 4095) < 2048;
	}

	static int32_t Dir_pos_ck(VECTOR* pV, VECTOR* pP, int16_t Hed_dir, int16_t Hani_dir)
	{
		int16_t Dir = Direction_ck(static_cast<int16_t>(pV->vx), static_cast<int16_t>(pV->vz), static_cast<int16_t>(pP->vx), static_cast<int16_t>(pP->vz));

		uint32_t Left = static_cast<uint32_t>((static_cast<int32_t>(Dir) - static_cast<int32_t>(Hed_dir)) + static_cast<int32_t>(Hani_dir));

		Left &= 4095u;

		uint32_t Right = static_cast<uint32_t>(static_cast<int32_t>(Hani_dir) << 1);

		uint32_t Compare = (Left < Right) ? 1u : 0u;

		return static_cast<int32_t>(Compare ^ 1u);
	}


	static int32_t Search_item_id(uint32_t id)
	{
		for (uint8_t i = 0; i < static_cast<uint32_t>(G->Sce->SaveData.Max_item_num); ++i)
		{
			if (G->Sce->SaveData.Item_work[i].Id == id) { return static_cast<int32_t>(i); }
		}

		return -1;
	}

	static int32_t Search_item(uint32_t type)
	{
		uint32_t idx = 0;
		int32_t emptyCount = 0;

		const uint32_t max = static_cast<uint32_t>(G->Sce->SaveData.Max_item_num);

		while (idx < max)
		{
			if (G->Sce->SaveData.Item_work[idx].Id == 0)
			{
				if (type == 0)
				{
					return static_cast<int32_t>(idx);
				}

				++emptyCount;
			}

			++idx;
		}

		return (type != 0) ? emptyCount : -1;
	}


	static int32_t Mess_set(uint32_t Pos_xy, uint16_t Attr, uint32_t Mess_no, uint32_t Stop_data)
	{
		if (!G->Message->IsStatic() || !G->Message->IsDynamic()) { return 0; }

		G->b_Pause.store(true);

		G->Message->ResetPrintCount();

		G->Sce->Status_flg |= STAT_AUTO_MOVE;

		auto PrintMessage = [&]()
			{
				if ((G->Sce->System_flg & (SYS_LANG_MAIN | SYS_LANG_SUB)) == SYS_LANG_SUB)
				{
					if (Mess_no < G->Room->MessageSub.size())
					{
						G->Message->Print(34, 185, Attr | 0x4000, G->Room->MessageSub[Mess_no], G->Message->PrintCount());
					}
				}
				else
				{
					if (Mess_no < G->Room->MessageSub.size())
					{
						G->Message->Print(34, 185, Attr | 0x4000, G->Room->Message[Mess_no], G->Message->PrintCount());
					}
				}
			};

		PrintMessage();

		while (G->b_Pause.load())
		{
			if ((static_cast<uint32_t>(G->Player->State()->KeyStateTrigger()) & static_cast<uint32_t>(Resident_Evil_Key::INSPECT)) != 0)
			{
				if (G->Message->IsComplete())
				{
					G->b_Pause.store(false);

					G->Message->ResetCanvas();

					G->Sce->Stop_flg = G->Sce->Stop_bak;

					G->Sce->Status_flg &= ~STAT_AUTO_MOVE;

					break;
				}
				else
				{
					G->Message->ResetCanvas();

					PrintMessage();
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		return 0;
	}


	static void Event_init(Scenario::Task* pSce, uint32_t Evt_no)
	{
		pSce->Status = STATUS_BUSY;
		pSce->Routine0 = 0;
		pSce->pS_SP = pSce->Stack[pSce->Sub_ctr];
		pSce->Ifel_ctr[0] = -1;
		pSce->Loop_ctr[0] = -1;

		if (!G->Room->Script->b_InitComplete.load())
		{
			pSce->Data = const_cast<uint8_t*>(std::get<Resident_Evil_2_Bytecode>(G->Room->Script->Init[Evt_no]).Raw().data());
		}
		else
		{
			pSce->Data = const_cast<uint8_t*>(std::get<Resident_Evil_2_Bytecode>(G->Room->Script->Loop[Evt_no]).Raw().data());
		}
	}

	static void Event_exec(uint32_t Task_level, uint32_t Evt_no)
	{
		if (G->Sce->Type == 0)
		{
			if (Task_level > 9)
			{
				Task_level = 2;

				if (G->Sce->Sce_task[2].Status != STATUS_READY)
				{
					while (true)
					{
						if (Task_level == 9) { break; }

						++Task_level;

						if (Task_level > 9) { return; }

						if (G->Sce->Sce_task[Task_level].Status == STATUS_READY) { break; }
					}
				}
			}
		}
		else if (Task_level > 13)
		{
			Task_level = 10;

			if (G->Sce->Sce_task[10].Status != STATUS_READY)
			{
				while (true)
				{
					if (Task_level == 13) { break; }

					++Task_level;

					if (Task_level > 13) { return; }

					if (G->Sce->Sce_task[Task_level].Status == STATUS_READY) { break; }
				}
			}
		}

		if (Task_level >= static_cast<uint32_t>(G->Sce->Sce_task.size()))
		{
			try
			{
				throw std::runtime_error("Resident Evil 2 Instruction: Event_exec - Task level out of range");
			}
			catch (...)
			{
				StdExCall().Exception(std::current_exception(), true, std::source_location::current());
			}
		}

		G->Sce->Sce_task[Task_level].Sub_ctr = 0;

		G->Sce->Sce_task[Task_level].Speed = { 0, 0, 0 };

		G->Sce->Sce_task[Task_level].DSpeed = { 0, 0, 0 };

		G->Sce->Sce_task[Task_level].ASpeed = { 0, 0, 0 };

		G->Sce->Sce_task[Task_level].ADSpeed = { 0, 0, 0 };
		
		Event_init(&G->Sce->Sce_task[Task_level], Evt_no);
	}


	static void Sce_aot_init(void)
	{
		for (size_t i = 0; i < G->Sce->Sce_aot.size(); i++)
		{
			std::memset(&G->Sce->Sce_aot[i], 0, sizeof(Scenario::AOT));
		}

		G->Sce->nSce_aot = 0;
	}

	static void Sce_rnd_set(void)
	{
		if ((G->Sce->System_flg & SYS_ZAPPING) != 0)
		{
			G->Sce->SaveData.Sce_random = static_cast<int16_t>(Rnd());
		}
		else
		{
			uint32_t Base = G->Sce->Random_base << 1;
			uint32_t High = Base & 0xFFFF0000u;
			uint32_t Low = (static_cast<uint32_t>(G->Sce->Random_base) + (Base >> 16)) & 0xFFFFu;
			G->Sce->Random_base = High | Low;
			G->Sce->SaveData.Sce_random = static_cast<int16_t>(G->Sce->Random_base);
		}
	}

	static void Sce_work_clr(void)
	{
		G->Sce->SaveData.F_atari = -1;
		G->Sce->SaveData.U_atari = -1;
		G->Sce->SaveData.Use_id = -1;
		G->Sce->SaveData.Get_id = -1;
		G->Sce->SaveData.Room_flg[1] = 0;
		G->Sce->Use_flg[0] = 0;
		G->Sce->Use_flg[1] = 0;
		G->Sce->Use_flg[2] = 0;
		G->Sce->Use_flg[3] = 0;
		G->Sce->Mizu_div_ctr = 0;
	}

	static void Sce_work_clr_set(void)
	{
		auto ClearWork = [](auto& Container)
			{
				for (auto& Element : Container)
				{
					if (Element) { Element->Close(); }
				}

				Container.clear();
				Container.shrink_to_fit();
			};

		ClearWork(G->Door);

		ClearWork(G->Enemy);

		for (auto i = 0; i < 10; i++)
		{
			G->Sce->Sce_task[i].Routine0 = 0;
			G->Sce->Sce_task[i].Status = STATUS_READY;
			G->Sce->Sce_task[i].Sub_ctr = static_cast<int8_t>(i);
			G->Sce->Sce_task[i].Task_level = -1;
			G->Sce->Sce_task[i].Ifel_ctr[0] = -1;
		}

		Sce_aot_init();

		G->Sce->SaveData.Room_flg[0] = 0;

		G->Sce->Mizu_div_max = 0;
		G->Sce->Mizu_div_ctr = 0;

		G->Sce->Rbj_reset_flg = 0;

		G->Sce->SaveData.Def_em_set = -1;
		G->Sce->SaveData.Def_aot_set = -1;

		G->Sce->Room_enemy_flg = 0;
		G->Sce->Room_player_flg = 0;

		//Cc_work.Ctex_old = 0xFF;

		G->Sce->C_id = 0xFF;
		G->Sce->C_model_type = 0xFF;
		G->Sce->C_kind = 0xFF;

		G->Sce->Se_vol = 0xFF;

		//Cc_work.Ccol_no = 0;

		G->Sce->Se_tmp0 = 0;
		G->Sce->Se_tmp1 = 0;

		//CD.Vol = 0x78;

		G->Sce->Status_flg = G->Sce->Status_flg & 0xfffff3ff;

		G->Sce->pC_em = nullptr;

		for (auto& Pri_be_flg : G->Sce->SaveData.Pri_be_flg)
		{
			for (auto& Flag : Pri_be_flg)
			{
				Flag = 0xFFFFFFFF;
			}
		}

		//if ((G->Player.Be_flg & 0x400) != 0)
		{
		//	G->Player.Be_flg = G->Player.Be_flg & 0xfffffbff;
		}

		//G->Player.Type._1_1_ = G->Player.Type._1_1_ & 0xf;
	}

	static void Sce_scheduler_main(void)
	{
		uint32_t Result = EXEC_NORMAL;

		for (size_t i = 0; i < 10; ++i)
		{
			auto& Task = G->Sce->Sce_task[i];

			if (Task.Status == STATUS_READY || Task.Data == nullptr)
			{
				continue;
			}

			while (true)
			{
				do {
#if 0
					auto it = iBytecode.find(static_cast<std::int8_t>(Task.Data[0]));
					if (it != iBytecode.end())
					{
						std::cout << Standard_String().FormatCStyle("Opcode: 0x%02X\tName: %s", static_cast<int>(Task.Data[0]), it->second.Name.c_str()) << std::endl;
					}
#endif
					Result = Sce_jmp_tbl[*Task.Data].func(&Task);

				} while (Result == EXEC_NORMAL);

				if (Result == EXEC_YIELD || Task.Sub_ctr < 0)
				{
					break;
				}

				const int8_t Sub_ctr = Task.Sub_ctr;

				if (Sub_ctr >= 4)
				{
					Task.Status = STATUS_READY;
					break;
				}

				if (Task.Ifel_ctr[Sub_ctr] < 0)
				{
					break;
				}

				--Task.pS_SP;

				Task.Data = reinterpret_cast<uint8_t*>(*Task.pS_SP);

				Task.Ifel_ctr[Sub_ctr]--;
			}
		}

		Sce_work_clr();
		// Sce_work_clr_at();
	}

	static void Sce_scheduler_set(void)
	{
		auto RoomFlagIndex = [](uintmax_t Stage_no, uintmax_t Room_no) -> uint32_t
			{
				switch (Stage_no)
				{
				case 0: return static_cast<uint32_t>(Room_no + 0x00);
				case 1: return static_cast<uint32_t>(Room_no + 0x1E);
				case 2: return static_cast<uint32_t>(Room_no + 0x3A);
				case 3: return static_cast<uint32_t>(Room_no + 0x48);
				case 4: return static_cast<uint32_t>(Room_no + 0x59);
				case 5: return static_cast<uint32_t>(Room_no + 0x63);
				case 6: return static_cast<uint32_t>(Room_no + 0x7B);
				default: return 0;
				}
			};

		if ((G->Sce->System_flg & SYS_ZAPPING) != 0)
		{
			if (Flg_ck(G->Sce->SaveData.Map_flg, RoomFlagIndex(G->Room->m_Stage, G->Room->m_Room)) == 0)
			{
				Flg_on(G->Sce->SaveData.Common_flg, 0xFF);
			}
			else
			{
				Flg_off(G->Sce->SaveData.Common_flg, 0xFF);
			}
		}

		Sce_rnd_set();
		Sce_work_clr();
		Sce_work_clr_set();

		G->Sce->Type = 0;

		G->Room->Script->b_InitComplete.store(false);

		if (!G->Room->Script->Init.empty())
		{
			G->Sce->pCScd = const_cast<uint8_t*>(std::get<Resident_Evil_2_Bytecode>(G->Room->Script->Init[0]).Raw().data());
			Event_exec(0, 0);
		}

		G->Room->Script->b_InitComplete.store(true);

		if (!G->Room->Script->Loop.empty())
		{
			G->Sce->pCScd = const_cast<uint8_t*>(std::get<Resident_Evil_2_Bytecode>(G->Room->Script->Loop[0]).Raw().data());
			Event_exec(1, 0);
		}

		Sce_scheduler_main();
		//Sce_se_set();
		//Sce_col_chg_init();
		//Sce_mirror_init();
		//Sce_kirakira_set();
	}

	static void Sce_scheduler(void)
	{
		if ((G->Sce->Stop_flg & 0x2000000) == 0)
		{
			Sce_rnd_set();

			G->Sce->Type = 0;

			G->Sce->pCScd = const_cast<uint8_t*>(std::get<Resident_Evil_2_Bytecode>(G->Room->Script->Loop[0]).Raw().data());

			Event_exec(0, 1);

			Sce_scheduler_main();
		}
	}


	static uint32_t Nothing(Scenario::AOT& pAot)
	{
		G->Sce->SaveData.F_atari = static_cast<int16_t>(G->Sce->Old_Fatari);
		G->Sce->SaveData.U_atari = static_cast<int16_t>(G->Sce->Old_Uatari);
		return 0;
	}

	static uint32_t Sce_Door(Scenario::AOT& pAot)
	{
		G->Player->Position() = { pAot.Next_pos_x, pAot.Next_pos_y, pAot.Next_pos_z };
		G->Player->Rotation() = { 0, pAot.Next_cdir_y, 0 };

		G->Player->ClampPosition(G->Player->Position());
		G->Player->ClampRotation(G->Player->Rotation());

#if 0
		std::wcout << L"Sce_Door: " << Standard_String().FormatCStyle(L"%ws\\room%x%02x%d.rdt", G->Room->m_Path.wstring().c_str(), pAot.Next_stage + 1, pAot.Next_room, G->Player->m_PlayerID) << std::endl;
#endif

		G->OpenRDT(Standard_String().FormatCStyle(L"%ws\\room%x%02x%d.rdt", G->Room->m_Path.wstring().c_str(), pAot.Next_stage + 1, pAot.Next_room, G->Player->m_PlayerID), pAot.Next_cut);

		return 0;
	}

	static uint32_t Sce_Item(Scenario::AOT& pAot)
	{
		G->Sce->Get_item_id = pAot.iItem;

		if (((G->Sce->System_flg & SYS_LANG_SUB) != 0) && (G->Sce->Get_item_id == 30))
		{
			pAot.nItem = static_cast<uint16_t>(2);	// modify ink ribbon count?
		}

		bool Result = ((pAot.Action & 1) != 0);

		if (Result)
		{
			G->Player->State()->Set(Inspect_Kneel);
		}
		else
		{
			G->Sce->Status_flg |= STAT_STATUS_SCREEN;
		}

		// St_type = 2;

		// TODO: implement status screen

		return static_cast<uint32_t>(Result);
	}

	static uint32_t Sce_Normal(Scenario::AOT& pAot)
	{
		return 0;
	}

	static uint32_t Sce_Message(Scenario::AOT& pAot)
	{
		auto Mess_no = pAot.Data0 & 0xFF;
		auto Attr = pAot.Data1 & 0xFF;
		auto Stop_data = pAot.Data2;

		Resident_Evil_2_Instruction::Mess_set(0, static_cast<uint16_t>(Attr + 0x300), static_cast<uint16_t>(Mess_no), static_cast<uint32_t>(static_cast<uint16_t>(Stop_data)) << 16);

		return 0;
	}

	static uint32_t Sce_Event(Scenario::AOT& pAot)
	{
		if ((G->Sce->Stop_flg & 0x2000000) == 0)
		{
			uint8_t Evt_no = static_cast<uint8_t>(pAot.Data1 >> 8);

			Event_exec(pAot.Data0, static_cast<uint32_t>(Evt_no));
		}

		return 0;
	}

	static uint32_t Sce_Flg_chg(Scenario::AOT& pAot)
	{
		auto& Flag = pAot.Data0;
		auto& Bit = pAot.Data1;
		auto& Operator = pAot.Data2;

		auto* pFlag = Flg_addr(Flag);

		if (Operator != 0)
		{
			Flg_on(pFlag, Bit);
		}
		else
		{
			Flg_off(pFlag, Bit);
		}

		return 0;
	}

	static uint32_t Sce_Water(Scenario::AOT& pAot)
	{
		if (!G->Sce->pEm)
		{
			std::cout << "Sce_Water: player model is empty" << std::endl;
			return 0;
		}

		G->Sce->pEm->Water = static_cast<int16_t>(pAot.Data0);

		return 0;
	}

	static uint32_t Sce_Move(Scenario::AOT& pAot)
	{
		if (!G->Sce->pEm)
		{
			std::cout << "Sce_Move: player model is empty" << std::endl;
			return 0;
		}

		G->Sce->pEm->Position().x += static_cast<int32_t>(pAot.Data0);
		G->Sce->pEm->Position().y += static_cast<int32_t>(pAot.Data1);
		G->Sce->pEm->Position().z += static_cast<int32_t>(pAot.Data2);

		//G->Sce->pEm->Be_flg = G->Sce->pEm->Be_flg | 0x20000;

		return 0;
	}

	static uint32_t Sce_Save(Scenario::AOT& pAot)
	{
		// TODO

		/*auto Save_entrance = []()
			{
				if (G.Back_func_rtn == 0)
				{
					if (Search_item_id(30) < 0)
					{
						G->Sce->Stop_flg = G->Game->Stop_bak;

						Mess_set(0, 0x100, 0, 0xff000000);	// "...if I had an ink ribbon..."
					}
					else
					{
						Mess_set(0, 0x100, 1, 0xff000000);	// "...use the ink ribbon?"
					}
				}
				else if ((G.Back_func_rtn == 1) && ((G->Sce->Mess_flg & 0x80) == 0))
				{
					if ((G->Sce->Mess_flg & 1) == 0)
					{
						G->Sce->System_flg |= SYS_SAVE_GAME;
					}
					else
					{
						G->Sce->Stop_flg = G->Game->Stop_bak;
					}
				}
			};*/

		//G->Game->Stop_bak = G->Sce->Stop_flg;

		G->Sce->Stop_flg |= 0xff000000;

		G->Sce->SaveData.Save_area = static_cast<uint8_t>(pAot.Data0 & 0xFF);

		return 0;
	}

	static uint32_t Sce_Itembox(Scenario::AOT& pAot)
	{
		// TODO
		return 0;
	}

	static uint32_t Sce_Damage(Scenario::AOT& pAot)
	{
		// TODO

		auto& Model = G->Sce->pEm;

		//if ((Model->Damage_cnt & 0x80) == 0)
		if (!Model->State()->b_Damage)
		{
			//if ((Model->Status_flg & 2) == 0)
			{
			//	Model->Damage_cnt |= 0x80;

				Model->iHealth -= static_cast<int32_t>(pAot.Data1);

				if (Model->iHealth < 0)
				{
					if (Model->Routine_0 != 3)
					{
						Model->Routine_0 = 3;
						Model->Routine_1 = 0;
						Model->Routine_2 = 0;
						Model->Routine_3 = 0;

						Model->State()->Set(Death);
					}
				}
				else
				{
					Model->Routine_0 = 2;
					Model->Routine_1 = pAot.Data0 && 0xFF;
					Model->Routine_2 = 0;
					Model->Routine_3 = 0;

					Model->State()->Set(Damage_Front_Minor);

					int16_t Hontai_x = static_cast<int16_t>(G->Sce->pCAot->X + (G->Sce->pCAot->W >> 1));
					int16_t Hontai_z = static_cast<int16_t>(G->Sce->pCAot->Z + (G->Sce->pCAot->D >> 1));

					int16_t Dir = Direction_ck(Hontai_x, Hontai_z, static_cast<int16_t>(Model->Position().x), static_cast<int16_t>(Model->Position().z));

					//Model->Speed() = { 200, 0, 0 };
					Model->Speed().x = 200;

					int32_t muki = static_cast<int32_t>(Dir - static_cast<int16_t>(Model->Rotation().y));

					Model->AddSpeedXZ_orig(muki);

					//FUN_8003947c(4, 0);			// controller vibration
					//FUN_80039514(10, 0x96, 0);	// controller vibration
				}
			}
			//else
			{
			//	Model->Status_flg &= 0xfffd;
			}
		}

		return 1;
	}

	static uint32_t Sce_Status(Scenario::AOT& pAot)
	{
		// TODO

		//G->Sce->Game->Doordemo_flg = 1;

		//G->Sce->St->St_type = 0;

		//G->Sce->Game->Stop_bak = G->Sce->Stop_flg;

		G->Sce->Status_flg |= STAT_STATUS_SCREEN;

		return 0;
	}

	static uint32_t Sce_Hikidashi(Scenario::AOT& pAot)
	{
		// TODO
		return 0;
	}

	static uint32_t Sce_Windows(Scenario::AOT& pAot)
	{
		// TODO

		//G->Sce->Game->Doordemo_flg = 7;

		//G->Sce->Window_no = (static_cast<uint32_t>(static_cast<uint8_t>(pAot.Data1 & 0xFF)) << 4) + static_cast<uint32_t>(static_cast<uint8_t>(pAot.Data0 & 0xFF));

		return 0;
	}

	static uint32_t Sce_at_check(std::shared_ptr<Resident_Evil_Model> pEm, uint32_t attribute, uint32_t auto_flg)
	{
		if (!pEm) { return 0; }

		static constexpr std::array<const char*, 15> SceFuncStr =
		{
			"Nothing",
			"Door",
			"Item",
			"Normal",
			"Message",
			"Event",
			"Flag Change",
			"Water",
			"Move",
			"Save",
			"Item Box",
			"Damage",
			"Status",
			"Drawer",
			"Computer"
		};

		bool Hit_flg = false;

		SVECTOR Forward{ 620, 0, 0 };
		VECTOR2 Probe{};

		int16_t F_Atari = 0;
		int16_t U_Atari = 0;

		Rot_vector(static_cast<int16_t>(pEm->Rotation().y), &Forward, &Forward);

		Probe.x = static_cast<int32_t>(Forward.vx) + pEm->Position().x;
		Probe.z = static_cast<int32_t>(Forward.vz) + pEm->Position().z;

		G->Sce->pEm = pEm;

		for (size_t i = 0; i < G->Sce->Sce_aot.size(); i++)
		{
			auto& Aot = G->Sce->Sce_aot[i];

			G->Sce->pCAot = &Aot;

			if (Aot.Id == SCE_NOTHING) { continue; }

			G->Sce->Old_Fatari = G->Sce->SaveData.F_atari;
			G->Sce->Old_Uatari = G->Sce->SaveData.U_atari;

			if ((Aot.Type & attribute) == 0) { continue; }

			if (((Aot.Type & SCE_MANUAL) != auto_flg) || !((Aot.nFloor & 0x80) != 0 || pEm->nFloor() == Aot.nFloor)) { continue; }

			/*if ((Aot.Super & 0x80) == 0)
			{
				Aot.Data0 = 0;
				Aot.Data1 = 0;
			}
			else if ((Aot.Super & 0x40) == 0)
			{
				Aot.Data0 = static_cast<int16_t>(G->Room->Object[Aot.Super & 0x3F]->Position().x);
				Aot.Data1 = static_cast<int16_t>(G->Room->Object[Aot.Super & 0x3F]->Position().z);
			}
			else
			{
				Aot.Data0 = static_cast<int16_t>(G->Enemy[Aot.Super & 0x3F]->Position().x);
				Aot.Data1 = static_cast<int16_t>(G->Enemy[Aot.Super & 0x3F]->Position().z);
			}

			Aot.Data2 = Aot.Data0 + ((Aot.Type & SCE_POINT4) == 0 ? Aot.X : Aot.Xz[0][0]);*/

			Hit_flg = false;

			F_Atari = G->Sce->SaveData.F_atari;
			U_Atari = G->Sce->SaveData.U_atari;

			const bool b_Front = (Aot.Type & SCE_FRONT) != 0;
			const bool b_Under = (Aot.Type & SCE_UNDER) != 0;
			const bool b_Point4 = (Aot.Type & SCE_POINT4) != 0;

			if (b_Under)
			{
				if (b_Point4)
				{
					Hit_flg = G->Geometry->Collision4P(pEm->Position(), Aot.Xz);
				}
				else
				{
					Hit_flg = G->Geometry->CollisionBox(pEm->Position(), Aot.X, Aot.Z, Aot.W, Aot.D);
				}

				if (Hit_flg)
				{
					F_Atari = G->Sce->SaveData.F_atari;
					U_Atari = static_cast<int16_t>(i);
				}
				else if (b_Front)
				{
					if (b_Point4)
					{
						Hit_flg = G->Geometry->Collision4P(Probe, Aot.Xz);
					}
					else
					{
						Hit_flg = G->Geometry->CollisionBox(Probe, Aot.X, Aot.Z, Aot.W, Aot.D);
					}

					if (Hit_flg)
					{
						F_Atari = static_cast<int16_t>(i);
						U_Atari = G->Sce->SaveData.U_atari;
					}
				}
			}
			else if (b_Front)
			{
				if (b_Point4)
				{
					Hit_flg = G->Geometry->Collision4P(Probe, Aot.Xz);
				}
				else
				{
					Hit_flg = G->Geometry->CollisionBox(Probe, Aot.X, Aot.Z, Aot.W, Aot.D);
				}

				if (Hit_flg)
				{
					F_Atari = static_cast<int16_t>(i);
					U_Atari = G->Sce->SaveData.U_atari;
				}
			}

			if (!Hit_flg) { continue; }

			G->Sce->AotStr = SceFuncStr[Aot.Id];

			G->Sce->SaveData.F_atari = F_Atari;
			G->Sce->SaveData.U_atari = U_Atari;

			pEm->At_sce_no = static_cast<uint8_t>(i);

			if (auto_flg == 0)
			{
				Sce_at_jump_tbl[Aot.Id].func(Aot);
				continue;
			}

			return Sce_at_jump_tbl[Aot.Id].func(Aot);
		}

		return 0;
	}

	static void Sce_at(void)
	{
		if (!G->Player->State()) { return; }

		if ((static_cast<uint32_t>(G->Player->State()->KeyStateTrigger()) & static_cast<uint32_t>(Resident_Evil_Key::INSPECT)) != 0)
		{
			G->Sce->Status_flg |= STAT_INSPECT;
		}
		else
		{
			G->Sce->Status_flg &= ~STAT_INSPECT;
		}

		if (-1 > G->Player->iHealth) { return; }

		G->Player->Water = 0;

		if ((G->Sce->Status_flg & STAT_INSPECT) != 0)
		{
			Sce_at_check(G->Player, 1, SCE_MANUAL);
		}

		Sce_at_check(G->Player, 1, SCE_AUTOMATIC);

		for (size_t i = 0; i < G->Enemy.size(); i++)
		{
			auto& Enemy = G->Enemy[i];

			//if ((Enemy->Be_flg & 1) != 0)
			if (Enemy->b_Active)
			{
				Enemy->Water = 0;

				Sce_at_check(Enemy, 2, SCE_AUTOMATIC);
			}
		}

		for (size_t i = 0; i < G->Room->Object.size(); i++)
		{
			auto& Object = G->Room->Object[i];

			//if ((Object->Be_flg & 1) != 0)
			if (Object->b_Active)
			{
				Object->Water = 0;

				Sce_at_check(Object, 8, SCE_AUTOMATIC);
			}
		}

		//if ((G->SubPlayer->Be_flg & 1) != 0)
		if (G->SubPlayer->b_Active)
		{
			G->SubPlayer->Water = 0;

			Sce_at_check(G->SubPlayer, 8, SCE_AUTOMATIC);
		}
	}


	static uint32_t While_main(Scenario::Task* pSce, uint8_t* pData, uint8_t Ofs)
	{
		int16_t Value = 0;
		uint32_t OpResult = 0;

		uint32_t Result = Sce_jmp_tbl[*pData].func(pSce);

		int16_t* pCk_end = reinterpret_cast<int16_t*>(pSce->Data);

		while (pCk_end != reinterpret_cast<int16_t*>(pData + Ofs))
		{
			Value = *pCk_end;

			pSce->Data = reinterpret_cast<uint8_t*>(pCk_end + 1);

			OpResult = Sce_jmp_tbl[*(reinterpret_cast<uint8_t*>(pCk_end + 1))].func(pSce);

			pCk_end = reinterpret_cast<int16_t*>(pSce->Data);

			if (Value == 0)
			{
				Result &= OpResult;
			}
			else
			{
				Result |= OpResult;
			}
		}

		return Result;
	}

	static void Calc_branch(int32_t Operator, int16_t* Value, int32_t Num)
	{
		switch (Operator)
		{
		case CALC_ADD: *Value = static_cast<int16_t>(static_cast<uint16_t>(*Value) + static_cast<uint16_t>(Num)); break;
		case CALC_SUB: *Value = static_cast<int16_t>(static_cast<uint16_t>(*Value) - static_cast<uint16_t>(Num)); break;
		case CALC_MUL: *Value = static_cast<int16_t>(static_cast<uint16_t>(*Value) * static_cast<uint16_t>(Num)); break;
		case CALC_DIV:
		{
			if (Num == 0)
			{
				try
				{
					throw std::runtime_error("Resident Evil 2 Instruction: Calc_branch division by zero");
				}
				catch (...)
				{
					StdExCall().Exception(std::current_exception(), true, std::source_location::current());
				}
			}

			if ((Num == -1) && (*Value == static_cast<int16_t>(INT16_MIN)))
			{
				try
				{
					throw std::runtime_error("Resident Evil 2 Instruction: Calc_branch division overflow");
				}
				catch (...)
				{
					StdExCall().Exception(std::current_exception(), true, std::source_location::current());
				}
			}

			*Value = static_cast<int16_t>(static_cast<int32_t>(*Value) / static_cast<int32_t>(static_cast<int16_t>(Num)));

			break;
		}
		case CALC_MOD:
		{
			if (Num == 0)
			{
				try
				{
					throw std::runtime_error("Resident Evil 2 Instruction: Calc_branch modulo by zero");
				}
				catch (...)
				{
					StdExCall().Exception(std::current_exception(), true, std::source_location::current());
				}
			}

			if ((Num == -1) && (*Value == static_cast<int16_t>(INT16_MIN)))
			{
				try
				{
					throw std::runtime_error("Resident Evil 2 Instruction: Calc_branch modulo overflow");
				}
				catch (...)
				{
					StdExCall().Exception(std::current_exception(), true, std::source_location::current());
				}
			}

			*Value = static_cast<int16_t>(static_cast<int32_t>(*Value) % static_cast<int32_t>(static_cast<int16_t>(Num)));

			break;
		}
		case CALC_OR: *Value = static_cast<int16_t>(static_cast<uint16_t>(*Value) | static_cast<uint16_t>(Num)); break;
		case CALC_AND: *Value = static_cast<int16_t>(static_cast<uint16_t>(*Value) & static_cast<uint16_t>(Num)); break;
		case CALC_XOR: *Value = static_cast<int16_t>(static_cast<uint16_t>(*Value) ^ static_cast<uint16_t>(Num)); break;
		case CALC_NOT: *Value = static_cast<int16_t>(~static_cast<uint16_t>(*Value)); break;
		case CALC_SL: *Value = static_cast<int16_t>(static_cast<uint16_t>(*Value) << (static_cast<uint8_t>(Num) & 0x1F)); break;
		case CALC_SR_16BIT: *Value = static_cast<int16_t>(static_cast<uint16_t>(*Value) >> (static_cast<uint8_t>(Num) & 0x1F)); break;
		case CALC_SR_32BIT: *Value = static_cast<int16_t>(*Value >> (static_cast<uint8_t>(Num) & 0x1F)); break;
		}
	}

	static void Member_set_branch(std::shared_ptr<Resident_Evil_Model>& Work, uint32_t Member, int32_t Value)
	{
		if (!Work)
		{
			std::cout << "Member_set_branch: Work is nullptr" << std::endl;
			return;
		}

		switch (Member)
		{
		case MEMBER_BE_FLAG0: Work->b_Active.store(static_cast<bool>(Value)); break;

		case MEMBER_ROUTINE0: Work->Routine_0 = static_cast<uint8_t>(Value); break;
		case MEMBER_ROUTINE1: Work->Routine_1 = static_cast<uint8_t>(Value); break;
		case MEMBER_ROUTINE2: Work->Routine_2 = static_cast<uint8_t>(Value); break;
		case MEMBER_ROUTINE3: Work->Routine_3 = static_cast<uint8_t>(Value); break;
		case MEMBER_ID: Work->SetFileID(Value); break;
		case MEMBER_TYPE: Work->SetWeaponFileID(Value); break;

		case MEMBER_POS_X: Work->Position().x = Value; break;
		case MEMBER_POS_Y: Work->Position().y = Value; break;
		case MEMBER_POS_Z: Work->Position().z = Value; break;
		case MEMBER_CDIR_X: Work->Rotation().x = Value; break;
		case MEMBER_CDIR_Y: Work->Rotation().y = Value; break;
		case MEMBER_CDIR_Z: Work->Rotation().z = Value; break;
		case MEMBER_NFLOOR: Work->SetnFloor(Value); break;

		case MEMBER_DEST_X: Work->Dest_x = static_cast<int16_t>(Value); break;
		case MEMBER_DEST_Z: Work->Dest_z = static_cast<int16_t>(Value); break;
		case MEMBER_SCE_FLAG: Work->Sce_flg = Value; break;
		case MEMBER_SCE_FREE0: Work->Sce_free0 = Value; break;
		case MEMBER_SCE_FREE1: Work->Sce_free1 = Value; break;
		case MEMBER_SCE_FREE2: Work->Sce_free2 = Value; break;
		case MEMBER_SCE_FREE3: Work->Sce_free3 = Value; break;
		case MEMBER_SPD_X_BYTE: Work->Speed().x = static_cast<uint8_t>(Value); break;
		case MEMBER_SPD_X: Work->Speed().x = Value; break;
		case MEMBER_SPD_Y: Work->Speed().y = Value; break;
		case MEMBER_SPD_Z: Work->Speed().z = Value; break;

		case MEMBER_AT_W: Work->Hitbox().w = static_cast<uint16_t>(Value); break;
		case MEMBER_AT_H: Work->Hitbox().h = static_cast<uint16_t>(Value); break;
		case MEMBER_AT_D: Work->Hitbox().d = static_cast<uint16_t>(Value); break;
		}
	}

	static int32_t Load_member_branch(std::shared_ptr<Resident_Evil_Model>& Work, uint32_t Member)
	{
		if (!Work)
		{
			std::cout << "Load_member_branch: Work is nullptr" << std::endl;
			return Member;
		}

		switch (Member)
		{
		case MEMBER_BE_FLAG0: return Work->b_Active.load() ? 1 : 0;

		case MEMBER_ROUTINE0: return Work->Routine_0;
		case MEMBER_ROUTINE1: return Work->Routine_1;
		case MEMBER_ROUTINE2: return Work->Routine_2;
		case MEMBER_ROUTINE3: return Work->Routine_3;
		case MEMBER_ID: return Work->FileID();
		case MEMBER_TYPE: return Work->WeaponFileID();

		case MEMBER_POS_X: return Work->Position().x;
		case MEMBER_POS_Y: return Work->Position().y;
		case MEMBER_POS_Z: return Work->Position().z;
		case MEMBER_CDIR_X: return Work->Rotation().x;
		case MEMBER_CDIR_Y: return Work->Rotation().y;
		case MEMBER_CDIR_Z: return Work->Rotation().z;
		case MEMBER_NFLOOR: return Work->nFloor();

		case MEMBER_DEST_X: return Work->Dest_x;
		case MEMBER_DEST_Z: return Work->Dest_z;
		case MEMBER_SCE_FLAG: return Work->Sce_flg;
		case MEMBER_SCE_FREE0: return Work->Sce_free0;
		case MEMBER_SCE_FREE1: return Work->Sce_free1;
		case MEMBER_SCE_FREE2: return Work->Sce_free2;
		case MEMBER_SCE_FREE3: return Work->Sce_free3;
		case MEMBER_SPD_X_BYTE: return static_cast<uint8_t>(Work->Speed().x);
		case MEMBER_SPD_X: return Work->Speed().x;
		case MEMBER_SPD_Y: return Work->Speed().y;
		case MEMBER_SPD_Z: return Work->Speed().z;

		case MEMBER_AT_W: return Work->Hitbox().w;
		case MEMBER_AT_H: return Work->Hitbox().h;
		case MEMBER_AT_D: return Work->Hitbox().d;
		}

		return Member;
	}

	static int16_t& Load_member_addr_branch(std::shared_ptr<Resident_Evil_Model>& Work, uint32_t Member)
	{
		static int16_t dummy = 0;
		static uint32_t Value = 0;

		if (!Work)
		{
			std::cout << "Load_member_addr_branch: Work is nullptr" << std::endl;
			return reinterpret_cast<int16_t&>(dummy);
		}

		switch (Member)
		{
		case MEMBER_BE_FLAG0: Value = Work->b_Active.load() ? 1 : 0; return reinterpret_cast<int16_t&>(Value);

		case MEMBER_ROUTINE0: return reinterpret_cast<int16_t&>(Work->Routine_0);
		case MEMBER_ROUTINE1: return reinterpret_cast<int16_t&>(Work->Routine_1);
		case MEMBER_ROUTINE2: return reinterpret_cast<int16_t&>(Work->Routine_2);
		case MEMBER_ROUTINE3: return reinterpret_cast<int16_t&>(Work->Routine_3);
		case MEMBER_ID: Value = Work->FileID(); return reinterpret_cast<int16_t&>(Value);
		case MEMBER_TYPE: Value = Work->WeaponFileID(); return reinterpret_cast<int16_t&>(Value);

		case MEMBER_POS_X: return reinterpret_cast<int16_t&>(Work->Position().x);
		case MEMBER_POS_Y: return reinterpret_cast<int16_t&>(Work->Position().y);
		case MEMBER_POS_Z: return reinterpret_cast<int16_t&>(Work->Position().z);
		case MEMBER_CDIR_X: return reinterpret_cast<int16_t&>(Work->Rotation().x);
		case MEMBER_CDIR_Y: return reinterpret_cast<int16_t&>(Work->Rotation().y);
		case MEMBER_CDIR_Z: return reinterpret_cast<int16_t&>(Work->Rotation().z);
		case MEMBER_NFLOOR: Value = Work->nFloor(); return reinterpret_cast<int16_t&>(Value);

		case MEMBER_DEST_X: return reinterpret_cast<int16_t&>(Work->Dest_x);
		case MEMBER_DEST_Z: return reinterpret_cast<int16_t&>(Work->Dest_z);
		case MEMBER_SCE_FLAG: return reinterpret_cast<int16_t&>(Work->Sce_flg);
		case MEMBER_SCE_FREE0: return reinterpret_cast<int16_t&>(Work->Sce_free0);
		case MEMBER_SCE_FREE1: return reinterpret_cast<int16_t&>(Work->Sce_free1);
		case MEMBER_SCE_FREE2: return reinterpret_cast<int16_t&>(Work->Sce_free2);
		case MEMBER_SCE_FREE3: return reinterpret_cast<int16_t&>(Work->Sce_free3);
		case MEMBER_SPD_X_BYTE: return reinterpret_cast<int16_t&>(Work->Speed().x);
		case MEMBER_SPD_X: return reinterpret_cast<int16_t&>(Work->Speed().x);
		case MEMBER_SPD_Y: return reinterpret_cast<int16_t&>(Work->Speed().y);
		case MEMBER_SPD_Z: return reinterpret_cast<int16_t&>(Work->Speed().z);

		case MEMBER_AT_W: return reinterpret_cast<int16_t&>(Work->Hitbox().w);
		case MEMBER_AT_H: return reinterpret_cast<int16_t&>(Work->Hitbox().h);
		case MEMBER_AT_D: return reinterpret_cast<int16_t&>(Work->Hitbox().d);
		}

		return reinterpret_cast<int16_t&>(dummy);
	}


	static uint32_t Nop(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Nop*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Nop);

		return EXEC_NORMAL;
	}

	static uint32_t Evt_end(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Evt_end*>(pSce->Data);

		if (pSce->Sub_ctr != 0)
		{
			int8_t Sub_ctr = pSce->Sub_ctr - 1;

			int8_t Ifel_ctr = pSce->Ifel_ctr[Sub_ctr];

			pSce->Data = pSce->Ret_addr[Sub_ctr];

			pSce->Sub_ctr = static_cast<int8_t>(Sub_ctr);

			//pSce->pS_SP = pSce->Stack[Sub_ctr] + Ifel_ctr + 1;
			pSce->pS_SP = pSce->Stack[Sub_ctr] + static_cast<std::size_t>(Ifel_ctr + 1);

			return EXEC_NORMAL;
		}

		pSce->Status = STATUS_READY;

		return EXEC_YIELD;
	}

	static uint32_t Evt_next(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Evt_next*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Evt_next);

		return EXEC_YIELD;
	}

	static uint32_t Evt_chain(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Evt_chain*>(pSce->Data);

		Event_init(pSce, Op->Evt_no);

		return EXEC_NORMAL;
	}

	static uint32_t Evt_exec(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Evt_exec*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Evt_exec);

		Event_exec(Op->Task_level, Op->Evt_no);

		return EXEC_NORMAL;
	}

	static uint32_t Evt_kill(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Evt_kill*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Evt_kill);

		G->Sce->Sce_task[Op->Evt_no].Status = STATUS_READY;

		return EXEC_NORMAL;
	}

	static uint32_t Ifel_ck(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Ifel_ck*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Ifel_ck);
		
		pSce->Ifel_ctr[pSce->Sub_ctr]++;

		*pSce->pS_SP = reinterpret_cast<uintptr_t>(pSce->Data) + Op->Ofs;

		pSce->pS_SP++;

		return EXEC_NORMAL;
	}

	static uint32_t Else_ck(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Else_ck*>(pSce->Data);

		pSce->pS_SP--;

		pSce->Data += Op->Ofs;

		pSce->Ifel_ctr[pSce->Sub_ctr]--;

		return EXEC_NORMAL;
	}

	static uint32_t Endif(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Endif*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Endif);

		pSce->pS_SP--;

		pSce->Ifel_ctr[pSce->Sub_ctr]--;

		return EXEC_NORMAL;
	}

	static uint32_t Sleep(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sleep*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Nop);

		int8_t Sub_ctr = pSce->Sub_ctr;

		int8_t Loop_ctr = pSce->Loop_ctr[Sub_ctr] + 1;

		pSce->Loop_ctr[Sub_ctr] = Loop_ctr;

		pSce->Lcnt[Sub_ctr][Loop_ctr] = Op->Lcnt;

		return EXEC_NORMAL;
	}

	static uint32_t Sleeping(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sleeping*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		pSce->Lcnt[Sub_ctr][pSce->Loop_ctr[Sub_ctr]]--;

		if (pSce->Lcnt[Sub_ctr][pSce->Loop_ctr[Sub_ctr]] == 0)
		{
			pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sleeping);

			pSce->Loop_ctr[Sub_ctr]--;
		}

		return EXEC_YIELD;
	}

	static uint32_t Wsleep(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Wsleep*>(pSce->Data);

		pSce->Loop_ctr[pSce->Sub_ctr]++;

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Wsleep);

		return EXEC_NORMAL;
	}

	static uint32_t Wsleeping(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Wsleeping*>(pSce->Data);

		if (((G->Sce->Status_flg & STAT_XA_VOICE) == 0) && (static_cast<int32_t>(G->Sce->Mess_flg) >= 0))
		{
			pSce->Data += sizeof(Resident_Evil_2_Bytecode::Wsleeping);
			pSce->Loop_ctr[pSce->Sub_ctr]--;
		}

		return EXEC_YIELD;
	}

	static uint32_t For(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::For*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		if (Op->Lcnt != 0)
		{
			int8_t Loop_ctr = pSce->Loop_ctr[Sub_ctr] + 1;

			pSce->Loop_ctr[Sub_ctr] = Loop_ctr;

			pSce->Lcnt[Sub_ctr][Loop_ctr] = Op->Lcnt;

			pSce->Data += sizeof(Resident_Evil_2_Bytecode::For);

			pSce->Lbreak[Sub_ctr][Loop_ctr] = pSce->Data + Op->Ofs;

			pSce->Lstack[Sub_ctr][Loop_ctr] = pSce->Data;

			pSce->Loop_if_class[Sub_ctr][Loop_ctr] = pSce->Ifel_ctr[Sub_ctr];

			return EXEC_NORMAL;
		}

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::For) + Op->Ofs;

		return EXEC_NORMAL;
	}

	static uint32_t Next(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Next*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		int8_t Loop_ctr = pSce->Loop_ctr[Sub_ctr];

		pSce->Lcnt[Sub_ctr][Loop_ctr]--;

		if (pSce->Lcnt[Sub_ctr][Loop_ctr] != 0)
		{
			pSce->Data = pSce->Lstack[Sub_ctr][Loop_ctr];

			return EXEC_NORMAL;
		}

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Next);

		pSce->Loop_ctr[Sub_ctr]--;

		return EXEC_NORMAL;
	}

	static uint32_t While(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::While*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		int8_t Loop_ctr = pSce->Loop_ctr[Sub_ctr] + 1;

		pSce->Loop_ctr[Sub_ctr] = Loop_ctr;

		pSce->Lstack[Sub_ctr][Loop_ctr] = pSce->Data;

		uint8_t* pData = pSce->Data + sizeof(Resident_Evil_2_Bytecode::While);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::While);

		pSce->Lbreak[Sub_ctr][Loop_ctr] = pData + Op->Ofs;

		pSce->Loop_if_class[Sub_ctr][Loop_ctr] = pSce->Ifel_ctr[Sub_ctr];

		if (While_main(pSce, pData, Op->ConditionLen) == 0)
		{
			pSce->Data = pSce->Lbreak[Sub_ctr][Loop_ctr];

			pSce->Loop_ctr[Sub_ctr]--;
		}

		return EXEC_NORMAL;
	}

	static uint32_t Ewhile(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Ewhile*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		pSce->Data = pSce->Lstack[Sub_ctr][pSce->Loop_ctr[Sub_ctr]];

		pSce->Loop_ctr[Sub_ctr]--;

		return EXEC_NORMAL;
	}

	static uint32_t Do(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Do*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Do);

		int8_t Sub_ctr = pSce->Sub_ctr;

		int8_t Loop_ctr = pSce->Loop_ctr[Sub_ctr] + 1;

		pSce->Loop_ctr[Sub_ctr] = Loop_ctr;

		pSce->Lstack[Sub_ctr][Loop_ctr] = pSce->Data;

		pSce->Lbreak[Sub_ctr][Loop_ctr] = pSce->Data + Op->Ofs;

		pSce->Loop_if_class[Sub_ctr][Loop_ctr] = pSce->Ifel_ctr[Sub_ctr];

		return EXEC_NORMAL;
	}

	static uint32_t Edwhile(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Edwhile*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Edwhile);

		int8_t Sub_ctr = pSce->Sub_ctr;

		if (While_main(pSce, pSce->Data, Op->ConditionLen) != 0)
		{
			pSce->Data = pSce->Lstack[Sub_ctr][pSce->Loop_ctr[Sub_ctr]];

			return EXEC_NORMAL;
		}

		pSce->Loop_ctr[Sub_ctr]--;

		return EXEC_NORMAL;
	}

	static uint32_t Switch(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Switch*>(pSce->Data);

		uint8_t* pNext;

		int8_t Sub_ctr = pSce->Sub_ctr;

		int8_t* pLoop_ctr = &pSce->Loop_ctr[Sub_ctr];

		int8_t Loop_ctr = pSce->Loop_ctr[Sub_ctr] + 1;

		*pLoop_ctr = Loop_ctr;

		uint8_t* pData = pSce->Data + sizeof(Resident_Evil_2_Bytecode::Switch);

		pSce->Lbreak[Sub_ctr][Loop_ctr] = pData + Op->Ofs;

		pSce->Loop_if_class[Sub_ctr][Loop_ctr] = pSce->Ifel_ctr[Sub_ctr];

		do {

			if (*pData == 0x15)	// Default
			{
				pNext = pData + sizeof(Resident_Evil_2_Bytecode::Default);

				pSce->Data = pNext;

				return EXEC_NORMAL;
			}

			if (*pData == 0x16)	// Eswitch
			{
				*pLoop_ctr = *pLoop_ctr - 1;

				pNext = pData + sizeof(Resident_Evil_2_Bytecode::Eswitch);

				pSce->Data = pNext;

				return EXEC_NORMAL;
			}

			pNext = pData + sizeof(Resident_Evil_2_Bytecode::Case);

			if (G->Sce->SaveData.Sce_work[Op->Work_no - 4] == *(int16_t*)(pData + sizeof(Resident_Evil_2_Bytecode::Switch)))
			{
				pSce->Data = pNext;

				return EXEC_NORMAL;
			}

			pData = pNext + *(uint16_t*)(pData + 2);

		} while (true);
	}

	static uint32_t Case(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Case*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Case);

		return EXEC_NORMAL;
	}

	static uint32_t Default(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Default*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Default);

		return EXEC_NORMAL;
	}

	static uint32_t Eswitch(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Eswitch*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Eswitch);

		pSce->Loop_ctr[pSce->Sub_ctr]--;

		return EXEC_NORMAL;
	}

	static uint32_t Goto(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Goto*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		pSce->Ifel_ctr[Sub_ctr] = Op->Ifel_ctr;

		//pSce->pS_SP = pSce->Stack[Sub_ctr] + (Op->Ifel_ctr + 1u);
		pSce->pS_SP = pSce->Stack[Sub_ctr] + static_cast<std::size_t>(Op->Ifel_ctr + 1);

		pSce->Loop_ctr[Sub_ctr] = Op->Loop_ctr;

		pSce->Data += Op->Ofs;

		return EXEC_NORMAL;
	}

	static uint32_t Gosub(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Gosub*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		pSce->Ret_addr[Sub_ctr] = pSce->Data + sizeof(Resident_Evil_2_Bytecode::Gosub);

		pSce->Ifel_ctr[Sub_ctr + 1] = -1;

		pSce->Loop_ctr[Sub_ctr + 1] = -1;

		pSce->pS_SP = pSce->Stack[Sub_ctr + 1];

		//const uint16_t* pCScd = reinterpret_cast<const uint16_t*>(G->Sce->pCScd);

		pSce->Sub_ctr = Sub_ctr + 1;

		//pSce->Data = G->Sce->pCScd + pCScd[Op->Evt_no];

		// in this implementation, individual scripts are not stored in a single contiguous block
		pSce->Data = const_cast<uint8_t*>(std::get<Resident_Evil_2_Bytecode>(G->Room->Script->Loop[Op->Evt_no]).Raw().data());

		return EXEC_NORMAL;
	}

	static uint32_t Return(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Return*>(pSce->Data);
		
		int8_t Sub_ctr = static_cast<int8_t>(--pSce->Sub_ctr);

		pSce->Sub_ctr = Sub_ctr;

		pSce->Data = pSce->Ret_addr[Sub_ctr];

		//pSce->pS_SP = pSce->Stack[Sub_ctr] + pSce->Ifel_ctr[Sub_ctr] + 1;
		pSce->pS_SP = pSce->Stack[Sub_ctr] + static_cast<std::size_t>(pSce->Ifel_ctr[Sub_ctr] + 1);

		return EXEC_NORMAL;
	}

	static uint32_t Break(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Break*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		int8_t Loop_ctr = pSce->Loop_ctr[Sub_ctr];

		pSce->Data = pSce->Lbreak[Sub_ctr][Loop_ctr];

		int8_t Loop_if_class = pSce->Loop_if_class[Sub_ctr][Loop_ctr];

		pSce->Loop_ctr[Sub_ctr] = Loop_ctr - 1;

		pSce->Ifel_ctr[Sub_ctr] = Loop_if_class;

		return EXEC_NORMAL;
	}

	static uint32_t For2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::For2*>(pSce->Data);

		int8_t Sub_ctr = pSce->Sub_ctr;

		if (G->Sce->SaveData.Sce_work[Op->Flag - 4] == 0)
		{
			pSce->Data += sizeof(Resident_Evil_2_Bytecode::For2) + Op->Ofs;
		}
		else
		{
			int8_t Loop_ctr = pSce->Loop_ctr[Sub_ctr] + 1;

			pSce->Loop_ctr[Sub_ctr] = Loop_ctr;

			pSce->Lcnt[Sub_ctr][Loop_ctr] = G->Sce->SaveData.Sce_work[Op->Flag - 4];

			pSce->Data += sizeof(Resident_Evil_2_Bytecode::For2);

			pSce->Lbreak[Sub_ctr][Loop_ctr] = pSce->Data + Op->Ofs;

			pSce->Lstack[Sub_ctr][Loop_ctr] = pSce->Data;

			pSce->Loop_if_class[Sub_ctr][Loop_ctr] = pSce->Ifel_ctr[Sub_ctr];
		}

		return EXEC_NORMAL;
	}

	static uint32_t Break_point(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Break_point*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Break_point);

		try
		{
			throw std::runtime_error("Resident Evil 2 Instruction: Break_point");
		}
		catch (...)
		{
			StdExCall().Exception(std::current_exception(), false, std::source_location::current());
		}

		return EXEC_NORMAL;
	}

	static uint32_t Work_copy(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Work_copy*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Work_copy);

		if (Op->DataType == 0)
		{
			uint8_t Value = static_cast<uint8_t>(G->Sce->SaveData.Sce_work[Op->Work_no - 4]);
			std::memcpy(&pSce->Data[Op->Work_dest], &Value, sizeof(uint8_t));
		}
		else
		{
			int16_t Value = static_cast<int16_t>(G->Sce->SaveData.Sce_work[Op->Work_no - 4]);
			std::memcpy(&pSce->Data[Op->Work_dest], &Value, sizeof(int16_t));
		}

		return EXEC_NORMAL;
	}

	static uint32_t Ck(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Ck*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Ck);

		uint32_t* pFlag = Flg_addr(Op->Flag);

		return static_cast<uint32_t>((Flg_ck(pFlag, Op->Bit) != 0) != (Op->OnOff == 0));
	}

	static uint32_t Set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Set);

		uint32_t* pFlag = Flg_addr(Op->Flag) + (Op->Bit >> 5);

		uint32_t Mask = 0x80000000U >> (Op->Bit & 0x1F);

		if (Op->Operator == 0)
		{
			*pFlag &= ~Mask;
		}
		else if (Op->Operator == 1)
		{
			*pFlag |= Mask;
		}
		else if (Op->Operator == 7)
		{
			*pFlag ^= Mask;
		}

		return EXEC_NORMAL;
	}

	static uint32_t Cmp(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Cmp*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Cmp);

		int16_t Value = static_cast<int16_t>(G->Sce->SaveData.Sce_work[Op->Flag - 4]);

		switch (Op->Operator)
		{
		case CMP_EQ: return static_cast<uint32_t>(Value == Op->Num);
		case CMP_GT: return static_cast<uint32_t>(Value > Op->Num);
		case CMP_GE: return static_cast<uint32_t>(Value >= Op->Num);
		case CMP_LT: return static_cast<uint32_t>(Value < Op->Num);
		case CMP_LE: return static_cast<uint32_t>(Value <= Op->Num);
		case CMP_NE: return static_cast<uint32_t>(Value != Op->Num);
		case CMP_AND: return static_cast<uint32_t>((static_cast<uint16_t>(Value) & static_cast<uint16_t>(Op->Num)) != 0);
		}

		return static_cast<uint32_t>(G->Sce->SaveData.Sce_work[Op->Flag - 4]);
	}

	static uint32_t Save(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Save*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Save);

		G->Sce->SaveData.Sce_work[Op->Flag - 4] = Op->Num;

		return EXEC_NORMAL;
	}

	static uint32_t Copy(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Copy*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Copy);

		G->Sce->SaveData.Sce_work[Op->Flag0 - 4] = G->Sce->SaveData.Sce_work[Op->Flag1 - 4];

		return EXEC_NORMAL;
	}

	static uint32_t Calc(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Calc*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Calc);

		Calc_branch(Op->Operator, &G->Sce->SaveData.Sce_work[Op->Flag - 4], Op->Num);

		return EXEC_NORMAL;
	}

	static uint32_t Calc2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Calc2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Calc2);

		Calc_branch(Op->Operator, &G->Sce->SaveData.Sce_work[Op->Flag0 - 4], G->Sce->SaveData.Sce_work[Op->Flag1 - 4]);

		return EXEC_NORMAL;
	}

	static uint32_t Sce_rnd(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_rnd*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_rnd);

		Sce_rnd_set();

		return EXEC_NORMAL;
	}

	static uint32_t Cut_chg(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Cut_chg*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Cut_chg);

		G->Sce->Status_flg |= STAT_CAM_LOCK;

		Cut_chg_main(Op->Cut_no);

		G->Sce->Status_flg |= SYS_PSP_TRANS;

		//if ((Op->Cut_no & 0x80) != 0)
		//{
		//	Main.Bg_chg_flg = 0;
		//}

		return EXEC_NORMAL;
	}

	static uint32_t Cut_old(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Cut_old*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Cut_old);

		Cut_chg_main(G->Sce->Cut_old);

		G->Sce->Status_flg = G->Sce->Status_flg & 0xfffffeff | SYS_PSP_TRANS;

		return EXEC_NORMAL;
	}

	static uint32_t Message_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Message_on*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Message_on);

		G->Sce->Stop_bak = G->Sce->Stop_flg;

		G->Sce->Stop_flg |= (Op->Stop_data << 16);

		Mess_set(0, Op->Attr | 0x300, Op->Mess_no, Op->Stop_data << 16);

		return EXEC_NORMAL;
	}

	static uint32_t Aot_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Aot_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Aot_set);

		if (G->Sce->Sce_aot[Op->Aot].Id == SCE_NOTHING)
		{
			G->Sce->nSce_aot++;
		}

		G->Sce->Sce_aot[Op->Aot].Is4P = false;
		G->Sce->Sce_aot[Op->Aot].Id = Op->Id;
		G->Sce->Sce_aot[Op->Aot].Type = Op->Type;
		G->Sce->Sce_aot[Op->Aot].nFloor = Op->nFloor;
		G->Sce->Sce_aot[Op->Aot].Super = Op->Super;
		G->Sce->Sce_aot[Op->Aot].X = Op->X;
		G->Sce->Sce_aot[Op->Aot].Z = Op->Z;
		G->Sce->Sce_aot[Op->Aot].W = Op->W;
		G->Sce->Sce_aot[Op->Aot].D = Op->D;
		G->Sce->Sce_aot[Op->Aot].Data0 = Op->Data0;
		G->Sce->Sce_aot[Op->Aot].Data1 = Op->Data1;
		G->Sce->Sce_aot[Op->Aot].Data2 = Op->Data2;

		return EXEC_NORMAL;
	}

	static uint32_t Obj_model_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Obj_model_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Obj_model_set);

		if (!G->Room->Object[Op->Om_no])
		{
			std::cout << "ObjModelSet: " << (int)Op->Om_no << " is empty" << std::endl;
			return EXEC_NORMAL;
		}

		auto& Object = G->Room->Object[Op->Om_no];

		Object->m_ObjectID = Op->Id;
		//Object->Cc_ctr = Cc_ctr;
		//Object->Cc_wait = Cc_wait;
		//Object->Cc_num = Cc_num;
		//Object->nFloor = nFloor;
		//Object->pSuper = Super;
		//Object->Type = Type;
		Object->b_Active = Op->Be_flg & OM_ENABLED ? true : false;	// TODO - parse Be_flg flags
		//Object->Attribute = Attribute;

		Object->Position().x = Op->Pos_x;// + Op->Ofs_x;
		Object->Position().y = Op->Pos_y;// + Op->Ofs_y;
		Object->Position().z = Op->Pos_z;// + Op->Ofs_z;

		Object->Rotation().x = Op->Cdir_x;
		Object->Rotation().y = Op->Cdir_y;
		Object->Rotation().z = Op->Cdir_z;

		Object->Hitbox().w = Op->At_w;
		Object->Hitbox().h = Op->At_h;
		Object->Hitbox().d = Op->At_d;

		return EXEC_NORMAL;
	}

	static uint32_t Work_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Work_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Work_set);

		pSce->Speed = { 0, 0, 0 };
		pSce->DSpeed = { 0, 0, 0 };
		pSce->ASpeed = { 0, 0, 0 };
		pSce->ADSpeed = { 0, 0, 0 };

		switch (Op->Work_type)
		{
		case WORK_TYPE_PLAYER: pSce->pWork = G->Player; break;
		case WORK_TYPE_SUB_PLAYER: pSce->pWork = G->SubPlayer; break;
		case WORK_TYPE_ENEMY:
		{
			if (Op->Work_no >= G->Enemy.size())
			{
				std::cout << "WorkSet: Enemy " << (int)Op->Work_no << " is out of range" << std::endl;
			}
			else
			{
				pSce->pWork = G->Enemy[Op->Work_no];
			}
			break;
		}
		case WORK_TYPE_OBJECT:
		{
			if (Op->Work_no >= G->Room->Object.size())
			{
				std::cout << "WorkSet: Object " << (int)Op->Work_no << " is out of range" << std::endl;
			}
			else
			{
				pSce->pWork = G->Room->Object[Op->Work_no];
			}
			break;
		}
		case WORK_TYPE_DOOR:
		{
			if (Op->Work_no >= G->Door.size())
			{
				std::cout << "WorkSet: Door " << (int)Op->Work_no << " is out of range" << std::endl;
			}
			else
			{
				pSce->pWork = G->Door[Op->Work_no];
			}
			break;
		}
		}

		return EXEC_NORMAL;
	}

	static uint32_t Speed_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Speed_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Speed_set);

		switch (Op->Spd_type)
		{
		case SPEED_SPD_X: pSce->Speed.x = Op->Num; break;
		case SPEED_SPD_Y: pSce->Speed.y = Op->Num; break;
		case SPEED_SPD_Z: pSce->Speed.z = Op->Num; break;
		case SPEED_DSPD_X: pSce->DSpeed.x = Op->Num; break;
		case SPEED_DSPD_Y: pSce->DSpeed.y = Op->Num; break;
		case SPEED_DSPD_Z: pSce->DSpeed.z = Op->Num; break;
		case SPEED_ASPD_X: pSce->ASpeed.x = Op->Num; break;
		case SPEED_ASPD_Y: pSce->ASpeed.y = Op->Num; break;
		case SPEED_ASPD_Z: pSce->ASpeed.z = Op->Num; break;
		case SPEED_ADSPD_X: pSce->ADSpeed.x = Op->Num; break;
		case SPEED_ADSPD_Y: pSce->ADSpeed.y = Op->Num; break;
		case SPEED_ADSPD_Z: pSce->ADSpeed.z = Op->Num; break;
		}

		return EXEC_NORMAL;
	}

	static uint32_t Add_speed(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Add_speed*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Add_speed);

		pSce->pWork->Position().x += pSce->Speed.x;
		pSce->pWork->Position().y += pSce->Speed.y;
		pSce->pWork->Position().z += pSce->Speed.z;
		pSce->pWork->ClampPosition(pSce->pWork->Position());

		pSce->pWork->Rotation().x += pSce->DSpeed.x;
		pSce->pWork->Rotation().y += pSce->DSpeed.y;
		pSce->pWork->Rotation().z += pSce->DSpeed.z;
		pSce->pWork->ClampRotation(pSce->pWork->Rotation());

		return EXEC_NORMAL;
	}

	static uint32_t Add_aspeed(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Add_aspeed*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Add_aspeed);

		pSce->Speed.x += pSce->ASpeed.x;
		pSce->Speed.y += pSce->ASpeed.y;
		pSce->Speed.z += pSce->ASpeed.z;

		pSce->DSpeed.x += pSce->ADSpeed.x;
		pSce->DSpeed.y += pSce->ADSpeed.y;
		pSce->DSpeed.z += pSce->ADSpeed.z;

		return EXEC_NORMAL;
	}

	static uint32_t Pos_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Pos_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Pos_set);

		pSce->pWork->Position().x = Op->Pos_x;
		pSce->pWork->Position().y = Op->Pos_y;
		pSce->pWork->Position().z = Op->Pos_z;

		return EXEC_NORMAL;
	}

	static uint32_t Dir_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Dir_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Dir_set);

		pSce->pWork->Rotation().x = Op->Cdir_x;
		pSce->pWork->Rotation().y = Op->Cdir_y;
		pSce->pWork->Rotation().z = Op->Cdir_z;

		return EXEC_NORMAL;
	}

	static uint32_t Member_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Member_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Member_set);

		Member_set_branch(pSce->pWork, Op->Member, Op->Num);

		return EXEC_NORMAL;
	}

	static uint32_t Member_set2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Member_set2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Member_set2);

		Member_set_branch(pSce->pWork, Op->Member, G->Sce->SaveData.Sce_work[Op->Flag - 4]);

		return EXEC_NORMAL;
	}

	static uint32_t Se_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Se_on*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Se_on);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sca_id_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sca_id_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sca_id_set);

		auto Collision = G->Room->Sca->Get(Op->Col_no);

		Collision->Id.Data = static_cast<uint16_t>((static_cast<uint8_t>(Collision->Id.Data) ^ static_cast<uint8_t>(Op->Id)) & 0x7Fu) ^ Op->Id;

		return EXEC_NORMAL;
	}

	static uint32_t Flr_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Flr_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Flr_set);

		if (Op->OnOff)
		{
			G->Room->Flr->Get(Op->Flr_no)->Se_no &= 0xff7f;
		}
		else
		{
			G->Room->Flr->Get(Op->Flr_no)->Se_no |= 0x80;
		}

		return EXEC_NORMAL;
	}

	static uint32_t Dir_ck(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Dir_ck*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Dir_ck);

		return static_cast<uint32_t>(Goto00_ck(pSce->pWork, Op->Vec_x, Op->Vec_z, Op->Add_dir) == 0);
	}

	static uint32_t Sce_espr_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_espr_on*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_espr_on);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Door_aot_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Door_aot_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Door_aot_set);

		if (G->Sce->Sce_aot[Op->Aot].Id == SCE_NOTHING)
		{
			G->Sce->nSce_aot++;
		}

		auto& Door = G->Sce->Sce_aot[Op->Aot];

		Door.Is4P = false;
		Door.Id = Op->Id;
		Door.Type = Op->Type;
		Door.nFloor = Op->nFloor;
		Door.Super = Op->Super;
		Door.X = Op->X;
		Door.Z = Op->Z;
		Door.W = Op->W;
		Door.D = Op->D;

		Door.Next_pos_x = Op->Next_pos_x;
		Door.Next_pos_y = Op->Next_pos_y;
		Door.Next_pos_z = Op->Next_pos_z;
		Door.Next_cdir_y = Op->Next_cdir_y;

		Door.Next_stage = Op->Next_stage;
		Door.Next_room = Op->Next_room;
		Door.Next_cut = Op->Next_cut;
		Door.Next_nfloor = Op->Next_nfloor;

		Door.Dtex_type = Op->Dtex_type;
		Door.Door_type = Op->Door_type;
		Door.Knock_type = Op->Knock_type;
		Door.Key_id = Op->Key_id;
		Door.Key_type = Op->Key_type;
		Door.Free = Op->Free;

		return EXEC_NORMAL;
	}

	static uint32_t Cut_auto(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Cut_auto*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Cut_auto);

		if (Op->OnOff)
		{
			G->Sce->Status_flg &= STAT_CAM_LOCK;
		}
		else
		{
			G->Sce->Status_flg |= STAT_CAM_LOCK;
		}

		return EXEC_NORMAL;
	}

	static uint32_t Member_copy(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Member_copy*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Member_copy);

		G->Sce->SaveData.Sce_work[Op->Flag - 4] = static_cast<int16_t>(Load_member_branch(pSce->pWork, Op->Member));

		return EXEC_NORMAL;
	}

	static uint32_t Member_cmp(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Member_cmp*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Member_cmp);

		const int32_t Value = Load_member_branch(pSce->pWork, Op->Member);

		switch (Op->Operator)
		{
		case CMP_EQ: return static_cast<uint32_t>(Value == Op->Num);
		case CMP_GT: return static_cast<uint32_t>(Value > Op->Num);
		case CMP_GE: return static_cast<uint32_t>(Value >= Op->Num);
		case CMP_LT: return static_cast<uint32_t>(Value < Op->Num);
		case CMP_LE: return static_cast<uint32_t>(Value <= Op->Num);
		case CMP_NE: return static_cast<uint32_t>(Value != Op->Num);
		case CMP_AND: return static_cast<uint32_t>((static_cast<uint32_t>(Value) & static_cast<uint32_t>(Op->Num)) != 0);
		}

		return static_cast<uint32_t>(Op->Operator < 7);
	}

	static uint32_t Plc_motion(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_motion*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_motion);

		//pSce->pWork->Move_cnt = 0;
		pSce->pWork->ResetClip();

		// TODO
		uint32_t Routine = (static_cast<uint32_t>(Op->Routine_0) << 8) | 4u;
		pSce->pWork->Routine_0 = static_cast<uint8_t>(Routine & 0xFF);
		pSce->pWork->Routine_1 = static_cast<uint8_t>((Routine >> 8) & 0xFF);
		pSce->pWork->Routine_2 = static_cast<uint8_t>((Routine >> 16) & 0xFF);
		pSce->pWork->Routine_3 = static_cast<uint8_t>((Routine >> 24) & 0xFF);

		std::cout << "PlcMotion: " << std::endl;
		std::wcout << L"\tFile: " << pSce->pWork->Filename().stem().wstring() << std::endl;
		std::cout << "\tRoutine: " << (int)pSce->pWork->Routine_0 << "," << (int)pSce->pWork->Routine_1 << "," << (int)pSce->pWork->Routine_2 << "," << (int)pSce->pWork->Routine_3 << std::endl;
		std::cout << "\tMove_no: " << (int)Op->Move_no << std::endl;
		std::cout << "\tSce_flg: " << (int)Op->Sce_flg << std::endl;
		std::cout << "\t\t";
		for (int i = 0; i < 16; i++)
		{
			if (Op->Sce_flg & (1 << (15 - i)))
			{
				std::cout << "1";
			}
			else
			{
				std::cout << "0";
			}
		}
		std::cout << std::endl << std::endl;

		if (pSce->pWork->Routine_0 == 1)
		{
			pSce->pWork->SetAnimIndex(AnimationIndex::Weapon);
		}
		else if (pSce->pWork->Routine_0 == 4)
		{
			pSce->pWork->SetAnimIndex(AnimationIndex::Room);
			pSce->pWork->b_DrawWeapon = false;
		}

		if (pSce->pWork->Animation(pSce->pWork->AnimIndex())->IsOpen() && Op->Move_no < pSce->pWork->Animation(pSce->pWork->AnimIndex())->Clip.size())
		{
			pSce->pWork->iClip.store(Op->Move_no);
		}

		pSce->pWork->Sce_flg = static_cast<uint16_t>(Op->Sce_flg);

		pSce->pWork->Sce_free0 = 0;
		pSce->pWork->Sce_free1 = 0;

		//
		//G->Player->State()->b_Sleep = true;

		return EXEC_NORMAL;
	}

	static uint32_t Plc_dest(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_dest*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_dest);

		// TODO

		std::cout << "Plc_dest: " << std::endl;
		std::wcout << L"\tFile: " << pSce->pWork->Filename().stem().wstring() << std::endl;

		if (((pSce->pWork->Sce_flg & 4) == 0) || (pSce->pWork->Routine_1 != Op->Routine_0))
		{
			uint32_t Routine = (static_cast<uint32_t>(Op->Routine_0) << 8) | 4u;
			pSce->pWork->Routine_0 = static_cast<uint8_t>(Routine & 0xFF);
			pSce->pWork->Routine_1 = static_cast<uint8_t>((Routine >> 8) & 0xFF);
			pSce->pWork->Routine_2 = static_cast<uint8_t>((Routine >> 16) & 0xFF);
			pSce->pWork->Routine_3 = static_cast<uint8_t>((Routine >> 24) & 0xFF);
			std::cout << "\tRoutine " << (int)pSce->pWork->Routine_0 << "," << (int)pSce->pWork->Routine_1 << "," << (int)pSce->pWork->Routine_2 << "," << (int)pSce->pWork->Routine_3 << std::endl;
		}

		std::cout << "\tDest_x: " << Op->Dest_x << std::endl;
		std::cout << "\tDest_z: " << Op->Dest_z << std::endl;
		std::cout << "\tSet_flg_no: " << (int)Op->Set_flg_no << std::endl;
		std::cout << "\t\t";
		for (int i = 0; i < 16; i++)
		{
			if (Op->Set_flg_no & (1 << (15 - i)))
			{
				std::cout << "1";
			}
			else
			{
				std::cout << "0";
			}
		}
		std::cout << std::endl << std::endl;

		auto Id = pSce->pWork->FileID();
		size_t idx = 0;

		if (Id < 0x10)
		{
			idx = static_cast<size_t>(Id) + 0x10;
		}
		else
		{
			idx = static_cast<size_t>(Id) - 0x40;
		}

		/*switch (Op->Routine_0)
		{
		case 4:
		case 0x12: pSce->pWork->Sce_free0 = static_cast<uint16_t>(Subpl_walk[idx][1]); break;
		case 5: pSce->pWork->Sce_free0 = static_cast<uint16_t>(Subpl_run[idx][1]); break;
		case 7: pSce->pWork->Sce_free0 = static_cast<uint16_t>(Subpl_back[idx][1]); break;
		case 8: pSce->pWork->Sce_free0 = static_cast<uint16_t>(Subpl_ato[idx][1]); break;
		case 9: pSce->pWork->Sce_free0 = static_cast<uint16_t>(Subpl_dir[idx][1]); break;
		}*/

		pSce->pWork->Sce_flg = 0;
		pSce->pWork->Sce_free1 = 0;
		pSce->pWork->Set_flg_no = Op->Set_flg_no;
		pSce->pWork->Dest_x = Op->Dest_x;
		pSce->pWork->Dest_z = Op->Dest_z;
		pSce->pWork->Sce_free2 = pSce->pWork->Dest_x;
		pSce->pWork->Sce_free3 = pSce->pWork->Dest_z;

		return EXEC_NORMAL;
	}

	static uint32_t Plc_neck(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_neck*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_neck);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Plc_ret(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_ret*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_ret);

		// TODO

		pSce->pWork->Routine_0 = 1;
		pSce->pWork->Routine_1 = 0;
		pSce->pWork->Routine_2 = 0;
		pSce->pWork->Routine_3 = 0;

		//if (pSce->pWork->Id < 0x10)
		{
		//	pSce->pWork->Type = pSce->pWork->Type & 0xfff;
		}

		//pSce->pWork->Neck_flg = PLC_NECK_FRONT;

		//pSce->pWork->Type = pSce->pWork->Type & 0xafff;

		G->Player->State()->b_Sleep = false;

		pSce->pWork->SetAnimIndex(AnimationIndex::Weapon);

		G->Player->State()->Set(Idle);

		pSce->pWork->b_DrawWeapon = true;

		return EXEC_NORMAL;
	}

	static uint32_t Plc_flg(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_flg*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_flg);

		if (pSce->pWork)
		{
			switch (Op->Operator)
			{
			case 0: pSce->pWork->Sce_flg |= Op->Num; break;
			case 1: pSce->pWork->Sce_flg = Op->Num; break;
			case 2: pSce->pWork->Sce_flg ^= Op->Num; break;
			}
		}

		return EXEC_NORMAL;
	}

	static uint32_t Sce_em_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_em_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_em_set);

		// TODO

		G->Enemy.push_back(std::make_shared<Resident_Evil_Model>());

		auto& Enemy = G->Enemy.back();

		Enemy->SetGame(Video_Game::Resident_Evil_2);
		Enemy->PlatformSetup(G->Window->Get(), G->GTE, G->Render, G->Camera->b_HorzFlip, G->Camera->b_VertFlip);

		if (Standard_FileSystem().Exists(Standard_String().FormatCStyle(L"%ws\\em0%02x.emd", G->m_EnemyPath.wstring().c_str(), Op->Id)))
		{
			Enemy->Open(Standard_String().FormatCStyle(L"%ws\\em0%02x.emd", G->m_EnemyPath.wstring().c_str(), Op->Id));
		}

		Enemy->m_EnemyID = Op->Id;
		//Enemy->Type = Op->Type;
		//Enemy->nFloor = Op->nFloor;
		//Enemy->Sound_flg = Op->Sound_flg;
		//Enemy->Model_type = Op->Model_type;
		//Enemy->Em_set_flg = Op->Em_set_flg;
		//std::cout << "Id " << (int)Op->Id << " Model_type " << (int)Op->Model_type << std::endl;

		Enemy->Position().x = Op->Pos_x;
		Enemy->Position().y = Op->Pos_y;
		Enemy->Position().z = Op->Pos_z;
		Enemy->Rotation().y = Op->Cdir_y;

		//Enemy->Motion = Op->Motion;
		//Enemy->Ctr_flg = Op->Ctr_flg;
		//std::cout << "Id " << (int)Op->Id << " Motion " << (int)Op->Motion << std::endl;

		return EXEC_NORMAL;
	}

	static uint32_t Col_chg_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Col_chg_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Col_chg_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Aot_reset(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Aot_reset*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Aot_reset);

		G->Sce->Sce_aot[Op->Aot].Is4P = false;
		G->Sce->Sce_aot[Op->Aot].Id = Op->Id;
		G->Sce->Sce_aot[Op->Aot].Type = static_cast<uint8_t>(Op->Type | (G->Sce->Sce_aot[Op->Aot].Type & 0x80));
		G->Sce->Sce_aot[Op->Aot].Data0 = Op->Data0;
		G->Sce->Sce_aot[Op->Aot].Data1 = Op->Data1;
		G->Sce->Sce_aot[Op->Aot].Data2 = Op->Data2;

		return EXEC_NORMAL;
	}

	static uint32_t Aot_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Aot_on*>(pSce->Data);

		G->Sce->pCAot = &G->Sce->Sce_aot[Op->Aot];

		auto Id = G->Sce->pCAot->Id;

		Sce_at_jump_tbl[G->Sce->pCAot->Id].func(*G->Sce->pCAot);

		// hack: if this is a door, immediately return EXEC_YIELD and halt script execution
		if (Id == SCE_DOOR)
		{
			pSce->Status = STATUS_READY;
			return EXEC_YIELD;
		}

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Aot_on);

		return EXEC_NORMAL;
	}

	static uint32_t Super_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Super_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Super_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Super_reset(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Super_reset*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Super_reset);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Plc_gun(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_gun*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_gun);

		uint32_t Routine = (static_cast<uint32_t>(Op->Routine_0) << 8) | 4u;

		// TODO
		//pSce->pWork->Routine_0 = static_cast<uint8_t>(Routine & 0xFF);
		//pSce->pWork->Routine_1 = static_cast<uint8_t>((Routine >> 8) & 0xFF);
		//pSce->pWork->Routine_2 = static_cast<uint8_t>((Routine >> 16) & 0xFF);
		//pSce->pWork->Routine_3 = static_cast<uint8_t>((Routine >> 24) & 0xFF);

		pSce->pWork->Sce_free0 = 0;
		pSce->pWork->Sce_free1 = 0;

		return EXEC_NORMAL;
	}

	static uint32_t Cut_replace(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Cut_replace*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Cut_replace);

		const uint8_t Fcut = Op->Fcut;
		const uint8_t Tcut = Op->Tcut;

		auto& Rvd = G->Room->Rvd->data();

		for (size_t i = 0; i < Rvd.size(); ++i)
		{
			auto& entry = Rvd[i];

			if (entry.Fcut == Fcut) entry.Fcut = Tcut;
			else if (entry.Fcut == Tcut) entry.Fcut = Fcut;

			if (entry.Tcut == Fcut) entry.Tcut = Tcut;
			else if (entry.Tcut == Tcut) entry.Tcut = Fcut;
		}

		if (G->Sce && G->Sce->Cccut_next)
		{
			const auto ptr = reinterpret_cast<std::uintptr_t>(G->Sce->Cccut_next);
			const auto base = reinterpret_cast<std::uintptr_t>(Rvd.data());
			const auto totalBytes = Rvd.size() * sizeof(Rvd[0]);

			if (ptr >= base && ptr < base + totalBytes)
			{
				const size_t idx = (ptr - base) / sizeof(Rvd[0]);
				if (idx < Rvd.size())
				{
					auto& nextEntry = Rvd[idx];
					if (nextEntry.Fcut == Fcut)
					{
						Cut_change(Tcut);
					}
				}
			}
		}

		return EXEC_NORMAL;
	}

	static uint32_t Sce_espr_kill(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_espr_kill*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_espr_kill);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Door_model_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Door_model_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Door_model_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Item_aot_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Item_aot_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Item_aot_set);

		auto& Item = G->Sce->Sce_aot[Op->Aot];

		Item.Is4P = false;

		Item.Id = Op->Id;
		Item.Type = Op->Type;
		Item.nFloor = Op->nFloor;
		Item.Super = Op->Super;
		Item.X = Op->X;
		Item.Z = Op->Z;
		Item.W = Op->W;
		Item.D = Op->D;
		Item.iItem = Op->iItem;
		Item.nItem = Op->nItem;
		Item.Item_flg = Op->Item_flg;
		Item.Om_no = Op->Om_no;
		Item.Action = Op->Action;

		return EXEC_NORMAL;
	}

	static uint32_t Sce_key_ck(Scenario::Task* pSce)
	{
		auto* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_key_ck*>(pSce->Data);

		uint16_t Mask = static_cast<uint16_t>(pSce->Data[2]) | (static_cast<uint16_t>(pSce->Data[3]) << 8);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_key_ck);

		uint32_t Flag = Op->Flag;

		uint32_t KeyState = std::to_underlying(G->Player->State()->KeyState());

		if ((KeyState & Mask) == 0)
		{
			Flag ^= 1u;
		}

		return Flag;
	}

	static uint32_t Sce_trg_ck(Scenario::Task* pSce)
	{
		auto* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_trg_ck*>(pSce->Data);

		uint16_t Mask = static_cast<uint16_t>(pSce->Data[2]) | (static_cast<uint16_t>(pSce->Data[3]) << 8);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_trg_ck);

		uint32_t Flag = Op->Flag;

		uint32_t KeyState = std::to_underlying(G->Player->State()->KeyStateTrigger());

		if ((KeyState & Mask) == 0)
		{
			Flag ^= 1u;
		}

		return Flag;
	}

	static uint32_t Sce_bgm_control(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_bgm_control*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_bgm_control);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_espr_control(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_espr_control*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_espr_control);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_fade_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_fade_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_fade_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_espr3d_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_espr3d_on*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_espr3d_on);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Member_calc(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Member_calc*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Member_calc);

		int16_t& addr = Load_member_addr_branch(pSce->pWork, Op->Member);

		Calc_branch(Op->Operator, &addr, Op->Num);

		return EXEC_NORMAL;
	}

	static uint32_t Member_calc2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Member_calc2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Member_calc2);

		int16_t& addr = Load_member_addr_branch(pSce->pWork, Op->Member);

		Calc_branch(Op->Operator, &addr, G->Sce->SaveData.Sce_work[Op->Work_no - 4]);

		return EXEC_NORMAL;
	}

	static uint32_t Sce_bgmtbl_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_bgmtbl_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_bgmtbl_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Plc_rot(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_rot*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_rot);

		if (Op->Free_no == 0)
		{
			pSce->pWork->Sce_free0 = Op->Sce_free;
		}
		else if (Op->Free_no == 1)
		{
			pSce->pWork->Sce_free1 = Op->Sce_free;
		}

		return EXEC_NORMAL;
	}

	static uint32_t Xa_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Xa_on*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Xa_on);

		// TODO

		//G->Sce->Status_flg |= 0x20;

		return EXEC_NORMAL;
	}

	static uint32_t Weapon_chg(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Weapon_chg*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Weapon_chg);

		// TODO

		if (Standard_FileSystem().Exists(Standard_String().FormatCStyle(L"%ws\\pl%02xW%02x.plw", G->m_PlayerPath.wstring().c_str(), G->Player->m_PlayerID, Op->Plw_no)))
		{
			G->Player->Open(Standard_String().FormatCStyle(L"%ws\\pl%02xW%02x.plw", G->m_PlayerPath.wstring().c_str(), G->Player->m_PlayerID, Op->Plw_no));
		}

		return EXEC_NORMAL;
	}

	static uint32_t Plc_cnt(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_cnt*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_cnt);

		// TODO
		//pSce->pWork->Move_cnt = Op->Move_cnt;

		return EXEC_NORMAL;
	}

	static uint32_t Sce_shake_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_shake_on*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_shake_on);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Mizu_div_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Mizu_div_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Mizu_div_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Keep_Item_ck(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Keep_Item_ck*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Keep_Item_ck);

		return static_cast<uint32_t>(Search_item_id(Op->Item_no) >= 0);
	}

	static uint32_t Xa_vol(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Xa_vol*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Xa_vol);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Kage_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Kage_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Kage_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Cut_be_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Cut_be_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Cut_be_set);

		/*auto& Rvd = G->Room->Rvd->data();

		size_t baseIdx = Rvd.size();

		for (size_t i = 0; i < Rvd.size(); ++i)
		{
			if (Rvd[i].Fcut == Op->Fcut)
			{
				baseIdx = i;
				break;
			}
		}

		const size_t target = baseIdx + static_cast<size_t>(Op->Vcut_num);

		Rvd[target].Be_flg = Op->Be_flg;

		return EXEC_NORMAL;*/

		for (size_t i = 0; i < G->Room->Rvd->data().size(); ++i)
		{
			if (G->Room->Rvd->Get(i)->Fcut == Op->Fcut)
			{
				if (i + Op->Vcut_num < G->Room->Rvd->data().size())
				{
					G->Room->Rvd->Get(i + Op->Vcut_num)->Be_flg = Op->Be_flg;
				}
				break;
			}
		}

		return EXEC_NORMAL;
	}

	static uint32_t Sce_Item_lost(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_Item_lost*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_Item_lost);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Plc_gun_eff(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_gun_eff*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_gun_eff);

		return EXEC_NORMAL;
	}

	static uint32_t Sce_espr_on2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_espr_on2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_espr_on2);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_espr_kill2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_espr_kill2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_espr_kill2);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Plc_stop(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_stop*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_stop);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Aot_set_4p(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Aot_set_4p*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Aot_set_4p);

		G->Sce->Sce_aot[Op->Aot].Is4P = true;
		G->Sce->Sce_aot[Op->Aot].Id = Op->Id;
		G->Sce->Sce_aot[Op->Aot].Type = Op->Type;
		G->Sce->Sce_aot[Op->Aot].nFloor = Op->nFloor;
		G->Sce->Sce_aot[Op->Aot].Super = Op->Super;
		for (size_t i = 0; i < 4; i++)
		{
			G->Sce->Sce_aot[Op->Aot].Xz[i][0] = Op->Xz[i][0];
			G->Sce->Sce_aot[Op->Aot].Xz[i][1] = Op->Xz[i][1];
		}
		G->Sce->Sce_aot[Op->Aot].Data0 = Op->Data0;
		G->Sce->Sce_aot[Op->Aot].Data1 = Op->Data1;
		G->Sce->Sce_aot[Op->Aot].Data2 = Op->Data2;

		return EXEC_NORMAL;
	}

	static uint32_t Door_aot_set_4p(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Door_aot_set_4p*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Door_aot_set_4p);

		G->Sce->Sce_aot[Op->Aot].Is4P = true;
		G->Sce->Sce_aot[Op->Aot].Id = Op->Id;
		G->Sce->Sce_aot[Op->Aot].Type = Op->Type;
		G->Sce->Sce_aot[Op->Aot].nFloor = Op->nFloor;
		G->Sce->Sce_aot[Op->Aot].Super = Op->Super;
		for (size_t i = 0; i < 4; i++)
		{
			G->Sce->Sce_aot[Op->Aot].Xz[i][0] = Op->Xz[i][0];
			G->Sce->Sce_aot[Op->Aot].Xz[i][1] = Op->Xz[i][1];
		}
		G->Sce->Sce_aot[Op->Aot].Next_pos_x = Op->Next_pos_x;
		G->Sce->Sce_aot[Op->Aot].Next_pos_y = Op->Next_pos_y;
		G->Sce->Sce_aot[Op->Aot].Next_pos_z = Op->Next_pos_z;
		G->Sce->Sce_aot[Op->Aot].Next_cdir_y = Op->Next_cdir_y;
		G->Sce->Sce_aot[Op->Aot].Next_stage = Op->Next_stage;
		G->Sce->Sce_aot[Op->Aot].Next_room = Op->Next_room;
		G->Sce->Sce_aot[Op->Aot].Next_cut = Op->Next_cut;
		G->Sce->Sce_aot[Op->Aot].Next_nfloor = Op->Next_nfloor;
		G->Sce->Sce_aot[Op->Aot].Dtex_type = Op->Dtex_type;
		G->Sce->Sce_aot[Op->Aot].Door_type = Op->Door_type;
		G->Sce->Sce_aot[Op->Aot].Knock_type = Op->Knock_type;
		G->Sce->Sce_aot[Op->Aot].Key_id = Op->Key_id;
		G->Sce->Sce_aot[Op->Aot].Key_type = Op->Key_type;

		return EXEC_NORMAL;
	}

	static uint32_t Item_aot_set_4p(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Item_aot_set_4p*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Item_aot_set_4p);

		G->Sce->Sce_aot[Op->Aot].Is4P = true;
		G->Sce->Sce_aot[Op->Aot].Id = Op->Id;
		G->Sce->Sce_aot[Op->Aot].Type = Op->Type;
		G->Sce->Sce_aot[Op->Aot].nFloor = Op->nFloor;
		G->Sce->Sce_aot[Op->Aot].Super = Op->Super;
		for (size_t i = 0; i < 4; i++)
		{
			G->Sce->Sce_aot[Op->Aot].Xz[i][0] = Op->Xz[i][0];
			G->Sce->Sce_aot[Op->Aot].Xz[i][1] = Op->Xz[i][1];
		}
		G->Sce->Sce_aot[Op->Aot].iItem = Op->iItem;
		G->Sce->Sce_aot[Op->Aot].nItem = Op->nItem;
		G->Sce->Sce_aot[Op->Aot].Item_flg = Op->Item_flg;
		G->Sce->Sce_aot[Op->Aot].Om_no = Op->Om_no;
		G->Sce->Sce_aot[Op->Aot].Action = Op->Action;

		return EXEC_NORMAL;
	}

	static uint32_t Light_pos_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Light_pos_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Light_pos_set);

		auto iCut = G->Camera->m_Cut;

		if (Op->XYZ == 11) { G->Room->Lit->Get(iCut)->Pos[Op->Pos_no].x = Op->Position; }

		else if (Op->XYZ == 12) { G->Room->Lit->Get(iCut)->Pos[Op->Pos_no].y = Op->Position; }

		else if (Op->XYZ == 13) { G->Room->Lit->Get(iCut)->Pos[Op->Pos_no].z = Op->Position; }

		return EXEC_NORMAL;
	}

	static uint32_t Light_kido_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Light_kido_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Light_kido_set);

		auto iCut = G->Camera->m_Cut;

		G->Room->Lit->Get(iCut)->L[Op->Brit_no] = Op->Brightness;

		return EXEC_NORMAL;
	}

	static uint32_t Rbj_reset(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Rbj_reset*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Rbj_reset);

		// TODO
		//Replace_rdt();

		G->Sce->Rbj_reset_flg = 1;

		return EXEC_NORMAL;
	}

	static uint32_t Sce_scr_move(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_scr_move*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_scr_move);

		// TODO: vertical camera scroll

		return EXEC_NORMAL;
	}

	static uint32_t Parts_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Parts_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Parts_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Movie_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Movie_on*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Movie_on);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Splc_ret(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Splc_ret*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Splc_ret);

		//pSce->pWork->Routine_0 = 1;
		//pSce->pWork->Routine_1 = 0;
		//pSce->pWork->Routine_2 = 0;
		//pSce->pWork->Routine_3 = 0;

		//pSce->pWork->pEnemy_neck = &G->Player.Be_flg;

		//pSce->pWork->Neck_flg = 0;

		//pSce->pWork->Type = pSce->pWork->Type & 0xafff | 0x400;

		G->Sce->Status_flg |= SYS_SPAD_STACK;

		pSce->pWork->iHealth = static_cast<int32_t>(G->Sce->SaveData.Dummy[0]);

		pSce->pWork->iHealthMax = 200;

		//if ((pSce->pWork->Id == 0x45) || (pSce->pWork->Id == 0x4F))
		//{
		//	pbVar1 = (byte*)((int)pSce->pWork->Free + 9);
		//	*pbVar1 = *pbVar1 & 0xdf;
		//}

		return EXEC_NORMAL;
	}

	static uint32_t Splc_sce(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Splc_sce*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Splc_sce);

		//pSce->pWork->Routine_0 = 4;
		//pSce->pWork->Routine_1 = 6;
		//pSce->pWork->Routine_2 = 0;
		//pSce->pWork->Routine_3 = 0;

		//pSce->pWork->pEnemy_neck = &pSce->pWork->Be_flg;

		//pSce->pWork->Neck_flg = 0x92;

		//pSce->pWork->Type = pSce->pWork->Type & 0xfbff | 0x5000;

		G->Sce->Status_flg &= SYS_SPAD_STACK;

		pSce->pWork->iHealth = -1;

		pSce->pWork->iHealthMax = -1;

		return EXEC_NORMAL;
	}

	static uint32_t Super_on(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Super_on*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Super_on);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Mirror_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Mirror_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Mirror_set);

		G->Sce->Status_flg |= STAT_MIRROR_MATRIX;

		G->Sce->Mirror_flg = Op->Flag;
		G->Sce->Mirror_pos = Op->Position;
		G->Sce->Mirror_min = Op->Min;
		G->Sce->Mirror_max = Op->Max;

		return EXEC_NORMAL;
	}

	static uint32_t Sce_fade_adjust(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_fade_adjust*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_fade_adjust);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_espr3d_on2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_espr3d_on2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_espr3d_on2);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_Item_get(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_Item_get*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_Item_get);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_line_start(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_line_start*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_line_start);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_line_main(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_line_main*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_line_main);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_line_end(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_line_end*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_line_end);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_parts_bomb(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_parts_bomb*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_parts_bomb);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_parts_down(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_parts_down*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_parts_down);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Light_color_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Light_color_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Light_color_set);

		auto iCut = G->Camera->m_Cut;

		G->Room->Lit->Get(iCut)->Col[Op->Col_no].r = Op->R;
		G->Room->Lit->Get(iCut)->Col[Op->Col_no].g = Op->G;
		G->Room->Lit->Get(iCut)->Col[Op->Col_no].b = Op->B;

		return EXEC_NORMAL;
	}

	static uint32_t Light_pos_set2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Light_pos_set2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Light_pos_set2);

		if (Op->XYZ == 11) { G->Room->Lit->Get(Op->Lit_no)->Pos[Op->Pos_no].x = Op->Position; }
		else if (Op->XYZ == 12) { G->Room->Lit->Get(Op->Lit_no)->Pos[Op->Pos_no].y = Op->Position; }
		else if (Op->XYZ == 13) { G->Room->Lit->Get(Op->Lit_no)->Pos[Op->Pos_no].z = Op->Position; }

		return EXEC_NORMAL;
	}

	static uint32_t Light_kido_set2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Light_kido_set2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Light_kido_set2);

		G->Room->Lit->Get(Op->Lit_no)->L[Op->Brit_no] = Op->Brightness;

		return EXEC_NORMAL;
	}

	static uint32_t Light_color_set2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Light_color_set2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Light_color_set2);

		G->Room->Lit->Get(Op->Lit_no)->Col[Op->Col_no].r = Op->R;
		G->Room->Lit->Get(Op->Lit_no)->Col[Op->Col_no].g = Op->G;
		G->Room->Lit->Get(Op->Lit_no)->Col[Op->Col_no].b = Op->B;

		return EXEC_NORMAL;
	}

	static uint32_t Se_vol(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Se_vol*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Se_vol);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Sce_Item_cmp(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_Item_cmp*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_Item_cmp);

		// TODO

		/*if ((char)St.Equip_item_no < 0)
		{
			bVar2 = 0;
		}
		else
		{
			bVar2 = G->Sce->SaveData.Item_work[St._8_4_ & 0xff].Num;
		}

		switch (Op->Operator)
		{
		case CMP_EQ: return static_cast<uint32_t>(bVar2 == Op->Num);
		case CMP_GT: return static_cast<uint32_t>(Op->Num < bVar2);
		case CMP_GE: return static_cast<uint32_t>(Op->Num <= bVar2);
		case CMP_LT: return static_cast<uint32_t>(bVar2 < Op->Num);
		case CMP_LE: return static_cast<uint32_t>(bVar2 <= Op->Num);
		case CMP_NE: return static_cast<uint32_t>(bVar2 != Op->Num);
		case CMP_AND: return static_cast<uint32_t>((Op->Num & bVar2) != 0);
		}*/

		return EXEC_NORMAL;
	}

	static uint32_t Sce_espr_kill_all(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_espr_kill_all*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_espr_kill_all);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Plc_heal(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_heal*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_heal);

		G->Player->iHealth = G->Player->iHealthMax;
		//G.Unknown_flg1 = 0;
		//G.Unknown_flg2._0_1_ = 0;

		return EXEC_NORMAL;
	}

	static uint32_t St_map_hint(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::St_map_hint*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::St_map_hint);

		//G->Sce->St->St_type = 4;

		//G->Sce->Game->Doordemo_flg = 1;

		G->Sce->Status_flg |= STAT_STATUS_SCREEN;

		//G->Sce->Game->Stop_bak = G->Sce->Stop_flg;

		//G->Sce->St->Dmode[3][0].tag._2_1_ = Op->Tag;

		return EXEC_NORMAL;
	}

	static uint32_t Sce_em_pos_ck(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_em_pos_ck*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_em_pos_ck);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Poison_ck(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Poison_ck*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Poison_ck);

		// TODO
		
		//return (G->Unknown_flg1 != 0);

		return EXEC_NORMAL;
	}

	static uint32_t Poison_clr(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Poison_clr*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Poison_clr);

		//G.Unknown_flg1 = 0;

		//G.Unknown_flg2._0_1_ = 0;

		//G->Player->Routine_0 = 1;
		//G->Player->Routine_1 = 0;
		//G->Player->Routine_2 = 0;
		//G->Player->Routine_3 = 0;

		return EXEC_NORMAL;
	}

	static uint32_t Sce_Item_lost2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_Item_lost2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_Item_lost2);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Plc_life_max(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Plc_life_max*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Plc_life_max);

		G->Sce->SaveData.Dummy1[1] = 0;

		G->Sce->SaveData.Dummy[0] = static_cast<uint16_t>(200);

		G->SubPlayer->iHealth = 200;

		G->SubPlayer->iHealthMax = 200;

		return EXEC_NORMAL;
	}

	static uint32_t Vib_set0(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Vib_set0*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Vib_set0);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Vib_set1(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Vib_set1*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Vib_set1);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Vib_fade_set(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Vib_fade_set*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Vib_fade_set);

		// TODO

		return EXEC_NORMAL;
	}

	static uint32_t Item_aot_set2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Item_aot_set2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Item_aot_set2);

		auto& Item = G->Sce->Sce_aot[Op->Aot];
		Item.Is4P = false;
		Item.Id = Op->Id;
		Item.Type = Op->Type;
		Item.nFloor = Op->nFloor;
		Item.Super = Op->Super;
		Item.X = Op->X;
		Item.Z = Op->Z;
		Item.W = Op->W;
		Item.D = Op->D;
		Item.iItem = Op->iItem;
		Item.nItem = Op->nItem;
		Item.Item_flg = Op->Flag;
		Item.Om_no = Op->Om_no;
		Item.Action = Op->Action;
		Item.Rnd_flg = Op->Rnd_flg;

		return EXEC_NORMAL;
	}

	static uint32_t Sce_em_set2(Scenario::Task* pSce)
	{
		auto const* Op = reinterpret_cast<Resident_Evil_2_Bytecode::Sce_em_set2*>(pSce->Data);

		pSce->Data += sizeof(Resident_Evil_2_Bytecode::Sce_em_set2);

		G->Enemy.push_back(std::make_shared<Resident_Evil_Model>());

		auto& Enemy = G->Enemy.back();

		Enemy->SetGame(Video_Game::Resident_Evil_2);
		Enemy->PlatformSetup(G->Window->Get(), G->GTE, G->Render, G->Camera->b_HorzFlip, G->Camera->b_VertFlip);

		if (Standard_FileSystem().Exists(Standard_String().FormatCStyle(L"%ws\\em0%02x.emd", G->m_EnemyPath.wstring().c_str(), Op->Id)))
		{
			Enemy->Open(Standard_String().FormatCStyle(L"%ws\\em0%02x.emd", G->m_EnemyPath.wstring().c_str()));
		}

		//Enemy->Id = Id;
		//Enemy->Type = Type;
		//Enemy->nFloor = nFloor;
		//Enemy->Sound_flg = Sound_flg;
		//Enemy->Model_type = Model_type;
		//Enemy->Em_set_flg = Em_set_flg;

		Enemy->Position().x = Op->Pos_x;
		Enemy->Position().y = Op->Pos_y;
		Enemy->Position().z = Op->Pos_z;
		Enemy->Rotation().y = Op->Cdir_y;

		//Enemy->Motion = Motion;
		//Enemy->Ctr_flg = Ctr_flg;

		return EXEC_NORMAL;
	}

}


ScriptFunction Sce_jmp_tbl[] =
{
	/* 0x00 */ { Resident_Evil_2_Instruction::Nop },
	/* 0x01 */ { Resident_Evil_2_Instruction::Evt_end },
	/* 0x02 */ { Resident_Evil_2_Instruction::Evt_next },
	/* 0x03 */ { Resident_Evil_2_Instruction::Evt_chain },
	/* 0x04 */ { Resident_Evil_2_Instruction::Evt_exec },
	/* 0x05 */ { Resident_Evil_2_Instruction::Evt_kill },
	/* 0x06 */ { Resident_Evil_2_Instruction::Ifel_ck },
	/* 0x07 */ { Resident_Evil_2_Instruction::Else_ck },
	/* 0x08 */ { Resident_Evil_2_Instruction::Endif },
	/* 0x09 */ { Resident_Evil_2_Instruction::Sleep },
	/* 0x0A */ { Resident_Evil_2_Instruction::Sleeping },
	/* 0x0B */ { Resident_Evil_2_Instruction::Wsleep },
	/* 0x0C */ { Resident_Evil_2_Instruction::Wsleeping },
	/* 0x0D */ { Resident_Evil_2_Instruction::For },
	/* 0x0E */ { Resident_Evil_2_Instruction::Next },
	/* 0x0F */ { Resident_Evil_2_Instruction::While },
	/* 0x10 */ { Resident_Evil_2_Instruction::Ewhile },
	/* 0x11 */ { Resident_Evil_2_Instruction::Do },
	/* 0x12 */ { Resident_Evil_2_Instruction::Edwhile },
	/* 0x13 */ { Resident_Evil_2_Instruction::Switch },
	/* 0x14 */ { Resident_Evil_2_Instruction::Case },
	/* 0x15 */ { Resident_Evil_2_Instruction::Default },
	/* 0x16 */ { Resident_Evil_2_Instruction::Eswitch },
	/* 0x17 */ { Resident_Evil_2_Instruction::Goto },
	/* 0x18 */ { Resident_Evil_2_Instruction::Gosub },
	/* 0x19 */ { Resident_Evil_2_Instruction::Return },
	/* 0x1A */ { Resident_Evil_2_Instruction::Break },
	/* 0x1B */ { Resident_Evil_2_Instruction::For2 },
	/* 0x1C */ { Resident_Evil_2_Instruction::Break_point },
	/* 0x1D */ { Resident_Evil_2_Instruction::Work_copy },
	/* 0x1E */ { Resident_Evil_2_Instruction::Nop },
	/* 0x1F */ { Resident_Evil_2_Instruction::Nop },
	/* 0x20 */ { Resident_Evil_2_Instruction::Nop },
	/* 0x21 */ { Resident_Evil_2_Instruction::Ck },
	/* 0x22 */ { Resident_Evil_2_Instruction::Set },
	/* 0x23 */ { Resident_Evil_2_Instruction::Cmp },
	/* 0x24 */ { Resident_Evil_2_Instruction::Save },
	/* 0x25 */ { Resident_Evil_2_Instruction::Copy },
	/* 0x26 */ { Resident_Evil_2_Instruction::Calc },
	/* 0x27 */ { Resident_Evil_2_Instruction::Calc2 },
	/* 0x28 */ { Resident_Evil_2_Instruction::Sce_rnd },
	/* 0x29 */ { Resident_Evil_2_Instruction::Cut_chg },
	/* 0x2A */ { Resident_Evil_2_Instruction::Cut_old },
	/* 0x2B */ { Resident_Evil_2_Instruction::Message_on },
	/* 0x2C */ { Resident_Evil_2_Instruction::Aot_set },
	/* 0x2D */ { Resident_Evil_2_Instruction::Obj_model_set },
	/* 0x2E */ { Resident_Evil_2_Instruction::Work_set },
	/* 0x2F */ { Resident_Evil_2_Instruction::Speed_set },
	/* 0x30 */ { Resident_Evil_2_Instruction::Add_speed },
	/* 0x31 */ { Resident_Evil_2_Instruction::Add_aspeed },
	/* 0x32 */ { Resident_Evil_2_Instruction::Pos_set },
	/* 0x33 */ { Resident_Evil_2_Instruction::Dir_set },
	/* 0x34 */ { Resident_Evil_2_Instruction::Member_set },
	/* 0x35 */ { Resident_Evil_2_Instruction::Member_set2 },
	/* 0x36 */ { Resident_Evil_2_Instruction::Se_on },
	/* 0x37 */ { Resident_Evil_2_Instruction::Sca_id_set },
	/* 0x38 */ { Resident_Evil_2_Instruction::Flr_set },
	/* 0x39 */ { Resident_Evil_2_Instruction::Dir_ck },
	/* 0x3A */ { Resident_Evil_2_Instruction::Sce_espr_on },
	/* 0x3B */ { Resident_Evil_2_Instruction::Door_aot_set },
	/* 0x3C */ { Resident_Evil_2_Instruction::Cut_auto },
	/* 0x3D */ { Resident_Evil_2_Instruction::Member_copy },
	/* 0x3E */ { Resident_Evil_2_Instruction::Member_cmp },
	/* 0x3F */ { Resident_Evil_2_Instruction::Plc_motion },
	/* 0x40 */ { Resident_Evil_2_Instruction::Plc_dest },
	/* 0x41 */ { Resident_Evil_2_Instruction::Plc_neck },
	/* 0x42 */ { Resident_Evil_2_Instruction::Plc_ret },
	/* 0x43 */ { Resident_Evil_2_Instruction::Plc_flg },
	/* 0x44 */ { Resident_Evil_2_Instruction::Sce_em_set },
	/* 0x45 */ { Resident_Evil_2_Instruction::Col_chg_set },
	/* 0x46 */ { Resident_Evil_2_Instruction::Aot_reset },
	/* 0x47 */ { Resident_Evil_2_Instruction::Aot_on },
	/* 0x48 */ { Resident_Evil_2_Instruction::Super_set },
	/* 0x49 */ { Resident_Evil_2_Instruction::Super_reset },
	/* 0x4A */ { Resident_Evil_2_Instruction::Plc_gun },
	/* 0x4B */ { Resident_Evil_2_Instruction::Cut_replace },
	/* 0x4C */ { Resident_Evil_2_Instruction::Sce_espr_kill },
	/* 0x4D */ { Resident_Evil_2_Instruction::Door_model_set },
	/* 0x4E */ { Resident_Evil_2_Instruction::Item_aot_set },
	/* 0x4F */ { Resident_Evil_2_Instruction::Sce_key_ck },
	/* 0x50 */ { Resident_Evil_2_Instruction::Sce_trg_ck },
	/* 0x51 */ { Resident_Evil_2_Instruction::Sce_bgm_control },
	/* 0x52 */ { Resident_Evil_2_Instruction::Sce_espr_control },
	/* 0x53 */ { Resident_Evil_2_Instruction::Sce_fade_set },
	/* 0x54 */ { Resident_Evil_2_Instruction::Sce_espr3d_on },
	/* 0x55 */ { Resident_Evil_2_Instruction::Member_calc },
	/* 0x56 */ { Resident_Evil_2_Instruction::Member_calc2 },
	/* 0x57 */ { Resident_Evil_2_Instruction::Sce_bgmtbl_set },
	/* 0x58 */ { Resident_Evil_2_Instruction::Plc_rot },
	/* 0x59 */ { Resident_Evil_2_Instruction::Xa_on },
	/* 0x5A */ { Resident_Evil_2_Instruction::Weapon_chg },
	/* 0x5B */ { Resident_Evil_2_Instruction::Plc_cnt },
	/* 0x5C */ { Resident_Evil_2_Instruction::Sce_shake_on },
	/* 0x5D */ { Resident_Evil_2_Instruction::Mizu_div_set },
	/* 0x5E */ { Resident_Evil_2_Instruction::Keep_Item_ck },
	/* 0x5F */ { Resident_Evil_2_Instruction::Xa_vol },
	/* 0x60 */ { Resident_Evil_2_Instruction::Kage_set },
	/* 0x61 */ { Resident_Evil_2_Instruction::Cut_be_set },
	/* 0x62 */ { Resident_Evil_2_Instruction::Sce_Item_lost },
	/* 0x63 */ { Resident_Evil_2_Instruction::Plc_gun_eff },
	/* 0x64 */ { Resident_Evil_2_Instruction::Sce_espr_on2 },
	/* 0x65 */ { Resident_Evil_2_Instruction::Sce_espr_kill2 },
	/* 0x66 */ { Resident_Evil_2_Instruction::Plc_stop },
	/* 0x67 */ { Resident_Evil_2_Instruction::Aot_set_4p },
	/* 0x68 */ { Resident_Evil_2_Instruction::Door_aot_set_4p },
	/* 0x69 */ { Resident_Evil_2_Instruction::Item_aot_set_4p },
	/* 0x6A */ { Resident_Evil_2_Instruction::Light_pos_set },
	/* 0x6B */ { Resident_Evil_2_Instruction::Light_kido_set },
	/* 0x6C */ { Resident_Evil_2_Instruction::Rbj_reset },
	/* 0x6D */ { Resident_Evil_2_Instruction::Sce_scr_move },
	/* 0x6E */ { Resident_Evil_2_Instruction::Parts_set },
	/* 0x6F */ { Resident_Evil_2_Instruction::Movie_on },
	/* 0x70 */ { Resident_Evil_2_Instruction::Splc_ret },
	/* 0x71 */ { Resident_Evil_2_Instruction::Splc_sce },
	/* 0x72 */ { Resident_Evil_2_Instruction::Super_on },
	/* 0x73 */ { Resident_Evil_2_Instruction::Mirror_set },
	/* 0x74 */ { Resident_Evil_2_Instruction::Sce_fade_adjust },
	/* 0x75 */ { Resident_Evil_2_Instruction::Sce_espr3d_on2 },
	/* 0x76 */ { Resident_Evil_2_Instruction::Sce_Item_get },
	/* 0x77 */ { Resident_Evil_2_Instruction::Sce_line_start },
	/* 0x78 */ { Resident_Evil_2_Instruction::Sce_line_main },
	/* 0x79 */ { Resident_Evil_2_Instruction::Sce_line_end },
	/* 0x7A */ { Resident_Evil_2_Instruction::Sce_parts_bomb },
	/* 0x7B */ { Resident_Evil_2_Instruction::Sce_parts_down },
	/* 0x7C */ { Resident_Evil_2_Instruction::Light_color_set },
	/* 0x7D */ { Resident_Evil_2_Instruction::Light_pos_set2 },
	/* 0x7E */ { Resident_Evil_2_Instruction::Light_kido_set2 },
	/* 0x7F */ { Resident_Evil_2_Instruction::Light_color_set2 },
	/* 0x80 */ { Resident_Evil_2_Instruction::Se_vol },
	/* 0x81 */ { Resident_Evil_2_Instruction::Sce_Item_cmp },
	/* 0x82 */ { Resident_Evil_2_Instruction::Sce_espr_kill_all },
	/* 0x83 */ { Resident_Evil_2_Instruction::Plc_heal },
	/* 0x84 */ { Resident_Evil_2_Instruction::St_map_hint },
	/* 0x85 */ { Resident_Evil_2_Instruction::Sce_em_pos_ck },
	/* 0x86 */ { Resident_Evil_2_Instruction::Poison_ck },
	/* 0x87 */ { Resident_Evil_2_Instruction::Poison_clr },
	/* 0x88 */ { Resident_Evil_2_Instruction::Sce_Item_lost2 },
	/* 0x89 */ { Resident_Evil_2_Instruction::Plc_life_max },
	/* 0x8A */ { Resident_Evil_2_Instruction::Vib_set0 },
	/* 0x8B */ { Resident_Evil_2_Instruction::Vib_set1 },
	/* 0x8C */ { Resident_Evil_2_Instruction::Vib_fade_set },
	/* 0x8D */ { Resident_Evil_2_Instruction::Item_aot_set2 },
	/* 0x8E */ { Resident_Evil_2_Instruction::Sce_em_set2 }
};


ScenarioFunction Sce_at_jump_tbl[] =
{
	/* 0x00 */ { Resident_Evil_2_Instruction::Nothing },
	/* 0x01 */ { Resident_Evil_2_Instruction::Sce_Door },
	/* 0x02 */ { Resident_Evil_2_Instruction::Sce_Item },
	/* 0x03 */ { Resident_Evil_2_Instruction::Sce_Normal },
	/* 0x04 */ { Resident_Evil_2_Instruction::Sce_Message },
	/* 0x05 */ { Resident_Evil_2_Instruction::Sce_Event },
	/* 0x06 */ { Resident_Evil_2_Instruction::Sce_Flg_chg },
	/* 0x07 */ { Resident_Evil_2_Instruction::Sce_Water },
	/* 0x08 */ { Resident_Evil_2_Instruction::Sce_Move },
	/* 0x09 */ { Resident_Evil_2_Instruction::Sce_Save },
	/* 0x0A */ { Resident_Evil_2_Instruction::Sce_Itembox },
	/* 0x0B */ { Resident_Evil_2_Instruction::Sce_Damage },
	/* 0x0C */ { Resident_Evil_2_Instruction::Sce_Status },
	/* 0x0D */ { Resident_Evil_2_Instruction::Sce_Hikidashi },
	/* 0x0E */ { Resident_Evil_2_Instruction::Sce_Windows },
};


void Resident_Evil_2_Bytecode::SceSchedulerSet(void)
{
	Resident_Evil_2_Instruction::Sce_scheduler_set();
}

void Resident_Evil_2_Bytecode::SceScheduler(void)
{
	Resident_Evil_2_Instruction::Sce_scheduler();
}

void Resident_Evil_2_Bytecode::NullScheduler(void)
{
	Resident_Evil_2_Instruction::Sce_work_clr_set();
}


void Scenario::ProcessAOT(void)
{
	Resident_Evil_2_Instruction::Sce_at();
}