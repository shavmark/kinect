#pragma once


namespace Software2552 {
	class Router{
	public:
		void setup();
		void update();
		void bodyIndexFromTCP(ofImage& image);

		TCPServer server;
		TCPClient client;

	};
}