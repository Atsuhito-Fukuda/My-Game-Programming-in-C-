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
 
	�E�Ⴆ��GpuBase�N���X�ł́A���_�f�[�^�̃t�@�C������ID(GLuint)�����ѕt����std::unordered_map��
	�@�o�^���邪�A�����t�@�C�����ĂьĂэ��܂ꂽ�ꍇ�ɓ���ID���g���܂킵�A�����Ă���ID���g�p����҂�
	  ���Ȃ��Ȃ��Ă͂��߂ēo�^���폜����悤�ɂ����������B

	�EIDcountHashMap�ł�std::unordered_map�ɉ����āu�g�p���v�𐔂���悤�ɂ��Ă���B

		�ERead()���Ċ��ɓo�^�ς݂������ꍇ�ɂ́u�g�p���v��+1���ARemove()�����-1����B
 
		�E�g�p����0�ɂȂ��map����폜����B

	�EID(unsigned int)��n���č폜�ł���悤�ɁAID���疼�O�����t�����ł���unordered_map��p�ӂ��Ă���B


	�EIdcountHashMap��ID��K�{�Ƃ��Ă��邽�߁A�e���v���[�g�N���X�ł��邪�����I�ȃC���X�^���X��������
	�@������N���X�𐧌����Ă���B

		�Eidmap ::IDinfo, idmap::IDinfo_i�N���X�͂���ɂ�����B

 
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

// �����I�C���X�^���X�� ------------------------------------------

template class IDcountHashMap<std::string, IDinfo>;
template class IDcountHashMap<std::string, IDinfo_i>;	

/***************************************************************************

	IDcountHashMap::InstanceInfo

****************************************************************************/

/*
	
	�EInstance�Ɓu�g�p���v�������o�Ɏ��B

	�Estd::unordered_map�ɂ͂��̃N���X��o�^����B

*/

template <typename Key, typename Instance>
IDcountHashMap<Key, Instance>::InstanceInfo::InstanceInfo(void) : instance(), count() {}

template <typename Key, typename Instance>
IDcountHashMap<Key, Instance>::InstanceInfo::InstanceInfo(const Instance& instance) : instance(instance), count() {}