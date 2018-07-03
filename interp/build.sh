g++ -o main main.cpp -ldl -march=native -std=c++11 -I../SimpleDatabase/

echo 'Main finished compiling'

# build the libs imported by IcScript import.ics
g++ -shared -fPIC helloworld.cpp -o helloworld.so -std=c++11 -I../SimpleDatabase/