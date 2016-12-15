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


//TODO lock na weak ptrach, żeby sie kompilowało
template <class Virus>
class VirusGenealogy {
	typedef typename Virus::id_type id_type;
	
	class Node/* : std::enable_shared_from_this<Node>*/ {
	public:
		VirusGenealogy* vir_gen;
		Virus virus;
		std::map<id_type, std::shared_ptr<Node> > children;
		std::map<id_type, std::weak_ptr<Node> > parents;

		//TODO wykminic jak tu zrobic weak ptr czy cos
		Node(VirusGenealogy* vgen, id_type const &id) : vir_gen(vgen), virus(id) {}
		~Node() {
		//NOTE dzieje sie double delete jak ten kod zostawimy - do rozkminienia
// 			std::cout << "trololololololololo" << std::endl;
// 			id_type id = virus.get_id();
// 			for (auto it : children)
// 				it.second->parents.erase(id);
			
// 			vir_gen->genealogy.erase(id);
		}
	};
	
	std::shared_ptr<Node> stem;
	std::map<id_type, std::weak_ptr<Node> > genealogy;

public:

  VirusGenealogy operator=(VirusGenealogy _) = delete;
  VirusGenealogy(VirusGenealogy &_) = delete;

	// Tworzy nową genealogię.
	// Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
	VirusGenealogy(id_type const &stem_id)
		/*: stem(new Node(this, stem_id))*/ {
		stem.reset(new Node(this, stem_id));
		std::weak_ptr<Node> wp = stem;
		genealogy.emplace(stem_id, wp);
	}

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
		if (auto p = genealogy.at(id).lock())
		{
			std::vector<id_type> vec;
			for (auto it = p->children.cbegin(); it != p->children.cend(); it++)
				vec.push_back((*it).first);
			return vec;
		}
		
		return std::vector<id_type>();
	}

	// Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<id_type> get_parents(id_type const &id) const {
		if (!genealogy.count(id))
			throw VirusNotFound();
		if (auto p = genealogy.at(id).lock())
		{
			std::vector<id_type> vec;
			for (auto it = p->parents.cbegin(); it != p->parents.cend(); it++)
				vec.push_back((*it).first);
			return vec;
		}
		
		return std::vector<id_type>();
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
		return *genealogy.at(id)->virus;
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

		std::shared_ptr<Node> sp_node(new Node(this, id));
		genealogy.insert(make_pair(id, sp_node));

		for (auto it : parent_ids) {
			if (auto p = genealogy.at(it).lock()) {
				p->children.emplace(id, sp_node);
				p->parents.emplace(it, genealogy.at(it));
			} //TODO co jesli nie pyknie
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

		auto parent_node = genealogy.at(parent_id).lock();
		auto child_node = genealogy.at(child_id).lock();
		
// 		if (oba locki sie udaly) //TODO
		{
			parent_node->children.emplace(child_id, child_node);
			child_node->parents.emplace(parent_id, parent_node);
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
		
		std::weak_ptr<Node> node = genealogy.at(id);
		for (auto it : node->parents) {
			it->second->children.erase(id);
		}
		/*
		
		
		std::vector<id_type> childs(childrens[id].begin(), childrens[id].end());
		for (auto it : childs) {
			parents[it].erase(id);
			if (parents[it].size() == 0)
				remove(it);
		}

		elements.erase(id);
		parents.erase(id);
		childrens.erase(id);
		*/
	}
	
};


#endif
