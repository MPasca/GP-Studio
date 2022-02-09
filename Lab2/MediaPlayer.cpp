#include "MediaPlayer.h"

namespace audio {
	MediaPlayer::MediaPlayer(irrklang::vec3df newPosition) {
		this->crtSong = 1;
		this->volume = 0.4f;
		this->position = newPosition;

		this->soundEngine = irrklang::createIrrKlangDevice();
		if (this->soundEngine) {
			this->soundEngine->setDefault3DSoundMinDistance(15.0f);
		}
		this->currentSong = nullptr;
	}

	void MediaPlayer::setListenerPosition(glm::vec3 listenerPosition,
										  glm::vec3 lookDirection,
										  glm::vec3 upVector) {
		this->listenerPosition = irrklang::vec3df(listenerPosition.x, listenerPosition.y, listenerPosition.z);
		this->lookDirection = irrklang::vec3df(lookDirection.x, lookDirection.y, lookDirection.z);
		this->upVector = irrklang::vec3df(upVector.x, upVector.y, upVector.z);

		soundEngine->setListenerPosition(this->listenerPosition, this->lookDirection, velPerSecond, this->upVector);
	}

	void MediaPlayer::addAudioFile(std::string audioFile) {
		songs.push_back(audioFile);
	}

	void MediaPlayer::playSong() {
		int index = 0;
		for (std::string s : songs) {
			if (index == crtSong) {
				std::string stringFile = "./audios/" + s + ".mp3";
				//currentSong = soundEngine->play3D(stringFile.c_str(), position, false, false, true);
				currentSong = soundEngine->play2D(stringFile.c_str(), false, false, true);

				std::cout << "current song: " << s.c_str() << '\n';
				currentSong->setVolume(volume);
			}
			index++;
		}
	}

	void MediaPlayer::playSoundEffect(std::string audioFile) {
		std::string stringFile = "./audios/effects/" + audioFile + ".mp3";
		soundEngine->play2D(stringFile.c_str(), true, false, false);
		soundEngine->setSoundVolume(0.05f);
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
		std::cout << "volume: " << volume << "\n";
		currentSong->setVolume(volume);
	}

	void MediaPlayer::volDown() {
		volume -= 0.1f;
		std::cout << "volume: " << volume << "\n";
		currentSong->setVolume(volume);
	}

	void MediaPlayer::pauseSong() {
		soundEngine->setAllSoundsPaused();
	}
}