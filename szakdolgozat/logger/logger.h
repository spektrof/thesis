#include <fstream>
#define DEBUG(x) do { std::ofstream file("logger/log.txt", std::fstream::out | std::fstream::app); file << x; file.close(); } while (0)