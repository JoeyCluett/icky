g++ -c -o Icky.o icky/Icky.cpp -I../SimpleDatabase -std=c++11 -march=native -O3 -Wall
g++ -c -o IckyInst.o icky/IckyInst.cpp -std=c++11 -march=native -O3 -Wall
g++ -c -o IckyPut.o icky/IckyPut.cpp -std=c++11 -march=native -O3 -Wall
g++ -c -o IckyVerify.o icky/IckyVerify.cpp -std=c++11 -march=native -O3 -Wall

g++ -o main main.cpp Icky.o IckyInst.o IckyPut.o IckyVerify.o -ldl -march=native -std=c++11 -I../SimpleDatabase/
