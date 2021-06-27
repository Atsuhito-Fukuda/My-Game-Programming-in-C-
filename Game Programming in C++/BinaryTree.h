#pragma once
#include <vector>
#include <iostream>
#include <string>

/***************************************************************************

	BaseBinaryTree

****************************************************************************/

template <typename Instance>
class BaseBinaryTree {

public:
	BaseBinaryTree(unsigned int dimension, unsigned int tablecap = defaulttablecap);
	~BaseBinaryTree(void);

	void Register(const Instance& instance, const std::vector<char>& code, unsigned int codelength);
	bool Update(void);
	void Clear(void);

	bool NextNode(void);
	bool NextEdge(void);
	bool NextParent(void);
	const Instance& present(void);

protected:
	class Node;

	const unsigned int dimension;
	const unsigned int branchlength;	
	unsigned int getcodelength(const std::vector<char>& code1, const std::vector<char>& code2);
	Node* getnode(const std::vector<char>& code, unsigned int codelength);
	void Remove(Node* node);

private:
	static void remove(Node* node);
	static bool startreach;

	void noderegister(void);
	std::vector<Node*> nodetable;	
	Node* startnode;
	Node* presentnode;
	unsigned int tablenum;
	Node dummy;
protected:
	static const unsigned int defaulttablecap;

private:
	BaseBinaryTree(void);

// debug - - - - - - - - - - - - - - - - - - - - - - -
public:
	void Showresult(void) {
		if (startnode) draw(startnode, "");
	}
	void ShowTable(void){

		for (auto node : nodetable) {
			std::cout << "< ";
			show(node);
			std::cout << ">\n";
		}

	}
	void Showtablenum(void) {
		std::cout << nodetable.size() << "\n";
	}

private:
	void draw(Node* node, std::string string) {

		for (unsigned int n(0); n < branchlength; ++n) {
			if (node->branch[n]) draw(node->branch[n], string + std::to_string(n));
		}

		std::cout << "[" << string << "] : ";
		show(node);
		std::cout << "\n";

	}
	virtual void show(Node* node) {
		std::cout <<  "* ";
	}

};

/***************************************************************************

	BaseBinaryTree::Node

****************************************************************************/

template <typename Instance>
class BaseBinaryTree<Instance>::Node {	

	friend class BaseBinaryTree;

public:
	Node(const BaseBinaryTree<Instance>* tree);
	bool IsNulledge(void);
	bool Isedge(void);
	Instance instance;

private:
	Node* parent;
	std::vector<Node*> branch;
	static void deletetree(Node* start);

	Node(void);
};

/***************************************************************************

	BinaryTree

****************************************************************************/

template <typename Instance>
class BinaryTree : public BaseBinaryTree<std::vector<Instance>> {

public:
	BinaryTree(unsigned int dimension, unsigned int tablecap = BaseBinaryTree<std::vector<Instance>>::defaulttablecap, unsigned int removetablecap = defaultremovetablecap);
	~BinaryTree(void);

	class NodeInfo;
	void Register(NodeInfo& returninfo, const Instance& instance, const std::vector<char>& code1, const std::vector<char>& code2);
	void Remove(NodeInfo& nodeinfo);
	bool Update(void);

private:
	void remove(NodeInfo& info);
	bool isvergininfo(NodeInfo& nodeinfo);

	BaseBinaryTree<BaseBinaryTree<std::vector<Instance>>::Node*> removetree;
	static const unsigned int defaultremovetablecap;

	BinaryTree(void);
// hide - - - - - - - - - - - - - - - - - - - - - - - 
	void Register(const std::vector<Instance>& instance, const std::vector<char>& code, unsigned int codelength){}
	void Clear(void) {}	

// debug - - - - - - - - - - - - - - - - - - - - - - -
	virtual void show(BaseBinaryTree<std::vector<Instance>>::Node* node) override {
		for (const auto& e : node->instance) {
			std::cout << e << " ";
		}
	}

};

/***************************************************************************

	BinaryTree::NodeInfo

****************************************************************************/

template <typename Instance>
class BinaryTree<Instance>::NodeInfo {	

	friend class BinaryTree;

public:
	NodeInfo(void);
	~NodeInfo(void);
private:
	std::vector<char> code;
	unsigned int codelength;
	Instance instance;		
	BinaryTree<Instance>* tree;	

	NodeInfo& operator =(const NodeInfo& nodeinfo);
	NodeInfo(const NodeInfo& nodeinfo);

};