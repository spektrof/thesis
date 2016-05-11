/* Keszitette: Lukacs Peter
   Kulso fajlba torteno naplozas macro segitsegevel
*/
#include <fstream>
#define LOG(x) do { std::ofstream file("Logger/log.txt", std::fstream::out | std::fstream::app); file << x; file.close(); } while (0)