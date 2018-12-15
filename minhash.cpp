#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <climits>
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <stdlib.h>
using namespace std;

//primeNumber debería ser el primer número primo superior a Ndoc
unsigned int primeNumber = 53;
unsigned int nDoc = 3;
//unsigned int setSize = 5;
unsigned int nHashFunctions = 12;
int k = 6;
struct index {
	unsigned int a;
	unsigned int b;
};

//kShingles contiene una matriz de shingles, donde las filas son los documentos
vector< set <string > > docShingles;	

// setShingles contiene el set de los shingles de todos los documentos
set <string >  setShingles;
	
//indexHash contiene un vector con una tupla que se usará para crear diferentes permutaciones hasheadas,	
vector<index> indexHash(nHashFunctions, index{});

//vShingles contiene una matrix con el booleano de las ocurrencias de cada documento respecto el set total de shingles 
//(siendo 1 una ocurrencia positiva y 0 una negativa)
vector< vector < bool> > booleanShingles;
//signatureMatrix contendrá la posición del cada shingle de cada documento pero permutado 
vector< vector < unsigned int > > signatureMatrix;	


ostream &operator<<(ostream &os, const index &i){
	return os <<"a = "<<i.a<<" b = "<<i.b;
}

template <class T>
void output (const  vector < T >  &v) {
	for (unsigned int i = 0; i < v.size(); i++) {
		cout <<"i: "<<i<<", "<< v[i]<<endl;
	}
}

template <class T>
void output (const vector < vector < T > > &v) {
	for (unsigned int i = 0; i < v.size(); i++) {
		for (unsigned int j = 0; j < v[i].size(); j++) {
			cout << v[i][j]<<" ";
		}
		cout<<endl;
	}
}
void outputSet( set <string > &s ){
	for (auto iterador = s.begin(); iterador != s.end(); iterador++){
		cout << *iterador<<endl;	
	}
}
void output2(vector < set < string > > &s){
	for(unsigned int i = 0; i<s.size();i++){
		cout<<"documento: "<<i<<endl;
		outputSet(s[i]);
		cout<<endl;
	}
}
int sizeSet(const vector <vector <string> > &v) {
	set<string> s;
	for (unsigned int i = 0; i < v.size(); i++) {
		for (unsigned int j = 0; j < v[i].size(); j++) {
			s.insert(v[i][j]);
		}
	}
	cout<<endl<<endl;
	return s.size();
}
void initIndex(vector<index> &v) {
	for (unsigned int i = 0; i < v.size(); i++) {
		srand(time(NULL)*(i+137477057));
		v[i].a = rand();	
		v[i].b = rand();
	}
}

void minhashSignatures(vector < vector < unsigned int > > & v,const  vector< vector< bool> > &b, const vector <index> &vIndex) {
	//el hasheo es del estilo (a*i + b) mod primeNumber
	//miro cada row de b y después cada fila y si da true, modifico v si el valor del hash es inferior a este
	for (unsigned int shingle = 0; shingle < b.size(); shingle++) {
		for (unsigned doc = 0; doc < b[0].size(); doc++) {
			if(b[shingle][doc]) {
				for(unsigned int n = 0; n < vIndex.size(); n++) {
					int hashValue = (vIndex[n].a * shingle + vIndex[n].b) % primeNumber /* primeNumber aquí*/;
					if (v[n][doc] > hashValue) {
						v[n][doc] = hashValue;
					}	
				}
			}
		}
	}
}

float sim(const vector < vector< unsigned int > > &v, const int &a, const int &b){
	// dado dos filas de signatures a y b, devuelve la similitud entre esos dos documentos 
	float value = 0;
	for (unsigned int i = 0; i < v.size(); i++) {
		if(v[i][a] == v[i][b]) value++;
	}
	return value/v.size();
}

void init(int n, char *numeroDoc[]){
	if(n!=2) {
		cout<<endl<<"El programa recibe cuantos documentos hay, los documentos tienen que estar de la forma docN.txt"<<endl;
		exit(1);
	}
	nDoc=atoi(numeroDoc[1]);
}

void prueba (){
	vector<index> v(2, index{});
	v[0].a = 1;
	v[0].b = 1;
	v[1].a = 3;
	v[1].b = 1;	
	int mod = 5;
	vector< vector <bool> > b(5, vector<bool>(4, false));
	b[0][0] = true;
	b[0][3] = true;
	b[1][2] = true;
	b[2][1] = true;
	b[2][3] = true;
	b[3][0] = true;
	b[3][2] = true;
	b[3][3] = true;
	b[4][2] = true;
	vector < vector < unsigned int > > kek (2, vector< unsigned int > (4, UINT_MAX));
        minhashSignatures(kek, b, v);
	output(kek);
	cout<<endl<<endl;

	cout<<sim(kek,0,0)<<" "<<sim(kek,0,1)<<" "<<sim(kek,0,2)<<" "<<sim(kek,0,3)<<endl;
	cout<<sim(kek,1,0)<<" "<<sim(kek,1,1)<<" "<<sim(kek,1,2)<<" "<<sim(kek,1,3)<<endl;
	cout<<sim(kek,2,0)<<" "<<sim(kek,2,1)<<" "<<sim(kek,2,2)<<" "<<sim(kek,2,3)<<endl;
	cout<<sim(kek,3,0)<<" "<<sim(kek,3,1)<<" "<<sim(kek,3,2)<<" "<<sim(kek,3,3)<<endl;
}

void  kShingle ( set < string> &setShingles, vector< set<string> > &kShingle){
	// esta función añade a setShingles todos los shingles de todos los documentos
	// mientras que kShingle tendrá el set de shingles de cada documento
	string s, word;
	string aux = " ";
	for (int j = 1; j <= nDoc; ++j){
		string fileAux;
		set <string> shingleDoc;
		fileAux = to_string(j); 
		ifstream file("doc"+fileAux+".txt");
		for (unsigned int i = 0; i < k and file>>word; i++){
			if(i==0) s = word;
	       		else s.append(" "+word );
		}
		setShingles.insert(s);
		shingleDoc.insert(s);
		while (file>>word){
			s=s.substr(s.find_first_of(aux)+1);
	       		s.append(" "+word);
			shingleDoc.insert(s);
			setShingles.insert(s);
		}
		kShingle.push_back(shingleDoc);
	}
}

void initBooleanShingles(vector< vector< bool > > &vShingles, const set<string> setShingle, const vector< set< string> > &docShingles){
	// vShingles tiene tamaño el cardinal del set global de shinglex x número de documentos
	// el valor de cada celda es true si la celda (iterador, fila) contiene el elemento en el set global, si no, será falso
	int i = 0;
	for(auto iterador = setShingle.begin(); iterador != setShingle.end(); iterador++){
		for(unsigned int j = 0; j<docShingles.size(); j++){
			if(docShingles[j].find(*iterador)!=docShingles[j].end()) vShingles[i][j]=true;
			else vShingles[i][j] = false;
		}
		i++;
	}
}
void generateCandidates(){
	
}

int main(int argc, char *argv[]) {
	init(argc, argv);

	// ponemos los valores setShingles y kShingles a punto (crea shingles y los añade individualmente por documento 
	// en docShingles y globalmente en setShingles
	kShingle(setShingles, docShingles);
	
	// ponemos los valores de indexHash a punto
	initIndex(indexHash);

	booleanShingles = vector< vector < bool> > (setShingles.size(), vector<bool> (nDoc,false));	
	//iniciamos vShingles 
	initBooleanShingles(booleanShingles, setShingles, docShingles);
	

	signatureMatrix = vector< vector < unsigned int > > (nHashFunctions, vector<unsigned int> (nDoc, UINT_MAX));
	minhashSignatures(signatureMatrix, booleanShingles, indexHash);
	
	cout<<sim(signatureMatrix,1,0)<<endl;
}
