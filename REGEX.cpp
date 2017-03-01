
/**
take a regex as input with functions as |(or) *(klene clousre) and ab(as a.b  concatenation)
*/

#include<iostream>
#include<vector>
#include<cstdlib>
#include<string>
#include<map>
#include <stack>
#include<set>

using namespace std;

int count =0;


// state for NFA having its id and a map which stores the vector of states for each symbol
class state{

	public:
		
		//id of the state
		int id;
		bool st;
		bool en;
		// a map which store the next state pointers for each symbol
		map<char,vector<state*> > mymap;
		
		state(){
			st = false;
			en = false;
		}
		
		state(int st){
			id = st;
			st = false;
			en = false;
		}
};

//NFA that has a single start and final state 
class NFA{

public:
	//no of states
	int n; 
	state* start;
	state* Final;
	set<state*>allstate;

	//create an empty NFA with a id
	NFA* createEmpty();
	
	//create a NFA with a single symbol
	NFA* createSymbol(char sym);
	
	
	//for each symbol read the given functions are called recursively to build the 
	//NFA for given regex
	
	
	//concate two diff NFAs
	NFA* concate(NFA* next);
	
	//the union of two NFAs
	NFA* Or(NFA* next);

	//klene closure for the NFA
	NFA* star();
	
	//plus operation for given NFA
	NFA* plus(); 
	
	
};


NFA* Ncopy(NFA* N){
	
	NFA* copy = new NFA;
	
	map<state*,state*>oldnew;
	set<state*>::iterator p;
	p= N->allstate.begin();
	while(p != N->allstate.end())
	{
		state* t = new state(count++);
		oldnew.insert(pair<state*,state*>((*p),t));
		p++;
	}
	
	///////////////////////
	set<state*>::iterator itr;
	itr = N->allstate.begin();
	
	while(itr!= N->allstate.end()){
		
		state* q1 = (*itr);
		map<char,vector<state*> >::iterator k;
		k = q1->mymap.begin();
		
		while(k!=q1->mymap.end()){
		
			vector<state*>vec;
			map<state*,state*>::iterator x;
			x = oldnew.find(q1); //*x->second is *itr map
		
			for(int j=0;j<k->second.size();j++){
				
				map<state*,state*>::iterator lk;
				lk = oldnew.find(k->second[j]);
				
				vec.push_back(lk->second);
			}
			
			(x->second)->mymap.insert(pair<char,vector<state*> >(k->first,vec));
			
			k++;		
		}
		
		
		itr++;
		
	}
	
	map<state*,state*>::iterator x;
	x = oldnew.find(N->start);
	copy->start = x->second;
	
	map<state*,state*>::iterator y;
	y = oldnew.find(N->Final);
	copy->Final = y->second;	
	
	
	return copy;
	
}

class DS{
	public:
		string id;
		set<state*>Dstate;
		map<char,DS* >T;
		bool start;
		bool end;
		bool mark;
		
		DS(){
			mark = false;
			start = false;
			end = false;
		}
	
		void makeid(){
			if(this->Dstate.empty()==false){
				set<state*>::iterator it;
				for(it=this->Dstate.begin();it!=this->Dstate.end();it++){
					state* temp = (*it);
					char ch = temp->id+'0';
					this->id +=ch;
				}
			}
		}
	
};

class DFA{
	public:
		DS* start;
		DS* end;
		vector<char>alphabet;
	
};

//make a DFA node of all state reachable by e from given set
DS* Epsilon(set<state*> in)
{
	//if empty return empty set object
	if(in.empty()==true)
	{
		DS* ob = new DS;
		set<state*> p;
		ob->Dstate = p;
		return ob;
	}
	else{
	
	DS* ds = new DS;
	stack<state*>st;
	set<state*>::iterator p;
	
	for(p=in.begin();p!=in.end();p++){
		st.push((*p));
	}
	
	set<state*>result;
	result = in;
	
	while(st.empty()==false)
	{
		state* temp = st.top();
		st.pop();
		map<char,vector<state*> >::iterator itr;
		itr = temp->mymap.find('e');
		
		if(itr!= temp->mymap.end()){

			for(int i=0;i<itr->second.size();i++)
			{
				set<state*>::iterator s;
				s = result.find(itr->second[i]);
				if(s==result.end()){
					result.insert(itr->second[i]);
					st.push(itr->second[i]);
				}
			}
		}
	}
	
	set<state*>::iterator k;
	k= result.begin();
	while(k!=result.end()){
		
		if( (*k)->en == true ){
			ds->end = true;
		}
		k++;
	}
	
	ds->Dstate = result;
	return ds;
	
	}
	//end else
}

//return empty state if no trans
set<state*> Move(set<state*>in,char sym)
{
	set<state*>result;
	set<state*>::iterator itr;
	
	itr= in.begin();
	while(itr!=in.end()){
		state* temp;
		temp = (*itr);
		
		map<char,vector<state*> >::iterator p;
		p = temp->mymap.find(sym);
		if(p != temp->mymap.end() ){
			
			vector<state*>::iterator v;
			for(v= p->second.begin();v!= p->second.end();v++){
				result.insert((*v));
			}
			
		}
		
		itr++;
	}
	
	return result;
	
}


vector<char> Sigma (string str);
string postfix(string in);
int pref(char o1);
bool num(char ch);
bool op(char ch);


//create a NFA for single symbol with a final state
// delta(q0,sym)=qf
NFA* NFA :: createSymbol(char sym)
{
	NFA* n = new NFA;
	n->start = new state(count++);
	n->Final = new state(count++);
	
	n->allstate.insert(n->start);
	n->allstate.insert(n->Final);
	
	n->start->mymap[sym].push_back(n->Final);
	return n;
}


//create a NFA with e transition 
// delta(q0,e) = qf
NFA* NFA:: createEmpty()
{
	NFA* n = new NFA;
	n->start = new state(count++);
	n->Final = new state(count++);
	
	n->allstate.insert(n->start);
	n->allstate.insert(n->Final);
	
	n->start->mymap['e'].push_back(n->Final);
	return n;
	
}

//concate the NFA with given NFA
NFA* NFA::concate(NFA* X)
{
	NFA* n = new NFA;
	n->start = new state;
	n->Final = new state;
	
	n->allstate = this->allstate;
	n->allstate.insert(X->allstate.begin(),X->allstate.end() );
	
	this->Final->mymap['e'].push_back(X->start);
	n->start = this->start;
	n->Final = X->Final;
	return n;
}


NFA* NFA::Or(NFA* Y)
{
	NFA* n = new NFA;
	n->start = new state(count++);
	n->Final = new state(count++);
	
	n->allstate = this->allstate;
	n->allstate.insert(Y->allstate.begin(),Y->allstate.end());
	n->allstate.insert(n->start);
	n->allstate.insert(n->Final);
	
	n->start->mymap['e'].push_back(Y->start);
	map<char,vector<state*> >::iterator p;
	p = n->start->mymap.find('e');
	if(p != n->start->mymap.end() ){
		p->second.push_back(this->start);
	}
		
	this->Final->mymap['e'].push_back(n->Final);
	Y->Final->mymap['e'].push_back(n->Final);
	
	return n;
	
}


NFA* NFA::star()
{
	NFA* n = new NFA;
	n->start = new state(count++);
	n->Final = new state(count++);
	
		
	n->allstate = this->allstate;
	n->allstate.insert(n->start);
	n->allstate.insert(n->Final);
	
	this->Final->mymap['e'].push_back(this->start);
	
	n->start->mymap['e'].push_back(this->start);

	map<char,vector<state*> >::iterator p;
	p = n->start->mymap.find('e');
	if(p != n->start->mymap.end() ){
		p->second.push_back(n->Final);
	}
	
	map<char,vector<state*> >::iterator t;
	t = this->Final->mymap.find('e');
	if(t != this->Final->mymap.end() ){
		t->second.push_back(n->Final);
	}
	
	return n;
}


NFA* NFA::plus()
{
	NFA* n = new NFA;
	
	NFA* th = this;
	NFA* f1 = Ncopy(th);
	NFA* f2 = Ncopy(th);
	NFA* f3 = Ncopy(th);
	n->Final = new state(count++);
	n->start = new state;
	
	//th->f1
	map<char,vector<state*> >::iterator itr0;
	itr0 = th->Final->mymap.find('e');
	if(itr0!=th->Final->mymap.end()){
		itr0->second.push_back(f1->start);
	}
	else{
		th->Final->mymap['e'].push_back(f1->start);
	}
	
	//f1->f2
	map<char,vector<state*> >::iterator itr1;
	itr1 = f1->Final->mymap.find('e');
	if(itr1!=f1->Final->mymap.end()){
		itr1->second.push_back(f2->start);
	}
	else{
		f1->Final->mymap['e'].push_back(f2->start);
	}
	
	//f2->f3
	map<char,vector<state*> >::iterator itr2;
	itr2 = f2->Final->mymap.find('e');
	if(itr2!=f2->Final->mymap.end()){
		itr2->second.push_back(f3->start);
	}
	else{
		f2->Final->mymap['e'].push_back(f3->start);
	}
	
	//f3->f2
	map<char,vector<state*> >::iterator b;
	b = f3->Final->mymap.find('e');
	if(b!=f3->Final->mymap.end()){
		b->second.push_back(f2->start);
	}
	else{
		f3->Final->mymap['e'].push_back(f2->start);
	}
	
	//f3->final
	map<char,vector<state*> >::iterator en;
	en = f3->Final->mymap.find('e');
	if(en!=f3->start->mymap.end()){
		en->second.push_back(n->Final);
	}
	else{
		f3->Final->mymap['e'].push_back(n->Final);
	}
	
	n->start = this->start;
	
	return n;

}



vector<char> Sigma (string str){
	vector<char>vec;
	for(int i=0;i<str.length();i++){
		if(num(str.at(i))==true&&str.at(i)!='e'){
			vec.push_back(str.at(i));
		}
	}
	return vec;
	
}


bool is_unmarked(set<DS*> X){
	
	set<DS*>::iterator p;
	p = X.begin();
	while(p!=X.end()){
		if( (*p)->mark == false){
			return true;
		}
		++p;
	}
	return false;
}

DS* unmark(set<DS*> X){
	set<DS*>::iterator p;
	p = X.begin();
	while(p!=X.end()){
		if( (*p)->mark == false){
			return (*p);
		}
		
		p++;
	}
	return NULL;
}

bool present(set<DS*>S,DS* X){
		
		set<DS*>::iterator p;
		p = S.begin();
		while(p != S.end()){
			if( (*p)->Dstate == X->Dstate ){
				return true;
			}
			p++;
		}
		
		return false;
}


DS* Find_it(set<DS*> X,DS* Y){
	
	DS* ptr = new DS;
	set<DS*>::iterator p;
	p = X.begin();
	while(p!=X.end()){
		
		if(Y->Dstate == (*p)->Dstate){
			ptr = (*p);
			return ptr;	
		}
		
		p++;
	}
	
	
	return ptr;
}


DFA* makeDFA(NFA* n,vector<char>alpha){
	DFA* dfa = new DFA;
	set<DS*>S;
	
	set<state*> Y;
	n->Final->en = true;
	Y.insert(n->start);
	
	DS* q = new DS;

	q = Epsilon(Y);
	q->start = true;
	S.insert(q);
	dfa->start = q;
	
	while(is_unmarked(S)==true){
		
		DS* temp = unmark(S);
		temp->mark = true;
		
		for(int i=0;i<alpha.size();i++)
		{
			DS* o1 = new DS;
			o1 = Epsilon(Move(temp->Dstate,alpha[i]));
		
			if(o1->Dstate.empty()==false){
			
				if(present(S,o1)==false){
					S.insert(o1);
					temp->T.insert(pair<char,DS*>(alpha[i],o1));
				}	
				else if(present(S,o1)==true)
				{
					DS* ex = Find_it(S,o1);
					temp->T.insert(pair<char,DS*>(alpha[i],ex));
				}
			
				
			}
		}
		
	}
	
	dfa->alphabet = alpha;
	return dfa;
	
	
}


bool traverse(DFA* X,string str){
	
	
	bool flg=false;
	
	DS* temp = X->start;
	
	for(int i=0;i<str.length();i++){
		char ch = str.at(i);
		if(ch == 'e'){
			flg = temp->end;
		}
		else{
			
			map<char,DS*>::iterator p;
		
			p = temp->T.find(ch);
			if(p!=temp->T.end()){
				temp = p->second;
				flg = p->second->end;
			}	
			else{
				flg = false;
				break;
			}
		
		}
	}
	
	return flg;
	
}
/////////////////
string inCon(string str)
{
	string out="";
	out+=str.at(0);
	for(int i=1;i<str.length();i++)
	{
		if(num(str[i-1])==true&&(num(str[i])==true || str[i]=='('))
		{
			out+=".";
			out+= str.at(i);
			
		}
		else if(str[i-1]==')' && (str.at(i)=='(' || num(str[i])==true))
		{
			out+=".";
			out+=str.at(i);
		}
		else if(str[i-1]=='*' && (str[i]=='(' || num(str[i])==true )){
			out+=".";
			out+=str.at(i);
		}
		else if(str[i-1]=='+' && (str[i]=='(' || num(str[i])==true )){
			out+=".";
			out+=str.at(i);
		}
		else
		{
			out+=str.at(i);
		}
	}

	return out;
}

string make_post(string in)
{
	stack<char>st;
	string out="";
	
	for(int i=0;i<in.length();i++){
		
		if(num(in.at(i))==true){
			out+=in.at(i);
		}
		else if(in.at(i)=='('){
			st.push(in.at(i));
		}
		else if(in.at(i)==')'){
			while(st.top()!='('||st.empty()==true){
				char c = st.top();
				st.pop();
				out+=c;
			}
			
			if(st.empty()==false){
				st.pop();
			}
		
		}
		else if(op(in.at(i))==true){
			
			char ch = in.at(i);
			if(st.empty()==true)
			{
				st.push(ch);
			}
			else{
				if(pref(ch)>pref(st.top())){
					st.push(ch);
				}
				else if(pref(ch)<=pref(st.top())){
				
					while(st.empty()==false&&pref(ch)<=pref(st.top())){
						out+=st.top();
						st.pop();
					}
					st.push(ch);
				}
				
			}
		}
		
	}
	
	while(st.empty()==false){
		out+=st.top();
		st.pop();
	}
	return out;
}


////////////////////////////
//take input make a expression tree and create the NFA for it
int main()
{
	NFA *n;
	stack<NFA*>st;
	vector<char>al;
	
	string regex;
	
	cin>>regex;
	
	al = Sigma(regex);
	
	string xyz = inCon(regex);
	string post = make_post(xyz);
	
	for(int i=0;i<post.length();i++)
	{
	
		char ch = post.at(i);
		
		if(num(ch)==true)
		{
			
			if(ch == 'e'){
				NFA* temp = new NFA;
				temp = temp->createEmpty();
				st.push(temp);
			}
			else
			{
				NFA* temp = new NFA;
				temp=temp->createSymbol(ch);
				st.push(temp);
			}
			
		}
		else if(op(ch)==true||ch=='.')
		{
			if(ch=='.')
			{
				NFA* ans = new NFA;
				NFA* w = st.top();
				st.pop();
				if(st.empty()==false){
					NFA* q = st.top();
					st.pop();
					ans = q->concate(w);
					st.push(ans);
				}
				else{
					st.push(w);
				}
			}
			else if(ch=='*'){
				NFA* ans = new NFA;
				NFA* w = st.top();
				st.pop();
				
				ans = w->star();
				st.push(ans);
			}
			else if(ch=='|')
			{
				NFA* ans = new NFA;
				NFA* w = st.top();
				st.pop();
				NFA* q = st.top();
				st.pop();
				ans = q->Or(w);
				st.push(ans);
			}	
			else if(ch=='+')
			{
				NFA* ans = new NFA;
				NFA* w = st.top();
				st.pop();
				
				ans = w->plus();
				st.push(ans);
			}
		}
		
	}
	
	if(st.empty()==false){
	
		n = st.top();
		st.pop();
	
	
	n->start->st=true;
	n->Final->en = true;
	

	DFA* dfa = new DFA;
	dfa = makeDFA(n,al);
	
	dfa->start->makeid();
	
	string in;
	
	
	while(cin>>in){
		bool flg = traverse(dfa,in);
		if(flg==true){
			cout<<"matched\n";
		}
		else
			cout<<"not matched\n";
		
	}
}
	
}


int pref(char o1)
{
	if(o1=='*')
		return 3;
	else if(o1=='+')
		return 2;
	else if(o1=='.')
		return 1;
	else if(o1=='|')
		return 0;
	else
		return -1;
}


bool num(char ch)
{
	if(ch >= 48 && ch<= 57)
		return true;
	else if(ch == 'e')
		return true;
	else
		return false;
		
}

bool op(char ch)
{
	if(ch == '*' || ch == '|' || ch == '+'||ch=='.')
		return true;
	else 
		return false;
	
}

/*

//convert given string into postfix expression
string postfix(string in)
{
	stack<char>st;
	string out="";
	bool flg=false;


	for(int i=0;i<in.length();i++){
		char ch = in.at(i);
		if(num(ch)==true&&flg==false){
			out+=ch;
			flg = true;
		}
		else if(num(ch)==true&&flg==true)
		{
			if(st.empty()==true)
			{
				st.push('.');
			}
			else
			{
				if(pref('.')>pref(st.top())){
					st.push('.');
				}
				else if(pref('.')<=pref(st.top())){
					char c = st.top();
					st.pop();
					out+=c;
					st.push('.');
				}
				
			}
			flg = true;
			out+=ch;
			
		}
		else if(ch=='*')
		{
			out+=ch;
			flg = true;
		}
		else if(ch=='+')
		{
			out+=ch;
			flg= true;
		}
		else if(op(ch)==true)
		{
			if(st.empty()==true)
			{
				st.push(ch);
			}
			else
			{
				if(pref(ch)>pref(st.top())){
					st.push(ch);
				}
				else if(pref(ch)<=pref(st.top())){
					char c = st.top();
					st.pop();
					out+=c;
					st.push(ch);
				}
				
			}
			flg = false;
		}
		else if(ch=='(' ){
			int j =  in.find(')',i);
			int len = j-i-1;
			string str = in.substr(i+1,len);
			string n = postfix(str);
		
			if(flg==true)
			{
				if(st.empty()==true)
				{
					st.push('.');
				}
				else
				{
				
				if(pref('.')>pref(st.top())){
					st.push('.');
				}
				else if(pref('.')<=pref(st.top())){
					char c = st.top();
					st.pop();
					out+=c;
					st.push('.');
					}
				}
			}
		
			out+=n;

			i = j;
			
			flg = true;
				
		}
		
	}
	
	while(st.empty()==false)
	{
		char c1 = st.top();
		out += c1;
		st.pop();
	}
	
	return out;
	
}

*/




