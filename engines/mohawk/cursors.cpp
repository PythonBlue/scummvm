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

#include "mohawk/cursors.h"
#include "mohawk/mohawk.h"
#include "mohawk/resource.h"

#include "common/macresman.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "common/formats/winexe_ne.h"
#include "common/formats/winexe_pe.h"
#include "graphics/cursorman.h"
#include "graphics/maccursor.h"
#include "graphics/wincursor.h"

#ifdef ENABLE_MYST
#include "mohawk/myst.h"
#include "mohawk/myst_graphics.h"
#endif

namespace Mohawk {

void CursorManager::showCursor() {
	CursorMan.showMouse(true);
}

void CursorManager::hideCursor() {
	CursorMan.showMouse(false);
}

void CursorManager::setDefaultCursor() {
	Graphics::Cursor *cursor = Graphics::makeDefaultWinCursor();

	CursorMan.replaceCursor(cursor);

	delete cursor;
}

void CursorManager::setCursor(uint16 id) {
	// For the base class, just use the default cursor always
	setDefaultCursor();
}
    
void CursorManager::setCursorAnimated(uint16 idNew, uint16 idOld) {
        // For the base class, just use the default cursor always
        setDefaultCursor();
}

void CursorManager::setMacCursor(Common::SeekableReadStream *stream) {
	assert(stream);

	Graphics::MacCursor *macCursor = new Graphics::MacCursor();

	if (!macCursor->readFromStream(*stream))
		error("Could not parse Mac cursor");

	CursorMan.replaceCursor(macCursor);

	delete macCursor;
	delete stream;
}

void DefaultCursorManager::setCursor(uint16 id) {
	setMacCursor(_vm->getResource(_tag, id));
}

#ifdef ENABLE_MYST

MystCursorManager::MystCursorManager(MohawkEngine_Myst *vm) : _vm(vm) {
}

MystCursorManager::~MystCursorManager() {
}

void MystCursorManager::showCursor() {
	CursorMan.showMouse(true);
}

void MystCursorManager::hideCursor() {
	CursorMan.showMouse(false);
}

void MystCursorManager::setCursor(uint16 id) {
	// Zero means empty cursor
	if (id == 0) {
		static const byte emptyCursor[4] = { 0, 0, 0, 0 };
		CursorMan.replaceCursor(&emptyCursor, 2, 2, 0, 0, 0);
		return;
	}

	Common::SeekableReadStream *clrcStream = _vm->getResource(ID_CLRC, id);
	uint16 hotspotX = clrcStream->readUint16LE();
	uint16 hotspotY = clrcStream->readUint16LE();
	delete clrcStream;

	// Both Myst and Myst ME use the "MystBitmap" format for cursor images.
	MohawkSurface *mhkSurface = _vm->_gfx->findImage(id);
	Graphics::Surface *surface = mhkSurface->getSurface();

	// Myst ME stores some cursors as 24bpp images instead of 8bpp
	if (surface->format.bytesPerPixel == 1) {
		// The transparent color is almost always 255, except for the main cursor (100)
		// in the D'ni archive, where it is 0.
		// Using the color of the first pixel as the transparent color for the main cursor always works.
		byte transparentColor;
		if (id == kDefaultMystCursor) {
			transparentColor = ((byte *)surface->getPixels())[0];
		} else {
			transparentColor = 255;
		}
		CursorMan.replaceCursor(surface->getPixels(), surface->w, surface->h, hotspotX, hotspotY, transparentColor);

		// We're using the screen palette for the original game, but we need
		// to use this for any 8bpp cursor in ME.
		if (_vm->isGameVariant(GF_ME))
			CursorMan.replaceCursorPalette(mhkSurface->getPalette(), 0, 256);
	} else {
		Graphics::PixelFormat pixelFormat = g_system->getScreenFormat();
		CursorMan.replaceCursor(surface->getPixels(), surface->w, surface->h, hotspotX, hotspotY, pixelFormat.RGBToColor(255, 255, 255), false, &pixelFormat);
	}
}


    
    void MystCursorManager::setCursorAnimated(uint16 idNew, uint16 idOld) {
        // Zero means empty cursor
        if (idNew == 0) {
            static const byte emptyCursor[4] = { 0, 0, 0, 0 };
            CursorMan.replaceCursor(&emptyCursor, 2, 2, 0, 0, 0);
            return;
        }
        Common::SeekableReadStream *clrcStream = _vm->getResource(ID_CLRC, idNew);
        uint16 hotspotX = clrcStream->readUint16LE();
        uint16 hotspotY = clrcStream->readUint16LE();
        delete clrcStream;
        
        // Both Myst and Myst ME use the "MystBitmap" format for cursor images.
        MohawkSurface *mhkSurface = _vm->_gfx->findImage(idNew);
        Graphics::Surface *surface = mhkSurface->getSurface();
        
        // Myst ME stores some cursors as 24bpp images instead of 8bpp
        if (surface->format.bytesPerPixel == 1) {
            // The transparent color is almost always 255, except for the main cursor (100)
            // in the D'ni archive, where it is 0.
            // Using the color of the first pixel as the transparent color for the main cursor always works.
            byte transparentColor;
            if (idNew == kDefaultMystCursor) {
                transparentColor = ((byte *)surface->getPixels())[0];
            } else {
                transparentColor = 255;
            }
            //_vm->_sound->playEffect(800);
            MohawkSurface *mhkSurface2 = _vm->_gfx->findImage(idOld + 3);
            int16 frameCountX = 0;
            int16 frameCountY = 0;
            while (frameCountX < 48)
            {
                mhkSurface2->setOffsetX((35 * frameCountX) - 1);
                mhkSurface2->setOffsetY((38 * frameCountY) - 1);
                Graphics::Surface *surface2 = mhkSurface2->getSurface();
                CursorMan.replaceCursor(surface2->getPixels(), 35, 38, hotspotX, hotspotY, transparentColor);
                if (frameCountX % 6 == 5)
                    frameCountY++;
                frameCountX++;
                _vm->wait(25);
            }
            CursorMan.replaceCursor(surface->getPixels(), surface->w, surface->h, hotspotX, hotspotY, transparentColor);
            
            // We're using the screen palette for the original game, but we need
            // to use this for any 8bpp cursor in ME.
            if (_vm->getFeatures() & GF_ME)
                CursorMan.replaceCursorPalette(mhkSurface->getPalette(), 0, 256);
        } else {
            Graphics::PixelFormat pixelFormat = g_system->getScreenFormat();
            //_vm->_sound->playEffect(800);
            MohawkSurface *mhkSurface2 = _vm->_gfx->findImage(idOld + 3);
            Graphics::Surface *surface2 = mhkSurface2->getSurface();
            Graphics::Surface frame;
            frame.create(35, 38, pixelFormat);
            
            int16 frameCount = 0;
            int16 frameCountX = 0;
            int16 frameCountY = 0;
            while (frameCount < 48)
            {
                for (int x = 0; x < 35; x++)
                {
                    for (int y = 0; y < 38; y++)
                    {
                        const uint32 *src = (const uint32 *)surface2->getBasePtr(x + (35 * frameCountX), y + (38 * frameCountY));
                        uint32 *dst = (uint32 *)frame.getBasePtr(x, y);
                        uint8 a, r, g, b;
                        pixelFormat.colorToARGB(*src++, a, r, g, b);
                        *dst++ = (uint32) pixelFormat.ARGBToColor(a, r, g, b);
                        //Graphics::colorToARGB< Graphics::ColorMasks<8888> >(*src++, a, r, g, b);
                        //*dst++ = (uint32) Graphics::ARGBToColor< Graphics::ColorMasks<8888> >(a, r, g, b);
                    }
                }
                CursorMan.replaceCursor(frame.getPixels(), frame.w, frame.h, hotspotX + 9, hotspotY + 10, pixelFormat.RGBToColor(255, 255, 255), false, &pixelFormat);
                _vm->wait(25);
                frameCount++;
                frameCountX++;
                if (frameCountX % 6 == 0)
                {
                    frameCountX = 0;
                    frameCountY++;
                }
            }
            CursorMan.replaceCursor(surface->getPixels(), surface->w, surface->h, hotspotX, hotspotY, pixelFormat.RGBToColor(255, 255, 255), false, &pixelFormat);
        }
    }

void MystCursorManager::setDefaultCursor() {
	setCursor(kDefaultMystCursor);
}

#endif

MacCursorManager::MacCursorManager(const Common::String &appName) {
	if (!appName.empty()) {
		_resFork = new Common::MacResManager();

		if (!_resFork->open(appName)) {
			// Not all have cursors anyway, so this is not a problem
			delete _resFork;
			_resFork = nullptr;
		}
	} else {
		_resFork = nullptr;
	}
}

MacCursorManager::~MacCursorManager() {
	delete _resFork;
}

void MacCursorManager::setCursor(uint16 id) {
	if (!_resFork) {
		setDefaultCursor();
		return;
	}

	// Try a color cursor first
	Common::SeekableReadStream *stream = _resFork->getResource(MKTAG('c','r','s','r'), id);

	// Fall back to monochrome cursors
	if (!stream)
		stream = _resFork->getResource(MKTAG('C','U','R','S'), id);

	if (stream)
		setMacCursor(stream);
	else
		setDefaultCursor();
}

LivingBooksCursorManager_v2::LivingBooksCursorManager_v2() {
	// Try to open the system archive if we have it
	_sysArchive = new MohawkArchive();

	if (!_sysArchive->openFile("system.mhk")) {
		delete _sysArchive;
		_sysArchive = nullptr;
	}
}

LivingBooksCursorManager_v2::~LivingBooksCursorManager_v2() {
	delete _sysArchive;
}

void LivingBooksCursorManager_v2::setCursor(uint16 id) {
	if (_sysArchive && _sysArchive->hasResource(ID_TCUR, id)) {
		setMacCursor(_sysArchive->getResource(ID_TCUR, id));
	} else {
		// TODO: Handle generated cursors
	}
}

void LivingBooksCursorManager_v2::setCursor(const Common::String &name) {
	if (!_sysArchive)
		return;

	uint16 id = _sysArchive->findResourceID(ID_TCUR, name);
	if (id == 0xffff)
		error("Could not find cursor '%s'", name.c_str());
	else
		setCursor(id);
}

NECursorManager::NECursorManager(const Common::String &appName) {
	Common::NEResources *exe = new Common::NEResources();
	if (exe->loadFromEXE(appName)) {
		// Not all have cursors anyway, so it's not a problem if this fails
		loadCursors(exe);
	}
	delete exe;
}

PECursorManager::PECursorManager(const Common::String &appName) {
	Common::PEResources *exe = new Common::PEResources();
	if (exe->loadFromEXE(appName)) {
		// Not all have cursors anyway, so it's not a problem if this fails
		loadCursors(exe);
	}
	delete exe;
}

WinCursorManager::~WinCursorManager() {
	for (uint i = 0; i < _cursors.size(); i++) {
		delete _cursors[i].cursorGroup;
	}
}

void WinCursorManager::loadCursors(Common::WinResources *exe) {
	const Common::Array<Common::WinResourceID> cursorGroups = exe->getIDList(Common::kWinGroupCursor);

	_cursors.resize(cursorGroups.size());
	for (uint i = 0; i < cursorGroups.size(); i++) {
		_cursors[i].id = cursorGroups[i].getID();
		_cursors[i].cursorGroup = Graphics::WinCursorGroup::createCursorGroup(exe, cursorGroups[i]);
	}
}

void WinCursorManager::setCursor(uint16 id) {
	for (uint i = 0; i < _cursors.size(); i++) {
		if (_cursors[i].id == id) {
			Graphics::Cursor *cursor = _cursors[i].cursorGroup->cursors[0].cursor;
			CursorMan.replaceCursor(cursor);
			return;
		}
	}

	// Last resort (not all have cursors)
	setDefaultCursor();
}

} // End of namespace Mohawk
