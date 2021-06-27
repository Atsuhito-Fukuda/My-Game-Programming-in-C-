#include "IDmap.h"
#include <string>
#include "Error.h"

using namespace idmap;

/***************************************************************************

	IDinfo

****************************************************************************/

IDinfo::IDinfo(void) : ID() {}
IDinfo::IDinfo(uintptr_t ID) : ID(ID) {}

IDinfo_i::IDinfo_i(void) : ivar() {}
IDinfo_i::IDinfo_i(uintptr_t id, int ivar) : IDinfo(id), ivar(ivar) {}

/***************************************************************************

	IDcountHashMap

****************************************************************************/

/*
 
	・例えばGpuBaseクラスでは、頂点データのファイル名とID(GLuint)を結び付けてstd::unordered_mapに
	　登録するが、同名ファイルが再び呼び込まれた場合に同じIDを使いまわし、そしてそのIDを使用する者が
	  いなくなってはじめて登録を削除するようにしたかった。

	・IDcountHashMapではstd::unordered_mapに加えて「使用数」を数えるようにしている。

		・Read()して既に登録済みだった場合には「使用数」を+1し、Remove()すると-1する。
 
		・使用数が0になるとmapから削除する。

	・ID(unsigned int)を渡して削除できるように、IDから名前等を逆引きできるunordered_mapを用意している。


	・IdcountHashMapはIDを必須としているため、テンプレートクラスであるが明示的なインスタンス化をして
	　扱えるクラスを制限している。

		・idmap ::IDinfo, idmap::IDinfo_iクラスはこれにあたる。

 
*/


template <typename Key, typename Instance>
IDcountHashMap<Key, Instance>::IDcountHashMap(const std::string& name, unsigned int mapsize) : map(mapsize), reverse_map(mapsize), name(name) {}

template <typename Key, typename Instance>
typename IDcountHashMap<Key, Instance>::registered IDcountHashMap<Key, Instance>::Read(const Key& key, Instance& copy) {	

	bool isregistered(false);

	auto iter(map.find(key));
	if (iter != map.end()) {
		++(iter->second.count);
		copy = iter->second.instance;
		isregistered = true;
	}

	return isregistered;

}	

template <typename Key, typename Instance>
void IDcountHashMap<Key, Instance>::Register(const Key& key, const Instance& instance) {	

	if (map.find(key) != map.end()) error::tIDcountHashMap::AlreadyRegistered(name, key);
	else {
		if (reverse_map.find(instance.ID) == reverse_map.end()) {
			map[key] = InstanceInfo(instance);
			++(map[key].count);
			reverse_map[instance.ID] = key;
		}
		else
			error::tIDcountHashMap::IDmultipled(name, key);
	}

}


template <typename Key, typename Instance>
typename IDcountHashMap<Key, Instance>::empty IDcountHashMap<Key, Instance>::Remove(uintptr_t ID) {

	bool isempty(false);

	auto iter_r(reverse_map.find(ID));
	if (iter_r != reverse_map.end()) {

		if ((--(map[iter_r->second].count)) == 0) {	
			map.erase(iter_r->second);
			reverse_map.erase(ID);
			isempty = true;
		}

	}
	else error::tIDcountHashMap::NoIDtoDelete(name);

	return isempty;

}

// 明示的インスタンス化 ------------------------------------------

template class IDcountHashMap<std::string, IDinfo>;
template class IDcountHashMap<std::string, IDinfo_i>;	

/***************************************************************************

	IDcountHashMap::InstanceInfo

****************************************************************************/

/*
	
	・Instanceと「使用数」をメンバに持つ。

	・std::unordered_mapにはこのクラスを登録する。

*/

template <typename Key, typename Instance>
IDcountHashMap<Key, Instance>::InstanceInfo::InstanceInfo(void) : instance(), count() {}

template <typename Key, typename Instance>
IDcountHashMap<Key, Instance>::InstanceInfo::InstanceInfo(const Instance& instance) : instance(instance), count() {}