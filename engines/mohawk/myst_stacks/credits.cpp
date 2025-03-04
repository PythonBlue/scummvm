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

#include "mohawk/myst.h"
#include "mohawk/myst_areas.h"
#include "mohawk/myst_card.h"
#include "mohawk/myst_graphics.h"
#include "mohawk/cursors.h"
#include "mohawk/sound.h"
#include "mohawk/video.h"
#include "mohawk/myst_stacks/credits.h"

#include "common/system.h"

namespace Mohawk {
namespace MystStacks {

// NOTE: Credits Start Card is 10000

Credits::Credits(MohawkEngine_Myst *vm) :
		MystScriptParser(vm, kCreditsStack),
		_creditsRunning(false),
        	_creditsEnded(false),
		_curImage(0) {
	setupOpcodes();
}

Credits::~Credits() {
}

void Credits::setupOpcodes() {
	// "Stack-Specific" Opcodes
	REGISTER_OPCODE(100, Credits, o_quit);

	// "Init" Opcodes
	REGISTER_OPCODE(200, Credits, o_runCredits);
}

void Credits::disablePersistentScripts() {
	_creditsRunning = false;
    	_creditsEnded = true;
}

void Credits::runPersistentScripts() {
	if (!_creditsRunning && !_creditsEnded){
		_creditsRunning = true;
		_vm->getCard()->drawBackground();
        	_vm->_gfx->runTransition(kTransitionScrollToTop, Common::Rect(544, 333), 100, 0);
		_startTime = _vm->getTotalPlayTime();
		return;
	}

	if (_vm->getTotalPlayTime() - _startTime >= 15 * 1000) {
		_curImage++;

		// After the 6th image has shown, it's time to quit
		if (_curImage >= 7) {
			//_vm->quitGame();
            		_creditsEnded = true;
            		_creditsRunning = false;
			return;
		}

		// Draw next image
		_vm->getCard()->drawBackground();
		//_vm->_gfx->copyBackBufferToScreen(Common::Rect(544, 333));
		_vm->_gfx->runTransition(kTransitionScrollToTop, Common::Rect(544, 333), 100, 0);


		_startTime = _vm->getTotalPlayTime();
	}
}

uint16 Credits::getVar(uint16 var) {
	switch(var) {
	case 0: // Credits Image Control
		return _curImage;
	case 1: // Credits Music Control (Good / bad ending)
		return _globals.ending != kBooksDestroyed;
	default:
		return MystScriptParser::getVar(var);
	}
}

void Credits::o_runCredits(uint16 var, const ArgumentsArray &args) {
	// The credits stack does not have all the cursors, reset to the default cursor.
	_globals.heldPage = kNoPage;
	_vm->setMainCursor(kDefaultMystCursor);

	// Activate the credits
	_curImage = 0;
	_startTime = _vm->getTotalPlayTime();
	_creditsEnded = false;
	_vm->_gfx->clearScreen();
	_vm->_gfx->clearBackBuffer();
}

} // End of namespace MystStacks
} // End of namespace Mohawk
