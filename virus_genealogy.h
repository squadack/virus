#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <iostream> //do debuga

class VirusNotFound : public std::exception {
	const char* what() const noexcept {
		return "VirusNotFound";
	}
};

class VirusAlreadyCreated : public std::exception {
	const char* what() const noexcept {
		return "VirusAlreadyCreated";
	}
};
class TriedToRemoveStemVirus : public std::exception {
	const char* what() const noexcept {
		return "TriedToRemoveStemVirus";
	}
};


template <class Virus>
class VirusGenealogy {
	typedef typename Virus::id_type id_type;
	
	struct Node : std::enable_shared_from_this<Node> {
		VirusGenealogy* vir_gen;
		Virus virus;
		std::map<id_type, Node*>* genmap;
		std::set< std::shared_ptr<Node> > children;
		std::set<Node*> parents;

		Node(id_type const &id, std::map<id_type, Node*>* gm) : virus(id), genmap(gm) {
// 			TODO ustawić stem i id
		}
		~Node() {
			for (auto it : children)
				it->parents.erase(this);
// 			genmap->erase(__coś__) //TODO zrobić erase
		}
	};
	
	std::shared_ptr<Node> stem;
	std::map<id_type, Node*> genealogy;

	VirusGenealogy(VirusGenealogy &old) {
	  stem = old.stem;
	  genealogy = old.genealogy;
	  }

public:

	VirusGenealogy operator=(VirusGenealogy _) = delete;
//	VirusGenealogy(VirusGenealogy &_) = delete;

	// Tworzy nową genealogię.
	// Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
	VirusGenealogy(id_type const &stem_id) : stem(new Node(stem_id, &genealogy)) {}

	// Zwraca identyfikator wirusa macierzystego.
	id_type get_stem_id() const {
		return stem->virus.get_id();
	}
	
	// Zwraca listę identyfikatorów bezpośrednich następników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<id_type> get_children(id_type const &id) const {
		if (!genealogy.count(id))
			throw VirusNotFound();
		std::vector<id_type> vec;
		Node* p = genealogy.at(id);
			for (auto it = p->children.cbegin(); it != p->children.cend(); it++)
				vec.push_back((*it)->virus.get_id());
		return vec;
		
	}

	// Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<id_type> get_parents(id_type const &id) const {
		if (!genealogy.count(id))
			throw VirusNotFound();
		std::vector<id_type> vec;
		Node* p = genealogy.at(id);
			for (auto it = p->parents.cbegin(); it != p->parents.cend(); it++)
				vec.push_back((*it)->virus.get_id());
		return vec;
		
	}

	// Sprawdza, czy wirus o podanym identyfikatorze istnieje.
	bool exists(id_type const &id) const {
		return genealogy.count(id);
	}

	// Zwraca referencję do obiektu reprezentującego wirus o podanym
	// identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	Virus& operator[](id_type const &id) const {
		if (!genealogy.count(id))
			throw VirusNotFound();
		
			return genealogy.at(id)->virus;
	}

	// Tworzy węzeł reprezentujący nowy wirus o identyfikatorze id
	// powstały z wirusów o podanym identyfikatorze parent_id lub
	// podanych identyfikatorach parent_ids.
	// Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
	// id już istnieje.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
	// poprzedników nie istnieje.
	void create(id_type const &id, std::vector<id_type> const &parent_ids) {
		if (parent_ids.empty())
			throw VirusNotFound();
		if (genealogy.count(id))
			throw VirusAlreadyCreated();
		for (auto it : parent_ids)
			if (!genealogy.count(it))
				throw VirusNotFound();

    VirusGenealogy copy(*this);
    try {
		  std::shared_ptr<Node> sp_node(new Node(id, &genealogy));
		  //TODO, bo tu większa magia się dzieje
  // 		genealogy.insert(make_pair(id, sp_node));
    } catch (...) {
        *this = copy;
        throw;
    }
	}

	void create(id_type const &id, id_type const &parent_id) {
		create(id, std::vector<id_type>({parent_id}));
	}

	// Dodaje nową krawędź w grafie genealogii.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
	void connect(id_type const &child_id, id_type const &parent_id) {
		if (!genealogy.count(child_id))
			throw VirusNotFound();
		if (!genealogy.count(parent_id))
			throw VirusNotFound();

    VirusGenealogy copy(*this);
    try {

		Node* parent_node = genealogy.at(parent_id);
		Node* child_node = genealogy.at(child_id);
		//TODO sprawdzic czy oba wstawienia sie udaly i w razie co wycofac
		child_node->parents.insert(parent_node);
// 		try
		parent_node->children.insert(child_node->shared_from_this());
		} catch (...) {
      *this = copy;
      throw;
    }
	}

	
	// Usuwa wirus o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	// Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
	// wirusa macierzystego.
	void remove(id_type const &id) {
// 		dla kazdego parenta usunac id z mapy dzieci
// 		wtedy polecą wszystkie shared pointery i reszta spada w ręce destruktora
		if (!genealogy.count(id))
			throw VirusNotFound();
		if (id == stem->virus.get_id())
			throw TriedToRemoveStemVirus();
		
		VirusGenealogy copy(*this);
    try {
		  std::shared_ptr<Node> sh_id = genealogy.at(id)->shared_from_this();
		  for (auto it : sh_id->parents)
			  it->children.erase(sh_id);
	  } catch (...) {
      *this = copy;
      throw;
    }
	}
};


#endif
