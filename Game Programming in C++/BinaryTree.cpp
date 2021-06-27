#include "BinaryTree.h"
#include "Error.h"

/***************************************************************************

	BaseBinaryTree

****************************************************************************/

/*

	・各ノードはInstanceを変数にもつ。

	・8bit整数(code)と読み取る「長さ(codelength)」の2つがTreeの位置を指す。
		・codeのbit数字は枝分かれ番号を示す。codelengthの長さ分先頭bitから枝分かれを繰り返す。

	・BaseBinaryTreeは登録と削除は別途行う。(NodeInfoをもたず自動削除機能等はない)

	・BaseBinaryTreeはUpdate()を行うことでTreeを走査し、全ノードを幅優先探索で配列に集草する

*/

template <typename Instance>
bool BaseBinaryTree<Instance>::startreach(false);

template <typename Instance>
const unsigned int BaseBinaryTree<Instance>::defaulttablecap(500);

template <typename Instance>
BaseBinaryTree<Instance>::BaseBinaryTree(unsigned int dimension, unsigned int tablecap/* = defaulttablecap*/) :
	startnode(), nodetable(), presentnode(&dummy), tablenum(), dimension(dimension),
	branchlength((unsigned int)pow(2,dimension)), dummy(this)
{
	nodetable.reserve(tablecap);
}

template <typename Instance>
BaseBinaryTree<Instance>::~BaseBinaryTree(void) {
	Clear();	
}

template <typename Instance>
void BaseBinaryTree<Instance>::Register(const Instance& instance, const std::vector<char>& code, unsigned int codelength) {	// copy禁止のものをretturnできても後が面倒なので
	if(code.size() != dimension) error::tBinaryTree::dimensionmismatch();	
	getnode(code, codelength)->instance = instance;
}	

template <typename Instance>
bool BaseBinaryTree<Instance>::Update(void) {	

	bool exist(true);

	noderegister();

	if (nodetable.size()) 	
		presentnode = nodetable[tablenum];

	else {
		presentnode = &dummy;
		exist = false;
	}

	return exist;

}

template <typename Instance>
void BaseBinaryTree<Instance>::Clear(void) {
	if (startnode) Node::deletetree(startnode);		
		startnode = nullptr;	
		Update();
}

template <typename Instance>
bool BaseBinaryTree<Instance>::NextNode(void) {	

	bool exist(true);	

	if ((++tablenum) < nodetable.size()) 
		presentnode = nodetable[tablenum];	

	else {	
		presentnode = &dummy;	
		tablenum = (unsigned int)nodetable.size();	
		exist = false;
	}

	return exist;

}

template <typename Instance>
bool BaseBinaryTree<Instance>::NextEdge(void) {	

	bool exist(false);	

	while (NextNode()) {	

		if (presentnode->Isedge()) {	
			exist = true;
			break;
		}

	}

	return exist;

}

template <typename Instance>
bool BaseBinaryTree<Instance>::NextParent(void) {

	bool exist(true);

	if (presentnode->parent) 
		presentnode = presentnode->parent;
	
	else {
		presentnode = &dummy;
		exist = false;
	}

	return exist;

}

template <typename Instance>
const Instance& BaseBinaryTree<Instance>::present(void) {
	return presentnode->instance;
}

template <typename Instance>
void BaseBinaryTree<Instance>::Remove(Node* node) {

	remove(node);

	if (startreach) {
		startnode = nullptr;
		startreach = false;	
	}

}


template <typename Instance>
void BaseBinaryTree<Instance>::remove(Node* node) {

	Node* parent(node->parent);

	delete node;

	if (parent) {

		bool unbranch(true);

		for (auto& branch : parent->branch) {

			if (branch) {
				if (branch == node) branch = nullptr;	
				else unbranch = false;
			}

		}

		if (unbranch && (parent->instance == Instance()))
			remove(parent);

	}
	else
		startreach = true;

}

template <typename Instance>
void BaseBinaryTree<Instance>::noderegister(void) {	

	tablenum = 0;	
	nodetable.clear();

	if (startnode) nodetable.emplace_back(startnode);

	for (unsigned int n(0); n < nodetable.size(); ++n) {	

		for (auto branch : nodetable[n]->branch) {
			if (branch) nodetable.emplace_back(branch);
		}

	}


}

template <typename Instance>	
unsigned int BaseBinaryTree<Instance>::getcodelength(const std::vector<char>& code1, const std::vector<char>& code2) {

	unsigned int length(0); int bitpoint(0b10000000);

	for (; length < 8; ++length) {	//2つのcodeの先頭からの共通bit部分の長さを取得

		for (unsigned int i(0); i < dimension; ++i) {

			if ((code1[i] & bitpoint) != (code2[i] & bitpoint)) goto End;

		}

		bitpoint >>= 1;

	}

End:
	return length;

} //結局中でcopyは必要なのでコピー渡し。vectorは生成に時間がかかる？

template <typename Instance>
typename BaseBinaryTree<Instance>::Node* BaseBinaryTree<Instance>::getnode(const std::vector<char>& code, unsigned int codelength) {

	if (!startnode) startnode = new Node(this);

	Node* result(startnode);  int bitpoint(0b10000000);

	while (codelength--) {

		int branchnum(0);

		for (unsigned int i(0); i < dimension; ++i) { //各軸の同じ位置のbitを並べて1つの数字にする
			branchnum <<= 1;
			branchnum |= (int)((bool)(code[i] & bitpoint));
		}

		bitpoint >>= 1;

		Node** temp(&result->branch[branchnum]); //上で取り出した数字番目の枝へ移る

		if (!(*temp)) {	//nodeが存在していないならここで生成
			*temp = new Node(this);
			(*temp)->parent = result;
		}

		result = *temp;

	}// codelength分この処理を繰り返す

	return result;

}

/***************************************************************************

	BaseBinaryTree::Node

****************************************************************************/

template <typename Instance>
BaseBinaryTree<Instance>::Node::Node(const BaseBinaryTree<Instance>* tree) : parent(), branch(tree->branchlength), instance()
{}	

template <typename Instance>
bool BaseBinaryTree<Instance>::Node::Isedge(void) {

	bool unbranch(true);

	for (auto branch : this->branch) {

		if (branch) {
			unbranch = false;
			break;	
		}

	}

	return unbranch;

}

template <typename Instance>
bool BaseBinaryTree<Instance>::Node::IsNulledge(void) {
	return Isedge() && (instance == Instance());
}

template <typename Instance>
void BaseBinaryTree<Instance>::Node::deletetree(Node* start) {

	for (auto branch : start->branch) {
		if (branch) deletetree(branch);
	}

	delete start;

}

/***************************************************************************

	BinaryTree

****************************************************************************/

/*

	・NodeInfoに登録位置のcode,codelength,登録したInstance,TreeIDを記録する。

	・RegisterやRemoveを行う際にはこのNodeInfoが必要となり、自動で前回の登録を修正してくれる。


	・余分となった枝の"伐採"を行うために"removetree"を用意している。"

		・removetreeはＴree構造で、本TreeでNodeのRemove()時にノードがNulledgeなら(本Treeと同じ位置に)登録する。
		・オブジェクトの移動時と衝突処理時の2回でオブジェクトは動き回り、重複登録が考えられるのでTree構造を採用する。

		・本TreeのUpdate()時にremovetreeをUpdate()し、removetreeの全ノードを配列に収納。
		・その後removetreeのedgeをたどり、本Treeの該当ノードがNulldgeなら親を含めた余分な枝を削除。
		・余分な枝の削除後に本Treeの全ノードを配列に収納。


*/

namespace object {
	using ObjectID = unsigned int;
}

template class BaseBinaryTree<std::vector<object::ObjectID>>;
template class BinaryTree<object::ObjectID>;
template class BaseBinaryTree<BaseBinaryTree<std::vector<object::ObjectID>>::Node*>;

template <typename Instance>
const unsigned int BinaryTree<Instance>::defaultremovetablecap(BaseBinaryTree<std::vector<Instance>>::defaulttablecap * 2 / 5);

template <typename Instance>
BinaryTree<Instance>::BinaryTree(unsigned int dimension, unsigned int tablecap/* = defaulttablecap*/, unsigned int removetablecap/* = defaultremovetablecap*/) :
	BaseBinaryTree<std::vector<Instance>>(dimension, tablecap), removetree(dimension, removetablecap) {}

template <typename Instance>
BinaryTree<Instance>::~BinaryTree(void) {
	message::tBinaryTree::Nodeinfoleft();
}

template <typename Instance>
void BinaryTree<Instance>::Register(NodeInfo& returninfo, const Instance& instance, const std::vector<char>& code1, const std::vector<char>& code2) {	

	if ((code1.size() != this->dimension) || (code2.size() != this->dimension))	
		error::tBinaryTree::dimensionmismatch();	

	remove(returninfo);

	returninfo.code = code1;	
	returninfo.codelength = BaseBinaryTree<std::vector<Instance>>::getcodelength(code1, code2);
	returninfo.instance = instance;

	BaseBinaryTree<std::vector<Instance>>::getnode(code1, returninfo.codelength)->instance.emplace_back(instance);
	
}

template <typename Instance>
void BinaryTree<Instance>::Remove(NodeInfo& nodeinfo) {
	remove(nodeinfo);
	nodeinfo.tree = nullptr;	
}

template <typename Instance>
void BinaryTree<Instance>::remove(NodeInfo& info) {

	if (isvergininfo(info)) return; 
		
	auto node(BaseBinaryTree<std::vector<Instance>>::getnode(info.code, info.codelength)/*info.node*/);

	for (auto iter(node->instance.begin()); iter != node->instance.end(); ++iter) {

		if (*iter == info.instance) {
			node->instance.erase(iter);
			break;
		}

	}

	if (node->IsNulledge())
		removetree.Register(node, info.code, info.codelength);

}

template <typename Instance>
bool BinaryTree<Instance>::Update(void) {

	if (removetree.Update()) {

		if (!removetree.present()) removetree.NextEdge();	

		do {

			if (removetree.present()->IsNulledge())	
				BaseBinaryTree<std::vector<Instance>>::Remove(removetree.present());
			
		} while (removetree.NextEdge());
	
	}

	removetree.Clear();

	return ((BaseBinaryTree<std::vector<Instance>>*)this)->Update();

}

template <typename Instance>
bool BinaryTree<Instance>::isvergininfo(NodeInfo& nodeinfo) {

	bool virgin(false);

	if (!nodeinfo.tree) {
		nodeinfo.tree = this;
		nodeinfo.code.resize(this->dimension);	
		virgin = true;
	}
	else if (nodeinfo.tree != this)	
		error::tBinaryTree::treemismatch();

	return virgin;

}	

/***************************************************************************

	BinaryTree::NodeInfo

****************************************************************************/

/*

	・外部からはアクセス禁止&コピー禁止でTreeに登録されているオブジェクトと1対1に対応する。

	・NodeInfoはremovetreeに登録することを考えてcodeとcodelengthを位置情報として持つ。

	・NodeInfoをメンバに持つオブジェクトがデストラクトされるとTreeの方からも削除するようにしている。

	・先にTreeをdeleteした場合はNodeInfoのデータは残る。

*/

template <typename Instance>
BinaryTree<Instance>::NodeInfo::NodeInfo(void) : code(), codelength(), instance(), tree()/*, node()*/ {}

template <typename Instance>
BinaryTree<Instance>::NodeInfo::~NodeInfo(void) {
	if (tree) tree->Remove(*this);
}