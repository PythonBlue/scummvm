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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NANCY_ACTION_SECONDARYMOVIE_H
#define NANCY_ACTION_SECONDARYMOVIE_H

#include "engines/nancy/video.h"
#include "engines/nancy/renderobject.h"

#include "engines/nancy/action/actionrecord.h"

namespace Nancy {
namespace Action {

class PlaySecondaryMovie : public ActionRecord, public RenderObject {
public:
	static const byte kMovieSceneChange			= 5;
	static const byte kMovieNoSceneChange		= 6;

	static const byte kPlayerCursorAllowed		= 1;
	static const byte kNoPlayerCursorAllowed	= 2;

	static const byte kPlayMovieForward			= 1;
	static const byte kPlayMovieReverse			= 2;

	struct FlagAtFrame {
		int16 frameID;
		FlagDescription flagDesc;
	};

	PlaySecondaryMovie() : RenderObject(8) {}
	virtual ~PlaySecondaryMovie();

	void init() override;
	void updateGraphics() override;
	void onPause(bool pause) override;

	void readData(Common::SeekableReadStream &stream) override;
	void execute() override;

	Common::String _videoName; // 0x00
	Common::String _paletteName;

	uint16 _videoSceneChange = kMovieNoSceneChange; // 0x1C
	byte _playerCursorAllowed = kPlayerCursorAllowed; // 0x1E
	byte _playDirection = kPlayMovieForward; // 0x20, 2E
	uint16 _firstFrame = 0; // 0x22, 30
	uint16 _lastFrame = 0; // 0x24, 32
	Common::Array<FlagAtFrame> _frameFlags; // 0x26
	MultiEventFlagDescription _triggerFlags; // 0x80

	SoundDescription _sound; // 0xA8

	SceneChangeDescription _sceneChange; // 0xCA
	Common::Array<SecondaryVideoDescription> _videoDescs; // 0xD4

protected:
	Common::String getRecordTypeName() const override { return "PlaySecondaryMovie"; }
	bool isViewportRelative() const override { return true; }

	AVFDecoder _decoder;
	Graphics::ManagedSurface _fullFrame;
	int _curViewportFrame = -1;
	bool _isFinished = false;
};

} // End of namespace Action
} // End of namespace Nancy

#endif // NANCY_ACTION_SECONDARYMOVIE_H
