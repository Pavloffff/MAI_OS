# lsof -i | grep calcula | cut -f 2 -d ' ' | xargs kill -9
all:
	g++ control.cpp -lzmq -pthread -o control -w -g
	g++ calculation.cpp -lzmq -o calculation -w -g
	