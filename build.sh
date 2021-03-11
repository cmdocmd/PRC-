cd "enet"
cmake CMakeLists.txt
make
cp libenet.a ../libenet.a
cd ..
g++ -Wall -I/usr/include/cppconn -o "server" "main.cpp" -std=c++11 -L. -lboost_serialization -lboost_system -lenet -iquote. -L/usr/lib -lmysqlcppconn
