#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <climits>
#include <algorithm>

using namespace std;

//primeNumber debería ser el primer número primo superior a Ndoc
unsigned int primeNumber = 5;
unsigned int nDoc = 4;
unsigned int setSize = 5;
unsigned int nHashFunctions = 2;
struct index {
	unsigned int a;
	unsigned int b;
};
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
		for (unsigned int j = 0; j < v[0].size(); j++) {
			cout << v[i][j]<<" ";
		}
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
	return s.size();
}
void init(vector<index> &v) {
	for (unsigned int i = 0; i < v.size(); i++) {
		srand(time(NULL)*(i+137477057));
		v[i].a = rand();	
		v[i].b = rand();
	}
}

void getShingles (vector< vector < bool > > &v) {
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
	float value = 0;
	for (unsigned int i = 0; i < v.size(); i++) {
		if(v[i][a] == v[i][b]) value++;
	}
	return value/v.size();
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
int main() {
//	prueba();

	//inicializa los valores a y b para hashear,	
	vector<index> indexHash(nHashFunctions, index{});
	init(indexHash);
	
//	output(indexHash);
	
	//getShingles consigue la tabla de las ocurrencias del documento j con el shingle i. 
	//es decir que j es el número de documentos y i es el número total de shingles (set.size de los shingles)
	vector< vector < bool> > vShingles(setSize, vector<bool> (nDoc,false)); 
	getShingles(vShingles);
	
	//Consigue la signature matrix de 
	vector< vector < unsigned int > > signatureMatrix(nHashFunctions, vector<unsigned int> (nDoc, UINT_MAX));
	minhashSignatures(signatureMatrix, vShingles, indexHash);

}
