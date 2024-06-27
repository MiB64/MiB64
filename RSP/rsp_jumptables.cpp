/*
 * MiB64 - A Nintendo 64 emulator.
 *
 * Project64 (c) Copyright 2001 Zilmar, Jabo, Smiff, Gent, Witten
 * Projectg64 Legacy (c) Copyright 2010 PJ64LegacyTeam
 * MiB64 (c) Copyright 2024 MiB64Team
 *
 * MiB64 Homepage: www.mib64.net
 *
 * Permission to use, copy, modify and distribute MiB64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * MiB64 is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for MiB64 or software derived from MiB64.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so if they want them.
 *
 */

#include <windows.h>
#include <unordered_map>
#include <set>
#include <vector>

#include "rsp_memory.h"
#include "../hash/xxhash.h"
#include "../hash/xxh_x86dispatch.h"

class JumpTableDesc {
public:
	JumpTableDesc() = default;
	JumpTableDesc(const JumpTableDesc&) = default;
	JumpTableDesc(JumpTableDesc&&) = default;
	auto operator=(const JumpTableDesc&)->JumpTableDesc & = default;
	auto operator=(JumpTableDesc&&)->JumpTableDesc & = default;

	void reset() {
		_maxRecompiledOpcode = 0;
		_hash = 0;
		_IMEM.resize(0x1000);
		_jumpTable.resize(0x1000);
		for (size_t i = 0; i < 0x1000; ++i) {
			_IMEM[i] = IMEM[i];
			_jumpTable[i] = 0;
		}
	}

	auto getMaxRecompiledOpcode() const->DWORD {
		return _maxRecompiledOpcode;
	}

	void updateJumpTableHash(DWORD maxRecompiledOpcode) {
		_maxRecompiledOpcode = maxRecompiledOpcode;
		_hash = XXH3_64bits(_IMEM.data(), _maxRecompiledOpcode + 4);
	}

	auto getHash() const->XXH64_hash_t {
		return _hash;
	}

	auto getJumpTable() -> void** {
		return reinterpret_cast<void**>(_jumpTable.data());
	}

private:
	DWORD _maxRecompiledOpcode = 0;
	XXH64_hash_t _hash = 0;
	std::vector<BYTE> _IMEM;
	std::vector<BYTE> _jumpTable;
};

DWORD NoOfRspMaps;
DWORD RspTable;
static std::vector<JumpTableDesc> JumpTables;
static std::unordered_map<XXH64_hash_t, DWORD> JumpTablesMap;
static std::set<DWORD> IMEMLengths;
void** RspJumpTable;

extern "C" {
	void ClearJumpTables() {
		RspJumpTable = NULL;

		NoOfRspMaps = 0;
		JumpTablesMap.clear();
		IMEMLengths.clear();
	}
}

extern "C" {
	DWORD GetMaxRecompiledOpcode() {
		return JumpTables[RspTable].getMaxRecompiledOpcode();
	}
}

extern "C" {
	void UpdateJumpTableHash(DWORD maxRecompiledOpcode) {
		auto& jumpTable = JumpTables[RspTable];
		if (maxRecompiledOpcode > jumpTable.getMaxRecompiledOpcode()) {
			auto oldHash = jumpTable.getHash();

			jumpTable.updateJumpTableHash(maxRecompiledOpcode);

			if (oldHash != 0 && JumpTablesMap.find(oldHash) != JumpTablesMap.end()) {
				JumpTablesMap.erase(oldHash);
			}
			JumpTablesMap[jumpTable.getHash()] = RspTable;

			IMEMLengths.clear();
			for (auto& jt : JumpTables) {
				IMEMLengths.insert(jt.getMaxRecompiledOpcode() + 4);
			}
		}
	}
}

void SetRspJumpTable(void) {
	DWORD currentLength = 0;
	XXH3_state_t* state = XXH3_createState();
	XXH3_64bits_reset(state);


	for (auto length : IMEMLengths) {
		DWORD delta = length - currentLength;
		XXH3_64bits_update(state, IMEM + currentLength, delta);
		currentLength = length;
		auto currentHash = XXH3_64bits_digest(state);

		auto it = JumpTablesMap.find(currentHash);
		if (it != JumpTablesMap.end()) {
			RspTable = it->second;
			RspJumpTable = JumpTables[RspTable].getJumpTable();
			return;
		}
	}

	XXH3_freeState(state);

	if (NoOfRspMaps == JumpTables.size()) {
		JumpTables.emplace_back();
	}
	RspTable = NoOfRspMaps;
	NoOfRspMaps += 1;

	auto& table = JumpTables[RspTable];
	table.reset();
	RspJumpTable = table.getJumpTable();
}
