#include<iostream>
#include<fstream>
#include<string>
#include<streambuf>
#include<sstream>
#include <set>
using namespace std;

set<string> kShingle (ifstream &file, const int &k){
    set<string> setShingle;
    string s, word;
    string aux = " ";
    for (unsigned int i = 0; i < k and file>>word; i++){
        if(i==0) s = word;
        else s.append(" "+word );
    }
    setShingle.insert(s);
    while (file>>word){
        s=s.substr(s.find_first_of(aux)+1);
        s.append(" "+word);
        setShingle.insert(s);
    }
    return setShingle;
}

int main(){
    int k=8;
    string line;
    ifstream file("doc.txt");
    set<string> s = kShingle(file, k);
    
    for (set<string>:: iterator it = s.begin(); it!=s.end(); it++){
        cout<< *it <<endl;
    }
    
}
