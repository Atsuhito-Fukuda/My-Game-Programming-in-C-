#pragma once
#include <unordered_map>
#include <string>
#include <cstdint>
#include "ARRAY.h"

/***************************************************************************

	IDinfo

****************************************************************************/

namespace idmap {

	class IDinfo {
	public:
		IDinfo(void);
		IDinfo(uintptr_t ID);
		uintptr_t ID;
	};

	class IDinfo_i : public IDinfo {
	public:
		IDinfo_i(void);
		IDinfo_i(uintptr_t id, int ivar);
		int ivar;
	};	

}

/***************************************************************************

	IDcountHashMap

****************************************************************************/

namespace idmap {
	template <typename Key, typename Instance>
	class IDcountHashMap;
}


template <typename Key, typename Instance>
class idmap::IDcountHashMap {	

private:
	using registered = bool;
	using empty = bool;
public:
	IDcountHashMap(const std::string& name, unsigned int mapsize);

	registered Read(const Key& key, Instance& copy);
	void Register(const Key& key, const Instance& instance);
	empty Remove(uintptr_t ID);

private:
	const std::string name;
	class InstanceInfo;
	std::unordered_map<Key, InstanceInfo> map;	
	std::unordered_map<uintptr_t, Key> reverse_map;

	IDcountHashMap(void);

	class InstanceInfo {
	public:
		InstanceInfo(void);
		InstanceInfo(const Instance& instance);
		Instance instance;
		unsigned int count;

	};

};

/***************************************************************************

	IDgenerateMap

****************************************************************************/

namespace idmap {
	template <typename Instance>
	class IDgenerateMap;
}

template <typename Instance>
class idmap::IDgenerateMap {	

public:
	using ID = unsigned int;
	using BoardID = unsigned int;

	IDgenerateMap(const std::string& mapname, unsigned int mapsectorlength, unsigned int boardlength);
	~IDgenerateMap(void);
	ID Register(Instance* instance, std::vector<BoardID>& table);
	Instance* Read(ID id);
	const Board<Instance>& ReadBoard(BoardID tableid);
	BoardID Remove(ID id);
	void Remove(std::vector<BoardID>& table);		
private:
	const std::string mapname;
	const unsigned int fixedboardlength;	
	const unsigned int mapsectorlength;
	Board<Board<Instance>>& map;
	IDgenerateMap(void);	

};

/***************************************************************************

	IDgenerateMap

****************************************************************************/

/*

	・Instanceのptrを決まった数並べた配列、を並べた配列。(Board<Board<Instance>>)

	・各配列(Instanceのptrを収納)には「テーブル(Boardとも呼んでいる)番号(ID)」が割り振られている。

	・テーブルの番号とテーブルの中での要素の番号から(絶対的な)要素のID番号を取り決めている。

	・各テーブルは動的に生成・削除する。

		・要素を登録・読み出す際にはテーブル番号を指定する。(一杯になればテーブルを生成する。)

		・RemoveはIDまたはテーブルを指定して取り除く。
	
*/

template <typename Instance>
idmap::IDgenerateMap<Instance>::IDgenerateMap(const std::string& mapname, unsigned int mapsectorlength, unsigned int boardlength) :
	mapname(mapname), fixedboardlength(boardlength), mapsectorlength(mapsectorlength), map(*(new Board<Board<Instance>>(this->mapsectorlength)))
{
	if ((this->fixedboardlength == 0) || (this->mapsectorlength == 0))
		error::tIDgenerateMap::BoardSize0(this->mapname);

}	

template <typename Instance>
idmap::IDgenerateMap<Instance>::~IDgenerateMap(void) {
	
}

template <typename Instance>
typename idmap::IDgenerateMap<Instance>::ID idmap::IDgenerateMap<Instance>::Register(Instance* instance, std::vector<BoardID>& table) {

	unsigned int n(0);
	while (true) {

		for (; n < table.size(); ++n) {
			if (map[table[n]]->Register(instance))
				return fixedboardlength * table[n] + map[table[n]]->getcurrentpoint();
		}

		Board<Instance>* newboard(new Board<Instance>(fixedboardlength));
		while (!map.Register(newboard)) map.ReAllocate(map.getlength() + mapsectorlength); 

		table.push_back(map.getcurrentpoint());

	}

}
template <typename Instance>
Instance* idmap::IDgenerateMap<Instance>::Read(ID id) {

	Instance* instance(nullptr);

	if (id) {

		Board<Instance>* board(map[id / fixedboardlength]);
		if (board) {
			instance = (*board)[id % fixedboardlength];
			if (!instance) error::tIDgenerateMap::nullinstance(mapname);
		}
		else error::tIDgenerateMap::nonexistBoard(mapname);

	}

	return instance;

}

template <typename Instance>
const Board<Instance>& idmap::IDgenerateMap<Instance>::ReadBoard(BoardID tableid) {

	const Board<Instance>* boardarray(map[tableid]);
	if (!boardarray) error::tIDgenerateMap::nonexistReadBoard(mapname);

	return *boardarray;

}

template <typename Instance>
typename idmap::IDgenerateMap<Instance>::BoardID idmap::IDgenerateMap<Instance>::Remove(ID id) {

	unsigned int deleteboard(0);

	if (id) {

		Board<Instance>* board(map[id / fixedboardlength]);
		if (!board) error::tIDgenerateMap::nonexistBoard(mapname);

		board->Remove(id % fixedboardlength);
		if (board->isempty()) map.Remove((deleteboard = id / fixedboardlength));

	}
	else error::tIDgenerateMap::idzeroremove(mapname);

	return deleteboard;

}

template <typename Instance>
void idmap::IDgenerateMap<Instance>::Remove(std::vector<BoardID>& table) {

	bool IDinclude(false);
	if (!table.size()) error::tIDgenerateMap::boardidzeroremove(mapname);

	for (auto boardID : table) {

		if (boardID) map.Remove(boardID);

		else {
			error::tIDgenerateMap::settBoardNullRemoveflag(false);
			for (unsigned int localID(1); localID < map[boardID]->getlength(); ++localID) {
				map[boardID]->Remove(localID);
				IDinclude = true;
			}
			error::tIDgenerateMap::settBoardNullRemoveflag(true);
		}
	}

	table.clear();
	if (IDinclude) table.emplace_back(0);	

} 

/*
public:
void show(void) {

		std::cout << "[ ";
		for (unsigned int board(0); board < map.getlength(); ++board) {
			std::cout << "[ ";
			if (map[board]) {
				for (unsigned int i(0); i < map[board]->getlength(); ++i) {
					if ((*map[board])[i]) std::cout << *(*map[board])[i] << " ";
					else std::cout << "* ";
				}
			}
			std::cout << "] ";
		}
		std::cout << "]\n";
	}
	void show2(void) {
		map.show();
	}
	void show3(unsigned int n) {
		if (map[n]) map[n]->show();
	}
*/