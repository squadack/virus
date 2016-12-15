#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H
#include<map>
#include<set>
#include<vector>
#include <memory>

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
	
	class Node {
// 		std::weak_ptr<VirusGenealogy> genealogy;
		std::unique_ptr<Virus> virus;
		std::map<id_type, std::shared_ptr<Node> > children;
		std::map<id_type, std::weak_ptr<Node> > parents;
	public:
		Node(id_type const &id) : virus(new Virus(id)) {}
	};
	
	std::shared_ptr<Node> stem;
	std::map<id_type, std::weak_ptr<Node> > genealogy;

public:
	// Tworzy nową genealogię.
	// Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
	VirusGenealogy(id_type const &stem_id)
		: stem(new Node(stem_id)) {
		genealogy.insert(make_pair(stem_id, stem));
	}

	// Zwraca identyfikator wirusa macierzystego.	
	id_type get_stem_id() const {
		return stem->virus->get_id();
	}
	
	// Zwraca listę identyfikatorów bezpośrednich następników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<id_type> get_children(id_type const &id) const {
		if (!genealogy.count(id))
			throw VirusNotFound();
		return std::vector<id_type>(genealogy.at(id)->children.cbegin(), genealogy.at(id)->children.cend()); //NOTE Squadack: zastanowić sie czy nie da sie tego ladniej
	}

	// Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<id_type> get_parents(id_type const &id) const {
		if (!genealogy.count(id))
			throw VirusNotFound();
		return std::vector<id_type>(genealogy.at(id)->parents.cbegin(), genealogy.at(id)->parents.cend()); //NOTE Squadack: zastanowić sie czy nie da sie tego ladniej
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

		std::shared_ptr<Node> sp_node(new Node(id));
		genealogy.insert(make_pair(id, sp_node));

		for (auto it : parent_ids) {
			genealogy.at(it)->children.insert(id, sp_node);
			genealogy.at(id)->parents.insert(it, genealogy.at(it));
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

		std::weak_ptr<Node> parent_node = genealogy.at(parent_id);
		std::weak_ptr<Node> child_node = genealogy.at(child_id);
		
		parent_node->children.insert(child_id, child_node);
		child_node->parents.insert(parent_id, parent_node);
	}

	/*
	// Usuwa wirus o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	// Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
	// wirusa macierzystego.
	void remove(id_type const &id) {
		if (!elements.count(id))
			throw VirusNotFound();
		if (id == stem_id)
			throw TriedToRemoveStemVirus();
		for (auto it : parents[id]) {
			if(childrens[it].count(id))
				childrens[it].erase(id);
		}

		std::vector<id_type> childs(childrens[id].begin(), childrens[id].end());
		for (auto it : childs) {
			parents[it].erase(id);
			if (parents[it].size() == 0)
				remove(it);
		}

		elements.erase(id);
		parents.erase(id);
		childrens.erase(id);
	}
	*/
};


#endif
