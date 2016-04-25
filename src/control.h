#pragma once


namespace Software2552 {
	class TCPReader {
	public:
		void setup();
		void update();
		void bodyIndexFromTCP(ofImage& image);
		void IRFromTCP(ofImage& image);
	private:
		TCPClient client;
	};
	class Router{
	public:
		void setup();
		void update();
		WriteOsc comms;
		void sendBodyIndex(string *, int clientID=-1);
		void sendIR(string *, int clientID = -1);
	private:
		TCPServer server;
	};
}