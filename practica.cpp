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
#include <map>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>
 
using namespace std;


/*
 * 
 * DADES
 *
 *
 */

unsigned int primeNumber = 101;	//primer número primo cardinal al set de shingles totales (necesario para el hash)
unsigned int nDoc = 3; //número de Documentos
unsigned int nHashFunctions = 300; //número de hash functions
unsigned int k = 8;	// cuantas palabras tendra cada Shingle?
unsigned int b =100;  //número de bandas
unsigned int r = 3; //número de filas para cada bucket (r*b == nHashFunctions)
double t = 0.0; //threshold
struct index {
	unsigned int a;
	unsigned int b;
};

//kShingles contiene una matriz de shingles, donde las filas son los documentos
vector< set <string > > docShingles;	

// setShingles contiene el set de los shingles de todos los documentos
set <string >  setShingles;
	
//indexHash contiene un vector con una tupla que se usará para crear diferentes permutaciones hasheadas,	
vector<index> indexHash;

//booleanShingles contiene una matrix con el booleano de las ocurrencias de cada documento respecto el set total de shingles 
//(siendo 1 una ocurrencia positiva y 0 una negativa)
vector< vector < bool> > booleanShingles;

//signatureMatrix contendrá la posición del cada shingle de cada documento pero permutado 
vector< vector < unsigned int > > signatureMatrix;	

//mapa con clave de los documentos candidatos
map<pair<int,int>, double> candidates;


/*
 *
 * OUTPUT FUNCTIONS
 *
 */

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


/* 
 *
 * INIT FUNCTIONS
 *
 */

void init(int n, char *params[]){
	if(n==1) {
		cout<<"Utilizando parametros por defecto..."<<endl;
	}
	else {
		cout<<"Utilizando parametros definidos por usuario..."<<endl;
		if(params[1]!="x") nDoc = atoi(params[1]);	
		if(params[2]!="x") nHashFunctions= atoi(params[2]);	
		if(params[3]!="x") k = atoi(params[3]);	
		if(params[4]!="x") b = atoi(params[4]);	
		if(params[5]!="x") r = atoi(params[5]);	
		if(params[6]!="x") t =atof(params[6]);	
	}	
	cout<<	" - número de Docuemntos: "<<nDoc<<endl<<
			" - número de funciones de Hash: "<<nHashFunctions<<endl<<
			" - valor de K (palabras de cada Shingle): "<<k<<endl<<
			" - número de bandas: "<<b<<endl<<
			" - número de filas hasheadas para cada banda: "<<r<<endl<<
			" - parametro threshold:"<<t<<endl;
	// inicializaciones correspondientes
	signatureMatrix = vector< vector < unsigned int > > (nHashFunctions, vector<unsigned int> (nDoc, UINT_MAX));
	indexHash = vector<index>(nHashFunctions, index{});
}
void initIndex() {
	clock_t c_start = clock();
	// iniciamos los valores de los indices para el hasheoº
	for (unsigned int i = 0; i < indexHash.size(); i++) {
		srand(time(NULL)*(i+137477057));
		indexHash[i].a = rand();	
		indexHash[i].b = rand();
	}
	clock_t c_end = clock();
        cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado en crear los indices para hashear: "
                << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
}
void initBooleanShingles(){
	//encuentra que valores de cada set de cada documento tienen alguna ocurrencia en el set global de shingles y los pone true, si no falso
	clock_t c_start = clock();
	// iniciamos la matrix de booleanos con su tamaño adecuado
	booleanShingles = vector< vector < bool> > (setShingles.size(), vector<bool> (nDoc,false));	
	int i = 0;
	for(auto iterador = setShingles.begin(); iterador != setShingles.end(); iterador++){
		for(unsigned int j = 0; j<docShingles.size(); j++){
			if(docShingles[j].find(*iterador)!=docShingles[j].end()) booleanShingles[i][j]=true;
			else booleanShingles[i][j] = false;
		}
		i++;
	}
	clock_t c_end = clock();
        cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado para encontrar las ocurrencias  de cada documento respecto el set global de shingles: "
                << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
}


/*
 *
 * MAIN FUNCTIONS
 *
 */

void minhashSignatures() {
	//el hasheo es del estilo (a*i + b) mod primeNumber
	//miro cada row de b y después cada fila y si da true, modifico v si el valor del hash es inferior a este
	clock_t c_start = clock();
	for (unsigned int shingle = 0; shingle < booleanShingles.size(); shingle++) {
		for (unsigned doc = 0; doc < booleanShingles[0].size(); doc++) {
			if(booleanShingles[shingle][doc]) {
				for(unsigned int n = 0; n < indexHash.size(); n++) {
					int hashValue = (indexHash[n].a * shingle + indexHash[n].b) % primeNumber /* primeNumber aquí*/;
					if (signatureMatrix[n][doc] > hashValue) {
						signatureMatrix[n][doc] = hashValue;
					}	
				}
			}
		}
	}
	clock_t c_end = clock();
        cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: para crear las signatures minhash: "
                << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
}
float jaccardSimSignature(const int &a, const int &b){
	clock_t c_start = clock();
	// dado dos filas de signatures a y b, devuelve la similitud entre esos dos documentos 
	float value = 0;
	for (unsigned int i = 0; i < signatureMatrix.size(); i++) {
		if(signatureMatrix[i][a] == signatureMatrix[i][b]) value++;
	}
	clock_t c_end = clock();
        //cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: "
          //      << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
	return value/signatureMatrix.size();
}
void  kShingle (){
	clock_t c_start = clock();
	// esta función añade a setShingles todos los shingles de todos los documentos
	// mientras que docShingle tendrá el set de shingles de cada documento
	string s, word;
	string aux = " ";
	for (int j = 1; j <= nDoc; ++j){
		string fileAux;
		set <string> actualDocShingle;
		fileAux = to_string(j); 
		ifstream file("data/doc"+fileAux+".txt");
		for (unsigned int i = 0; i < k and file>>word; i++){
			if(i==0) s = word;
	       		else s.append(" "+word );
		}
		setShingles.insert(s);
		actualDocShingle.insert(s);
		while (file>>word){
			s=s.substr(s.find_first_of(aux)+1);
	       		s.append(" "+word);
			actualDocShingle.insert(s);
			setShingles.insert(s);
		}
		docShingles.push_back(actualDocShingle);
	}
	clock_t c_end = clock();
        cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado para shinglear todos los documentos: "
                << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
}
void generateCandidates(){
	//genera los candidatos haciendo uso de buckets/bandas
	hash<string> hString;
	vector <vector <int> > buckets (b, vector<int> (nDoc));
	for(unsigned band = 0; band < b; band++){
		for (unsigned int col = 0; col < nDoc; col++){
			char aux[r]; 
			for(unsigned int row = 0; row < r; row++){
				aux[row]=signatureMatrix[col][band*r+row];
			}
			string aux1 = aux;
			buckets[band][col] = hString(aux1);
		}
	}
	for (unsigned int band = 0; band < b; band++){
		for (unsigned col = 0; col < (nDoc-1); col++){
			for (unsigned int k = col + 1; k < nDoc; k++){
				if(buckets[band][col] == buckets[band][k]) {
					pair<int,int> p;
					p.first = col;
					p.second = k;
					candidates[p]=0;	
				}
			}
		}
	}
	for(auto it = candidates.begin(); it!=candidates.end(); it++){
		it->second = jaccardSimSignature(it->first.first, it->first.second);
	// debug cout<<it->first.first<<" y "<<it->first.second<<" jaccard de "<<it->second<<endl;
	}	
}
void similarity() {
    	// Escribe por pantalla que documentos tienen una similitud superior o igual al threshold
	cout <<"valor Threshold: "<< t << endl;
	bool similars = false;
	std::map<pair<int,int>,double>::iterator it = candidates.begin();
	while (it != candidates.end()) {
        it->second=(it->second);
        if (it->second >= t){
		cout << "Similitud de ("<<"doc"<<it->first.first+1  << ".txt, " <<"doc"<<it->first.second+1<<".txt) = " << it->second << endl;
		similars=true;
        }
        it++;
    }
	if (!similars) {
	    cout << "Ningún documento se parece con ese threshold"<<endl;
	}
}
void LSH(){
	clock_t c_start = clock();
	generateCandidates();
	similarity();
	clock_t c_end = clock();
	cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: "
		<< 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl<<endl;
}
float jaccardSimilarity(set <string> a, const set <string> &b) {
	//consigue la similitud de dos sets
	clock_t c_start = clock();
        float intersection = 0;
        for(auto it = b.begin(); it != b.end(); it++) {
                if(!a.insert(*it).second)
                        intersection++;
        }
	clock_t c_end = clock();
        cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: "
                << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl<<endl;
        return intersection / (float) a.size();
}

float jaccardSimShingle(const int &a, const int &b){
	//consigue la similitud de los sets a y b (cada set son Shingles)
	set<string> aux(docShingles[a]);
	return jaccardSimilarity(aux, docShingles[b]);
}
void funcionalidades(){
	cout<<"envía 0 para salir"<<endl<<
		"envía 1 para obtener la similitud de Jaccard de dos documentos"<<endl<<
		"envía 2 para obtener la aproximación del grado de similitud de Jaccard a través de representaciones signatures minhash de dos docuemntos"<<endl<<
		"envía 3 para obtener la aproximiación del grado de similtud de Jaccard a través de representciones signatures minhash de todos los documentos a partir de t (utilizando LSH)"<<endl;
}

int main(int argc, char *argv[]) {
	init(argc, argv);
	int funcio;
	// ponemos los valores setShingles y kShingles a punto (crea shingles y los añade individualmente por documento 
	// en docShingles y globalmente en setShingles
	kShingle();
	
	// ponemos los valores de indexHash a punto
	initIndex();
	// conseguimos las signatures utilizando minhash

	//iniciamos booleanShingles 
	initBooleanShingles();
	
	//creamos las signatures y las ponemos a punto
	minhashSignatures();

	//dice por pantalla las funcionalidades disponibles
	cout<<endl;
	funcionalidades();

	while (cin>>funcio){
	int x,y;
		switch(funcio){
			case 0:
				exit(1);
				break;
			case 1:
				cout<<"Qué dos documentos (forma docx, docy) quieres obtener similitud de Jaccard? (valor menor que "<<nDoc<<")"<<endl;
				cin>> x >> y;
				cout<<"La similitud de doc"<<x<<".txt y doc"<<y<<".txt es de "<<jaccardSimShingle(x-1,y-1)<<endl<<endl;
				funcionalidades();
				break;
			case 2:
				cout<<"Qué dos documentos (forma docx,docy, quieres obtener la aproximación del grado de similitud usando signatures? (valor menor que "<<nDoc<<")"<<endl<<endl;
				cin>>x>>y;
				cout<<"La aproximación del grado de similitud de doc"<<x<<".txt y doc"<<y<<".txt es de "<<jaccardSimSignature(x-1,y-1)<<endl<<endl;
				funcionalidades();
				break;
			case 3:
				char c;
				cout<<"Deseas cambiar tu parametro de threshold? y/n"<<endl;
				cin>>c;
				if(c=='y'){
					cin>>t;
				}
				LSH();
				funcionalidades();
				break;
			default: 
				funcionalidades();
		}
	}
}	
