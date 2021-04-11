#include <string>

#include <iostream>
#include <list>

using namespace std;

class ForwardTable{
public:
    ForwardTable(string n, int os, int sec);
    string name;
    int offset;
    int section;
};

class SymbolTable{
public:
    SymbolTable(char s, int n, string na, int num_s, int val, char f);
    char sec;
    int num;
    string name;
    int num_sec;
    int value;
    char flag; 
    list <ForwardTable*> ft;  
};

class RelocationTable{
public:
    RelocationTable(string ns, int of, string t, int n);
    string name_section;
    int offset;
    string tip;
    int num_sym;
};


void Pass();