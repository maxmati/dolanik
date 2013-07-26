
module Dolan {
	struct Song
	{
		string path;
		string title;
		string album;
		string artist;
	};
	interface Music {
		void play(Song s);
		Song getCurrentSong();
		void stop();
		double adjustVolume(double delta);
		void replay();
		void setEqualizer(int band, double amp);
		void resetEqualizer();
		void clearQueue();
		double getVolume();
		void setVolume(double volume);
	};
};
