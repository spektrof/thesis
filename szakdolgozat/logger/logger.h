#include <fstream>
#define LOG(x) do { std::ofstream file("logger/log.txt", std::fstream::out | std::fstream::app); file << x; file.close(); } while (0)
#define LOG2(x) do { std::ofstream file("logger/todebug.txt", std::fstream::out | std::fstream::app); file << x; file.close(); } while (0)