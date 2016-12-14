#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H
#include<map>
#include<set>
#include<vector>

class VirusNotFound : public std::exception {
	const char* what() const throw() {
		return "VirusNotFound";
	}
};

class VirusAlreadyCreated : public std::exception {
	const char* what() const throw() {
		return "VirusAlreadyCreated";
	}
};
class TriedToRemoveStemVirus : public std::exception {
	const char* what() const throw() {
		return "TriedToRemoveStemVirus";
	}
};

template <class Virus>
class VirusGenealogy {
  typedef typename Virus::id_type id_type;
  id_type stem_id;
  std::map<id_type, std::set<id_type>> childrens;
  std::map<id_type, std::set<id_type>> parents;
  std::map<id_type, Virus> elements;

  public:
  // Tworzy nową genealogię.
  // Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
  VirusGenealogy(id_type const &stem_id) {
    this->stem_id = stem_id;
    elements[stem_id] = Virus(stem_id);
  }

  // Zwraca identyfikator wirusa macierzystego.
  id_type get_stem_id() const {
    return stem_id;
  }

  // Zwraca listę identyfikatorów bezpośrednich następników wirusa
  // o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
  std::vector<id_type> get_children(id_type const &id) { // TODO dodać const!!
    if(!elements.count(id))throw VirusNotFound();
    return std::vector<id_type>(childrens[id].cbegin(), childrens[id].cend());
  }

  // Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
  // o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
  std::vector<id_type> get_parents(id_type const &id) { //TODO dodać const!!
    if(!elements.count(id))throw VirusNotFound();
    return std::vector<id_type>(parents[id].cbegin(), parents[id].cend());
  }

  // Sprawdza, czy wirus o podanym identyfikatorze istnieje.
  bool exists(id_type const &id) const {
    return elements.count(id);
  }

  // Zwraca referencję do obiektu reprezentującego wirus o podanym
  // identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
  Virus& operator[](id_type const &id) { // TODO dodać const
    if(!elements.count(id))throw VirusNotFound();
    return elements[id];
  }

  // Tworzy węzeł reprezentujący nowy wirus o identyfikatorze id
  // powstały z wirusów o podanym identyfikatorze parent_id lub
  // podanych identyfikatorach parent_ids.
  // Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
  // id już istnieje.
  // Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
  // poprzedników nie istnieje.
  void create(id_type const &id, std::vector<id_type> const &parent_ids) {
    if(elements.count(id))throw VirusAlreadyCreated();
    elements[id] = Virus(id);
    for(auto it : parent_ids) {
      if(!elements.count(it))throw VirusNotFound();
      childrens[it].insert(id);
      parents[id].insert(it);
    }
  }

  void create(id_type const &id, id_type const &parent_id) {
      create(id, std::vector<id_type>({parent_id}));
  }

  // Dodaje nową krawędź w grafie genealogii.
  // Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
  void connect(id_type const &child_id, id_type const &parent_id) {
    if(!elements.count(child_id))throw VirusNotFound();
    if(!elements.count(parent_id))throw VirusNotFound();
    childrens[parent_id].insert(child_id);
    parents[child_id].insert(parent_id);
  }

  // Usuwa wirus o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
  // Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
  // wirusa macierzystego.

  void remove(id_type const &id) {
    if(!elements.count(id))throw VirusNotFound();
    if(id == stem_id)throw TriedToRemoveStemVirus();
    for(auto it : parents[id]) {
      if(childrens[it].count(id))
        childrens[it].erase(id);
    }
    
    std::vector<id_type> childs(childrens[id].begin(), childrens[id].end());
    for(auto it : childs) {
      parents[it].erase(id);
      if(parents[it].size() == 0)
        remove(it);
    }

    elements.erase(id);
    parents.erase(id);
    childrens.erase(id);
  }
};
#endif
