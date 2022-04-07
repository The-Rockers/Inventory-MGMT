//THIS IS THE FINAL NO-SPLAY VERSION

/*
    *Author: Luis E. Ortiz
    *Creation Date: 2/28/2019
    *Modification Date: 2/28/2019
    *Purpose: Code for Simple int-to-int of Order Map ADT Implementation based on Splay Trees
    *NOTE: only basic methods of the Order Map ADT (i.e., put, erase, find, size, empty) implemented

    2019-10-10 BW bugfixes
    2020-02-10 BW updated parsing
    2020-02-12 BW cleaned up splay operations
    2020-02-17 BW added inventory

	NOTE 2: Implement the inventory management system first, then implement splay tree

	Name: Nicholas (Nick) Rock
	UMID: 17588613
	CIS 350 Section 001
*/


#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <cstddef>
#include <vector>
#include <cctype>
#include <sstream>
#include <algorithm>


using namespace std;

// Utility functions
void loadFile(string fname, fstream& file)
{
	file.open(fname.c_str());
	if (file.fail())
	{
		cout << "Cannot open file " << fname << endl;
	}
}

string lowercase(string s)
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		s[i] = std::tolower(s[i]);
	}
	return s;
}

void login(string usr, string pw) {
	string credFileName = "credentials.txt";
	string line;
	string test_u, test_p;
	char splitter = ':';		//format for file will be: each line is one set of credentials, separated by colon. eg USERNAME:PASSWORD
	fstream credential;

	loadFile(credFileName, credential);
	while (getline(credential, line)) {
		test_u = line.substr(0, line.find(splitter));
		cout << test_u;
	}
	cin >> test_p;
	credential.close();
}

// simple data structure used to create nodes for (linked-list) BST-based implementation of an ordered map ADT
// from string keys to integer values
struct Node {
	string key;
	int ht;
	int value;
	int sum;
	Node* left;
	Node* right;
	Node* parent;
    // constructors
	Node(string k, int v) :
		key(k), value(v), left(NULL), right(NULL), parent(NULL) { }
	Node(string k, int v, Node* l, Node* r, Node* p) :
		key(k), value(v), left(l), right(r), parent(p) { }
};



/*
 *Purpose: Class definition of a simple implementation of an ordered map ADT, 
 mapping integers keys to integer values, using a binary search tree (BST) 
 *NOTE: only basic methods of the ordered map ADT (i.e., put, erase, find, size, empty), and some auxiliary functions (e.g., successor and predecessor) implemented
 *NOTE: For simplicity, and consistency with implementation in other programming languages (e.g., Java and Python), the documentation below refers to variables that are actually pointers to a node in the tree simply as a node in the tree, without the pointer qualification, whenever the distinction is clear from the context
 *NOTE: For consistency with implementation in other programming languages, this implementation does not overload the ++ operator to implement the successor function; similarly for the -- and the predecessor function
*/
class BSTMap
{
	public:
		Node* root;
	private:
		void deleteAll();
		Node* successor(Node* w) const;
		Node* removeNode(Node* w);
		int n;
	protected:
		void printAux(const Node* w) const;  // print utility
	public:
		BSTMap() : root(NULL), n(0) { };
		~BSTMap();
		Node** find(string k) const;
		Node* put(string k, int v);
		Node* erase(string k);
		Node* eraseNode(Node* w);
		int size() const;
		bool empty() const;
		void print() const;   // print as parenthetic string
		void printTree(Node* s, int space) const;
		void calcSum(Node* s);
};


//*Purpose: Implement member functions/methods of BSTMap class
// utility/aux function to print out a parenthetic string representation of the BST
// INPUT: a node w in the BST whose subtree is to be printed out; or NULL
void BSTMap::printAux(const Node* w) const {
	if (w) {
		cout << "[" << w->key << ":" << w->value << "]";
		cout << "(";
		printAux(w->left);
		cout << "),(";
		printAux(w->right);
		cout << ")";
	}
}

// print out a parenthetic string representation of the whole BST
void BSTMap::print() const {
	printAux(root);
	cout << endl;
}

// prints out a string representation of the whole BST using a reverse inorder traversal
void BSTMap::printTree(Node* s, int space) const {
	int addSpace = 8;
	// base case
	if (!s)
	{
		return;
	}
	// add more whitespace
	space = space + addSpace;
	// print right
	this->printTree(s->right, space);

	cout << endl;
	for (int i = addSpace; i < space; i++)
		cout << " ";
	cout << s->key << ":" << s->value << endl;

	// print left
	this->printTree(s->left, space);
}

// POSTCONDITION: The BST is empty
void BSTMap::deleteAll()
{
	Node* w = root;
	while (w) {
		if (!(w->left || w->right)) {
			Node* x = w;
			w = w->parent;
			if (w) {
				if (w->left == x) w->left = NULL;
				else w->right = NULL;
			}
			delete x;
			n--;
			continue;
		}
		w = (w->left) ? w->left : w->right;
	}
}

// Destructor
// POSTCONDITION: The BST is empty
BSTMap::~BSTMap() {
	deleteAll();
}

// INPUT: a node w in the BST
// OUTPUT: the parent of w, which may be NULL if w is the root of the BST
// PRECONDITION: w is external (i.e., the left or right subtree, or both, are empty)
// POSTCONDITION: the size of the BST is reduced by 1, and w is properly removed from the BST
Node* BSTMap::removeNode(Node* w) {
	Node* z = w->parent;
	Node* x = (w->left) ? w->left : w->right;
	if (x) x->parent = z;
	if (z) {
		if (z->left == w) z->left = x;
		else z->right = x;
	}
	else root = x;
	delete w;
	n--;
	return z;
}

// INPUT: a string key k
// OUTPUT: a 2-element array, where
// element 0 is w, the node with key k, if k is already in the ordered map, or NULL otherwise; and
// element 1 is z, the parent of node w, or NULL if w is NULL or the root
// or the last node visited while trying to find a node with key k in the BST
Node** BSTMap::find(string k) const {
	Node* w = root;
	Node* z = NULL;
	while (w && (w->key != k)) {
		z = w;
		w = (w->key > k) ? w->left : w->right;
	}
	Node** wAndZ = new Node*[2];
	wAndZ[0] = w;
	wAndZ[1] = z;
	return wAndZ;
}

// INPUT: a key-value pair k (string), v (int)
// OUTPUT: if k is already in the ordered map, then output the node containing k, with v replacing the previous map value for k;
// otherwise, output the new node in the tree with the corresponding key-value pair k and v.
// POSTCONDITION: if key k is already in the ordered map, then the node containing k in the BST has v as its new map value;
// otherwise, the size of the BST is increased by 1, and a new node with key-value pair k and v is properly added as a leaf to the BST;
// if the BST was empty, then the new node becomes the root of the BST (and thus its only node)
Node* BSTMap::put(string k, int v) {
	Node** wAndZ = find(k);
	Node* w = wAndZ[0];
	Node* z = wAndZ[1];
	delete wAndZ;
	if (w) {
		w->value = v;
		return w;
	}
	Node* x = new Node(k, v, NULL, NULL, z);
	if (z) {
		if (z->key > k) z->left = x;
		else z->right = x;
	}
	else root = x;
		n++;
	if (n == 1) root = x;
		return x;
}

// INPUT: a node w in the tree, or NULL
// OUTPUT: NULL if w is NULL; otherwise, the parent of w or
// its successor if w is internal (i.e., has both non-empty left and right subtrees)
// POSTCONDITION: if w is NULL, the BST remains the same; otherwise, the size of the BST is reduced by 1,
// and w, or its sucessor if w is internal, is properly removed from the BST;
// also, if w is internal, then the (key,value) pair of the successor node is copied to w before the successor node is removed
Node* BSTMap::eraseNode(Node* w) {
	if (!w) return NULL;
	if (w->left && w->right) {
		Node* x = successor(w);
		w->key = x->key;
		w->value = x->value;
		w = x;
	}
	Node* z = removeNode(w);
	return z;
}

// INPUT: a string key k
// OUTPUT: the node in the tree corresponding to the parent of the node actually removed from the BST during the operation removing the key k from the ordered map; or, if k is not a key in the ordered map, then output the node in the tree last visited during the search for key k, or NULL if the tree is empty
// POSTCONDITION: no node in the BST has key k: that is, if the BST had a node with key k, then it is properly removed; otherwise, the BST remains intact
Node* BSTMap::erase(string k) {
	Node** wAndZ = find(k);
	Node* w = wAndZ[0];
	Node* z = wAndZ[1];
	delete wAndZ;
	return ((w) ? eraseNode(w) : z);
}

// INPUT: a node w in the tree; or NULL
// OUTPUT: the predecessor node of w,
// which is the node in the tree with the key immediately following the key of w in the inorder sequence of keys in the ordered map; or
// NULL if w is NULL or w has no predecessor node in the tree
Node* BSTMap::successor(Node* w) const {
	if (!w) return NULL;
	if (w->right) {
		w = w->right;
		while (w->left) w = w->left;
	}
	else {
		Node* z = w;
		w = w->parent;
		while (w->right == z) {
			z = w;
			w = w->parent;
		}
	}
	return w;
}

// OUTPUT: size of the tree
int BSTMap::size() const {
	return n;
}

// OUTPUT: true if the tree is empty; false otherwise
bool BSTMap::empty() const {
	return (!root);
}

//NO OUTPUT IT JUST UPDATES THE SUM
void BSTMap::calcSum(Node* s) {
	if (s->left != NULL) {				//traversal left
		calcSum(s->left);
	}
	if (s->right != NULL) {				//traversal right
		calcSum(s->right);
	}

	if ((s->left != NULL) && (s->right != NULL)) {				//visit clauses
		s->sum = s->value + (s->left->sum) + (s->right->sum);
	}
	else if ((s->left == NULL) && (s->right != NULL)) {			//visits and sums each node child
		s->sum = s->value + (0) + (s->right->sum);
	}
	else if ((s->left != NULL) && (s->right == NULL)) {
		s->sum = s->value + (s->left->sum) + (0);
	}
	else if ((s->left == NULL) && (s->right == NULL)) {
		s->sum = s->value;
	}

}




/*
 *Purpose: Class definition of a simple implementation of an ordered map ADT, 
 mapping integers keys to integer values, using a splay tree, based on the BSTMap class
 *NOTE: only basic methods (i.e., put, erase, and find), including those inherited from BSTMap (i.e., size and empty), and some auxiliary functions (e.g., successor and predecessor) implemented
*/
class SplayTreeMap : public BSTMap {
	private:
		void zig(Node* x);
		void zigZig(Node* x);
		void zigZag(Node* x);
		void splay(Node* x);
	public:
		SplayTreeMap() { };  // default constructor
		Node* findSplay(string k);
		Node* putSplay(string k, int v);
		Node* eraseSplay(string k);
};

// INPUT: a node x in the splay tree 
// PRECONDITION: 
// POSTCONDITION: 
void SplayTreeMap::zig(Node* x) {
	Node *y = x->parent;

	if (x == y->left) {
		//RIGHT rotate
		//Xp -> Yp, Yl -> Xr, Xr -> Y
		x->parent = y->parent;
		y->left = x->right;
		x->right = y;
	}
	else if (x == y->right) {
		//LEFT rotate
		//Xp -> Yp, Yr -> Xl, Xl -> Y
		x->parent = y->parent;
		y->right = x->left;
		x->left = y;
	}
	//Rotate X & Y
}

// INPUT: a node x in the splay tree
// PRECONDITION: 
// POSTCONDITION:
void SplayTreeMap::zigZig(Node* x) {
    // Rotate X & Y
	zig(x);
	zig(x);
	// Rotate X & Z 
}

// INPUT: a node x in the splay tree 
// PRECONDITION:
// POSTCONDITION:
void SplayTreeMap::zigZag(Node* x) {
	// Rotate X & Y
	zigZig(x);
	// Rotate X & Z
}

// INPUT: a node x in the splay tree 
// PRECONDITION: 
// POSTCONDITION: 
// PSEUDOCODE PROVIDED IN HOMEWORK 3, ADAPTED TO WORK IN THIS PROGRAM
void SplayTreeMap::splay(Node* x) {
	if (x != nullptr) {
		while ((x->parent != nullptr)) {
			Node *y = x->parent;
			if (y->parent != (nullptr)) {
				zig(x);
			}
			else {
				Node *z = y->parent;
				if ((y->left == x) == (z->left == y)) {
					zigZig(x);
				}
				else {
					zigZag(x);
				}
			}
		}
	}
root = x;
}

// INPUT: a string key k
// OUTPUT: the node w with key k, if k is already in the ordered map; NULL otherwise
// POSTCONDITION: the splay tree is restructured by properly splaying x (immediately after the search operation, but before the splay operation), if x is not the root already (i.e., x becomes the root of the splay tree, if it wasn't already, and x is the root after the full splay operation)
Node* SplayTreeMap::findSplay(string k) {
	Node** wAndZ = this->find(k);
	Node* w = wAndZ[0]; // w is the node with key k, if it exists, or NULL otherwise
	Node* z = wAndZ[1]; // z is the parent of node w, or if w is NULL, the last node found while searching for key k
	delete wAndZ;
	Node* x = (w) ? w : z;
	this->splay(x);
	return x;

}

// INPUT: a key-value pair k (string), v (integer)
// OUTPUT: if k is already in the ordered map, then output the node containing k, with v replacing the previous map value for k;
// otherwise, output the new node with the corresponding key-value pair k and v.
// POSTCONDITION: if key k is already in the ordered map, then the node containing k in the splay tree has v as its new map value;
// otherwise, the size of the splay tree is increased by 1, and a new node with key-value pair k and v is properly added to the splay tree;
// if the splay tree was empty, then the new node becomes the root of the BST (and thus it's only node); in addition, the splay tree is restructured by properly splaying x immediately after the insertion operation, but before the splay operation, if x is not the root already (i.e., x becomes the root of the splay tree, if it wasn't already, and x is the root after the full splay operation)
Node* SplayTreeMap::putSplay(string k, int v) {
	Node* x = this->put(k,v);
	this->splay(x);
	return x;
}

// INPUT: a string key k
// OUTPUT: the node x in the tree corresponding to the parent of the node actually removed from the splay tree during the operation removing the key k from the ordered map; or, if k is not a key in the ordered map, then x is the node in the tree last visited during the search for key k, or NULL if the tree is empty
// POSTCONDITION: no node in the splay tree has key k: that is, if the splay tree had a node with key k, then it is properly removed, and the size of the tree is reduced by 1, otherwise the size of the splay tree remains the same; in addition, the splay tree is restructured by properly splaying the node pointed to by x (immediately after the erase operation, but before the splay operation), if x is not NULL and it is not the root already (i.e., x becomes the root of the splay tree, so that x is the root after the full splay operation)
Node* SplayTreeMap::eraseSplay(string k) {
	Node* x = this->erase(k);
	this->splay(x);
	return x;
}



/*
 *Purpose: Class for inventory management using a splay tree map to track entries (string key, integer amount/value)
*/
class SplayTreeInventory {
private:
	SplayTreeMap* stmap;

public:
	int numProducts();
	int available(string id);
	int size();
	void into(string id, int amount);
	void out(string id, int amount);
	void printProduct(string id);
	void printTree();
	void printAll();
	void printAllHelper(Node* s);
	void printSize();
    // constructor
	SplayTreeInventory() : stmap(new SplayTreeMap()) {}

};

// OUTPUT: number of different items in inventory
int SplayTreeInventory::numProducts()
{
	return this->stmap->size();
}

// INPUT: a string key id
// OUTPUT: the value of the entry (amount) corresponding to the input key, if the entry exists
//   return 0 otherwise
// PRECONDITION:
// POSTCONDITION:
int SplayTreeInventory::available(string id)
{
	//Node *x = this->stmap->findSplay(id);
	Node **x = this->stmap->find(id);
	return (**x).value;
	
}

// INPUT: string key id, integer value
// PRECONDITION:
// POSTCONDITION:
void SplayTreeInventory::into(string id, int amount)
{
	Node **i = this->stmap->find(id);
	

	if (i[0] == NULL) {
		this->stmap->put(id, amount);
	}
	else {
		(**i).value += amount;
	}
}

// INPUT: string key id, integer value
// PRECONDTION: ID and amount are valid inputs
// POSTCONDITION: the node at ID is emptied. If an additional out operation were run, nothing would happen
void SplayTreeInventory::out(string id, int amount)
{
	//Node *x = this->stmap->findSplay(id);
	Node **x = this->stmap->find(id);
	

	if ((**x).value <= amount) {
		this->stmap->erase(id);
	}
	else {
		(**x).value -= amount;
	}

}

// PRECONDITION:
// POSTCONDITION:
// OUTPUT: total number of items in the inventory
int SplayTreeInventory::size()
{
	this->stmap->calcSum(stmap->root);
	return this->stmap->root->sum;

}

// OUTPUT: string representation of an entry with a specified key id
void SplayTreeInventory::printProduct(string id)
{
	int i = this->available(id);
	cout << id << ": " << i << endl;
}

// helper function to print all entries
void SplayTreeInventory::printAllHelper(Node* s)
{
	if (s == NULL)
	{
		return;
	}
	printAllHelper(s->left);
	cout << s->key << ": " << s->value << endl;
	printAllHelper(s->right);
}

// OUTPUT: prints string representation of all entries in the inventory
void SplayTreeInventory::printAll()
{
	printAllHelper(this->stmap->root);
}

// OUTPUT: prints string representation of the splay tree representing the inventory
void SplayTreeInventory::printTree()
{
	this->stmap->printTree(this->stmap->root, 0);
}

// OUTPUT: prints number of entries in the inventory
void SplayTreeInventory::printSize()
{
	cout << this->size() << endl;
}


int main() {
	login("abc", "123");
	string inputFilename = "input.txt";
	string line;

	SplayTreeInventory S;
	// open input file
	fstream inputFile;
	loadFile(inputFilename, inputFile);
	while (getline(inputFile, line))
	{
		// trim whitespace
		// echo input
		cout << line << endl;
		// parse input using a stringstream
		stringstream lineSS(line);
		string token;
		string command = "";
		// store tokens in a vector
		vector<string> tokens;
		while (getline(lineSS, token, ' '))
		{
			// trim whitespace
			token.erase(token.find_last_not_of(" \n\r\t") + 1);
			tokens.push_back(token);
		}

		if (tokens.size() > 0)
		{
			command = tokens[0]; // first token is the command
		}
        
		if (tokens.size() > 1)
		{
			if (command == "in")
			{
				// add a node with specified value, default to 1
				if (tokens.size() > 2)
				{
					S.into(tokens[1], stoi(tokens[2]));
				}
				else
				{
					S.into(tokens[1], 1);
				}
			}
			if (command == "out")
			{
				// add a node with specified value, default to 1
				if (tokens.size() > 2)
				{
					S.out(tokens[1], stoi(tokens[2]));
				}
				else
				{
					S.out(tokens[1], 1);
				}
			}
			if (command == "print_item")
			{
				S.printProduct(tokens[1]);
			}
		}
		if (command == "print")
		{
			S.printAll();
		}
		if (command == "size")
		{
			S.printSize();
		}
		if (command == "print_tree")
		{
			S.printTree();
		}


	}
	inputFile.close();
	system("pause");
	return EXIT_SUCCESS;
}