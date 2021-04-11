#include "project.h"
#include <iostream>
#include <fstream>
#include <list>
#include <regex>
#include <iterator>
#include <map>

using namespace std;

SymbolTable::SymbolTable(char s, int n, string na, int num_s, int val, char f){
    sec = s;
    num = n;
    name = na;
    num_sec = num_s;
    value = val;
    flag = f;
}

RelocationTable::RelocationTable(string ns, int of, string t, int n){
    name_section = ns;
    offset = of;
    tip = t;
    num_sym = n;
}

ForwardTable::ForwardTable(string n, int of, int sec){
    name = n;
    offset = of;
    section = sec;
}

void Pass(){

    int lastID = 0;
    int LC = 0;
    int current_sec = -1;

    list <SymbolTable*> symbolTable;
    list <ForwardTable*> forwardTable;
    list <list<RelocationTable*>> relocationTable;

    map <string, int> mapOfOC;
    mapOfOC["int"] = 3;
    mapOfOC["call"] = 4;
    mapOfOC["jmp"] = 5;
    mapOfOC["jeq"] = 6;
    mapOfOC["jne"] = 7;
    mapOfOC["jgt"] = 8;
    mapOfOC["push"] = 9;
    mapOfOC["pop"] = 10;
    mapOfOC["xchg"] = 11;
    mapOfOC["mov"] = 12;
    mapOfOC["add"] = 13;
    mapOfOC["sub"] = 14;
    mapOfOC["mul"] = 15;
    mapOfOC["div"] = 16;
    mapOfOC["cmp"] = 17;
    mapOfOC["not"] = 18;
    mapOfOC["and"] = 19;
    mapOfOC["or"] = 20;
    mapOfOC["xor"] = 21;
    mapOfOC["test"] = 22;
    mapOfOC["shl"] = 23;
    mapOfOC["shr"] = 24;


    list <list<int>> code;
    list <int> sec_code;

    string line;
    ifstream myfile ("input.txt");
    if(myfile.is_open())
    {
        while (getline(myfile, line))
        {
            
            line.append(" ");
            
            regex reg ("([a-zA-Z0-9]+)[\\S\\s]");
            regex hexanum("(0x[0-9a-f]+)");
            regex hexn("([0-9a-f]+)");
            smatch matches;
            
            while(regex_search(line, matches, reg)){

                string sec_name = matches.str(1);
                //cout << sec_name << "\n";
                line = matches.suffix().str();
                //cout << line << "\n";
                
                if(sec_name.compare("global")==0){
                    //cout << "GLOBAL";
                    while (regex_search(line, matches, reg)){
                    string name_curr = matches.str(1);
                    line = matches.suffix().str();
                    SymbolTable* curr = new SymbolTable('m', lastID++, name_curr , 0, 0, 'g');
                    ForwardTable* curr_forward = new ForwardTable(name_curr, -1, -1);
                    curr->ft.push_back(curr_forward);
                    symbolTable.push_back(curr); 
                    forwardTable.push_back(curr_forward);
                    //cout << line << "\n";
                    }
                } else  if(sec_name.compare("extern")==0){
                    //cout << "EXTERN";
                    while (regex_search(line, matches, reg)){
                    string name_curr = matches.str(1);
                    line = matches.suffix().str();
                    SymbolTable* curr = new SymbolTable('m', lastID++, name_curr , -1, 0, 'g');
                    symbolTable.push_back(curr); 
                    //cout << line << "\n";
                    }
                } else   if(sec_name.compare("section")==0){
                   
                    if(current_sec == -1){
                        current_sec = lastID;
                    } else
                    {
                        list <SymbolTable*> :: iterator it; 
                        for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                        SymbolTable* item = *it;
                        if(item->num==current_sec) {
                            item->value = LC;
                            break;
                        }
                    }
                        LC = 0;
                        current_sec = lastID;
                    }
                    //cout << "SECTION";
                    regex_search(line, matches, reg);
                    string name_curr = matches.str(1);
                    line = matches.suffix().str();
                    SymbolTable* curr = new SymbolTable('c', lastID, name_curr , lastID, 0, 'l');
                    lastID++;
                    symbolTable.push_back(curr); 
                    RelocationTable* first_rel = new RelocationTable (sec_name, -1, "", current_sec);
                    list<RelocationTable*> rel;
                    rel.push_back(first_rel);
                    relocationTable.push_back(rel);

                    //CODE FOR SECTION CREATE LIST
                    sec_code.push_back(current_sec);
                    list <int> l;
                    //l.push_back(234);
                    code.push_back(l);
                    //cout << line << "\n";
                } else   if(sec_name.compare("end")==0){
                   
                        list <SymbolTable*> :: iterator it; 
                        for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                        SymbolTable* item = *it;
                        if(item->num==current_sec) {
                            item->value = LC;
                            break;
                            }
                        }
                        LC = 0;
                        break;
                    
                    
                }  
                else  if(sec_name.compare("intb")==0 || sec_name.compare("callb")==0 || sec_name.compare("jmpb")==0 ||
                           sec_name.compare("jeqb")==0 || sec_name.compare("jneb")==0 || sec_name.compare("jgtb")==0 ||
                           sec_name.compare("popb")==0 || sec_name.compare("pushb")==0 ||

                           sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                           sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                           sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 ||
                           
                           sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                           sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                           sec_name.compare("pop")==0 || sec_name.compare("push")==0   ){

                    regex reg_dir("(\\*%r[0-7])");
                    regex reg_ind("(\\*\\(%r[0-7])");
                    regex reg_dirsp("(\\*%sp)");
                    regex reg_indsp("(\\*\\(%sp)");
                    regex num("([0-7])");
                    regex star("(\\*[a-zA-Z0-9]+)[\\S\\s]");
                    regex name("([a-zA-Z]+)");
                    regex numebers("([0-9]+)");
                    regex re("(pc)");
                    regex reg_star("(r[0-7])");
                    regex sp("(sp)");
                    //cout << "1 bajtni" << "\n";

                    list <int> cod;
                    list <list<int>> :: iterator ic = code.begin();
                    int pom = 0;
                    list <int> :: iterator ci = sec_code.begin();
                    for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                        if(*ci == current_sec) break;
                    }
                    advance(ic, pom);
                    cod = *ic;
                    LC += 1;
                    if(sec_name.compare("intb")==0) cod.push_back(mapOfOC.at("int")*8);
                    if(sec_name.compare("intw")==0 or sec_name.compare("int")==0) cod.push_back(mapOfOC.at("int")*8+4);
                    if(sec_name.compare("callb")==0) cod.push_back(mapOfOC.at("call")*8);
                    if(sec_name.compare("callw")==0 or sec_name.compare("call")==0) cod.push_back(mapOfOC.at("call")*8+4);
                    if(sec_name.compare("jmpb")==0) cod.push_back(mapOfOC.at("jmp")*8);
                    if(sec_name.compare("jmpw")==0 or sec_name.compare("jmp")==0) cod.push_back(mapOfOC.at("jmp")*8+4);
                    if(sec_name.compare("jeqb")==0) cod.push_back(mapOfOC.at("jeq")*8);
                    if(sec_name.compare("jeqw")==0 or sec_name.compare("jeq")==0) cod.push_back(mapOfOC.at("jeq")*8+4);
                    if(sec_name.compare("jneb")==0) cod.push_back(mapOfOC.at("jne")*8);
                    if(sec_name.compare("jnew")==0 or sec_name.compare("jne")==0) cod.push_back(mapOfOC.at("jne")*8+4);
                    if(sec_name.compare("jgtb")==0) cod.push_back(mapOfOC.at("jgt")*8);
                    if(sec_name.compare("jgtw")==0 or sec_name.compare("jgt")==0) cod.push_back(mapOfOC.at("jgt")*8+4);
                    if(sec_name.compare("pushb")==0) cod.push_back(mapOfOC.at("push")*8);
                    if(sec_name.compare("pushw")==0 or sec_name.compare("push")==0) cod.push_back(mapOfOC.at("push")*8+4);
                    if(sec_name.compare("popb")==0) cod.push_back(mapOfOC.at("pop")*8);
                    if(sec_name.compare("popw")==0 or sec_name.compare("pop")==0) cod.push_back(mapOfOC.at("pop")*8+4);

                    sec_code.remove(current_sec);
                    sec_code.push_back(current_sec);
                    ic = code.erase(ic);
                    code.push_back(cod);


                    
                    //smatch dst_match;*/

                    ic = code.begin();
                    pom = 0;
                    ci = sec_code.begin();
                    for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                        if(*ci == current_sec) break;
                    }   
                    advance(ic, pom);
                    cod = *ic;

                    if(regex_search(line, matches, reg_dir) or regex_search(line, matches, reg_dirsp)) {
                        //cout << "OVO JE REG" << "\n";
                        //cout << matches.str(1) << "\n";
                        //========REGISTARSKO DIREKTNO=================
                        if(regex_search(line, matches, reg_dir)) {
                        string num_reg = matches.str(1);
                        string l = matches.suffix().str();
                        
                        regex_search(l, matches, num);
                        num_reg = matches.str(1);  

                        regex reg_high("(\\*%r[0-7]h)");

                        if(regex_search(line, matches, reg_high))  cod.push_back(32 + 2*stoi(num_reg)+1);
                        else   cod.push_back(32 + 2*stoi(num_reg));

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);
                        } else {
                        cod.push_back(32 + 2*6);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);
                        }

                        
                    } else if(regex_search(line, matches, reg_ind) or regex_search(line, matches,reg_dirsp)) {
                        //cout << "OVO JE IND" << "\n";
                        //cout << matches.str(1) << "\n";
                        //========REGISTARSKO INDIREKTNO=================

                        if(regex_search(line, matches, reg_ind)){

                        string num_reg = matches.str(1);
                        string l = matches.suffix().str();
                        
                        regex_search(l, matches, num);
                        num_reg = matches.str(1); 

                        regex reg_high("(\\*%r[0-7]h)");

                        if(regex_search(line, matches, reg_high))  cod.push_back(64 + 2*stoi(num_reg)+1);
                        else   cod.push_back(64 + 2*stoi(num_reg));
                        

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        } else {
                        cod.push_back(64 + 2*6);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);
                        }

                        
                    }
                    else if(regex_search(line, matches, star)){

                        if(regex_search(line, matches, re)){
                             cod.push_back(96 + 2*7);

                             regex zagrada("([a-zA-Z]+)");
                                if(regex_search(line, matches, zagrada)){
                                    cout << "PRE ZAGRADE " << matches.str(1);
                                   
                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);
                                }
                                    
                        } else
                        if(regex_search(line, matches, reg_star) or regex_search(line, matches, sp)){
                            //================REG POM=====================
                            cout << "REG POM KEC" << matches.str(1) << " " << line;
                            string rum = matches.str(1);
                            if(regex_search(rum, matches, num))
                            {
                                string num_reg = matches.str(1);
                                cout << num_reg;

                                regex reg_high("(r[0-7]h)");

                                if(regex_search(line, matches, reg_high)) cod.push_back(96 + 2*stoi(num_reg)+1);
                                else cod.push_back(96 + 2*stoi(num_reg));
                            } else {
                                cod.push_back(96 + 2*6);
                            }


                                regex zagrada("([^//()]+)");
                                if(regex_search(line, matches, zagrada)){
                                    cout << "PRE ZAGRADE " << matches.str(1);
                                    string broj = matches.str(1);
                                    if(regex_search(broj, matches, hexanum)){

                                        cout << "HEX POM ";
                                        string h = matches.str(1);

                                        regex_search(h, matches, hexn);
                                        string hh = matches.suffix().str();
                                        regex_search(hh, matches, hexn);
                                        string heksabroj = matches.str(1);

                                        int x;   
                                        std::stringstream ss;
                                        ss << std::hex << heksabroj;
                                        ss >> x;

                                        cout << "INT - REG MEMORIJSKO - " << x <<"\n";

                                        cod.push_back(x%256);
                                        cod.push_back(x/256);
                                        LC+=2;
                                            

                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);

                                        line = "";
                                        

                                        } else
                                    if(regex_search(broj, matches, name)){
                                        //nista

                                        cout << "Slovo";
                                        cout << matches.str(1);
                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);

                                    } else if(regex_search(broj, matches, numebers)){
                                    // broj
                                        cout << "Broj";
                                        cout << matches.str(1);

                                        cod.push_back(stoi(matches.str(1))%256);
                                        cod.push_back(stoi(matches.str(1))/256);
                                        LC+=2;
                                        

                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);

                                        line = "";
                                }
                                }
                            
                        } else {
                        //========================MEM=======================

                        cod.push_back(128);

                        cout << "MEM JEDAN";

                        if(regex_search(line, matches, hexanum)){

                            cout << "HEX POM ";
                            string h = matches.str(1);

                            regex_search(h, matches, hexn);
                            string hh = matches.suffix().str();
                            regex_search(hh, matches, hexn);
                            string heksabroj = matches.str(1);

                            int x;   
                            std::stringstream ss;
                            ss << std::hex << heksabroj;
                            ss >> x;

                            cout << "INT - REG MEMORIJSKO - " << x <<"\n";

                            cod.push_back(x%256);
                            cod.push_back(x/256);
                            LC+=2;
                                

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            line = "";

                        } else if(regex_search(line, matches, name)){
                            //nista
                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                        } else if(regex_search(line, matches, numebers)){
                            // broj
                            cout << matches.str(1);

                            cod.push_back(stoi(matches.str(1))%256);
                            cod.push_back(stoi(matches.str(1))/256);
                            LC+=2;
                            

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            line = "";
                        }

                        
                       
                        }

                    } else {
                    //==========================NEPOSREDNO=====================
                        cod.push_back(0);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);
                    }

                    LC += 1;

                    ic = code.begin();
                    pom = 0;
                    ci = sec_code.begin();
                    for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                        if(*ci == current_sec) break;
                    }   
                    advance(ic, pom);
                    cod = *ic;

                    while (regex_search(line, matches, reg)){

                    string name_curr = matches.str(1);
                    line = matches.suffix().str();

                    
                    smatch sm;
                    regex_search(line, sm, re);
                    string pc_jmp = sm.str(1);
                    //cout << name_curr;
                    cout << pc_jmp << "OVO JE PC \n";
                    cout << name_curr << "\n";

                    //RELOCATION ADD
                            
                    //search  
                    bool find_sym = false;
                    list <SymbolTable*> :: iterator it; 
                    for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                        SymbolTable* item = *it;
                        if((item->name.compare(name_curr)==0)) {
                            
                            find_sym = true;
                            if(sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                            sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                            sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 || 
                            
                            sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                            sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                            sec_name.compare("pop")==0 || sec_name.compare("push")==0  ) LC += 2;
                            else LC += 1;

                            if(item->value == 0) {
                            ForwardTable* curr_forward = new ForwardTable(name_curr, LC, current_sec);
                            item->ft.push_back(curr_forward);
                            forwardTable.push_back(curr_forward);
                            }

                            //RELOCATION ADD
                            list<RelocationTable*> ite;
                            list <list<RelocationTable*>> :: iterator rt; 
                            for(rt = relocationTable.begin(); rt != relocationTable.end(); ++rt) {
                                ite = *rt;
                                list <RelocationTable*> :: iterator srt; 
                                //cout << relocationTable.size() << "\n";
                                srt = ite.begin();  
                                RelocationTable* rtt = *srt;
                                /*cout << sec_name << "=============" << "\n";
                                cout << rtt->num_sym << " " << name_curr << "\n"; 
                                cout << current_sec << "\n";*/
                                if(rtt->num_sym == current_sec) {
                                    RelocationTable* currel;
                                    if(pc_jmp.compare("pc")==0) {
                                        if(item->num_sec == current_sec){
                                            //ista sekcija pc
                                            cout << "negativan pomeraj jer su u istoj sekciji ";
                                            int p = ((item->value-LC + 2)%256)  ;
                                            cout << (p&(255));
                                           
                                            cod.push_back(((item->value-LC + 2)%256));
                                            cod.push_back(((item->value-LC + 2)/256));
                                        } else
                                        if(item->flag == 'g') 
                                        {
                                        currel = new RelocationTable(name_curr, LC-1, "R_386_PC32", current_sec);
                                        /*if(sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                                        sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                                        sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 || 
                                        
                                        sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                                        sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                                        sec_name.compare("pop")==0 || sec_name.compare("push")==0    ) */
                                        cod.push_back(-2);
                                        cod.push_back(-1);
                                        ite.push_back(currel);
                                    
                                        //cout << ite.size() << "\n";
                                        rt = relocationTable.erase(rt);
                                        relocationTable.push_back(ite); 
                                        }
                                        else 
                                        {
                                        currel = new RelocationTable(name_curr, LC-1, "R_386_PC32", 4321);
                                        /*if(sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                                        sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                                        sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 || 
                                        
                                        sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                                        sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                                        sec_name.compare("pop")==0 || sec_name.compare("push")==0   )  */
                                        cod.push_back(item->value-2);
                                        cod.push_back(0);
                                        ite.push_back(currel);
                                    
                                        //cout << ite.size() << "\n";
                                        rt = relocationTable.erase(rt);
                                        relocationTable.push_back(ite); 
                                        }

                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);
                                    }
                                    else { 
                                        if(item->flag == 'g') 
                                       {
                                        currel = new RelocationTable(name_curr, LC-1, "R_386_32", item->num);
                                        if(sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                                        sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                                        sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 || 
                                        
                                        sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                                        sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                                        sec_name.compare("pop")==0 || sec_name.compare("push")==0   )  cod.push_back(0);
                                        cod.push_back(0);
                                        }
                                        else
                                        { 
                                        currel = new RelocationTable(name_curr, LC-1, "R_386_32", 4321);
                                        if(sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                                        sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                                        sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 || 
                                        
                                        sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                                        sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                                        sec_name.compare("pop")==0 || sec_name.compare("push")==0  )  cod.push_back(0);
                                        cod.push_back(item->value);
                                        }

                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);
                                        
                                    }
                                    if(pc_jmp.compare("pc")!=0) {ite.push_back(currel);
                                    
                                    //cout << ite.size() << "\n";
                                    rt = relocationTable.erase(rt);
                                    relocationTable.push_back(ite); 
                                    }
                                    break;
                                }
                            }
                            line = "";
                            break;
                        }
                     
                    }
                    if(find_sym == false && name_curr.compare("pc")!=0 && name_curr.compare("r0")!=0 && name_curr.compare("r1")!=0
                        && name_curr.compare("r2")!=0 && name_curr.compare("r3")!=0 && name_curr.compare("r4")!=0
                        && name_curr.compare("r5")!=0 && name_curr.compare("r6")!=0 && name_curr.compare("r7")!=0
                        && name_curr.compare("r0h")!=0 && name_curr.compare("r1h")!=0
                        && name_curr.compare("r2h")!=0 && name_curr.compare("r3h")!=0 && name_curr.compare("r4h")!=0
                        && name_curr.compare("r5h")!=0 && name_curr.compare("r6h")!=0 && name_curr.compare("r7h")!=0
                        && name_curr.compare("r0l")!=0 && name_curr.compare("r1l")!=0
                        && name_curr.compare("r2l")!=0 && name_curr.compare("r3l")!=0 && name_curr.compare("r4l")!=0
                        && name_curr.compare("r5l")!=0 && name_curr.compare("r6l")!=0 && name_curr.compare("r7l")!=0
                        && name_curr.compare("sp")!=0) {

                            if(sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                            sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                            sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 || 
                            
                            sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                            sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                            sec_name.compare("pop")==0 || sec_name.compare("push")==0    )  {cod.push_back(-2&255); LC+=1;}
                            cod.push_back(0);
    
                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            LC+=1;

                            SymbolTable* curr = new SymbolTable('m', lastID++, name_curr , -2, 0, 'l');
                            ForwardTable* curr_forward = new ForwardTable(name_curr, LC, current_sec);
 
                            curr->ft.push_back(curr_forward);
                            symbolTable.push_back(curr); 
                            forwardTable.push_back(curr_forward);

                            //RELOCATION ADD
                            list<RelocationTable*> ite;
                            list <list<RelocationTable*>> :: iterator rt; 
                            for(rt = relocationTable.begin(); rt != relocationTable.end(); ++rt) {
                                ite = *rt;
                                list <RelocationTable*> :: iterator srt; 
                                srt = ite.begin();  
                                RelocationTable* rtt = *srt;
                                //cout << rtt->num_sym << "\n";
                                if(rtt->num_sym == current_sec) {
                                    /*cout << sec_name << "=============" << "\n";*/
                                    RelocationTable* currel;
                                    //cout << ite.size() << "\n";
                                    if(pc_jmp.compare("pc")==0) currel = new RelocationTable(name_curr, LC-1, "R_386_PC32", current_sec);
                                    else currel = new RelocationTable(name_curr, LC-1, "R_386_32", 4321);
                                    ite.push_back(currel);
                                    //LC += 1;
                                    //cout << ite.size() << "\n";
                                    rt = relocationTable.erase(rt);
                                    relocationTable.push_back(ite);
                                    break;
                                }
                            }
                            

                           

                        //cout << line << "\n";
                    }
                    }
                    

            } else  if(sec_name.compare("xchgb")==0 || sec_name.compare("xchgw")==0 || sec_name.compare("addw")==0 || sec_name.compare("addb")==0 ||
                           sec_name.compare("movb")==0 || sec_name.compare("movw")==0 || sec_name.compare("subw")==0 || sec_name.compare("subb")==0 ||
                           sec_name.compare("divb")==0 || sec_name.compare("divw")==0 || sec_name.compare("mulw")==0 || sec_name.compare("mulb")==0 ||
                           sec_name.compare("cmpb")==0 || sec_name.compare("cmpw")==0 || sec_name.compare("notw")==0 || sec_name.compare("notb")==0 ||
                           sec_name.compare("andb")==0 || sec_name.compare("andw")==0 || sec_name.compare("xorw")==0 || sec_name.compare("xorb")==0 ||
                           sec_name.compare("orb")==0 || sec_name.compare("orw")==0 || sec_name.compare("testw")==0 || sec_name.compare("testb")==0 ||    
                           sec_name.compare("shlb")==0 || sec_name.compare("shrw")==0 || sec_name.compare("shlw")==0 || sec_name.compare("shrb")==0 ||
                           
                           sec_name.compare("xchg")==0 || sec_name.compare("add")==0 ||
                           sec_name.compare("mov")==0 || sec_name.compare("sub")==0  ||
                           sec_name.compare("div")==0 ||  sec_name.compare("mul")==0 || 
                           sec_name.compare("cmp")==0 || sec_name.compare("not")==0 ||
                           sec_name.compare("and")==0 || sec_name.compare("xor")==0 ||
                           sec_name.compare("or")==0 || sec_name.compare("test")==0 ||    
                           sec_name.compare("shl")==0 || sec_name.compare("shr")==0 ){

                    list <int> cod;
                    list <list<int>> :: iterator ic = code.begin();
                    int pom = 0;
                    list <int> :: iterator ci = sec_code.begin();
                    for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                        if(*ci == current_sec) break;
                    }
                    advance(ic, pom);
                    cod = *ic;
                    LC += 1;           

                    if(sec_name.compare("xchgb")==0) cod.push_back(mapOfOC.at("xchg")*8);
                    if(sec_name.compare("xchgw")==0 or sec_name.compare("xchg")==0) cod.push_back(mapOfOC.at("xchg")*8+4);
                    if(sec_name.compare("movb")==0) cod.push_back(mapOfOC.at("mov")*8);
                    if(sec_name.compare("movw")==0 or sec_name.compare("mov")==0) cod.push_back(mapOfOC.at("mov")*8+4);
                    if(sec_name.compare("addb")==0) cod.push_back(mapOfOC.at("add")*8);
                    if(sec_name.compare("addw")==0 or sec_name.compare("add")==0) cod.push_back(mapOfOC.at("add")*8+4);
                    if(sec_name.compare("subb")==0) cod.push_back(mapOfOC.at("sub")*8);
                    if(sec_name.compare("subw")==0 or sec_name.compare("sub")==0) cod.push_back(mapOfOC.at("sub")*8+4);
                    if(sec_name.compare("mulb")==0) cod.push_back(mapOfOC.at("mul")*8);
                    if(sec_name.compare("mulw")==0 or sec_name.compare("mul")==0) cod.push_back(mapOfOC.at("mul")*8+4);
                    if(sec_name.compare("divb")==0) cod.push_back(mapOfOC.at("div")*8);
                    if(sec_name.compare("divw")==0 or sec_name.compare("div")==0) cod.push_back(mapOfOC.at("div")*8+4);
                    if(sec_name.compare("cmpb")==0) cod.push_back(mapOfOC.at("cmp")*8);
                    if(sec_name.compare("cmpw")==0 or sec_name.compare("cmp")==0) cod.push_back(mapOfOC.at("cmp")*8+4);
                    if(sec_name.compare("notb")==0) cod.push_back(mapOfOC.at("not")*8);
                    if(sec_name.compare("notw")==0 or sec_name.compare("not")==0) cod.push_back(mapOfOC.at("not")*8+4);         
                    if(sec_name.compare("andb")==0) cod.push_back(mapOfOC.at("and")*8);
                    if(sec_name.compare("andw")==0 or sec_name.compare("and")==0) cod.push_back(mapOfOC.at("and")*8+4);
                    if(sec_name.compare("orb")==0) cod.push_back(mapOfOC.at("or")*8);
                    if(sec_name.compare("orw")==0 or sec_name.compare("or")==0) cod.push_back(mapOfOC.at("or")*8+4);
                    if(sec_name.compare("xorb")==0) cod.push_back(mapOfOC.at("xor")*8);
                    if(sec_name.compare("xorw")==0 or sec_name.compare("xor")==0) cod.push_back(mapOfOC.at("xor")*8+4);
                    if(sec_name.compare("testb")==0) cod.push_back(mapOfOC.at("test")*8);
                    if(sec_name.compare("testw")==0 or sec_name.compare("test")==0) cod.push_back(mapOfOC.at("test")*8+4);
                    if(sec_name.compare("shlb")==0) cod.push_back(mapOfOC.at("shl")*8);
                    if(sec_name.compare("shlw")==0 or sec_name.compare("shl")==0) cod.push_back(mapOfOC.at("shl")*8+4);
                    if(sec_name.compare("shrb")==0) cod.push_back(mapOfOC.at("shr")*8);
                    if(sec_name.compare("shrw")==0 or sec_name.compare("shr")==0) cod.push_back(mapOfOC.at("shr")*8+4);    

                    sec_code.remove(current_sec);
                    sec_code.push_back(current_sec);
                    ic = code.erase(ic);
                    code.push_back(cod);

                    regex reg_dir("(%r[0-7])");
                    regex reg_ind("(\\(%r[0-7])");
                    regex reg_dirsp("(%sp)");
                    regex reg_indsp("(\\(%sp)");
                    regex sp("(sp)");
                    regex num("([0-7])");
                    regex star("([a-zA-Z0-9]+)[\\S\\s]");
                    regex re("(pc)");
                    regex symbol("([a-zA-Z0-9]+)");
                    regex reg_star("(r[0-7])");
                    regex comma("([^,]+)");
                    regex neposr("(\\$)");
                    regex pomeraj("([a-zA-Z0-9]+(\\(%r[0-7]))");
                    regex pomerajhex("(0x[0-9a-f]+(\\(%r[0-7]))");
                    regex pomerajpc("(\\(%pc\\))");
                    regex pomerajsp("([a-zA-Z0-9]+(\\(%sp))");
                    regex ss(R"((?:^|\s)([+-]?[[:digit:]]*\.?[[:digit:]]+)(?=$|\s))");
                    string pc_jmp;
                    int pompc1 = 0;


                    string sad = "$a";
                    string dvadeset = "a5";

                    if(regex_search(sad, matches, symbol)){
                        cout << "POMERAJ DA \n" << matches.str(1);
                        if(regex_search(dvadeset, matches, symbol)){
                        cout << "reg iscitao br  \n"  << matches.str(1);;
                        
                    }
                    }


                    for(int k=0; k<2 ; k++){
                        ic = code.begin();
                        pom = 0;
                        ci = sec_code.begin();
                        for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                            if(*ci == current_sec) break;
                        }   
                        advance(ic, pom);
                        cod = *ic;
                    regex_search(line, matches, comma);
                    string first_opr = matches.str(1); 
                    pc_jmp = "";
                    if(line == " ") line = "";
                    if(k==1) first_opr = line;
                    
                    cout << first_opr << "\n";


                    if(regex_search(first_opr, matches, pomerajpc)){
                        LC +=1;
                        cout << "PC REL POM";
                        cout << first_opr << "-----------------------\n";
                        cout << line << "-----------------------\n";
           
                        pc_jmp = "pc";
                        cod.push_back(96 + 2*7);
                        regex zagrada("([a-zA-Z]+)");
                        if(k==0){
                            if(regex_search(first_opr, matches, zagrada)){
                                    cout << "PRE ZAGRADE 2 k=0-> " << matches.str(1) << "\n";

                                    first_opr = matches.str(1);

                                    regex second("([,$]+)"); 

                                    regex_search(line, matches, second);

                                    cout << "POSLE ZAGRADE 2 k=0-> " << matches.suffix().str() << "\n";

                                    string second_opr = matches.suffix().str();

                                    if(regex_search(second_opr, matches, pomerajpc)){
                                        cout << "PC DVA";
                                        pompc1 = 3;
                                    }
                                    else
                                    if(regex_search(second_opr, matches, pomeraj) or regex_search(second_opr, matches, pomerajsp)){
                                            //================REG POM=====================
                                            cout << "reg pom DVA";
                                            pompc1 = 3;
                                        } else 
                                    if(regex_search(second_opr, matches, reg_ind) or regex_search(second_opr, matches, reg_indsp)) {
                                        //cout << matches.str(1) << "\n";
                                        //========REGISTARSKO INDIREKTNO=================
                                        cout << "reg ind DVA";
                                        pompc1 = 1;
                                    } else
                                    if(regex_search(second_opr, matches, reg_dir) or regex_search(second_opr, matches, reg_dirsp)) {
                                        //========REGISTARSKO DIREKTNO==============
                                        cout << "reg dir DVA";
                                        pompc1 = 1;
                                    } 
                                    else if(regex_search(second_opr, matches,neposr)){
                                    //==========================NEPOSREDNO=====================
                                        if (sec_name.compare("xchgb")==0 || sec_name.compare("movb")==0 || sec_name.compare("addb")==0 ||
                                        sec_name.compare("subb")==0 || sec_name.compare("mulb")==0 || sec_name.compare("divb")==0 ||
                                        sec_name.compare("cmpb")==0 || sec_name.compare("notb")==0 || sec_name.compare("andb")==0 ||
                                        sec_name.compare("orb")==0 || sec_name.compare("testb")==0 || sec_name.compare("xorb")==0 ||
                                        sec_name.compare("shlb")==0 || sec_name.compare("shrb")==0) pompc1 = 2;
                                        else pompc1 = 3;

                                    } else if(regex_search(second_opr, matches, star)){

                                        cout << "MEM I REG POM" << "\n";

                                        if(regex_search(second_opr, matches, sp)){
                                            //================REG POM=====================
                                            cout << "reg pom ispod DVA";
                                            pompc1 = 3;
                                        }
                                        else 
                                        if(regex_search(second_opr, matches, reg_star)){
                                            //================REG POM=====================
                                            cout << "reg pom ispod DVA";
                                            pompc1 = 3;
                                            
                                        } else {
                                        //========================MEM1=======================
                                            cout << "MEM ispod DVA";
                                            pompc1 = 3;
                
                                        }

                                        } else {
                                            cout << "MEM " << "\n";
                                            cout << "MEMORIJA ispod DVA";
                                            pompc1 = 3;
                                        } 

                                   
                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);
                                }
                        }else{
                            if(regex_search(first_opr, matches, zagrada)){
                                    cout << "PRE ZAGRADE 2 k=1 " << matches.str(1);
                                   
                                        first_opr = matches.str(1);

                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);

                                        line = "";


                                }
                        }
                    }
                    else
                    if(regex_search(first_opr, matches, pomeraj) or regex_search(first_opr, matches, pomerajsp) 
                                    or regex_search(first_opr, matches, pomerajhex)){
                            //================REG POM=====================
                            LC += 1;
                            cout << "REG POM1\n";
                            if(regex_search(first_opr, matches, pomerajsp)){
                                cod.push_back(96 + 2*6);
                            }
                            else
                            if(regex_search(first_opr, matches, reg_star)){
                                string rnum = matches.str(1);
                            if(regex_search(rnum, matches, num))
                            {
                            string num_reg = matches.str(1);
                            regex reg_high("(r[0-7]h)");

                            if(regex_search(first_opr, matches, reg_high))  cod.push_back(96 + 2*stoi(num_reg)+1);
                            else  cod.push_back(96 + 2*stoi(num_reg));
                            }
                            

                            }

                            smatch numbermatch;

                            if(regex_search(first_opr, matches, hexanum)){

                            cout << "HEX POM ";
                            string h = matches.str(1);

                            regex_search(h, matches, hexn);
                            string hh = matches.suffix().str();
                            regex_search(hh, matches, hexn);
                            string heksabroj = matches.str(1);

                            int x;   
                            std::stringstream ss;
                            ss << std::hex << heksabroj;
                            ss >> x;

                            cout << "INT - REG MEMORIJSKO - " << x <<"\n";

                            cod.push_back(x%256);
                            cod.push_back(x/256);
                            LC+=2;
                                

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            regex_search(line, matches, reg);
                            line = matches.suffix().str(); 
                            //cout << line << ;
                            regex_search(line, matches, reg);
                            line = matches.suffix().str(); 
                            regex_search(line, matches, reg);
                            line = matches.str(1); 
                            cout << line << "\n";
                            if(k==1) line = "";
                            continue;

                            } else

                            {
                            regex numbne("([0-9]+\\()");
                            regex numbn("([0-9]+)");
                            
                            if(regex_search(first_opr, numbermatch, numbne)){
                                
                                cout << matches.suffix().str();
                                first_opr = numbermatch.str(1);
                                regex_search(first_opr, numbermatch, numbn);
                                first_opr = numbermatch.str(1);
                                cout << first_opr << " ----------";
                                if(sec_name.compare("xchgb")==0 || sec_name.compare("movb")==0 || sec_name.compare("addb")==0 ||
                                sec_name.compare("subb")==0 || sec_name.compare("mulb")==0 || sec_name.compare("divb")==0 ||
                                sec_name.compare("cmpb")==0 || sec_name.compare("notb")==0 || sec_name.compare("andb")==0 ||
                                sec_name.compare("orb")==0 || sec_name.compare("testb")==0 || sec_name.compare("xorb")==0 ||
                                sec_name.compare("shlb")==0 || sec_name.compare("shrb")==0 )  
                                {
                                    cod.push_back(stoi(first_opr)%256);
                                    LC+=1;
                                }
                                else  {
                                    cod.push_back(stoi(first_opr)%256);
                                    cod.push_back(stoi(first_opr)/256);
                                    LC+=2;
                                    }

                                sec_code.remove(current_sec);
                                sec_code.push_back(current_sec);
                                ic = code.erase(ic);
                                code.push_back(cod);

                                regex_search(line, matches, reg);
                                line = matches.suffix().str(); 
                                regex_search(line, matches, reg);
                                line = matches.suffix().str(); 
                                regex_search(line, matches, reg);
                                line = matches.str(1); 
                                cout << "LINE" << line ;
                                if(k==1) line = "";
                                continue;


                            
                            }

                            }

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            regex_search(first_opr, numbermatch, symbol);

                            first_opr = numbermatch.str(1); 

                            cout << "POM KAD SAM ISEKAO " << first_opr;

                        
                        } else 
                    if(regex_search(first_opr, matches, reg_ind) or regex_search(first_opr, matches, reg_indsp)) {
                        cout << "OVO JE IND" << "\n";
                        //cout << matches.str(1) << "\n";
                        //========REGISTARSKO INDIREKTNO=================

                        if(regex_search(first_opr, matches, reg_ind)){
                        string num_reg = matches.str(1);
                        //line = matches.suffix().str();
                        //cout << num_reg << "\n";
                        regex_search(num_reg, matches, num);
                        num_reg = matches.str(1); 
                        
                        regex reg_high("(r[0-7]h)");

                        if(regex_search(first_opr, matches, reg_high))  cod.push_back(64 + 2*stoi(num_reg)+1);
                        else cod.push_back(64 + 2*stoi(num_reg));

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        regex_search(line, matches, reg);
                        line = matches.suffix().str(); 

                        LC+=1;
                        
                        continue;

                        } else {
                        cod.push_back(64 + 2*6);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        regex_search(line, matches, reg);
                        line = matches.suffix().str(); 

                        LC+=1;
                        
                        continue;
                        }
                        
                    } else
                    if(regex_search(first_opr, matches, reg_dir) or regex_search(first_opr, matches, reg_dirsp)) {
                        cout << "OVO JE REG" << "\n";
                        //cout << matches.str(1) << "\n";
                        //========REGISTARSKO DIREKTNO=================

                        if(regex_search(first_opr, matches, reg_dir)){
                        string num_reg = matches.str(1);
                        //cout << num_reg << "\n";
                        //line = matches.suffix().str();  
                        
                        regex_search(num_reg, matches, num);
                        num_reg = matches.str(1);  

                        regex reg_high("(r[0-7]h)");

                        if(regex_search(first_opr, matches, reg_high))  cod.push_back(32 + 2*stoi(num_reg)+1);
                        else cod.push_back(32 + 2*stoi(num_reg));

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        regex_search(line, matches, reg);
                        line = matches.suffix().str(); 

                        LC+=1;
            
                        continue;

                        } else {
                        cod.push_back(32 + 2*6);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        regex_search(line, matches, reg);
                        line = matches.suffix().str(); 

                        LC+=1;
            
                        continue;
                        }
                    } 
                    else if(regex_search(first_opr, matches,neposr)){
                    //==========================NEPOSREDNO=====================

                        /*cout << first_opr << " ";
                        cout << matches.suffix().str();*/
                        cout << "NEPO" <<"\n";
                        cod.push_back(0);

                        LC+=1;

                        regex numbne("(\\$[0-9]+)");
                        regex numbhexe("(\\$0x[0-9a-f]+)");
                        smatch numbermatch;
                        if(regex_search(first_opr, matches, numbhexe)){
                        string h = matches.str(1);

                        cout << "H" << h;

                        regex_search(h, matches, hexn);
                        string hh = matches.suffix().str();
                        regex_search(hh, matches, hexn);
                        string heksabroj = matches.str(1);

                        int x;   
                        std::stringstream ss;
                        ss << std::hex << heksabroj;
                        ss >> x;

                        cout << "INT - MEMORIJSKO - " << x <<"\n";

                        cod.push_back(x%256);
                        cod.push_back(x/256);
                        LC+=2;
                            

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        regex_search(line, matches, reg);
                        line = matches.suffix().str(); 
                        //cout << line << ;
                        regex_search(line, matches, reg);
                        line = matches.str(1); 
                        //cout << line << "\n";
                        if(k==1) line = "";
                        continue;

                        } else

                        if(regex_search(first_opr, numbermatch, numbne)){
                            regex broj("([0-9]+)");
                            //cout << first_opr << " ";
                            string x = numbermatch.str(1);
                            regex_search(x, numbermatch, broj);
                            x = numbermatch.str(1);
                            cout << x;
                            if(sec_name.compare("xchgb")==0 || sec_name.compare("movb")==0 || sec_name.compare("addb")==0 ||
                            sec_name.compare("subb")==0 || sec_name.compare("mulb")==0 || sec_name.compare("divb")==0 ||
                            sec_name.compare("cmpb")==0 || sec_name.compare("notb")==0 || sec_name.compare("andb")==0 ||
                            sec_name.compare("orb")==0 || sec_name.compare("testb")==0 || sec_name.compare("xorb")==0 ||
                            sec_name.compare("shlb")==0 || sec_name.compare("shrb")==0 )  
                            {
                                cod.push_back(stoi(x)%256);
                                LC+=1;
                            }
                            else  {
                                cod.push_back(stoi(x)%256);
                                cod.push_back(stoi(x)/256);
                                LC+=2;
                                }

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            regex_search(line, matches, reg);
                            line = matches.suffix().str(); 
                            regex_search(line, matches, reg);
                            line = matches.str(1); 
                            continue;
                        }

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        regex_search(first_opr, numbermatch, symbol);

                        first_opr = numbermatch.str(1); 

                        /*regex_search(line, matches, reg);
                        line = matches.suffix().str(); */

                    } else if(regex_search(first_opr, matches, star)){

                        cout << "MEM I REG POM" << "\n";

                        if(regex_search(first_opr, matches, sp)){
                            //================REG POM=====================
                            cod.push_back(96 + 2*6);

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            regex_search(line, matches, reg);
                            line = matches.suffix().str(); 
                            

                            
                        }
                        else 
                        if(regex_search(first_opr, matches, reg_star)){
                            //================REG POM=====================
                            if(regex_search(line, matches, num))
                            {
                            string num_reg = matches.str(1);
                            regex reg_high("(r[0-7]h)");

                            if(regex_search(first_opr, matches, reg_high))  cod.push_back(96 + 2*stoi(num_reg)+1);
                            else  cod.push_back(96 + 2*stoi(num_reg));

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            regex_search(line, matches, reg);
                            line = matches.suffix().str(); 
                            }

                            
                        } else {
                        //========================MEM=======================

                        cout << "MEM1 " << "\n";

                        cod.push_back(128);

                        LC+=1;

                        smatch numbermatch;

                        if(regex_search(first_opr, matches, hexanum)){
                        string h = matches.str(1);

                        regex_search(h, matches, hexn);
                        string hh = matches.suffix().str();
                        regex_search(hh, matches, hexn);
                        string heksabroj = matches.str(1);

                        int x;   
                        std::stringstream ss;
                        ss << std::hex << heksabroj;
                        ss >> x;

                        cout << "INT - MEMORIJSKO - " << x <<"\n";

                        cod.push_back(x%256);
                        cod.push_back(x/256);
                        LC+=2;
                            

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        regex_search(line, matches, reg);
                        line = matches.suffix().str(); 
                        //cout << line << ;
                        regex_search(line, matches, reg);
                        line = matches.str(1); 
                        //cout << line << "\n";
                        if(k==1) line = "";
                        continue;

                        } else

                        if(regex_search(first_opr, numbermatch, ss)){
                            
                            
                                cod.push_back(stoi(first_opr)%256);
                                cod.push_back(stoi(first_opr)/256);
                                LC+=2;
                             

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            regex_search(line, matches, reg);
                            line = matches.suffix().str(); 
                            if(k==1) line = "";
                            continue;


                        
                        }

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        regex_search(line, matches, reg);
                        line = matches.suffix().str(); 

                        LC += 1;
                        }

                    } else {
                        cout << "MEM " << "\n";

                        cod.push_back(128);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);
                        
                        LC += 1;
                        /*regex_search(line, matches, reg);
                        line = matches.suffix().str(); */
                    } 

                    //LC += 1;

                    ic = code.begin();
                    pom = 0;
                    ci = sec_code.begin();
                    for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                        if(*ci == current_sec) break;
                    }   
                    advance(ic, pom);
                    cod = *ic;

                    

                    regex_search(line, matches, reg);

                    string name_curr = first_opr;
                    line = matches.suffix().str();
                    if(k==1) line = "";
                    
                    first_opr = line;

                    if(k==0 && pc_jmp == "pc") {
                        regex_search(line, matches, reg);

                        line = matches.suffix().str();
                        regex_search(line, matches, reg);

                        line = matches.str(1);
                        first_opr = line;
                    }
                    cout << "---------- ";
                    cout << name_curr << " OVOO JE ULAZAK GDE NE tREBA\n";
                    cout << first_opr << " OVOO JE ULAZAK GDE NE tREBA\n";
                    
                    
                    //cout << name_curr;
                    //cout << pc_jmp << "OVO JE PC \n";
                    //cout << name_curr << "\n";

                    //RELOCATION ADD
                            
                    //search  
                    bool find_sym = false;
                    list <SymbolTable*> :: iterator it; 
                    for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                        SymbolTable* item = *it;
                        if((item->name.compare(name_curr)==0) or item->name == name_curr) {
                            
                            find_sym = true;
                            if(sec_name.compare("xchgb")==0 || sec_name.compare("movb")==0 || sec_name.compare("addb")==0 ||
                            sec_name.compare("subb")==0 || sec_name.compare("mulb")==0 || sec_name.compare("divb")==0 ||
                            sec_name.compare("cmpb")==0 || sec_name.compare("notb")==0 || sec_name.compare("andb")==0 ||
                            sec_name.compare("orb")==0 || sec_name.compare("testb")==0 || sec_name.compare("xorb")==0 ||
                            sec_name.compare("shlb")==0 || sec_name.compare("shrb")==0 ) LC += 1;
                            else LC += 2;

                            if(item->value == 0 && item->num_sec < 0) {
                            ForwardTable* curr_forward = new ForwardTable(name_curr, LC, current_sec);
                            item->ft.push_back(curr_forward);
                            forwardTable.push_back(curr_forward);
                            }

                            //RELOCATION ADD
                            list<RelocationTable*> ite;
                            list <list<RelocationTable*>> :: iterator rt; 
                            for(rt = relocationTable.begin(); rt != relocationTable.end(); ++rt) {
                                ite = *rt;
                                list <RelocationTable*> :: iterator srt; 
                                //cout << relocationTable.size() << "\n";
                                srt = ite.begin();  
                                RelocationTable* rtt = *srt;
                                /*cout << sec_name << "=============" << "\n";
                                cout << rtt->num_sym << " " << name_curr << "\n"; 
                                cout << current_sec << "\n";*/
                                if(rtt->num_sym == current_sec) {
                                    cout << "PC JMP -> " << pc_jmp;
                                    RelocationTable* currel;
                                    if(pc_jmp.compare("pc")==0) {
                                        cout << "USAO U PC I NASAO SIMBOL TREBA DA REALOCIRAM ILI NE";
                                        if(item->num_sec == current_sec){
                                            //ista sekcija pc
                                            cout << "negativan pomeraj jer su u istoj sekciji 222222";
                                            int p = ((item->value-LC + 2)%256)  ;
                                            cout << (p&(255));
                                           
                                            cod.push_back((((item->value-LC + 2)%256))&255);
                                            cod.push_back(((item->value-LC + 2)/256));
                                        } else
                                            if(item->flag == 'g') 
                                        {
                                        currel = new RelocationTable(name_curr, LC-1, "R_386_PC32", item->num);
                                        /*if(sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                                        sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                                        sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 || 
                                        
                                        sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                                        sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                                        sec_name.compare("pop")==0 || sec_name.compare("push")==0    ) */
                                        if(k==1){
                                            cod.push_back(-2);
                                            cod.push_back(-1);
                                        }
                                        else{
                                            //TREBA UPISATI RASTOJANJE DO SLEDECE INSTRUKCIJE
                                            cod.push_back(-2 -pompc1);
                                            cod.push_back(0);     
                                        }
                                        ite.push_back(currel);
                                    
                                        //cout << ite.size() << "\n";
                                        rt = relocationTable.erase(rt);
                                        relocationTable.push_back(ite);
                                        }
                                        else 
                                        {
                                        currel = new RelocationTable(name_curr, LC-1, "R_386_PC32", 4321);
                                        /*if(sec_name.compare("intw")==0 || sec_name.compare("callw")==0 || sec_name.compare("jmpw")==0 ||
                                        sec_name.compare("jeqw")==0 || sec_name.compare("jnew")==0 || sec_name.compare("jgtw")==0 ||
                                        sec_name.compare("popw")==0 || sec_name.compare("pushw")==0 || 
                                        
                                        sec_name.compare("int")==0 || sec_name.compare("call")==0 || sec_name.compare("jmp")==0 ||
                                        sec_name.compare("jeq")==0 || sec_name.compare("jne")==0 || sec_name.compare("jgt")==0 ||
                                        sec_name.compare("pop")==0 || sec_name.compare("push")==0   )  */
                                        
                                        if(k==1){
                                            cod.push_back(item->value-2);
                                            cod.push_back(0);
                                        }
                                        else{
                                            //TREBA UPISATI RASTOJANJE DO SLEDECE INSTRUKCIJE
                                            cod.push_back(item->value-2-pompc1);
                                            cod.push_back(0);     
                                        }
                                        ite.push_back(currel);
                                    
                                        //cout << ite.size() << "\n";
                                        rt = relocationTable.erase(rt);
                                        relocationTable.push_back(ite); 
                                        }

                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);
                                    }
                                    else { 
                                        if(item->flag == 'g') 
                                       {
                                        currel = new RelocationTable(name_curr, LC, "R_386_32", item->num);
                                        if(sec_name.compare("xchgw")==0 || sec_name.compare("movw")==0 || sec_name.compare("addw")==0 ||
                                        sec_name.compare("subw")==0 || sec_name.compare("mulw")==0 || sec_name.compare("divw")==0 ||
                                        sec_name.compare("cmpw")==0 || sec_name.compare("notw")==0 || sec_name.compare("andw")==0 ||
                                        sec_name.compare("orw")==0 || sec_name.compare("testw")==0 || sec_name.compare("xorw")==0 ||
                                        sec_name.compare("shlw")==0 || sec_name.compare("shrw")==0 ||
                                        
                                        sec_name.compare("xchg")==0 || sec_name.compare("mov")==0 || sec_name.compare("add")==0 ||
                                        sec_name.compare("sub")==0 || sec_name.compare("mul")==0 || sec_name.compare("div")==0 ||
                                        sec_name.compare("cmp")==0 || sec_name.compare("not")==0 || sec_name.compare("and")==0 ||
                                        sec_name.compare("or")==0 || sec_name.compare("test")==0 || sec_name.compare("xor")==0 ||
                                        sec_name.compare("shl")==0 || sec_name.compare("shr")==0 ) cod.push_back(0);
                                        cod.push_back(0);
                                        }
                                        else
                                        { 
                                        currel = new RelocationTable(name_curr, LC, "R_386_32", 4321);
                                        cod.push_back(item->value);
                                        if(sec_name.compare("xchgw")==0 || sec_name.compare("movw")==0 || sec_name.compare("addw")==0 ||
                                        sec_name.compare("subw")==0 || sec_name.compare("mulw")==0 || sec_name.compare("divw")==0 ||
                                        sec_name.compare("cmpw")==0 || sec_name.compare("notw")==0 || sec_name.compare("andw")==0 ||
                                        sec_name.compare("orw")==0 || sec_name.compare("testw")==0 || sec_name.compare("xorw")==0 ||
                                        sec_name.compare("shlw")==0 || sec_name.compare("shrw")==0 ||
                                        
                                        sec_name.compare("xchg")==0 || sec_name.compare("mov")==0 || sec_name.compare("add")==0 ||
                                        sec_name.compare("sub")==0 || sec_name.compare("mul")==0 || sec_name.compare("div")==0 ||
                                        sec_name.compare("cmp")==0 || sec_name.compare("not")==0 || sec_name.compare("and")==0 ||
                                        sec_name.compare("or")==0 || sec_name.compare("test")==0 || sec_name.compare("xor")==0 ||
                                        sec_name.compare("shl")==0 || sec_name.compare("shr")==0 )  cod.push_back(0);
                                        
                                        }

                                        sec_code.remove(current_sec);
                                        sec_code.push_back(current_sec);
                                        ic = code.erase(ic);
                                        code.push_back(cod);
                                        
                                    }
                                    if(pc_jmp.compare("pc")!=0) {ite.push_back(currel);
                                    
                                    //cout << ite.size() << "\n";
                                    rt = relocationTable.erase(rt);
                                    relocationTable.push_back(ite); 
                                    }
                                    break;
                                
                            }
                            
                           
                        
                    }
                         break;
                    }

                    }
                    
                    if(find_sym == false && name_curr.compare("pc")!=0 && name_curr.compare("r0")!=0 && name_curr.compare("r1")!=0
                        && name_curr.compare("r2")!=0 && name_curr.compare("r3")!=0 && name_curr.compare("r4")!=0
                        && name_curr.compare("r5")!=0 && name_curr.compare("r6")!=0 && name_curr.compare("r7")!=0
                        && name_curr.compare("r0h")!=0 && name_curr.compare("r1h")!=0
                        && name_curr.compare("r2h")!=0 && name_curr.compare("r3h")!=0 && name_curr.compare("r4h")!=0
                        && name_curr.compare("r5h")!=0 && name_curr.compare("r6h")!=0 && name_curr.compare("r7h")!=0
                        && name_curr.compare("r0l")!=0 && name_curr.compare("r1l")!=0
                        && name_curr.compare("r2l")!=0 && name_curr.compare("r3l")!=0 && name_curr.compare("r4l")!=0
                        && name_curr.compare("r5l")!=0 && name_curr.compare("r6l")!=0 && name_curr.compare("r7l")!=0) {

                            cod.push_back(-2);
                            if(sec_name.compare("xchgw")==0 || sec_name.compare("movw")==0 || sec_name.compare("addw")==0 ||
                            sec_name.compare("subw")==0 || sec_name.compare("mulw")==0 || sec_name.compare("divw")==0 ||
                            sec_name.compare("cmpw")==0 || sec_name.compare("notw")==0 || sec_name.compare("andw")==0 ||
                            sec_name.compare("orw")==0 || sec_name.compare("testw")==0 || sec_name.compare("xorw")==0 ||
                            sec_name.compare("shlw")==0 || sec_name.compare("shrw")==0 ||
                            
                            sec_name.compare("xchg")==0 || sec_name.compare("mov")==0 || sec_name.compare("add")==0 ||
                            sec_name.compare("sub")==0 || sec_name.compare("mul")==0 || sec_name.compare("div")==0 ||
                            sec_name.compare("cmp")==0 || sec_name.compare("not")==0 || sec_name.compare("and")==0 ||
                            sec_name.compare("or")==0 || sec_name.compare("test")==0 || sec_name.compare("xor")==0 ||
                            sec_name.compare("shl")==0 || sec_name.compare("shr")==0  )  {cod.push_back(0); LC+=1;}
                            
    
                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);

                            LC+=1;

                            cout << name_curr << " DODAJE ";

                            SymbolTable* curr = new SymbolTable('m', lastID++, name_curr , -2, 0, 'l');
                            ForwardTable* curr_forward = new ForwardTable(name_curr, LC, current_sec);
 
                            curr->ft.push_back(curr_forward);
                            symbolTable.push_back(curr); 
                            forwardTable.push_back(curr_forward);

                            //RELOCATION ADD
                            list<RelocationTable*> ite;
                            list <list<RelocationTable*>> :: iterator rt; 
                            for(rt = relocationTable.begin(); rt != relocationTable.end(); ++rt) {
                                ite = *rt;
                                list <RelocationTable*> :: iterator srt; 
                                srt = ite.begin();  
                                RelocationTable* rtt = *srt;
                                //cout << rtt->num_sym << "\n";
                                if(rtt->num_sym == current_sec) {
                                    /*cout << sec_name << "=============" << "\n";*/
                                    RelocationTable* currel;
                                    //cout << ite.size() << "\n";
                                    if(pc_jmp.compare("pc")==0) currel = new RelocationTable(name_curr, LC, "R_386_PC32", 4321);
                                    else currel = new RelocationTable(name_curr, LC, "R_386_32", 4321);
                                    ite.push_back(currel);
                                    //LC += 1;
                                    //cout << ite.size() << "\n";
                                    rt = relocationTable.erase(rt);
                                    relocationTable.push_back(ite);
                                    break;
                                }
                            }

                    }
                    //}//

            } 
            }  else  if(sec_name.compare("byte")==0){
                    if(regex_search(line, matches, hexanum)){
                        /*cout << "\n" << "HEKSA" << "\n";
                        cout << matches.str(1);*/

                        string h = matches.str(1);

                        regex_search(h, matches, hexn);
                        //cout << "\n" << matches.suffix().str() <<"\n";

                        string hh = matches.suffix().str();

                        regex_search(hh, matches, hexn);
                        //cout << "\n" << matches.str(1) <<"\n";

                        string heksabroj = matches.str(1);

                        int x;   
                        std::stringstream ss;
                        ss << std::hex << heksabroj;
                        ss >> x;

                        cout << "INT - " << x <<"\n";

                        list <int> cod;
                        list <list<int>> :: iterator ic = code.begin();
                        int pom = 0;
                        list <int> :: iterator ci = sec_code.begin();
                        for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                            if(*ci == current_sec) break;
                        }
                        advance(ic, pom);
                        cod = *ic;

                        cod.push_back(x%256);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        LC+=1;

                        line = "";

                    } else
                    while (regex_search(line, matches, reg)){
                        string name_curr = matches.str(1);
                        line = matches.suffix().str();
                        LC += 1;

                        list <int> cod;
                        list <list<int>> :: iterator ic = code.begin();
                        int pom = 0;
                        list <int> :: iterator ci = sec_code.begin();
                        for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                            if(*ci == current_sec) break;
                        }
                        advance(ic, pom);
                        cod = *ic;
                        //LC += 1;
                        cod.push_back(stoi(name_curr)%256);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);
                        //cout << line << "\n";
                    }  
            } else  if(sec_name.compare("word")==0){
                    if(regex_search(line, matches, hexanum)){
                        /*cout << "\n" << "HEKSA" << "\n";
                        cout << matches.str(1);*/

                        string h = matches.str(1);

                        regex_search(h, matches, hexn);
                        //cout << "\n" << matches.suffix().str() <<"\n";

                        string hh = matches.suffix().str();

                        regex_search(hh, matches, hexn);
                        //cout << "\n" << matches.str(1) <<"\n";

                        string heksabroj = matches.str(1);

                        int x;   
                        std::stringstream ss;
                        ss << std::hex << heksabroj;
                        ss >> x;

                        cout << "INT - " << x <<"\n";

                        list <int> cod;
                        list <list<int>> :: iterator ic = code.begin();
                        int pom = 0;
                        list <int> :: iterator ci = sec_code.begin();
                        for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                            if(*ci == current_sec) break;
                        }
                        advance(ic, pom);
                        cod = *ic;

                        cod.push_back(x%256);
                        cod.push_back(x/256);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        LC+=2;

                        line = "";

                    } else
                    while (regex_search(line, matches, reg)){
                        string name_curr = matches.str(1);
                        line = matches.suffix().str();
                        LC += 2;

                        //search
                        bool find_sym = false;
                        list <SymbolTable*> :: iterator it; 
                        for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                            SymbolTable* item = *it;
                            cout << sec_name << "OVO JE sec_name";
                            if(item->name.compare(name_curr)==0) {
                                list <int> cod;
                                list <list<int>> :: iterator ic = code.begin();
                                int pom = 0;
                                list <int> :: iterator ci = sec_code.begin();
                                for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                                    if(*ci == current_sec) break;
                                }
                                advance(ic, pom);
                                cod = *ic;
                                //LC += 1;
                                if(item->flag == 'g'){
                                    cod.push_back(0);
                                    cod.push_back(0);
                                } else {
                                    cod.push_back(item->value%256);
                                    cod.push_back(item->value/256);
                                }
                                sec_code.remove(current_sec);
                                sec_code.push_back(current_sec);
                                ic = code.erase(ic);
                                code.push_back(cod);
                                break;
                            }
                        }

                        if(find_sym == false) {
                        regex word("([a-zA-Z])");
                        list <int> cod;
                        list <list<int>> :: iterator ic = code.begin();
                        int pom = 0;
                        list <int> :: iterator ci = sec_code.begin();
                        for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                            if(*ci == current_sec) break;
                        }
                        advance(ic, pom);
                        cod = *ic;
                        if(regex_search(name_curr, matches, word)){
                            SymbolTable* curr = new SymbolTable('m', lastID++, name_curr , -2, 0, 'l');
                            ForwardTable* curr_forward = new ForwardTable(name_curr, LC, current_sec);
 
                            curr->ft.push_back(curr_forward);
                            symbolTable.push_back(curr); 
                            forwardTable.push_back(curr_forward);

                            //RELOCATION ADD
                            list<RelocationTable*> ite;
                            list <list<RelocationTable*>> :: iterator rt; 
                            for(rt = relocationTable.begin(); rt != relocationTable.end(); ++rt) {
                                ite = *rt;
                                list <RelocationTable*> :: iterator srt; 
                                srt = ite.begin();  
                                RelocationTable* rtt = *srt;
                                //cout << rtt->num_sym << "\n";
                                if(rtt->num_sym == current_sec) {
                                    /*cout << sec_name << "=============" << "\n";*/
                                    RelocationTable* currel;
                                    //cout << ite.size() << "\n";
                                    currel = new RelocationTable(name_curr, LC, "R_386_32", 4321);
                                    ite.push_back(currel);
                                    //LC += 1;
                                    //cout << ite.size() << "\n";
                                    rt = relocationTable.erase(rt);
                                    relocationTable.push_back(ite);
                                    break;
                                }
                            }


                        } else {
                            
                            //LC += 1;
                            cod.push_back(stoi(name_curr)%256);
                            cod.push_back(stoi(name_curr)/256);

                            sec_code.remove(current_sec);
                            sec_code.push_back(current_sec);
                            ic = code.erase(ic);
                            code.push_back(cod);
                            //cout << line << "\n";
                        }
                        }
                        
                    }  
            } else  if(sec_name.compare("skip")==0){
                    //cout << "EXTERN";
                    if(regex_search(line, matches, hexanum)){
                        /*cout << "\n" << "HEKSA" << "\n";
                        cout << matches.str(1);*/

                        string h = matches.str(1);

                        regex_search(h, matches, hexn);
                        //cout << "\n" << matches.suffix().str() <<"\n";

                        string hh = matches.suffix().str();

                        regex_search(hh, matches, hexn);
                        //cout << "\n" << matches.str(1) <<"\n";

                        string heksabroj = matches.str(1);

                        int x;   
                        std::stringstream ss;
                        ss << std::hex << heksabroj;
                        ss >> x;

                        //cout << "INT - " << x <<"\n";

                        LC += x;

                        list <int> cod;
                        list <list<int>> :: iterator ic = code.begin();
                        int pom = 0;
                        list <int> :: iterator ci = sec_code.begin();
                        for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                            if(*ci == current_sec) break;
                        }
                        advance(ic, pom);
                        cod = *ic;
                        //LC += 1;
                        for(int i=0; i < x; i++) cod.push_back(0);

                        sec_code.remove(current_sec);
                        sec_code.push_back(current_sec);
                        ic = code.erase(ic);
                        code.push_back(cod);

                        line = "";

                    } else
                    while (regex_search(line, matches, reg)){
                        string name_curr = matches.str(1);
                        line = matches.suffix().str();
                        LC += stoi(name_curr);

                    list <int> cod;
                    list <list<int>> :: iterator ic = code.begin();
                    int pom = 0;
                    list <int> :: iterator ci = sec_code.begin();
                    for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                        if(*ci == current_sec) break;
                    }
                    advance(ic, pom);
                    cod = *ic;
                    //LC += 1;
                    for(int i=0; i < stoi(name_curr); i++) cod.push_back(0);

                    sec_code.remove(current_sec);
                    sec_code.push_back(current_sec);
                    ic = code.erase(ic);
                    code.push_back(cod);

                        //cout << line << "\n";
                    }  
            } 
            else  if(sec_name.compare("equ")==0){
                    //cout << "EXTERN";
                        regex_search(line, matches, reg);
                        string name_curr = matches.str(1);
                        //cout << name_curr << "OVO JE PRVI OPERAND UZ EQU" << "\n";
                        line = matches.suffix().str();
                        bool find_sym = false;
                        list <SymbolTable*> :: iterator it; 
                        for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                        SymbolTable* item = *it;
                        if(item->name.compare(name_curr)==0) {
                            find_sym = true;
                            break;
                            }
                        }
                        if(find_sym == false) {
                        SymbolTable* curr = new SymbolTable('m', lastID++, sec_name , -1 , 0, 'l');
                        symbolTable.push_back(curr); 
                        //cout << line << "\n";
                        }  
                        
                        string value;
                        while (regex_search(line, matches, reg)){
                            value = matches.str(1);
                            line = matches.suffix().str();
                            //LC += 2;
                            cout << value << "\n";
                        }  

                            
                            for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                            SymbolTable* item = *it;
                            if(item->name.compare(name_curr)==0) {
                                find_sym = true;
                                item->value = stoi(value);
                                item->num_sec = -2;
                                break;
                                }
                     
                            }
                 
            } else {
                    //search
                    bool find_sym = false;
                    list <SymbolTable*> :: iterator it; 
                    for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                        SymbolTable* item = *it;
                        cout << sec_name << "OVO JE sec_name";
                        if(item->name.compare(sec_name)==0) {
                            find_sym = true;
                            item->num_sec = current_sec;
                            item->value = LC;
                            list <ForwardTable*> :: iterator ft; 
                            for(ft = forwardTable.begin(); ft != forwardTable.end(); ) {
                                ForwardTable* rem = *ft;
                                if(rem->name.compare(sec_name)==0){
                                    //CODE UPDATE
                                    list <int> cod;
                                    list <list<int>> :: iterator ic = code.begin();
                                    int pom = 0;
                                    list <int> :: iterator ci = sec_code.begin();
                                    for(ci = sec_code.begin(); ci != sec_code.end(); pom++, ci++){
                                        //cout << *ci << " " << "PA " << rem->section << "\n";
                                        if(*ci == rem->section) break;
                                    }
                                    if(pom < sec_code.size()){
                                        advance(ic, pom);
                                        cod = *ic;

                                        list<int>::iterator it1;
                                        
                                        it1 = cod.begin();
                                        //cout << "REM OFSET" << rem->offset;
                                        advance (it1, rem->offset-2);            

                                        //cout << hex << *it1 << " ";
                                        
                                            if(item->flag == 'l' && (rem->section != item->num_sec)){
                                                cout << "D:" ;
                                               it1 = cod.erase (it1);
                                               cod.insert (it1, item->value-2);             
                                            } else if (item->flag == 'g' && (rem->section != item->num_sec)){
                                            it1 = cod.erase (it1);
                                            cod.insert (it1, -2);
                                            } else {
                                            it1 = cod.erase (it1);
                                            cod.insert (it1, LC - rem->offset);
                                            }
                                        

                                        sec_code.remove(rem->section);
                                        sec_code.push_back(rem->section);
                                        ic = code.erase(ic);
                                        code.push_back(cod);
                                    }
                                    ft = forwardTable.erase(ft);
                                } else ++ft;
                            }
                            //Relocation_table update symbol number
                            list<RelocationTable*> ite;
                            list <list<RelocationTable*>> :: iterator rt; 
                            for(rt = relocationTable.begin(); rt != relocationTable.end(); ++rt) {
                                ite = *rt;
                                list <RelocationTable*> :: iterator srt; 
                                for(srt = ite.begin(); srt != ite.end(); srt++) {
                                    RelocationTable* rtt = *srt;
                                    int secod = 0;

                                    if((rtt->num_sym == item->num_sec) && (rtt->tip == "R_386_PC32")) {
                                        cout << "BRISANJE REALOKACIJE";
                                       /* list <RelocationTable*> del;
                                        del = *rt*/
                                        srt = ite.erase(srt);
                                        rt = relocationTable.erase(rt);
                                        relocationTable.push_back(ite); 
                                    } else {
                                    if(rtt->name_section.compare("section")) secod = rtt->num_sym;
                                    if(rtt->name_section.compare(sec_name)==0){
                                        if(item->flag == 'l') {
                                            rtt->num_sym = item->num_sec;
                                            
                                        }
                                        
                                        else rtt->num_sym = item->num;

                                        
                                    } 

                                    }
                                }
                                
                            }
                            break;
                        }
                     
                    }
                    if(find_sym == false) {
                    SymbolTable* curr = new SymbolTable('m', lastID++, sec_name , current_sec, LC, 'l');
                    symbolTable.push_back(curr); 
                    //cout << line << "\n";

                    }
                    
            }

            }
            
        }
            
            

    
    }
            myfile.close();
            //cout << symbolTable.size() << "\n";
            cout << "===TABELA SIMBOLA===" << '\n';
            list <SymbolTable*> :: iterator it; 
            for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                    SymbolTable* item = *it;
                    cout << item->name << " ";
                    cout << item->num << " ";
                    cout << item->flag << " ";
                    cout << item->num_sec << " ";
                    cout << item->sec << " ";
                    cout << item->value << "\n";
                     
            }
    
            cout << '\n' << "===TABELA UNAPRED ODREDJENIH===" << '\n';

            list <ForwardTable*> :: iterator ft; 
            for(ft = forwardTable.begin(); ft != forwardTable.end(); ++ft) {
                    ForwardTable* item = *ft;
                    cout << item->name << " ";
                    cout << item->section << " ";
            }
            
            cout << "\n" << "\n";
            cout << "===TABELA RELOKACIJE===" << '\n';

            list <list<RelocationTable*>> :: iterator rt; 
            for(rt = relocationTable.begin(); rt != relocationTable.end(); ++rt) {
                    list<RelocationTable*> item = *rt;
                    list <RelocationTable*> :: iterator srt; 
                    //cout << relocationTable.size() << "\n";
                    //cout << item.size() << "\n";
                    for(srt = item.begin(); srt != item.end(); ++srt) {
                        RelocationTable* rtt = *srt;
                        cout << rtt->name_section << " == ";
                        cout << rtt->offset << " == ";
                        cout << rtt->num_sym << " == ";
                        cout << rtt->tip << "\n";
                    }
            }

            cout << "\n" ;
            cout << "===KOD===" << '\n';

            list <int> :: iterator code_num_sec = sec_code.begin();
            list <list<int>> :: iterator k; 
            for(k= code.begin(); k != code.end(); ++k) {
                    list <int> ki = *k;
                    list <int> :: iterator kli; 
                    //cout << relocationTable.size() << "\n";
                    //cout << item.size() << "\n";
                    cout << "section: " << *code_num_sec << "\n";
                    for(kli = ki.begin(); kli != ki.end(); ++kli) {
                        cout << hex << ((*kli)&255) << " ";
                    }
                    cout << "\n";
                    code_num_sec ++;
            }


            
            ofstream myfileout ("izlaz.txt");
            if (myfileout.is_open())
            {

                myfileout << "===TABELA SIMBOLA===" << '\n';
                myfileout << "IME " << " ";
                myfileout << "REDNI BROJ " << " ";
                myfileout << "LOKAL/GLOBAL " << " ";
                //myfileout << "SEKCIJA       " << " ";
                myfileout << "BROJ SECKIJE " << " ";
                myfileout << "VREDNOST  " << "\n";
                list <SymbolTable*> :: iterator it; 
                for(it = symbolTable.begin(); it != symbolTable.end(); ++it) {
                    SymbolTable* item = *it;
                    myfileout << item->name << " ";
                    myfileout << item->num << " ";
                    myfileout << item->flag << " ";
                    if(item->num_sec == -2) myfileout << "Aps" << " ";
                    else
                    if(item->num_sec == -1) myfileout << "UND" << "  ";
                    else myfileout << item->num_sec << " ";
                    //myfileout << item->sec << " ";
                    myfileout << item->value << "\n";
                     
            }
    
            cout << '\n' << "===TABELA UNAPRED ODREDJENIH===" << '\n';

            list <ForwardTable*> :: iterator ft; 
            for(ft = forwardTable.begin(); ft != forwardTable.end(); ++ft) {
                    ForwardTable* item = *ft;
                    cout << item->name << " ";
                    cout << item->section << " ";
            }
            
            cout << "\n" << "\n";
            myfileout << "===TABELA RELOKACIJE===" << '\n';

            list <list<RelocationTable*>> :: iterator rt; 
            for(rt = relocationTable.begin(); rt != relocationTable.end(); ++rt) {
                    list<RelocationTable*> item = *rt;
                    list <RelocationTable*> :: iterator srt; 
                    //cout << relocationTable.size() << "\n";
                    //cout << item.size() << "\n";
                    for(srt = item.begin(); srt != item.end(); ++srt) {
                        RelocationTable* rtt = *srt;
                        myfileout << rtt->name_section << " == ";
                        myfileout << rtt->offset << " == ";
                        myfileout << rtt->num_sym << " == ";
                        myfileout << rtt->tip << "\n";
                    }
            }

            cout << "\n" ;
            myfileout << "===KOD===" << '\n';

            list <int> :: iterator code_num_sec = sec_code.begin();
            list <list<int>> :: iterator k; 
            for(k= code.begin(); k != code.end(); ++k) {
                    list <int> ki = *k;
                    list <int> :: iterator kli; 
                    //cout << relocationTable.size() << "\n";
                    //cout << item.size() << "\n";
                    myfileout << "section: " << *code_num_sec << "\n";
                    for(kli = ki.begin(); kli != ki.end(); ++kli) {
                        myfileout << hex << ((*kli)&255) << " ";
                    }
                    myfileout << "\n";
                    code_num_sec ++;
            }

                myfileout.close();
            }

   

    return;
}


