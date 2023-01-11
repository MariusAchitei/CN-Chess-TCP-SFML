all:
	g++ client.cpp -o client -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
	g++ sv.cpp -pthread -o sv