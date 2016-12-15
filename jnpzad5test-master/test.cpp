#include <bits/stdc++.h>
#include "../virus_genealogy.h"
#include "virus_po_ludzku.h"
using namespace std;

class MyException: public std::exception {
public:
	virtual const char* what() const noexcept {
		return "MyException";
	}
};

const int RATIO = 12;
const int SEED = 699;

class my_int {
private:
	int id;
public:
	my_int(int x): id(x) {
		wyjatek();
	}
	my_int(int x, bool): id(x) {
	}
	my_int(): id(0) {
	}
	int to_int() {
		return id;
	}
	bool operator== (my_int v) const {
		wyjatek();
		return this -> id == v.id;
	}
	bool operator < (const my_int& v) const {
		wyjatek();
		return this -> id < v.id;
	}
	bool operator > (const my_int& v) const {
		wyjatek();
		return this -> id > v.id;
	}
	bool operator <= (const my_int& v) const {
		wyjatek();
		return this -> id <= v.id;
	}
	bool operator >= (const my_int& v) const {
		wyjatek();
		return this -> id >= v.id;
	}
	void wyjatek() const {
		if (rand() % RATIO == 0) {
// 			cout << "Rzuć wyjątkiem " << endl;
			throw MyException();
		}
	}
};

class Virus {
  public:
    typedef my_int id_type;
    Virus(id_type const &_id) : id(_id) {
		wyjatek();
    }
    Virus(){}//TODO
    bool operator== (Virus v) {
		wyjatek();
		return this -> id == v.id;
	}
	bool operator < (Virus v) {
		wyjatek();
		return this -> id < v.id;
	}
	bool operator > (Virus v) {
		wyjatek();
		return this -> id > v.id;
	}
	bool operator <= (Virus v) {
		wyjatek();
		return this -> id <= v.id;
	}
	bool operator >= (Virus v) {
		wyjatek();
		return this -> id >= v.id;
	}
    id_type get_id() const {
		wyjatek();
		return id;
    }
  private:
    id_type id;
	void wyjatek() const {
		if (rand() % RATIO == 0) {
// 			cout << "Rzuć wyjątkiem " << endl;
			throw MyException();
		}
	}
};

class test {
public:
	int counter = 1;
	VirusGenealogy<Virus>* vg;
	VirusNormalny<Virus>* v;
	test() {
		v = new VirusNormalny<Virus>(0);
		while(true) {
			try {
				vg = new VirusGenealogy<Virus>(0);
				break;
			}
			catch (MyException) {
			}
		}
		get_stem_id();
	}
	// Zwraca identyfikator wirusa macierzystego.
	void get_stem_id() {
		Virus::id_type stem = Virus::id_type(0, false);
		while(true) {
			try {
				stem = vg -> get_stem_id();
				break;
			}
			catch (MyException) {
			}
		}
		assert(v -> get_stem_id() == stem.to_int());
	}
	
	bool rowne(vector<Virus::id_type> a, vector<int> b) {
		if (a.size() != b.size()) return false;
		for (size_t i = 0; i < a.size(); i ++) {
			for (size_t j = 0; j <= b.size(); j ++) {
				if (j == b.size()) {
					return false;
				}
				if (b[j] == a[i].to_int()) {
					break;
				}
			}
		}
		return true;
	}
	
	bool rowne(vector<int> b, vector<Virus::id_type> a) {
		return rowne(a, b);
	}
	
	// Zwraca listę identyfikatorów bezpośrednich następników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	void get_children(int id) {
		printf("get_children %d\n", id);
		vector<Virus::id_type> vv;
		while(true) {
			try {
				vv = vg -> get_children(id);
				break;
			}
			catch (MyException) {
// 				cout << "Złapany\n";
			}
			catch (std::exception &e) {
				try {
					v -> get_children(id);
				} catch (std::exception &f) {
					assert(e.what() == f.what());
					return;
				}
				assert(false);
			}
		}
		assert(rowne(v->get_children(id), vv));
	}
	
	// Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
	// o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
	void get_parents(int id) {
		vector<Virus::id_type> vv;
		while(true) {
			try {
				vv = vg -> get_parents(id);
				break;
			}
			catch (MyException) {
			}
			catch (std::exception &e) {
				try {
					v -> get_parents(id);
				} catch (std::exception &f) {
					assert(e.what() == f.what());
					return;
				}
				assert(false);
			}
		}
		assert(rowne(v->get_parents(id), vv));
	}

	// Sprawdza, czy wirus o podanym identyfikatorze istnieje.
	void exists(int id) {
		bool exi;
		while(true) {
			try {
				exi = vg -> exists(id);
				break;
			}
			catch (MyException) {
			}
		}
		assert(exi == v -> exists(id));
	}

	// Tworzy węzeł reprezentujący nowy wirus o identyfikatorze id
	// powstały z wirusów o podanym identyfikatorze parent_id lub
	// podanych identyfikatorach parent_ids.
	// Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
	// id już istnieje.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
	// poprzedników nie istnieje.
	void create(int id, int parent_id) {
		while(true) {
			try {
				vg -> create(id, parent_id);
				break;
			}
			catch (MyException) {
			}
			catch (std::exception &e) {
				try {
					v -> create(id, parent_id);
				} catch (std::exception &f) {
					assert(e.what() == f.what());
					return;
				}
				assert(false);
			}
		}
		v -> create(id, parent_id);
	}
	
	void create(int id, std::vector<int> const &parent_ids) {
		std::vector<Virus::id_type> par;
		for (int x: parent_ids) {
			par.push_back(my_int(x, false));
		}
		while(true) {
			try {
				vg -> create(id, par);
				break;
			}
			catch (MyException) {
			}
			catch (std::exception &e) {
				try {
					v -> create(id, parent_ids);
				} catch (std::exception &f) {
					cout << e.what() << " " << f.what() << " ???";
					assert(e.what() == f.what());
					return;
				}
				cout << e.what() << " lol?\n";
				assert(false);
			}
		}
		v -> create(id, parent_ids);
	}

	// Dodaje nową krawędź w grafie genealogii.
	// Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
	void connect(int const &child_id, int const &parent_id) {
		while(true) {
			try {
				vg -> connect(child_id, parent_id);
				break;
			}
			catch (MyException) {
			}
			catch (std::exception &e) {
				try {
					v -> connect(child_id, parent_id);
				} catch (std::exception &f) {
					assert(e.what() == f.what());
					return;
				}
				assert(false);
			}
		}
		v -> connect(child_id, parent_id);
	}

	// Usuwa wirus o podanym identyfikatorze.
	// Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
	// Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
	// wirusa macierzystego.
	void remove(int id) {
		while(not try_remove(id));
	}
	bool try_remove(int id) {
		try {
			vg -> remove(id);
		}
		catch (MyException) {
				return false;
		}
		catch (std::exception e) {
			try {
				v -> remove(id);
			} catch (std::exception f) {
				assert(e.what() == f.what());
				return true;
			}
			assert(false);
		}
		v -> remove(id);
		return true;
	}
	
	int losuj() {
		return rand() % 1000;
	}
	
	void step() {
		vector<int> mamy = v-> wierzcholki();
		sort(mamy.begin(), mamy.end());
		int rrr = rand() % 4;
				vector<int> p;
				int i,j;
		switch(rrr) {
			case 0: case 1: //create
				for ( i = 0; i < (int)mamy.size(); i++) {
					if (rand() % mamy.size() < 2) {
						p.push_back(mamy[i]);
					}
				}
				cout << "Create " << counter + 1 << " : ";
				for (int x : p ){
					 cout << x << " ";
				}
				cout << endl;
				create(counter++, p);
				break;
			case 2: // connect
				if (mamy.size() == 1) break;
				i = rand() % (mamy.size() - 1);
				j = rand() % (mamy.size() - i - 1) + i + 1;
				cout << "Connect " << mamy[i] << " "  << mamy[j] << endl;
				connect(mamy[j], mamy[i]);
				break;
			case 3: //remove
				i = rand() % mamy.size() < 2;
				cout << "Remove " << mamy[i] << endl;
				remove(mamy[i]);
		}
		cout << "Step succesful " << endl;
		//Sprawdzanie poprawności
		mamy = v -> wierzcholki();
		get_stem_id();
		for (int x: mamy) {
			get_children(x);
			get_parents(x);
			exists(x);
		}
		for (int y = 0; y < 10; y ++) {
			int x = losuj();
			get_children(x);
			get_parents(x);
			exists(x);
		}
		
	}
	~test() {
		delete v;
		delete vg;
	}
};

int main() {
	srand(SEED);
	test t;
	for (int i = 0; i < 100; i ++)
		t.step();
}
