#ifndef VIRUS_NORMALNY_H
#define VIRUS_NORMALNY_H

#include <bits/stdc++.h>
#include "../virus_genealogy.h"
using namespace std;


const int N = 1000 * 1000;

template <class Virus>
class VirusNormalny {
	using id_type = int;
	
	vector<int> parents[N];
	vector<int> children[N];
	bool jest[N];
	int stem_id;
	vector<int> mamy;
	
	void usun(int x, vector<int>& vv) {
		for (size_t i = 0; i < vv.size(); i ++) {
			if (x == vv[i]) {
				swap(vv.back(), vv[i]);
				vv.pop_back();
				return;
			}
		}
	}
	
	void usun(int x) {
		jest[x] = false;
		for (int y: parents[x]) {
			usun(x, children[y]);
		}
		for (int y: children[x]) {
			usun(x, parents[y]);
			if (parents[y].empty()) {
				usun(y);
			}
		}
	}
	
public:
	// Tworzy nową genealogię.
	// Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
	VirusNormalny(int const &id) {
		mamy.push_back(id);
		stem_id = id;
		jest[id] = true;
	}
	
	// Zwraca identyfikator wirusa macierzystego.
	id_type get_stem_id() const {
		return stem_id;
	}
	
	// Zwraca listę identyfikatorów bezpośrednich następników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<id_type> get_children(id_type const &id) const {
		if (not jest[id]) {
			cout << "Nie ma wierzchołka " << id << endl;
			throw VirusNotFound();
		}
		return children[id];
	}
	
	// Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	std::vector<id_type> get_parents(id_type const &id) const {
		if (not jest[id]) {
			throw VirusNotFound();
		}
		return parents[id];
	}

	// Sprawdza, czy wirus o podanym identyfikatorze istnieje.
	bool exists(id_type const &id) const {
		return jest[id];
	}


	// Tworzy węzeł reprezentujący nowy wirus o identyfikatorze id
	// powstały z wirusów o podanym identyfikatorze parent_id lub
	// podanych identyfikatorach parent_ids.
	// Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
	// id już istnieje.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
	// poprzedników nie istnieje.
	void create(id_type const &id, id_type const &parent_id) {
		create(id, std::vector<id_type>(1, parent_id));
	}
	
	void create(id_type const &id, std::vector<int> parent_ids) {
		if (exists(id)) {
			throw VirusAlreadyCreated();
		}
		
		if (parent_ids.empty()) {
			throw VirusNotFound();
		}
		
		for (int x: parent_ids) {
			if (not jest[x]) {
				throw VirusNotFound();
			}
		}
		
		sort(parent_ids.begin(), parent_ids.end());
		parent_ids.resize(unique(parent_ids.begin(), parent_ids.end()) - parent_ids.begin());
		
		jest[id] = true;
		parents[id] = parent_ids;
		children[id].clear();
		for (int x: parent_ids) {
			children[x].push_back(id);
		}
		mamy.push_back(id);
	}

	// Dodaje nową krawędź w grafie genealogii.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
	void connect(id_type const &child_id, id_type const &parent_id) {
		if (not jest[child_id] || not jest[parent_id]) {
			throw VirusNotFound();
		}
		usun(parent_id, parents[child_id]);
		usun(child_id, children[parent_id]);
		parents[child_id].push_back(parent_id);
		children[parent_id].push_back(child_id);
	}

	// Usuwa wirus o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	// Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
	// wirusa macierzystego.
	void remove(id_type const &id) {
		if (id == get_stem_id()) {
			throw TriedToRemoveStemVirus();
		}
		if (not exists(id)) {
			throw VirusNotFound();
		}
		usun(id);
		usun(id, mamy);
	}
	
	vector<int> wierzcholki() {
		return mamy;
	}
};


#endif //VIRUS_GENEALOGY_H
