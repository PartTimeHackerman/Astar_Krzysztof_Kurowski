#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <list>

using namespace std;

struct Node {
	int id = -1;
	int x, y;
	float koszt = -1;
	int rodzic = -1;
	Node(int id, int x, int y, double koszt, int rodzic) : x(x), y(y), koszt(koszt), rodzic(rodzic){}
	Node() {}

	bool operator==(const Node& n){
		return id == n.id;
	}
};



int **wczytajZPliku(string nazwa_pliku, int rozmiar_x, int rozmiar_y) {
	int rows = rozmiar_y + 1;
	int **G;
	G = new int*[rows];
	while (rows--) G[rows] = new int[rozmiar_x];

	ifstream plik(nazwa_pliku.c_str());

	for (int i = 0; i < rozmiar_y; i++)
	{
		for (int j = 0; j < rozmiar_x; j++)
		{
			plik >> G[i][j];
		}
	}
	plik.close();
	return G;
}

double hueurystyka(int px, int py, int cx, int cy) {
	int x = pow(px - cx, 2);
	int y = pow(py - cy, 2);
	return sqrt(x + y);
}

double kosztDojscia(int px, int py, int cx, int cy) {
	double do_celu = hueurystyka(px, py, cx, cy);
	int koszt_ruchu = 1;
	return do_celu + koszt_ruchu;
}

bool contains(list<Node>& nodes, int id) {
	for (Node node : nodes) {
		if (node.id == id) return true;
	}
	return false;
}

Node najtanszy(list<Node>& nodes) {
	Node najtanszy;
	float najmniejszyKoszt = 99999999;
	for (Node node : nodes) {
		if (node.koszt < najmniejszyKoszt) { 
			najtanszy = node; 
			najmniejszyKoszt = node.koszt;
		}
	}
	return najtanszy;
}

Node findById(list<Node>& nodes, int id) {
	for (Node node : nodes) {
		if (node.id == id) {
			return node;
		}
	}
	return Node();
}

list<Node> astar(int **G, int rozmiar_x, int rozmiar_y, int *start, int *koniec) {
	Node **nodes = new Node*[rozmiar_x];
	for (int j = 0; j < rozmiar_y; j++)
	{
		nodes[j] = new Node[rozmiar_x];
	}
	int nodeId = 0;
	for (int i = 0; i < rozmiar_y; i++)
	{
		for (int j = 0; j < rozmiar_y; j++)
		{
			nodes[i][j].id = nodeId;
			nodes[i][j].x = i;
			nodes[i][j].y = j;
			nodeId++;
		}
	}
	int s1 =koniec[0];
	int s2 = koniec[1];
	Node startNode = nodes[start[0]][start[1]];
	Node endNode = nodes[koniec[0]][koniec[1]];
	list<Node> otwarta = {};
	list<Node> zamknieta = { startNode };
	list<Node> dostepne = {};
	bool celOsiagniety = false;

	while (!celOsiagniety) {
		Node node = zamknieta.front();
		dostepne.clear();
		if (node.x >= 0 && node.x < rozmiar_x && node.y - 1 >= 0 && node.y - 1 < rozmiar_y) if (G[node.x][node.y - 1] == 0) dostepne.push_front(nodes[node.x][node.y - 1]);
		if (node.x >= 0 && node.x < rozmiar_x && node.y + 1 >= 0 && node.y + 1 < rozmiar_y) if (G[node.x][node.y + 1] == 0) dostepne.push_front(nodes[node.x][node.y + 1]);
		if (node.x - 1 >= 0 && node.x -1 < rozmiar_x && node.y >= 0 && node.y < rozmiar_y) if (G[node.x - 1][node.y] == 0) dostepne.push_front(nodes[node.x - 1][node.y]);
		if (node.x + 1 >= 0 && node.x + 1 < rozmiar_x && node.y >= 0 && node.y < rozmiar_y) if (G[node.x + 1][node.y] == 0) dostepne.push_front(nodes[node.x + 1][node.y]);
		
		for (Node dostepna : dostepne) {
			if (contains(zamknieta, dostepna.id)) continue;
			float koszt = kosztDojscia(dostepna.x, dostepna.y, koniec[0], koniec[1]);
				if ((dostepna.koszt > 0 && dostepna.koszt < koszt) || dostepna.rodzic < 0) {
					dostepna.koszt = koszt;
					dostepna.rodzic = node.id;
					otwarta.push_front(dostepna);
				}
			
		}
		Node najmniejszyNode = najtanszy(otwarta);
		otwarta.remove(najmniejszyNode);
		zamknieta.push_front(najmniejszyNode);
		if (zamknieta.front().id == endNode.id || otwarta.size() == 0) celOsiagniety = true;
	}

	list<Node> sciezka = {};
	sciezka.push_front(zamknieta.front());

	while (true) {
		Node node = sciezka.front();
		int idRodzica = node.rodzic;
		if (idRodzica == -1)
			break;
		sciezka.push_front(findById(zamknieta, idRodzica));
		
	}
	return sciezka;

}

int main()
{
	int rozmiar_x = 20;
	int rozmiar_y = 20;
	int start[] = { 0, 0 };
	int koniec[] = { 19, 19 };
	string nazwa_pliku = "grid.txt";
	int **G = wczytajZPliku(nazwa_pliku, rozmiar_x, rozmiar_y);
	for (int i = 0; i < rozmiar_y; i++)
	{
		for (int j = 0; j < rozmiar_x; j++)
		{
			cout << " " << G[i][j];
		}cout << "\n";
	}

	list<Node> sciezka = astar(G, rozmiar_x, rozmiar_y, start, koniec);
	
	if(sciezka.size() == 0) cout << "Brak sciezki";
	cout << "\n";
	cout << "\n";
	for (Node node : sciezka) {
		int x = node.x;
		int y = node.y;
		G[x][y] = 3;
	}

	for (int i = 0; i < rozmiar_y; i++)
	{
		for (int j = 0; j < rozmiar_x; j++)
		{
			cout << " " << G[i][j];
		}cout << "\n";
	}

	getchar();
	for (int i = 0;i < rozmiar_y; i++)
	{
		delete[] G[i];
	}
	delete[] G;

	getchar();
	return 0;
}