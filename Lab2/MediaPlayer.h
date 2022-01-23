#pragma once

#include <irrKlang.h>
#include <iostream>
#include <string>
#include <vector>

namespace audio {
	class MediaPlayer {
		public:
			MediaPlayer();

			void addAudioFile(std::string audiofile);

			void playSong();
			void nextSong();
			void prevSong();

			void pauseSong();

			void volUp();
			void volDown();
		private:
			int crtSong;
			int volume;
			std::vector<std::string> songs;
			irrklang::ISound* currentSong;
			irrklang::ISoundEngine* soundEngine;
	};
}
