#include "MediaPlayer.h"

namespace audio {
	MediaPlayer::MediaPlayer() {
		soundEngine = irrklang::createIrrKlangDevice();
		currentSong = nullptr;
		crtSong = 1;
		volume = 0.4f;
	}

	void MediaPlayer::addAudioFile(std::string audioFile) {
		songs.push_back(audioFile);
	}

	void MediaPlayer::playSong() {
		int index = 0;
		for (std::string s : songs) {
			if (index == crtSong) {
				std::string stringFile = "./" + s;
				currentSong = soundEngine->play2D(stringFile.c_str(), false, false, true);
				std::cout << "current song: " << s.c_str() << '\n';
				currentSong->setVolume(0.4f);
			}
			index++;
		}
	}

	void MediaPlayer::nextSong() {
		soundEngine->removeAllSoundSources();

		crtSong++;
		if (crtSong == songs.size()) {
			crtSong = 0;
		}

		playSong();
	}

	void MediaPlayer::prevSong() {
		soundEngine->removeAllSoundSources();

		crtSong--;
		if (crtSong < 0) {
			crtSong = songs.size() - 1;
		}

		playSong();
	}

	void MediaPlayer::volUp() {
		volume += 0.1f;
		currentSong->setVolume(volume);
	}

	void MediaPlayer::volDown() {
		volume -= 0.1f;
		currentSong->setVolume(volume);
	}

	void MediaPlayer::pauseSong() {
		soundEngine->setAllSoundsPaused();
	}
}