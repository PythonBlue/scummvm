/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <time.h>

#include <mint/cookie.h>
#include <mint/osbind.h>

#define FORBIDDEN_SYMBOL_EXCEPTION_FILE
#define FORBIDDEN_SYMBOL_EXCEPTION_stdout
#define FORBIDDEN_SYMBOL_EXCEPTION_stderr
#define FORBIDDEN_SYMBOL_EXCEPTION_fputs
#define FORBIDDEN_SYMBOL_EXCEPTION_exit
#define FORBIDDEN_SYMBOL_EXCEPTION_time_h
#define FORBIDDEN_SYMBOL_EXCEPTION_getenv

#include "backends/platform/atari/osystem_atari.h"

#if defined(ATARI)
#include "backends/graphics/atari/atari-graphics-asm.h"
#include "backends/keymapper/hardware-input.h"
#include "backends/mutex/null/null-mutex.h"
#include "base/main.h"

#include "backends/saves/default/default-saves.h"
#include "backends/timer/default/default-timer.h"
#include "backends/events/atari/atari-events.h"
#include "backends/events/default/default-events.h"
#include "backends/mixer/atari/atari-mixer.h"
#include "backends/graphics/atari/atari-graphics.h"
#include "backends/graphics/atari/atari-graphics-superblitter.h"
#include "backends/graphics/atari/atari-graphics-supervidel.h"
#include "backends/graphics/atari/atari-graphics-videl.h"
#include "gui/debugger.h"

/*
 * Include header files needed for the getFilesystemFactory() method.
 */
#include "backends/fs/posix/posix-fs-factory.h"

extern "C" void atari_ikbd_init();
extern "C" void atari_ikbd_shutdown();

extern "C" void atari_200hz_init();
extern "C" void atari_200hz_shutdown();
extern "C" volatile uint32 counter_200hz;

extern void nf_init(void);
extern void nf_print(const char* msg);

OSystem_Atari::OSystem_Atari() {
	_fsFactory = new POSIXFilesystemFactory();
}

OSystem_Atari::~OSystem_Atari() {
	debug("OSystem_Atari::~OSystem_Atari()");

	if (_video_initialized) {
		Supexec(asm_screen_falcon_restore);
		_video_initialized = false;
	}

	if (_200hz_initialized) {
		Supexec(atari_200hz_shutdown);
		_200hz_initialized = false;
	}

	if (_ikbd_initialized) {
		Supexec(atari_ikbd_shutdown);
		_ikbd_initialized = false;
	}
}

static void critical_restore() {
	Supexec(asm_screen_falcon_restore);
	Supexec(atari_200hz_shutdown);
	Supexec(atari_ikbd_shutdown);
}

void OSystem_Atari::initBackend() {
	enum {
		MCH_ST = 0,
		MCH_STE,
		MCH_TT,
		MCH_FALCON,
		MCH_CLONE,
		MCH_ARANYM
	};

	long mch = MCH_ST<<16;
	Getcookie(C__MCH, &mch);
	mch >>= 16;

	if (mch != MCH_FALCON && mch != MCH_ARANYM) {
		error("ScummVM works only on Atari Falcon and ARAnyM");
	}

	if (mch == MCH_ARANYM && Getcookie(C_fVDI, NULL) == C_FOUND) {
		error("Disable fVDI, ScummVM accesses Videl directly");
	}

	nf_init();

	_timerManager = new DefaultTimerManager();
	_savefileManager = new DefaultSaveFileManager("saves");

	AtariEventSource *atariEventSource = new AtariEventSource();
	_eventManager = new DefaultEventManager(makeKeyboardRepeatingEventSource(atariEventSource));

	// AtariGraphicsManager needs _eventManager ready
	AtariGraphicsManager *atariGraphicsManager;
	if (hasSuperVidel())
		atariGraphicsManager = new AtariSuperVidelManager();
	else
		atariGraphicsManager = new AtariVidelManager();
	_graphicsManager = atariGraphicsManager;

	atariEventSource->setGraphicsManager(atariGraphicsManager);

	_mixerManager = new AtariMixerManager();
	// Setup and start mixer
	_mixerManager->init();

	Supexec(atari_ikbd_init);
	_ikbd_initialized = true;

	Supexec(atari_200hz_init);
	_200hz_initialized = true;

	Supexec(asm_screen_falcon_save);
	_video_initialized = true;

	(void)Setexc(VEC_PROCTERM, critical_restore);

	_startTime = counter_200hz;

	BaseBackend::initBackend();
}

Common::MutexInternal *OSystem_Atari::createMutex() {
	return new NullMutexInternal();
}

uint32 OSystem_Atari::getMillis(bool skipRecord) {
	// CLOCKS_PER_SEC is 200, so no need to use floats
	return 1000 * (counter_200hz - _startTime) / CLOCKS_PER_SEC;
}

void OSystem_Atari::delayMillis(uint msecs) {
	const uint32 threshold = getMillis() + msecs;
	while (getMillis() < threshold);
}

void OSystem_Atari::getTimeAndDate(TimeDate &td, bool skipRecord) const {
	//debug("getTimeAndDate");
	time_t curTime = time(0);
	struct tm t = *localtime(&curTime);
	td.tm_sec = t.tm_sec;
	td.tm_min = t.tm_min;
	td.tm_hour = t.tm_hour;
	td.tm_mday = t.tm_mday;
	td.tm_mon = t.tm_mon;
	td.tm_year = t.tm_year;
	td.tm_wday = t.tm_wday;
}

Common::KeymapArray OSystem_Atari::getGlobalKeymaps() {
	Common::KeymapArray globalMaps = BaseBackend::getGlobalKeymaps();

	Common::Keymap *keymap = ((AtariGraphicsManager*)_graphicsManager)->getKeymap();
	globalMaps.push_back(keymap);

	return globalMaps;
}

Common::HardwareInputSet *OSystem_Atari::getHardwareInputSet() {
	Common::CompositeHardwareInputSet *inputSet = new Common::CompositeHardwareInputSet();
	inputSet->addHardwareInputSet(new Common::MouseHardwareInputSet(Common::defaultMouseButtons));
	inputSet->addHardwareInputSet(new Common::KeyboardHardwareInputSet(Common::defaultKeys, Common::defaultModifiers));

	return inputSet;
}

void OSystem_Atari::quit() {
	debug("OSystem_Atari::quit()");

	g_system->destroy();

	exit(0);
}

void OSystem_Atari::logMessage(LogMessageType::Type type, const char *message) {
	FILE *output = 0;

	if (type == LogMessageType::kInfo || type == LogMessageType::kDebug)
		output = stdout;
	else
		output = stderr;

	fputs(message, output);
	fflush(output);

	nf_print(message);
}

void OSystem_Atari::addSysArchivesToSearchSet(Common::SearchSet &s, int priority) {
	{
		Common::FSDirectory currentDirectory{ getFilesystemFactory()->makeCurrentDirectoryFileNode()->getPath() };
		Common::FSNode dataNode = currentDirectory.getSubDirectory("data")->getFSNode();
		if (dataNode.exists() && dataNode.isDirectory() && dataNode.isReadable()) {
			s.addDirectory(dataNode.getPath(), dataNode, priority);
		}
	}
#ifdef DATA_PATH
	{
		// Add the global DATA_PATH to the directory search list
		// See also OSystem_SDL::addSysArchivesToSearchSet()
		Common::FSNode dataNode(DATA_PATH);
		if (dataNode.exists() && dataNode.isDirectory() && dataNode.isReadable()) {
			s.add(DATA_PATH, new Common::FSDirectory(dataNode, 4), priority);
		}
	}
#endif
}

Common::String OSystem_Atari::getDefaultConfigFileName() {
	const Common::String baseConfigName = OSystem::getDefaultConfigFileName();

	Common::String configFile;

	const char *envVar = getenv("HOME");
	if (envVar && *envVar) {
		configFile = envVar;
		configFile += '/';
		configFile += baseConfigName;

		if (configFile.size() < MAXPATHLEN)
			return configFile;
	}

	return baseConfigName;
}

void OSystem_Atari::update() {
	((DefaultTimerManager *)_timerManager)->checkTimers();
	((AtariMixerManager *)_mixerManager)->update();
}

OSystem *OSystem_Atari_create() {
	return new OSystem_Atari();
}

int main(int argc, char *argv[]) {
	g_system = OSystem_Atari_create();
	assert(g_system);

	// Invoke the actual ScummVM main entry point:
	int res = scummvm_main(argc, argv);
	g_system->destroy();
	return res;
}

#endif
