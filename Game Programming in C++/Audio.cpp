#include <fmod_studio.hpp>	
#include <fmod_errors.h> 
#include "Error.h"
#include "Audio.h"
#include "IDmap.h"
#include <unordered_map>
#include "Object.h"

/*
	[ namespace audio ]

	・ここではサウンドの再生・管理を扱う。(外部ライブラリFMODを用いる。)
	・AudioBaseクラスはサウンドを扱うstatic関数を用意。
	・InstanceManagerクラスは各objの管理するサウンド(情報)の配列を持つ。Audioクラスの一部で外部には宣言不明。(pimplイディオム)
	・Audioクラスの非static部分はobject::ObjectBaseクラスが継承する。

*/

namespace FMOD {	// FMODにあるもの
	namespace Studio {
		class System;
		class Bank;
		class EventDescription;
		class Bus;
		class EventInstance;
	}
}
namespace audio {
	class AudioBase;
	class AudioManager;
	using InstanceID = unsigned int;
	class InstanceInfo;
	enum Max : unsigned int {
		INSTANCENUM = enumInstanceInfo::mainvoice1 + 1
	};
}

class audio::AudioBase {

	static void geteventlist(const FMOD::Studio::Bank* fmodbank, std::vector<FMOD::Studio::EventDescription*>& table);
	static void getbuslist(const FMOD::Studio::Bank* fmodbank, std::vector<FMOD::Studio::Bus*>& table);

	using FmodbankIDinfo = idmap::IDinfo;
	static idmap::IDcountHashMap <std::string, FmodbankIDinfo> fmodbankmap;
	static std::unordered_map <std::string, FMOD::Studio::EventDescription*> fmodeventmap;
	static std::unordered_map <std::string, FMOD::Studio::Bus*> fmodbusmap;

public:

	static void PlayEvent(const std::string& eventname, InstanceInfo& info);
	static void InstanceManage(InstanceInfo& info, Playmode mode, float var = 0.0f, const std::string& name = "");
	static void SetInstance(InstanceInfo& info, const GLMatrix& mat);
	static void BusManage(const std::string& busname, Busmode mode, float var = 0.0f);

	static void LoadFmodbank(const std::string& bankname);
	static void UnloadFmodbank(std::uintptr_t bankID);

	static FMOD::Studio::System* mSystem;
	//static FMOD::System* mLowLevSystem;	

	class InstanceLinkage {
	public:
		InstanceLinkage(void);
		InstanceLinkage(FMOD::Studio::EventInstance* instance, InstanceInfo* info);
		~InstanceLinkage(void);

		FMOD::Studio::EventInstance* instance;
		InstanceInfo* info;
	};

	static Board<InstanceLinkage> instancemap;

};

using namespace audio;

/***************************************************************************

	InstanceInfo

****************************************************************************/

/*

	・AudioBaseにある、再生・管理中のFMODのEventInstance*を並べた配列のID(要素番号)を保持する

	・各objはこのクラスをメンバに持ち、このIDを通して自身のサウンドの管理を行う。

	・外部にはアク禁。

	・objがデストラクトされるとAudioBaseの方にサウンドの停止を通知する。

	・InstanceLinkage(FMODのEventInstance*を持つクラス)と相互にリンクしている。

*/

class audio::InstanceInfo {

	friend class AudioBase;

	InstanceID id;
	void destruct(void);

	InstanceInfo& operator =(const InstanceInfo& nodeinfo);//コピー禁止
	InstanceInfo(const InstanceInfo& nodeinfo);
public:		
	InstanceInfo(void); 
	~InstanceInfo(void);

};

InstanceInfo::InstanceInfo(void) : id() {}
InstanceInfo::~InstanceInfo(void) { destruct(); }
void InstanceInfo::destruct(void) { 
	error::Audio::noeventinstanceflag = false; 
	AudioBase::InstanceManage(*this, Playmode::Stop, 0.0f, ""); 
	error::Audio::noeventinstanceflag = true;
}

/***************************************************************************

	InstanceManager

****************************************************************************/

class audio::InstanceManager {

	friend class Audio;

	std::vector<audio::InstanceInfo> instanceinfotable;

	InstanceManager(void);

};

InstanceManager::InstanceManager(void) : instanceinfotable(Max::INSTANCENUM)
{}

/***************************************************************************

	InstanceLinkage

****************************************************************************/

/*

	・再生・管理中のFMODのEventInstance*と、そのサウンドを管理しているobj(のInstanceInfo)のptrを持つ

	・AudioBase(またはFMOD)の方でサウンドが終了した場合にobjの方も情報を更新する。

	・AudioBaseはInstanceLinkageを並べた配列を持ち、サウンドを管理している。

	・InstanceInfoと相互にリンクしている。

*/

AudioBase::InstanceLinkage::InstanceLinkage(void) : instance(), info() {}
AudioBase::InstanceLinkage::InstanceLinkage(FMOD::Studio::EventInstance* instance, InstanceInfo* info) :
	instance(instance), info(info) {}
AudioBase::InstanceLinkage::~InstanceLinkage(void) {
	instance->release();
	if (info) info->id = 0; 
}

/***************************************************************************

	AudioBase

****************************************************************************/

/*
 
	・再生したいサウンドのファイル名を渡すことでサウンドを鳴らす。

	・管理したいサウンドと管理項目、調整方法を伝えて調整する。

	・再生・管理中のサウンドを配列に保持している。 
 
*/

const int maxchannel(512);
enum mapsize : unsigned int {
	bankmap = 20, eventmap = 50, busmap = 50, instancemap = 32
};

FMOD::Studio::System* AudioBase::mSystem(nullptr);

idmap::IDcountHashMap <std::string, AudioBase::FmodbankIDinfo> AudioBase::fmodbankmap("FmodBankMap", mapsize::bankmap);
std::unordered_map <std::string, FMOD::Studio::EventDescription*> AudioBase::fmodeventmap(mapsize::eventmap);
std::unordered_map <std::string, FMOD::Studio::Bus*> AudioBase::fmodbusmap(mapsize::busmap);
Board<AudioBase::InstanceLinkage> AudioBase::instancemap(mapsize::instancemap);

void AudioBase::LoadFmodbank(const std::string& bankname) {

	FmodbankIDinfo bid_dummy;
	if (fmodbankmap.Read(bankname, bid_dummy)) return;

	FMOD::Studio::Bank* fmodbank(nullptr);
	FMOD_RESULT loadresult(mSystem->loadBankFile(bankname.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &fmodbank));
	error::Audio::FMODBankLoadCheck(loadresult, bankname);

	fmodbankmap.Register(bankname, FmodbankIDinfo(reinterpret_cast<std::uintptr_t>(fmodbank)));

	fmodbank->loadSampleData();

// load events - - - - - - - - - - - - - - - - - - - - - - - - -

	std::vector<FMOD::Studio::EventDescription*> eventtable;
	geteventlist(fmodbank, eventtable);

	std::vector<char> eventname(512);	
	for (auto event : eventtable) {

		event->getPath(eventname.data(), 512, nullptr);

		if (fmodeventmap.find(eventname.data()) == fmodeventmap.end())
			fmodeventmap[eventname.data()] = event;

		else error::Audio::MultipleRegister(eventname.data());

	}

// load buses - - - - - - - - - - - - - - - - - - - - - - - - - 

	std::vector<FMOD::Studio::Bus*> bustable;
	getbuslist(fmodbank, bustable);

	std::vector<char> busname(512);	
	for (auto bus : bustable) {

		bus->getPath(busname.data(), 512, nullptr);

		if (fmodbusmap.find(busname.data()) == fmodbusmap.end())
			fmodbusmap[busname.data()] = bus;

		else error::Audio::MultipleRegister(busname.data());

	}

}

void AudioBase::UnloadFmodbank(std::uintptr_t bankID) {

	if (!fmodbankmap.Remove(bankID)) return;

	FMOD::Studio::Bank* fmodbank(reinterpret_cast<FMOD::Studio::Bank*>(bankID));

// unload events - - - - - - - - - - - - - - - - - - - - - - - - -

	std::vector<FMOD::Studio::EventDescription*> eventtable;
	geteventlist(fmodbank, eventtable);

	std::vector<char> eventname(512);
	for (auto event : eventtable) {

		event->getPath(eventname.data(), 512, nullptr);
		fmodeventmap.erase(eventname.data());	

	}

// unload buses - - - - - - - - - - - - - - - - - - - - - - - - - 

	std::vector<FMOD::Studio::Bus*> bustable;
	getbuslist(fmodbank, bustable);

	std::vector<char> busname(512);
	for (auto bus : bustable) {

		bus->getPath(busname.data(), 512, nullptr);
		fmodbusmap.erase(busname.data());	

	}

// unload bank (rest) - - - - - - - - - - - - - - - - - - - - - - - - -

	fmodbank->unloadSampleData();
	fmodbank->unload();

}

void AudioBase::PlayEvent(const std::string& eventname, InstanceInfo& info) {

	auto iter = fmodeventmap.find(eventname);

	if (iter != fmodeventmap.end()) {

		info.destruct();

		FMOD::Studio::EventInstance* eventinstance(nullptr);
		iter->second->createInstance(&eventinstance);
		if (!eventinstance) error::Audio::CreateEventInstanceFalse(); 

		InstanceLinkage* linkage(new InstanceLinkage(eventinstance, &info));
		if (instancemap.Register(linkage)) {
			info.id = instancemap.getcurrentpoint();
			eventinstance->start();
		}
		else delete linkage;	
		
	}

	else error::Audio::FMODEventNotExist(eventname);

}

void AudioBase::InstanceManage(InstanceInfo& info, Playmode mode, float var/* = 0.0f*/, const std::string& name/* = ""*/) {

	if (info.id) {

		InstanceLinkage& linkage(*instancemap[info.id]);
		
		switch (mode) {

		case Playmode::Play: {
			bool paused(false);
			linkage.instance->getPaused(&paused);
			if (paused) linkage.instance->setPaused(false);
			else linkage.instance->start();
			break;
		}
		case Playmode::Pause: {
			linkage.instance->setPaused(true);
			break;
		}
		case Playmode::Restart: {
			linkage.instance->start();
			break;
		}
		case Playmode::Stop: {
			linkage.instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			linkage.info = nullptr;	
			info.id = 0;
			break;
		}
		case Playmode::FadeoutStop: {
			linkage.instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
			break;
		}
		case Playmode::Volume: {
			linkage.instance->setVolume(var);
			break;
		}
		case Playmode::Pitch: {
			linkage.instance->setPitch(var);
			break;
		}
		case Playmode::setParByName: {
			if (linkage.instance->setParameterByName(name.c_str(), var) == FMOD_ERR_EVENT_NOTFOUND)
				error::Audio::NoEventParameterName(name);
			break;
		}
		default:
			break;
		}
		
	}

	else error::Audio::NoEventInstance();	

}

FMOD_VECTOR fmodvec(const Vector3& vec);

void AudioBase::SetInstance(InstanceInfo& info, const GLMatrix& mat) {

	if (info.id) {

		InstanceLinkage& linkage(*instancemap[info.id]);

		FMOD_3D_ATTRIBUTES attributes;

		attributes.position = fmodvec(mat.pos);	
		attributes.forward = fmodvec(mat.unitary.Read_Zunit());
		attributes.up = fmodvec(mat.unitary.Read_Yunit());
		attributes.velocity = fmodvec(Vector3(0.0f, 0.0f, 0.0f)); 

		linkage.instance->set3DAttributes(&attributes);

	}
}

void AudioBase::BusManage(const std::string& busname, Busmode mode, float var/* = 0.0f*/) {

	auto iter = fmodbusmap.find(busname);

	if (iter != fmodbusmap.end()) {

		switch (mode) {

		case Busmode::BusPause: {
			iter->second->setPaused(true);
			break;
		}
		case Busmode::BusVolume: {
			if (var > 1.0f) var = 1.0f;
			else if (var < 0.0f) var = 0.0f;
			iter->second->setVolume(var);
			break;
		}
		default:
			break;
		}

	}
	else error::Audio::AudioBusNotExist(busname);

}

void AudioBase::geteventlist(const FMOD::Studio::Bank* fmodbank, std::vector<FMOD::Studio::EventDescription*>& table) {

	int eventnums(0);
	fmodbank->getEventCount(&eventnums);

	table.resize(eventnums);
	fmodbank->getEventList(table.data(), eventnums, &eventnums);	

}	

void AudioBase::getbuslist(const FMOD::Studio::Bank* fmodbank, std::vector<FMOD::Studio::Bus*>& table) {

	int busnums(0);
	fmodbank->getBusCount(&busnums);

	table.resize(busnums);
	fmodbank->getBusList(table.data(), busnums, &busnums);	

}

FMOD_VECTOR fmodvec(const Vector3& vec) {

	FMOD_VECTOR fmodvec;	
	fmodvec.x = vec[0];
	fmodvec.y = vec[2];
	fmodvec.z = vec[1];

	return fmodvec;
}

/***************************************************************************
	satellite
****************************************************************************/

void satellite::Camera::Listener(const Vector3& pos, const Vector3& up, const Vector3& foward) {
	
	FMOD_3D_ATTRIBUTES listener;

	listener.position = fmodvec(pos);
	listener.forward = fmodvec(up);
	listener.up = fmodvec(foward);
	listener.velocity = fmodvec(Vector3(0.0f, 0.0f, 0.0f)); 

	AudioBase::mSystem->setListenerAttributes(0, &listener);

}
void satellite::Game::Initialize(void) {
 
	FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_ERROR, FMOD_DEBUG_MODE_TTY);
	error::Audio::CreateFMODStdSysCheck(FMOD::Studio::System::create(&AudioBase::mSystem));
	error::Audio::FMODStdSysInitCheck(AudioBase::mSystem->initialize(maxchannel, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_CHANNEL_LOWPASS, nullptr));	
	//mSystem->getLowLevelSystem()

	AudioBase::LoadFmodbank("Assets/Audio/Master Bank.strings.bank");		
	AudioBase::LoadFmodbank("Assets/Audio/Master Bank.bank"); 

	AudioBase::instancemap.Register(new AudioBase::InstanceLinkage);

}
void satellite::Game::Update(void) {

	AudioBase::mSystem->update();

	for (unsigned int n(1); n < AudioBase::instancemap.getlength(); ++n) {

		AudioBase::InstanceLinkage* linkage(AudioBase::instancemap[n]);

		if (linkage) { 
			FMOD_STUDIO_PLAYBACK_STATE state;
			linkage->instance->getPlaybackState(&state);
			if (state == FMOD_STUDIO_PLAYBACK_STOPPED) AudioBase::instancemap.Remove(n);
		}
	}

}
void satellite::Game::Shutdown(void) { AudioBase::mSystem->release(); }

/***************************************************************************
	
	Audio

****************************************************************************/

Audio::Audio(void) : manager(*new InstanceManager), ObjectCore() {}
Audio::~Audio(void) { delete &manager; }

void Audio::update(void) {
	
	for (auto& info : manager.instanceinfotable) {
		AudioBase::SetInstance(info, mat);
	}

}
void Audio::PlaySound(const std::string& eventname, audio::enumInstanceInfo number) {
	AudioBase::PlayEvent(eventname, manager.instanceinfotable[number]);
}