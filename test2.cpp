#include "virus_genealogy.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

class Virus {
  public:
    typedef typename std::string id_type;
    Virus(id_type const _id) : id(_id) {
    }
    id_type get_id() const {
      return id;
    }
    Virus() { }
  private:
    id_type id;
};

int main() {
  VirusGenealogy<Virus> gen("A1H1");
  VirusGenealogy<Virus> gen2("ABCD");
  VirusGenealogy<Virus> gen3(gen); // NIE DZIAŁA
  gen = gen2; // NIE DZIAŁA
}
