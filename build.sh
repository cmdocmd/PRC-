g++ -g events.cpp main.cpp packets.cpp players.cpp server.cpp utils.cpp worlds.cpp mysql.cpp items.cpp dialog.cpp pathfinder.cpp commands.cpp -o server  -std=c++20 -L. -lboost_serialization -lboost_system  -lenet -iquote. -L/usr/lib -lmysqlcppconn