#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

float jaccardSimilarity(set <string> a, const set <string> &b) {
	float intersection = 0;
	for(auto it = b.begin(); it != b.end(); it++) {
		if(!a.insert(*it).second)
			intersection++; 
	}
	return intersection / (float) a.size();
}

int main() {
	set <string> s, t;
	//s = { "ab", "ba", "ac", "cd" };
	//t = { "ba", "ab", "ac", "cd" };
	s = {"0","1","2","5","6"};
	t =  {"0","2","3","4","5","7","9"};
	float js = jaccardSimilarity(s, t);
	cout << "The Jaccard Similarity is: " << js << endl;
}


/*
void printJSInput(const vector<string> s, const vector<string> t) {
	if (s.size() > 0 and t.size() > 0) {
		int i;
		for (i = 0; i < s.size() - 1; i++)
			cout << s[i] + " ";
		cout << s[i] << endl;
		for (i = 0; i < t.size() - 1; i++)
			cout << t[i] + " ";
		cout << t[i] << endl;
	}else cout << "Empty vectors" << endl;
}

//Eq size

float jaccardSimilarity(const vector<string> s, const vector<string> t) {
	printJSInput(s,t);
	if ((s.size() - t.size()) != 0)
		return -1.0;
	else{
		int n = s.size();
		float ret = 0.0;
		for (int i = 0; i < n; i++) {
			if (s[i] == t[i])
				ret += 1.0;
		}
		return ret / n;
	}
}


//Pre: s.size() >= 0 and t.size() >= 0
//Post: the returning value is the union of s and t 
vector<string> createOrderedUnion(vector<string> s, vector<string> t) {
	int nS = s.size();
	int nT = t.size();

	vector<string> unio(nS + nT);
	vector<string>::iterator it;

	sort(s, s + nS);
	sort(t, t + nT);

	it = set_union(s, s+nS, t, t+nT, unio.begin());
	//debido a los elementos comunes unio puede ser menor a la suma de los dos conjuntos
	unio.resize(it-unio.begin());
	return unio;
}

vector<string> createOrderedIntersection(vector<string> s, vector<string> t) {
	int nS = s.size();
	int nT = t.size();

	vector<string> inter(nS + nT);
	vector<string>::iterator it;

	sort(s, s + nS);
	sort(t, t + nT);

	it = set_intersection(s, s + nS, t, t + nT, inter.begin());
	
	inter.resize(it-inter.begin());
	return inter;
}

float jaccardSimilarity1(vector<string> s, vector<string> t) {
	printJSInput(s,t);
	if(s.size() < 0 or t.size() < 0) return -1.0;
	else {
		vector<string> u = createOrderedUnion(s,t);
		vector<string> i = createOrderedIntersection(s,t);

		return i.size()/u.size();
	}
}
*/