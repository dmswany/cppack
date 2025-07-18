#include <string>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include "../msgpack/include/msgpack/msgpack.hpp"

using u64 = std::uint64_t;

// wrapped this way since I dynamically allocate them and I couldn't pack a struct with
// a pointer to the map
struct myMap {
  std::multimap<u64, u64> map;

  template<class T>
  void pack(T &pack) {
    pack(map);
  }

  auto size() {
    return(map.size());
  }
  auto insert(std::pair<u64, u64> p) {
    map.insert(p);
  }

  auto count(u64 key) {
    return(map.count(key));
  }
};

int main(int argc, char* argv[]) {
  std::string in_file = "./msgpack-test.txt";
  std::ifstream *in_stream = new std::ifstream;
  myMap *mm1 = new myMap;
  myMap *mm2 = new myMap;

  in_stream->open(in_file);
  if (in_stream->fail()) {
    printf("Can't open file: %s\n", in_file.c_str());
    exit(-1);
  }

  std::string line, part;
  u64 a, b;

  while (std::getline(*in_stream, line)) {
    part = line.substr(0, line.find(","));
    a = std::strtoull(part.c_str(), NULL, 16);
    line.erase(0, line.find(",") + 1);
    b = std::strtoull(line.c_str(), NULL, 16);
    mm1->map.insert(std::make_pair(a,b));
  }

  FILE* ofp = std::fopen("test-before.txt", "w");
  for (auto const& [ma, mb] : mm1->map) {
    fprintf(ofp, "%016lX,%016lX\n", ma, mb);
  }
  std::fclose(ofp);

  std::vector<uint8_t> data_vec = msgpack::pack(*mm1);
  *mm2 = msgpack::unpack<myMap>(data_vec);

  ofp = std::fopen("test-after.txt", "w");
  for (auto const& [ma, mb] : mm2->map) {
    fprintf(ofp, "%016lX,%016lX\n", ma, mb);
  }
  std::fclose(ofp);
}