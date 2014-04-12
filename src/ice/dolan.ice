
module Dolan {
	struct Song
	{
		string path;
		string title;
		string album;
		string artist;
	};
	struct ServerInfo
	{
		string host;
		string port;
		string username;
		string password;
		string certFile;
		int id = -1; /*-1 if unused*/
	};
	sequence<ServerInfo> ServersInfo;
	interface Music {
		void play(int serverId, Song s);
		Song getCurrentSong(int serverId);
		void stop(int serverId);
		double adjustVolume(int serverId,double delta);
		void replay(int serverId);
		void setEqualizer(int serverId,int band, double amp);
		void resetEqualizer(int serverId);
		void clearQueue(int serverId);
		double getVolume(int serverId);
		void setVolume(int serverId, double volume);
	};
	interface Server {
		int connect(ServerInfo si);
		ServersInfo getServers();
		void disconnect(int id);
	};
};
