#include "BinaryTree.h"
#include "Error.h"

/***************************************************************************

	BaseBinaryTree

****************************************************************************/

/*

	�E�e�m�[�h��Instance��ϐ��ɂ��B

	�E8bit����(code)�Ɠǂݎ��u����(codelength)�v��2��Tree�̈ʒu���w���B
		�Ecode��bit�����͎}������ԍ��������Bcodelength�̒������擪bit����}��������J��Ԃ��B

	�EBaseBinaryTree�͓o�^�ƍ폜�͕ʓr�s���B(NodeInfo�������������폜�@�\���͂Ȃ�)

	�EBaseBinaryTree��Update()���s�����Ƃ�Tree�𑖍����A�S�m�[�h�𕝗D��T���Ŕz��ɏW������

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
void BaseBinaryTree<Instance>::Register(const Instance& instance, const std::vector<char>& code, unsigned int codelength) {	// copy�֎~�̂��̂�retturn�ł��Ă��オ�ʓ|�Ȃ̂�
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

	for (; length < 8; ++length) {	//2��code�̐擪����̋���bit�����̒������擾

		for (unsigned int i(0); i < dimension; ++i) {

			if ((code1[i] & bitpoint) != (code2[i] & bitpoint)) goto End;

		}

		bitpoint >>= 1;

	}

End:
	return length;

} //���ǒ���copy�͕K�v�Ȃ̂ŃR�s�[�n���Bvector�͐����Ɏ��Ԃ�������H

template <typename Instance>
typename BaseBinaryTree<Instance>::Node* BaseBinaryTree<Instance>::getnode(const std::vector<char>& code, unsigned int codelength) {

	if (!startnode) startnode = new Node(this);

	Node* result(startnode);  int bitpoint(0b10000000);

	while (codelength--) {

		int branchnum(0);

		for (unsigned int i(0); i < dimension; ++i) { //�e���̓����ʒu��bit����ׂ�1�̐����ɂ���
			branchnum <<= 1;
			branchnum |= (int)((bool)(code[i] & bitpoint));
		}

		bitpoint >>= 1;

		Node** temp(&result->branch[branchnum]); //��Ŏ��o���������Ԗڂ̎}�ֈڂ�

		if (!(*temp)) {	//node�����݂��Ă��Ȃ��Ȃ炱���Ő���
			*temp = new Node(this);
			(*temp)->parent = result;
		}

		result = *temp;

	}// codelength�����̏������J��Ԃ�

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

	�ENodeInfo�ɓo�^�ʒu��code,codelength,�o�^����Instance,TreeID���L�^����B

	�ERegister��Remove���s���ۂɂ͂���NodeInfo���K�v�ƂȂ�A�����őO��̓o�^���C�����Ă����B


	�E�]���ƂȂ����}��"����"���s�����߂�"removetree"��p�ӂ��Ă���B"

		�Eremovetree�͂sree�\���ŁA�{Tree��Node��Remove()���Ƀm�[�h��Nulledge�Ȃ�(�{Tree�Ɠ����ʒu��)�o�^����B
		�E�I�u�W�F�N�g�̈ړ����ƏՓˏ�������2��ŃI�u�W�F�N�g�͓������A�d���o�^���l������̂�Tree�\�����̗p����B

		�E�{Tree��Update()����removetree��Update()���Aremovetree�̑S�m�[�h��z��Ɏ��[�B
		�E���̌�removetree��edge�����ǂ�A�{Tree�̊Y���m�[�h��Nulldge�Ȃ�e���܂߂��]���Ȏ}���폜�B
		�E�]���Ȏ}�̍폜��ɖ{Tree�̑S�m�[�h��z��Ɏ��[�B


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

	�E�O������̓A�N�Z�X�֎~&�R�s�[�֎~��Tree�ɓo�^����Ă���I�u�W�F�N�g��1��1�ɑΉ�����B

	�ENodeInfo��removetree�ɓo�^���邱�Ƃ��l����code��codelength���ʒu���Ƃ��Ď��B

	�ENodeInfo�������o�Ɏ��I�u�W�F�N�g���f�X�g���N�g������Tree�̕�������폜����悤�ɂ��Ă���B

	�E���Tree��delete�����ꍇ��NodeInfo�̃f�[�^�͎c��B

*/

template <typename Instance>
BinaryTree<Instance>::NodeInfo::NodeInfo(void) : code(), codelength(), instance(), tree()/*, node()*/ {}

template <typename Instance>
BinaryTree<Instance>::NodeInfo::~NodeInfo(void) {
	if (tree) tree->Remove(*this);
}