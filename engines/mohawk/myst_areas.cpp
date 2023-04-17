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

#include "mohawk/myst_areas.h"
#include "mohawk/myst_card.h"
#include "mohawk/myst_graphics.h"
#include "mohawk/myst_scripts.h"
#include "mohawk/myst_sound.h"
#include "mohawk/video.h"

#include "common/events.h"
#include "common/system.h"
#include "common/textconsole.h"

namespace Mohawk {

MystArea::MystArea(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		_vm(vm),
		_parent(parent),
		_type(type) {

	if (parent == nullptr) {
		_flags = rlstStream->readUint16LE();
		_rect.left = rlstStream->readSint16LE();
		_rect.top = rlstStream->readSint16LE();

		if (_rect.top == -1) {
			debugC(kDebugResource, "Invalid _rect.top of -1 found - clamping to 0");
			_rect.top = 0;
		}

		_rect.right = rlstStream->readSint16LE();
		_rect.bottom = rlstStream->readSint16LE();
		_dest = rlstStream->readUint16LE();
	} else {
		_flags = parent->_flags;
		_rect.left = parent->_rect.left;
		_rect.top = parent->_rect.top;
		_rect.right = parent->_rect.right;
		_rect.bottom = parent->_rect.bottom;
		_dest = parent->_dest;
	}

	debugC(kDebugResource, "\tflags: 0x%04X", _flags);
	debugC(kDebugResource, "\tleft: %d", _rect.left);
	debugC(kDebugResource, "\ttop: %d", _rect.top);
	debugC(kDebugResource, "\tright: %d", _rect.right);
	debugC(kDebugResource, "\tbottom: %d", _rect.bottom);
	debugC(kDebugResource, "\tdest: %d", _dest);
}

MystArea::~MystArea() {
}

void MystArea::handleMouseUp() {
	if (_dest == 0) {
		warning("Movement type resource with null destination at position (%d, %d), (%d, %d)", _rect.left, _rect.top, _rect.right, _rect.bottom);
		return;
	}

	uint16 opcode;

	switch (_type) {
	case kMystAreaForward:
		opcode = 6;
		break;
	case kMystAreaLeft:
		opcode = 8;
		break;
	case kMystAreaRight:
		opcode = 7;
		break;
	default:
		opcode = 48;
		break;
	}

	_vm->_stack->setInvokingResource(this);
	_vm->_stack->runOpcode(opcode, 0);
}

bool MystArea::canBecomeActive() {
	return !unreachableZipDest() && (isEnabled() || (_flags & kMystUnknownFlag));
}

bool MystArea::unreachableZipDest() {
	return (_flags & kMystZipModeEnableFlag)
			&& !_vm->_gameState->isReachableZipDest(_vm->_stack->getStackId() , _dest);
}

bool MystArea::isEnabled() {
	return _flags & kMystHotspotEnableFlag;
}

void MystArea::setEnabled(bool enabled) {
	if (enabled)
		_flags |= kMystHotspotEnableFlag;
	else
		_flags &= ~kMystHotspotEnableFlag;
}

const Common::String MystArea::describe() {
	Common::String desc = Common::String::format("type: %2d rect: (%3d %3d %3d %3d)",
			_type, _rect.left, _rect.top, _rect.width(), _rect.height());

	if (_dest != 0)
		desc += Common::String::format(" dest: %4d", _dest);

	return desc;
}

void MystArea::drawBoundingRect() {
	if (_rect.isValidRect()) {
		if (!canBecomeActive())
			_vm->_gfx->drawRect(_rect, kRectUnreachable);
		else if (isEnabled())
			_vm->_gfx->drawRect(_rect, kRectEnabled);
		else
			_vm->_gfx->drawRect(_rect, kRectDisabled);
	}
}

MystAreaAction::MystAreaAction(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		MystArea(vm, type, rlstStream, parent) {
	debugC(kDebugResource, "\tResource Type 5 Script:");

	_script = vm->_stack->readScript(rlstStream, kMystScriptNormal);
}

void MystAreaAction::handleMouseUp() {
    if (_vm->_stack->getStackId() == kMystStack && _type == kMystAreaAction)
    {
        switch (_vm->getCard()->getId())
        {
            case 4159:
            case 4160:
            {
                if (this->getRect().right < 80)
                {
                    Common::Rect rectout = Common::Rect(43,166,77,194);
                    _vm->_gfx->copyImageSectionToScreen(4159, rectout, rectout);
                }
                break;
            }
            case 4162:
            case 4166:
            {
                if (this->getRect().right < 80)
                {
                    Common::Rect rectout = Common::Rect(34,162,75,219);
                    _vm->_gfx->copyImageSectionToScreen(4162, rectout, rectout);
                }
                break;
            }
        }
    }
    
	// Keep a reference to the stack so it is not freed if a script switches to another stack
	MystScriptParserPtr stack = _vm->_stack;

	stack->runScript(_script, this);
}

void MystAreaAction::handleMouseDown() {
    
    if (_vm->_stack->getStackId() == kMystStack && _type == kMystAreaAction)
    {
        switch (_vm->getCard()->getId())
        {
            case 4006:
            {
                _vm->playSoundBlocking(4006);
                int16 theXout = 322;
                int16 theYout = 232;
                Common::Rect rectout = Common::Rect(theXout,theYout,theXout + 50,theYout + 42);
                Common::Rect rectin = Common::Rect(theXout,58,theXout + 50,58 + 42);
                _vm->_gfx->copyImageSectionToBackBuffer(4006, rectin, rectout);
                _vm->_gfx->copyImageSectionToScreen(4007, Common::Rect(50,42), rectout);
                break;
            }
            case 4159:
            case 4160:
            {
                if (this->getRect().right < 80)
                {
                    _vm->playSoundBlocking(4006);
                    int16 theXout = 43;
                    int16 theYout = 166;
                    Common::Rect rectout = Common::Rect(theXout,theYout,theXout + 34,theYout + 28);
                    _vm->_gfx->copyImageSectionToScreen(4947, Common::Rect(34,28), rectout);
                }
                break;
            }
            case 4162:
            case 4166:
            {
                if (this->getRect().right < 80)
                {
                    _vm->playSoundBlocking(4006);
                    int16 theXout = 34;
                    int16 theYout = 162;
                    Common::Rect rectout = Common::Rect(theXout,theYout,theXout + 41,theYout + 57);
                    _vm->_gfx->copyImageSectionToScreen(4948, Common::Rect(41,57), rectout);
                }
                break;
            }
            case 4489:
            {
                if (this->getRect().left > 320)
                {
                    _vm->playSoundBlocking(4006);
                    int16 theXout = 423;
                    int16 theYout = 148;
                    Common::Rect rectout = Common::Rect(theXout,theYout,theXout + 37,theYout + 38);
                    _vm->_gfx->copyImageSectionToBackBuffer(4489, rectout, rectout);
                    _vm->_gfx->copyImageSectionToScreen(4485, Common::Rect(37,38), rectout);
                    break;
                }
            }
            case 4490:
            {
                if (this->getRect().left > 320)
                {
                _vm->playSoundBlocking(4006);
                int16 theXout = 415;
                int16 theYout = 147;
                Common::Rect rectout = Common::Rect(theXout,theYout,theXout + 40,theYout + 49);
                _vm->_gfx->copyImageSectionToBackBuffer(4490, rectout, rectout);
                _vm->_gfx->copyImageSectionToScreen(4486, Common::Rect(40,49), rectout);
                break;
                }
            }
            default:
                break;
        }
    }
}

const Common::String MystAreaAction::describe() {
	Common::String desc = MystArea::describe();

	if (!_script.empty()) {
		desc += " ops:";

		for (uint i = 0; i < _script.size(); i++)
			desc += " " + _vm->_stack->getOpcodeDesc(_script[i].opcode);
	}

	return desc;
}

// In Myst/Making of Myst, the paths are hardcoded ala Windows style without extension. Convert them.
Common::String MystAreaVideo::convertMystVideoName(const Common::String &name) {
	Common::String temp;

	for (uint32 i = 1; i < name.size(); i++) {
		if (name[i] == '\\')
			temp += '/';
		else
			temp += name[i];
	}

	return temp + ".mov";
}

MystAreaVideo::MystAreaVideo(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		MystAreaAction(vm, type, rlstStream, parent) {
	char c = 0;

	while ((c = rlstStream->readByte()) != 0) {
		_videoFile += c;
	}

	if ((_videoFile.size() & 1) == 0) {
		rlstStream->skip(1);
	}

	_videoFile = convertMystVideoName(_videoFile);
	_videoFile = _vm->selectLocalizedMovieFilename(_videoFile);

	// Position values require modulus 10000 to keep in sane range.
	_left = rlstStream->readSint16LE() % 10000;
	_top = rlstStream->readSint16LE() % 10000;
	_playOnCardChange = rlstStream->readUint16LE() != 0;
	_direction = rlstStream->readSint16LE();
	_playBlocking = rlstStream->readUint16LE();
	_loop = rlstStream->readUint16LE();
	_playRate = rlstStream->readUint16LE();

	// WORKAROUND: Myst v1.0 has playOnCardChange set to true
	// for the Myst flyby video shown during the intro.
	// This causes the flyby to play over the closed Myst book picture.
	// Later releases of the game have that flag set to false.
	// Here we apply a resource patch to match the newer releases.
	if (_videoFile == "qtw/intro/intro2.mov") {
		_playOnCardChange = false;
	}

	debugC(kDebugResource, "\tvideoFile: \"%s\"", _videoFile.c_str());
	debugC(kDebugResource, "\tleft: %d", _left);
	debugC(kDebugResource, "\ttop: %d", _top);
	debugC(kDebugResource, "\tloop: %d", _loop);
	debugC(kDebugResource, "\tdirection: %d", _direction);
	debugC(kDebugResource, "\tplayBlocking: %d", _playBlocking);
	debugC(kDebugResource, "\tplayOnCardChange: %d", _playOnCardChange);
	debugC(kDebugResource, "\tplayRate: %d", _playRate);
}

VideoEntryPtr MystAreaVideo::playMovie() {
	// Check if the video is already running
	VideoEntryPtr handle = _vm->_video->findVideo(_videoFile);

	// If the video is not running, play it
	if (!handle) {
		handle = _vm->_video->playMovie(_videoFile, Audio::Mixer::kSFXSoundType);
		if (!handle)
			error("Failed to open '%s'", _videoFile.c_str());

		handle->moveTo(_left, _top);
		handle->setLooping(_loop != 0);

		Common::Rational rate;
		if (_playRate != 0) {
			rate = Common::Rational(_playRate, 100);
		} else {
			rate = 1;
		}

		if (_direction == -1) {
			rate = -rate;
			handle->seek(handle->getDuration());
		}

		handle->setRate(rate);
	} else {
		// Resume the video
		handle->pause(false);
		handle->start();
	}

	if (_playBlocking) {
		_vm->waitUntilMovieEnds(handle);
		return VideoEntryPtr();
	}

	return handle;
}

VideoEntryPtr MystAreaVideo::getVideo() {
	// If the video is already in the manager, just return the handle
	VideoEntryPtr handle = _vm->_video->findVideo(_videoFile);
	if (!handle) {
		// If the video has not been loaded yet, do it but don't start playing it
		handle = _vm->_video->playMovie(_videoFile, Audio::Mixer::kSFXSoundType);
		if (!handle)
			error("Failed to open '%s'", _videoFile.c_str());
		handle->stop();
	}

	return handle;
}

void MystAreaVideo::handleCardChange() {
	if (_playOnCardChange)
		playMovie();
}

bool MystAreaVideo::isPlaying() {
	VideoEntryPtr handle = _vm->_video->findVideo(_videoFile);
	return handle && !handle->endOfVideo();
}

void MystAreaVideo::pauseMovie(bool pause) {
	VideoEntryPtr handle = _vm->_video->findVideo(_videoFile);
	if (handle && !handle->endOfVideo())
		handle->pause(pause);
}

MystAreaActionSwitch::MystAreaActionSwitch(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		MystArea(vm, type, rlstStream, parent) {
	_actionSwitchVar = rlstStream->readUint16LE();
	uint16 numSubResources = rlstStream->readUint16LE();
	debugC(kDebugResource, "\tactionSwitchVar: %d", _actionSwitchVar);
	debugC(kDebugResource, "\tnumSubResources: %d", numSubResources);

	for (uint16 i = 0; i < numSubResources; i++)
		_subResources.push_back(vm->loadResource(rlstStream, this));
}

MystAreaActionSwitch::~MystAreaActionSwitch() {
	for (uint32 i = 0; i < _subResources.size(); i++)
		delete _subResources[i];

	_subResources.clear();
}

void MystAreaActionSwitch::doSwitch(AreaHandler handler) {
	if (_actionSwitchVar == 0xFFFF) {
		if (_subResources.size() == 1)
			(_subResources[0]->*handler)();
		else if (_subResources.size() != 0)
			warning("Action switch resource with _numSubResources of %d, but no control variable", _subResources.size());
	} else {
		uint16 varValue = _vm->_stack->getVar(_actionSwitchVar);

		if (_subResources.size() == 1 && varValue != 0)
			(_subResources[0]->*handler)();
		else if (_subResources.size() != 0) {
			if (varValue < _subResources.size())
				(_subResources[varValue]->*handler)();
			else
				warning("Action switch resource Var %d: %d exceeds number of sub resources %d", _actionSwitchVar, varValue, _subResources.size());
		}
	}
}

void MystAreaActionSwitch::drawDataToScreen() {
	doSwitch(&MystArea::drawDataToScreen);
}

void MystAreaActionSwitch::handleCardChange() {
	doSwitch(&MystArea::handleCardChange);
}

void MystAreaActionSwitch::handleMouseUp() {
    if (_vm->_stack->getStackId() == kStoneshipStack)
    {
        if (_vm->getCard()->getId() == 2185 && _type == kMystAreaImageSwitch)
            _vm->playSoundBlocking(2294);
    }
	doSwitch(&MystArea::handleMouseUp);
}

void MystAreaActionSwitch::handleMouseDown() {
    if (_vm->_stack->getStackId() == kStoneshipStack)
    {
        if (_vm->getCard()->getId() == 2185 && _type == kMystAreaImageSwitch)
        {
            int16 theXin1 = 0;
            int16 theYin = 54;
            int16 theXout1 = 193;
            int16 theYout = 132;
            int16 theXin2 = 52;
            int16 theXout2 = 245;
            int16 theXin3 = 104;
            int16 theXout3 = 297;
            Common::Rect rectin1 = Common::Rect(theXin1,theYin,theXin1 + 52,theYin + 54);
            Common::Rect rectin2 = Common::Rect(theXin2,theYin,theXin2 + 52,theYin + 54);
            Common::Rect rectin3 = Common::Rect(theXin3,theYin,theXin3 + 52,theYin + 54);
            Common::Rect rectout1 = Common::Rect(theXout1,theYout,theXout1 + 52,theYout + 54);
            Common::Rect rectout2 = Common::Rect(theXout2,theYout,theXout2 + 52,theYout + 54);
            Common::Rect rectout3 = Common::Rect(theXout3,theYout,theXout3 + 52,theYout + 54);
            if (rectout1.contains(_vm->_system->getEventManager()->getMousePos()))
                _vm->_gfx->copyImageSectionToScreen(2383, rectin1, rectout1);
            else if (rectout2.contains(_vm->_system->getEventManager()->getMousePos()))
                _vm->_gfx->copyImageSectionToScreen(2383, rectin2, rectout2);
            else if (rectout3.contains(_vm->_system->getEventManager()->getMousePos()))
                _vm->_gfx->copyImageSectionToScreen(2383, rectin3, rectout3);
            _vm->_sound->playEffect(2013);
        }
    }
    if ((_vm->getCard()->getId() == 1008 || _vm->getCard()->getId() == 1010))
    {
        Common::Rect rectin = Common::Rect(0,0,32,32);
        Common::Rect rectout = Common::Rect(417,219,417 + 32,219 + 32);
        _vm->_gfx->copyImageSectionToScreen(1009, rectin, rectout);
        _vm->_sound->playEffect(1008);
    }
    if ((_vm->getCard()->getId() == 1177 || _vm->getCard()->getId() == 1179 || _vm->getCard()->getId() == 1180))
    {
        Common::Rect rectin = Common::Rect(0,0,32,32);
        Common::Rect rectout = Common::Rect(129,136,129 + 32,136 + 32);
        _vm->_gfx->copyImageSectionToScreen(1178, rectin, rectout);
        _vm->_sound->playEffect(1008);
    }
	doSwitch(&MystArea::handleMouseDown);
}

MystAreaImageSwitch::MystAreaImageSwitch(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		MystAreaActionSwitch(vm, type, rlstStream, parent) {
	_imageSwitchVar = rlstStream->readUint16LE();
	uint16 numSubImages = rlstStream->readUint16LE();
	debugC(kDebugResource, "\tvar8: %d", _imageSwitchVar);
	debugC(kDebugResource, "\tnumSubImages: %d", numSubImages);

	for (uint16 i = 0; i < numSubImages; i++) {
		debugC(kDebugResource, "\tSubimage %d:", i);

		SubImage subImage;
		subImage.wdib = rlstStream->readUint16LE();
		subImage.rect.left = rlstStream->readSint16LE();

		if (subImage.rect.left != -1) {
			subImage.rect.top = rlstStream->readSint16LE();
			subImage.rect.right = rlstStream->readSint16LE();
			subImage.rect.bottom = rlstStream->readSint16LE();
        } else {
			// Use the hotspot rect as the source rect since the subimage is fullscreen
			// Convert to bitmap coordinates (upside down)
			subImage.rect.left = _rect.left;
			subImage.rect.top = 333 - _rect.bottom;
			subImage.rect.right = _rect.right;
			subImage.rect.bottom = 333 - _rect.top;
		}
        
        if (_vm->_stack->getStackId() == kMystStack && subImage.wdib == 4706) {
            subImage.rect.top = subImage.rect.top - 1;
            subImage.rect.bottom = subImage.rect.bottom - 1;
        }
        
        if (_vm->_stack->getStackId() == kChannelwoodStack && subImage.wdib == 3161) {
            if (_rect.top < 300)
                subImage.rect.top = subImage.rect.top + 1;
        }

		debugC(kDebugResource, "\twdib: %d", subImage.wdib);
		debugC(kDebugResource, "\tleft: %d", subImage.rect.left);
		debugC(kDebugResource, "\ttop: %d", subImage.rect.top);
		debugC(kDebugResource, "\tright: %d", subImage.rect.right);
		debugC(kDebugResource, "\tbottom: %d", subImage.rect.bottom);

		_subImages.push_back(subImage);
	}
}

MystAreaImageSwitch::~MystAreaImageSwitch() {
}

void MystAreaImageSwitch::drawDataToScreen() {
	// Need to call overridden function to ensure
	// switch section is processed correctly.
	MystAreaActionSwitch::drawDataToScreen();

	bool drawSubImage = false;
	int16 subImageId = 0;

	if (_imageSwitchVar == 0xFFFF) {
		if (_subImages.size() == 1) {
			subImageId = 0;
			drawSubImage = true;
		} else if (_subImages.size() != 0)
			warning("Image Switch resource with _numSubImages of %d, but no control variable", _subImages.size());
	} else {
		uint16 varValue = _vm->_stack->getVar(_imageSwitchVar);

		if (_subImages.size() == 1 && varValue != 0) {
			subImageId = 0;
			drawSubImage = true;
		} else if (_subImages.size() != 0) {
			if (varValue < _subImages.size()) {
				subImageId = varValue;
				drawSubImage = true;
			} else
				warning("Image Switch Var %d: %d exceeds number of subImages %d", _imageSwitchVar, varValue, _subImages.size());
		}
	}
    Common::Rect rectToDraw = _subImages[subImageId].rect;
    switch(_vm->getCard()->getId())
    {
        case 4331:
        case 4333:
        case 4334:
        case 4336:
        case 4338:
        case 4340:
        case 4345:
        case 4347:
        case 4352:
        case 4354:
        case 4355:
        case 4356:
        case 4365:
        case 4372:
        {
            _rect.top = _rect.top - 1;
            _rect.bottom = _rect.bottom - 1;
            break;
        }
        case 4361:
        case 4368:
        {
            _rect.top = _rect.top - 1;
            _rect.bottom = _rect.bottom - 1;
            _rect.left = _rect.left + 4;
            _rect.right = _rect.right + 4;
            if (_subImages[subImageId].wdib == 0xffff)
            {
                rectToDraw.left = rectToDraw.left + 4;
                rectToDraw.right = rectToDraw.right + 4;
            }
                
            break;
        }
        case 4601:
            _rect.bottom = _rect.bottom - 1;
            break;
        default:
            break;
    }

	if (drawSubImage) {
		uint16 imageToDraw = _subImages[subImageId].wdib;

		// This special case means redraw background
		if (imageToDraw == 0xFFFF)
        {
            imageToDraw = _vm->getCard()->getBackgroundImageId();
        }

		_vm->_gfx->copyImageSectionToBackBuffer(imageToDraw, rectToDraw, _rect);
	}
}

//TODO: Merge with the method above?
void MystAreaImageSwitch::drawConditionalDataToScreen(uint16 state, bool update) {
	bool drawSubImage = false;
	int16 subImageId = 0;


	if (_subImages.size() == 1 && state != 0) {
		subImageId = 0;
		drawSubImage = true;
	} else if (_subImages.size() != 0) {
		if (state < _subImages.size()) {
			subImageId = state;
			drawSubImage = true;
		} else
			warning("Image Switch Var %d: %d exceeds number of subImages %d", _imageSwitchVar, state, _subImages.size());
	}
    
    Common::Rect rectToDraw = _subImages[subImageId].rect;
    switch(_vm->getCard()->getId())
    {
        case 4361:
        case 4368:
        {
            if (_subImages[subImageId].wdib == 0xffff)
            {
                rectToDraw.left = rectToDraw.left + 4;
                rectToDraw.right = rectToDraw.right + 4;
            }
                
            break;
        }
        case 4601:
            //_rect.bottom = _rect.bottom - 1;
            break;
        default:
            break;
    }

	if (drawSubImage) {
		uint16 imageToDraw = _subImages[subImageId].wdib;
        

		// This special case means redraw background
		if (imageToDraw == 0xFFFF)
			imageToDraw = _vm->getCard()->getBackgroundImageId();

		// Draw to screen
		if (update) {
            switch(imageToDraw)
            {
                case 6016:
                case 6017:
                {
                    _vm->_gfx->copyImageSectionToBackBuffer(imageToDraw, _subImages[subImageId].rect, _rect);
                    _vm->_gfx->runTransition(kTransitionDissolve, _rect, 20, 0);
                    return;
                }
                default:
                    break;
            }
			_vm->_gfx->copyImageSectionToScreen(imageToDraw, rectToDraw, _rect);
		} else {
			_vm->_gfx->copyImageSectionToBackBuffer(imageToDraw, rectToDraw, _rect);
		}
	}
}

uint16 MystAreaImageSwitch::getImageSwitchVar() {
	return _imageSwitchVar;
}

MystAreaImageSwitch::SubImage MystAreaImageSwitch::getSubImage(uint index) const {
	return _subImages[index];
}

void MystAreaImageSwitch::setSubImageRect(uint index, const Common::Rect &rect) {
	_subImages[index].rect = rect;
}

const Common::String MystAreaImageSwitch::describe() {
	Common::String desc = Common::String::format("%s var: %2d",
	                                             MystAreaActionSwitch::describe().c_str(), _imageSwitchVar);

	if (_subImages.size() > 0) {
		desc +=  " subImgs:";

		for (uint i = 0; i < _subImages.size(); i++)
			desc += Common::String::format(" %d", (int16)_subImages[i].wdib);
	}

	return desc;
}

// No MystResourceType9!

MystAreaSlider::MystAreaSlider(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		MystAreaDrag(vm, type, rlstStream, parent) {
	_dragSound = rlstStream->readUint16LE();

	debugC(kDebugResource, "\tdrag sound : %d", _dragSound);

	_sliderWidth = _rect.right - _rect.left;
	_sliderHeight = _rect.bottom - _rect.top;
}

MystAreaSlider::~MystAreaSlider() {
}

void MystAreaSlider::setStep(uint16 step) {
	_rect.top = _minV + _stepV * step - _sliderHeight / 2;
	_rect.bottom = _rect.top + _sliderHeight;
	_subImages[0].rect.top = 333 - _rect.bottom - 1;
	_subImages[0].rect.bottom = 333 - _rect.top - 1;
}

void MystAreaSlider::setPosition(uint16 pos) {
	Common::Point mouse;
	mouse.y = pos;
	updatePosition(mouse);
}

Common::Rect MystAreaSlider::boundingBox() {
	Common::Rect bb;

	bb.top = _rect.top;
	bb.bottom = _rect.bottom;
	bb.left = _rect.left;
	bb.right = _rect.right;

	if (_flagHV & 1) {
		bb.left = _minH - _sliderWidth / 2;
		bb.right = _maxH + _sliderWidth / 2;
	}

	if (_flagHV & 2) {
		bb.top = _minV - _sliderHeight / 2;
		bb.bottom = _maxV + _sliderHeight / 2;
	}

	bb.grow(1);

	return bb;
}

void MystAreaSlider::restoreBackground() {
	// Restore background
	Common::Rect src = boundingBox();
	Common::Rect dest = boundingBox();
	src.top = 332 - dest.bottom;
	src.bottom = 332 - dest.top;
	_vm->_gfx->copyImageSectionToScreen(_vm->getCard()->getBackgroundImageId(), src, dest);
}

void MystAreaSlider::handleMouseDown() {
	const Common::Point &mouse = _vm->_system->getEventManager()->getMousePos();
	updatePosition(mouse);

	MystAreaDrag::handleMouseDown();

	// Restore background
	restoreBackground();

	// Draw slider
	drawConditionalDataToScreen(2);
}

void MystAreaSlider::handleMouseUp() {
	const Common::Point &mouse = _vm->_system->getEventManager()->getMousePos();
	updatePosition(mouse);

	// Restore background
	restoreBackground();

	// Draw slider
	drawConditionalDataToScreen(1);

	// Save slider value
	uint16 value = 0;
	if (_flagHV & 2) {
		if (_stepsV)
			value = (_pos.y - _minV) / _stepV;
		else
			value = _pos.y;
	} else if (_flagHV & 1) {
		if (_stepsH)
			value = (_pos.x - _minH) / _stepH;
		else
			value = _pos.x;
	}

	_vm->_stack->setVarValue(_imageSwitchVar, value);

	MystAreaDrag::handleMouseUp();
}

void MystAreaSlider::handleMouseDrag() {
	const Common::Point &mouse = _vm->_system->getEventManager()->getMousePos();
	updatePosition(mouse);

	MystAreaDrag::handleMouseDrag();

	// Restore background
	restoreBackground();

	// Draw slider
	drawConditionalDataToScreen(2);
}

void MystAreaSlider::updatePosition(const Common::Point &mouse) {
	bool positionChanged = false;

	Common::Point mouseClipped;
	setPositionClipping(mouse, mouseClipped);

	if (_flagHV & 2) {
		if (_stepV) {
			uint16 center = _minV + _stepV * (mouseClipped.y - _minV) / _stepV;
			uint16 top = center - _sliderHeight / 2;

			if (_rect.top != top || _pos.y != center) {
				positionChanged = true;
				_pos.y = center;
				_rect.top = top;
			}
		} else {
			positionChanged = true;
			_pos.y = mouseClipped.y;
			_rect.top = mouseClipped.y - _sliderHeight / 2;
		}

		if (positionChanged) {
			_rect.bottom = _rect.top + _sliderHeight;
			_subImages[0].rect.top = 332 - _rect.bottom;
			_subImages[0].rect.bottom = 332 - _rect.top;
		}
	}

	if (_flagHV & 1) {
		if (_stepH) {
			uint16 center = _minH + _stepH * (mouseClipped.x - _minH) / _stepH;
			uint16 left = center - _sliderWidth / 2;

			if (_rect.left != left || _pos.x != center) {
				positionChanged = true;
				_pos.x = center;
				_rect.left = left;
			}
		} else {
			positionChanged = true;
			_pos.x = mouseClipped.x;
			_rect.left = mouseClipped.x - _sliderWidth / 2;
		}

		if (positionChanged)
			_rect.right = _rect.left + _sliderWidth;
	}

	if (positionChanged && _dragSound)
		_vm->_sound->playEffect(_dragSound);
}

MystAreaDrag::MystAreaDrag(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		MystAreaImageSwitch(vm, type, rlstStream, parent) {
	_flagHV = rlstStream->readUint16LE();
	_minH = rlstStream->readUint16LE();
	_maxH = rlstStream->readUint16LE();
	_minV = rlstStream->readUint16LE();
	_maxV = rlstStream->readUint16LE();
	_stepsH = rlstStream->readUint16LE();
	_stepsV = rlstStream->readUint16LE();
	_mouseDownOpcode = rlstStream->readUint16LE();
	_mouseDragOpcode = rlstStream->readUint16LE();
	_mouseUpOpcode = rlstStream->readUint16LE();

	debugC(kDebugResource, "\tdirection: %d", _flagHV);
	debugC(kDebugResource, "\thorizontal min: %d", _minH);
	debugC(kDebugResource, "\thorizontal max: %d", _maxH);
	debugC(kDebugResource, "\tvertical min: %d", _minV);
	debugC(kDebugResource, "\tvertical max: %d", _maxV);
	debugC(kDebugResource, "\thorizontal steps: %d", _stepsH);
	debugC(kDebugResource, "\tvertical steps: %d", _stepsV);
	debugC(kDebugResource, "\t_mouseDownOpcode: %d", _mouseDownOpcode);
	debugC(kDebugResource, "\t_mouseDragOpcode: %d", _mouseDragOpcode);
	debugC(kDebugResource, "\t_mouseUpOpcode: %d", _mouseUpOpcode);

	debugCN(kDebugResource, "Type 11 _mouseDownOpcode: %d\n", _mouseDownOpcode);
	debugCN(kDebugResource, "Type 11 _mouseDragOpcode: %d\n", _mouseDragOpcode);
	debugCN(kDebugResource, "Type 11 _mouseUpOpcode: %d\n", _mouseUpOpcode);

	for (byte i = 0; i < ARRAYSIZE(_lists); i++) {
		debugC(kDebugResource, "\tList %d:", i);

		uint16 listCount = rlstStream->readUint16LE();
		debugC(kDebugResource, "\t%d values", listCount);

		for (uint16 j = 0; j < listCount; j++) {
			_lists[i].push_back(rlstStream->readUint16LE());
			debugC(kDebugResource, "\tValue %d: %d", j, _lists[i][j]);
		}
	}

	_stepH = 0;
	_stepV = 0;

	if (_stepsH)
		_stepH = (_maxH - _minH) / (_stepsH - 1);

	if (_stepsV)
		_stepV = (_maxV - _minV) / (_stepsV - 1);
}

MystAreaDrag::~MystAreaDrag() {
}

void MystAreaDrag::handleMouseDown() {
	const Common::Point &mouse = _vm->_system->getEventManager()->getMousePos();
	setPositionClipping(mouse, _pos);

	_vm->_stack->setInvokingResource(this);
	_vm->_stack->runOpcode(_mouseDownOpcode, _imageSwitchVar);
}

void MystAreaDrag::handleMouseUp() {
	const Common::Point &mouse = _vm->_system->getEventManager()->getMousePos();
	setPositionClipping(mouse, _pos);

	_vm->_stack->setInvokingResource(this);
	_vm->_stack->runOpcode(_mouseUpOpcode, _imageSwitchVar);
}

void MystAreaDrag::handleMouseDrag() {
	const Common::Point &mouse = _vm->_system->getEventManager()->getMousePos();
	setPositionClipping(mouse, _pos);

	_vm->_stack->setInvokingResource(this);
	_vm->_stack->runOpcode(_mouseDragOpcode, _imageSwitchVar);
}

const Common::String MystAreaDrag::describe() {
	return Common::String::format("%s down: %s drag: %s up: %s",
			MystAreaImageSwitch::describe().c_str(),
			_vm->_stack->getOpcodeDesc(_mouseDownOpcode).c_str(),
			_vm->_stack->getOpcodeDesc(_mouseDragOpcode).c_str(),
			_vm->_stack->getOpcodeDesc(_mouseUpOpcode).c_str());
}

void MystAreaDrag::setPositionClipping(const Common::Point &mouse, Common::Point &dest) {
	if (_flagHV & 2)
		dest.y = CLIP<uint16>(mouse.y, _minV, _maxV);

	if (_flagHV & 1)
		dest.x = CLIP<uint16>(mouse.x, _minH, _maxH);
}

uint16 MystAreaDrag::getList1(uint16 index) {
	return (index < _lists[0].size()) ? _lists[0][index] : 0;
}

uint16 MystAreaDrag::getList2(uint16 index) {
	return (index < _lists[1].size()) ?  _lists[1][index] : 0;
}

uint16 MystAreaDrag::getList3(uint16 index) {
	return (index < _lists[2].size()) ?  _lists[2][index] : 0;
}

MystVideoInfo::MystVideoInfo(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		MystAreaDrag(vm, type, rlstStream, parent) {
	_numFrames = rlstStream->readUint16LE();
	_firstFrame = rlstStream->readUint16LE();
	uint16 frameWidth = rlstStream->readUint16LE();
	uint16 frameHeight = rlstStream->readUint16LE();
	_frameRect.left = rlstStream->readUint16LE();
	_frameRect.top = rlstStream->readUint16LE();

	_frameRect.right = _frameRect.left + frameWidth;
	_frameRect.bottom = _frameRect.top + frameHeight;

	debugC(kDebugResource, "\t_numFrames: %d", _numFrames);
	debugC(kDebugResource, "\t_firstFrame: %d", _firstFrame);
	debugC(kDebugResource, "\tframeWidth: %d", frameWidth);
	debugC(kDebugResource, "\tframeHeight: %d", frameHeight);
	debugC(kDebugResource, "\t_frameRect.left: %d", _frameRect.left);
	debugC(kDebugResource, "\t_frameRect.top: %d", _frameRect.top);
	debugC(kDebugResource, "\t_frameRect.right: %d", _frameRect.right);
	debugC(kDebugResource, "\t_frameRect.bottom: %d", _frameRect.bottom);
}

MystVideoInfo::~MystVideoInfo() {
}

void MystVideoInfo::drawFrame(uint16 frame) {
	uint16 currentFrame = _firstFrame + frame;
	_vm->_gfx->copyImageToScreen(currentFrame, _frameRect);
}

bool MystVideoInfo::pullLeverV() {
	const Common::Point &mouse = _vm->_system->getEventManager()->getMousePos();

	// Make the handle follow the mouse
	int16 maxStep = getStepsV() - 1;
	Common::Rect rect = getRect();
	int16 step = ((mouse.y - rect.top) * getStepsV()) / rect.height();
	step = CLIP<int16>(step, 0, maxStep);

	// Draw current frame
	drawFrame(step);

	// Return true if lever fully pulled
	return step == maxStep;
}

void MystVideoInfo::releaseLeverV() {
	const Common::Point &mouse = _vm->_system->getEventManager()->getMousePos();

	// Get current lever frame
	int16 maxStep = getStepsV() - 1;
	Common::Rect rect = getRect();
	int16 step = ((mouse.y - rect.top) * getStepsV()) / rect.height();
	step = CLIP<int16>(step, 0, maxStep);

	// Release lever
	for (int i = step; i >= 0; i--) {
		drawFrame(i);
		_vm->doFrame();
	}
}

MystAreaHover::MystAreaHover(MohawkEngine_Myst *vm, ResourceType type, Common::SeekableReadStream *rlstStream, MystArea *parent) :
		MystArea(vm, type, rlstStream, parent) {
	_enterOpcode = rlstStream->readUint16LE();
	_leaveOpcode = rlstStream->readUint16LE();

	debugC(kDebugResource, "\t_enterOpcode: %d", _enterOpcode);
	debugC(kDebugResource, "\t_leaveOpcode: %d", _leaveOpcode);
}

void MystAreaHover::handleMouseEnter() {
	// Pass along the enter opcode to the script parser
	// The variable to use is stored in the dest field
	_vm->_stack->runOpcode(_enterOpcode, _dest);
}

void MystAreaHover::handleMouseLeave() {
	// Pass along the leave opcode (with no parameters) to the script parser
	// The variable to use is stored in the dest field
	_vm->_stack->runOpcode(_leaveOpcode, _dest);
}

void MystAreaHover::handleMouseUp() {
	// Type 13 Resources do nothing on Mouse Clicks.
	// This is required to override the inherited default
	// i.e. MystArea::handleMouseUp
}

const Common::String MystAreaHover::describe() {
	return Common::String::format("%s enter: %s leave: %s",
			MystArea::describe().c_str(),
			_vm->_stack->getOpcodeDesc(_enterOpcode).c_str(),
			_vm->_stack->getOpcodeDesc(_leaveOpcode).c_str());
}

} // End of namespace Mohawk
