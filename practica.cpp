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

unsigned int primeNumber = 4999;	//primer número primo cardinal al set de shingles totales (necesario para el hash)
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

// esta tabla de pair contiene los valores b y nHashFunctions más óptimos para aproximarse a t = (1/b)^(1/r) y b*r = nHashFunctions 
// siendo r un valor arbitrario 3 y cada posición del vector representará (t+1)*10 (posición 0 = 0.10, posición 9 = 1)
int aproxValues[10] = {1000,125,37,15,8,4,3,2,1,1};
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
		if(params[6]!="x") t = atof(params[6]);
	}	
	cout<<	" - número de Docuemntos: "<<nDoc<<endl<<
			" - número de funciones de Hash: "<<nHashFunctions<<endl<<
			" - valor de K (palabras de cada Shingle): "<<k<<endl<<
			" - número de bandas: "<<b<<endl<<
			" - número de filas hasheadas para cada banda: "<<r<<endl<<
			" - valor de threshold: "<<t<<endl;
	// inicializaciones correspondientes
	signatureMatrix = vector< vector < unsigned int > > (nHashFunctions, vector<unsigned int> (nDoc, UINT_MAX));
	indexHash = vector<index>(nHashFunctions, index{});
}
void initPrimeNumber(){
	int i,j=2, number=setShingles.size();
	for(i=number+1;i<3000;i++) {
        	for(j=2;j <i;j++) {
			if(i %j==0) break;	
		}
        	if(i==j || i==1) {
			primeNumber = i;    
			break;
		}
	}
}
void initIndex() {
	initPrimeNumber();
	// iniciamos los valores de los indices para el hasheoº
	for (unsigned int i = 0; i < indexHash.size(); i++) {
		srand(time(NULL)*(i+137477057));
		indexHash[i].a = rand();	
		indexHash[i].b = rand();
	}
}
void initBooleanShingles(){
	//encuentra que valores de cada set de cada documento tienen alguna ocurrencia en el set global de shingles y los pone true, si no falso
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
}


/*
 *
 * MAIN FUNCTIONS
 *
 */

void minhashSignatures() {
	//el hasheo es del estilo (a*i + b) mod primeNumber
	//miro cada row de b y después cada fila y si da true, modifico v si el valor del hash es inferior a este
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
}
float jaccardSimSignature(const int &a, const int &b){
	// dado dos filas de signatures a y b, devuelve la similitud entre esos dos documentos 
	float value = 0;
	for (unsigned int i = 0; i < signatureMatrix.size(); i++) {
		if(signatureMatrix[i][a] == signatureMatrix[i][b]) value++;
	}
	return value/signatureMatrix.size();
}
void  kShingle (){
	// esta función añade a setShingles todos los shingles de todos los documentos
	// mientras que docShingle tendrá el set de shingles de cada documento
	string s, word;
	string aux = " ";
	for (int j = 1; j <= nDoc; ++j){
		string fileAux;
		set <string> actualDocShingle;
		fileAux = to_string(j); 
		ifstream file("data/docs_requisits/doc"+fileAux+".txt");
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
				if(col!=k and buckets[band][col] == buckets[band][k]) {
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
float jaccardSimilarity(set <string> &a, const int  &b) {
	//consigue la similitud de dos sets
        float intersection = 0;
        for(auto it = docShingles[b].begin(); it != docShingles[b].end(); it++) {
		if (!a.insert(*it).second) {
			//cout<<intersection<<" ";
			intersection+=1;
		}
        }
        return intersection / (float) a.size();
}
float jaccardSimShingle(const int &a, const int &b){
	//consigue la similitud de los sets a y b (cada set son Shingles)
	set<string> aux= docShingles[a];
	return jaccardSimilarity(aux, b);
}
void allJaccardSimilarity(int x){
	bool similars = false;
	for(unsigned int i = 0; i <nDoc-1; i++){
		for(unsigned int j = 1; j <nDoc; j++){
			if (x==1 and i!=j){ 
				float aux = jaccardSimShingle(i,j);
			        if(aux>=t) {
					cout << "Similitud de Jaccard ("<<"doc"<<i+1  << ".txt, " <<"doc"<<j+1<<".txt) = " << aux << endl;
					similars = true;
				}
			} else if (i!=j){
				float aux = jaccardSimSignature(i,j);
				if(aux>=t){
				 	cout << "Aproximación de similitud de Jaccard("<<"doc"<<i+1  << ".txt, " <<"doc"<<j+1<<".txt) = " << aux << endl;
					similars = true;
				}	
			}	
		}
	}
	if (!similars) {
	    cout << "Ningún documento se parece con ese threshold"<<endl;
	}
}

void funcionalidades(){
	cout<<"envía 0 para salir"<<endl<<
		"envía 1 para obtener la similitud de Jaccard de dos documentos"<<endl<<
		"envía 2 para obtener la aproximación del grado de similitud de Jaccard a través de representaciones signatures minhash de dos docuemntos"<<endl<<
		"envía 3 para obtener los documentos con una similitud de Jaccard superior a "<<t<<endl<<
		"envía 4 para obtener los documentos con la aproximación del grado de similitud de Jaccard superior a "<<t<<endl
		<<"envía 5 para obtener los documentos con la aproximiación del grado de similtud de Jaccard a través de representciones signatures minhash de todos los documentos a superior a "<<t<<"  (utilizando LSH)"<<endl;
}

int main(int argc, char *argv[]) {
	init(argc, argv);
	int funcio;
	// ponemos los valores setShingles y kShingles a punto (crea shingles y los añade individualmente por documento 
	// en docShingles y globalmente en setShingles
	clock_t c_start = clock();
	kShingle();
	clock_t c_end = clock();
        cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado para shinglear todos los documentos: "
                << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
	
	// ponemos los valores de indexHash a punto
	c_start = clock();
	initIndex();
	c_end = clock();
        cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado para encontrar las ocurrencias  de cada documento respecto el set global de shingles: "
                << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
	
	
	//iniciamos booleanShingles 
	c_start = clock();
	initBooleanShingles();
	
	//creamos las signatures y las ponemos a punto
	c_start = clock();
	minhashSignatures();
	c_end = clock();
        cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: para crear las signatures minhash: "
                << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;

	//dice por pantalla las funcionalidades disponibles
	cout<<setShingles.size()<<endl;
	cout<<endl;
	funcionalidades();
	while (cin>>funcio){
	int x,y,lastValue;
		switch(funcio){
			case 0:
				exit(1);
				break;
			case 1:
				cout<<"Qué dos documentos (a y b) quieres obtener similitud de Jaccard? (valor menor que "<<nDoc<<")"<<endl;
				cin>> x >> y;
				c_start = clock();
				cout<<"La similitud de doc"<<x<<".txt y doc"<<y<<".txt es de "<<jaccardSimShingle(x-1,y-1)<<endl<<endl;
				c_end = clock();
        			cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: "
             				 << 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl<<endl;
				funcionalidades();
				break;
			case 2:
				cout<<"Qué dos documentos (a y b), quieres obtener la aproximación del grado de similitud usando signatures? (valor menor que "<<nDoc<<")"<<endl<<endl;
				cin>>x>>y;
				c_start = clock();
				cout<<"La aproximación del grado de similitud de doc"<<x<<".txt y doc"<<y<<".txt es de "<<jaccardSimSignature(x-1,y-1)<<endl<<endl;
				c_end = clock();
        			cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: "
          				<< 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
				funcionalidades();
				break;
			case 3:
				c_start = clock();
				allJaccardSimilarity(1);
				c_end = clock();
        			cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: "
          				<< 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
				funcionalidades();
				break;
			case 4:
				c_start = clock();
				allJaccardSimilarity(2);
				c_end = clock();
        			cout<<fixed<<setprecision(2)<<"Tiempo de CPU utilizado: "
          				<< 1000.0*(c_end-c_start)/CLOCKS_PER_SEC <<"ms"<<endl;
				funcionalidades();
				break;
			case 5:
				char option;
				cout<<"Quieres que el valor de b y nHashFunctions sean óptimos? y/n"<<endl;
				cin>> option;
				if(option=='y'){
					case 'y':
					cout<<"Qué threshold quieres? intervalos de 0.1 a 1 en 0.1 en 0.1: "<<endl;
					cin>>t;
					lastValue = b;
					b=aproxValues[((int) t*10)-1];
					nHashFunctions = b*3;
				}
				else {
					cout<<"Quieres los valores nHasValue, b y t predeterminados? y/n"<<endl;
					cin>> option;
			 		if(option=='n'){		
					cout<<"Introduce tu valor nHashValue"<<endl;
					cin>>nHashFunctions;
					cout<<"Introduce tu valor b"<<endl;
					cin>>b;
					cout<<"Introduce tu valor t"<<endl;
					cin>>t;
					cout<<"Depende de la configuración usada podrás tener un porcentaje mayor o menor"<<
						" de falsos positivos o falsos negativos"<<endl;
					}
				}
					
				initIndex();	
				initBooleanShingles();
				minhashSignatures();
				LSH();		
				b = lastValue;
				nHashFunctions = b*3;	
				initIndex();
				initBooleanShingles();
				minhashSignatures();
				funcionalidades();
				break;
		}
	}
}	
