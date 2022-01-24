#pragma once

#include <irrKlang.h>
#include <iostream>
#include <string>
#include <vector>

#include "D:/Faculta/An 3/Sem 1/PG/glm/glm.hpp"

namespace audio {
	class MediaPlayer {
		public:
			MediaPlayer(irrklang::vec3df newPosition);
			void setListenerPosition(glm::vec3 listenerPosition,
									 glm::vec3 lookDirection,
									 glm::vec3 upVector);

			void addAudioFile(std::string audiofile);

			void playSong();
			void nextSong();
			void prevSong();
			
			void playSoundEffect(std::string audioFile);

			void pauseSong();

			void volUp();
			void volDown();
		private:
			int crtSong;
			float volume;
			irrklang::vec3df position;
			std::vector<std::string> songs;
			irrklang::ISound* currentSong;
			irrklang::ISoundEngine* soundEngine;

			irrklang::vec3df listenerPosition;
			irrklang::vec3df lookDirection;
			irrklang::vec3df velPerSecond = irrklang::vec3df(0, 0, 0);
			irrklang::vec3df upVector;
	};
}
