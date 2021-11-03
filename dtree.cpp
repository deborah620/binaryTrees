/**
 * CMSC 341 - Spring 2021
 * Project 2 - Binary Trees
 * DiscriminatorTree.cpp
 * Implementation for the DTree class.
 */

#include "dtree.h"

/**
 * Destructor, deletes all dynamic memory.
 */
DTree::~DTree() {
    clear();
}

/**
 * Overloaded assignment operator, makes a deep copy of a DTree.
 * @param rhs Source DTree to copy
 * @return Deep copy of rhs
 */
DTree& DTree::operator=(const DTree& rhs) {
    if (this != &rhs){
      //clear the lhs
        clear();
	//allocate new root
        _root = new DNode(rhs._root->_account);
        _root->copy(rhs._root);

    }return *this;
}

/**
 * Dynamically allocates a new DNode in the tree.
 * Should also update heights and detect imbalances in the traversal path
 * an insertion.
 * @param newAcct Account object to be contained within the new DNode
 * @return true if the account was inserted, false otherwise
 */
bool DTree::insert(Account newAcct) {
    if (!_root) {
        _root = new DNode(newAcct);
        return true;
    }

    //check if user already exists
    else if (retrieve(newAcct._disc))
        return false;

    //insert user
    else if (insertHelper(newAcct._disc, newAcct, _root)){
        return true;
    }
    else
        return false;
}

/**
 * Removes the specified DNode from the tree.
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
bool DTree::remove(int disc, DNode*& removed) {

    //desired node to remove is the root
    if (_root->_account._disc == disc && !(_root->_vacant)) {
        removed = _root;
        _root->_vacant = true;
        _root->_numVacant++;
        return true;
    }

    else {
        removed = removeHelper(disc, _root);

	if (removed != nullptr) {
            return true;
        }
    }
    return false;
}

/**
 * Retrieves the specified Account within a DNode.
 * @param disc discriminator int to search for
 * @return DNode with a matching discriminator, nullptr otherwise
 */
DNode* DTree::retrieve(int disc) {
    if (_root) {
      //node is root
        if (_root->_account._disc == disc && !(_root->_vacant))
            return _root;

        return _root->retrieve(disc, _root);
    }
    return nullptr;
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void DTree::clear() {
    if (_root) {
        _root->clear(_root);
    }
}

/**
 * Prints all accounts' details within the DTree.
 */
void DTree::printAccounts() const {
    _root->print(_root);
}

/**
 * Dump the DTree in the '()' notation.
 */
void DTree::dump(DNode* node) const {
    if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getAccount().getDiscriminator() << ":" << node->getSize() << ":" << node->getNumVacant();
    dump(node->_right);
    cout << ")";
}

/**
 * Returns the number of valid users in the tree.
 * @return number of non-vacant nodes
 */
int DTree::getNumUsers() const {
    return (_root->getSize() - _root->getNumVacant());
}

/**
 * Updates the size of a node based on the immediate children's sizes
 * @param node DNode object in which the size will be updated
 */
void DTree::updateSize(DNode* node) {
    int right = 0;
    int left = 0;

    if (node->_left)
        left = node->_left->getSize();

    if (node->_right)
        right = node->_right->getSize();

    node->_size = right + left + 1;
}


/**
 * Updates the number of vacant nodes in a node's subtree based on the immediate children
 * @param node DNode object in which the number of vacant nodes in the subtree will be updated
 */
void DTree::updateNumVacant(DNode* node) {
    int right = 0;
    int left = 0;

    if (node->_left)
        left = node->_left->getNumVacant();
   
    if (node->_right)
        right = node->_right->getNumVacant();
   
    node->_numVacant = right + left;

    //if node itself is vacant
    if (node->isVacant())
        node->_numVacant++;
}

/**
 * Checks for an imbalance, defined by 'Discord' rules, at the specified node.
 * @param checkImbalance DNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
bool DTree::checkImbalance(DNode* node) {
    int right = 0;
    int left = 0;

    if (node->_right)
        right = node->_right->getSize();

    if (node->_left)
        left = node->_left->getSize();

    //if the right or left are greater than 4, and one side is 50% or greater than the other
    if (right >= 4 || left >= 4) {
      if (right > left) {
	if (right >= (left * 1.5))
	  return true;
      }
      else if (left >= (right* 1.5))
	return true;

    }
    
    return false;
}

//----------------
/**
 * Begins and manages the rebalancing process for a 'Discrd' tree (pass by reference).
 * @param node DNode root of the subtree to balance
 */
void DTree::rebalance(DNode*& node) {
    DNode *parent = _root;
    bool isLeft = false;
    bool isRoot = false;

    if (node == _root)
      isRoot = true;
 
    else {

      while (parent->_left != node && parent->_right != node) {

        if (parent->getDiscriminator() > node->getDiscriminator()) {
	  parent = parent->_left;	
        }
        else if (parent->getDiscriminator() < node->getDiscriminator()) {
	  parent = parent->_right;
        }
      }

      if (parent->_left == node)
        isLeft = true;
      else if (parent->_right == node)
        isLeft = false;
    }

    updateSize(node);
    updateNumVacant(node);
    int size = node->getSize() - node->getNumVacant();

    DNode** dtreeArray;
    dtreeArray = new DNode*[size];

    int i = 0;

    node->rebalance(node, dtreeArray, i);

    int start = 0;
    int end = size - 1;

    rebuild(dtreeArray, start, end, node);

    if (isRoot)
      _root = node;
    else if (isLeft)
      parent->_left = node;
    else
      parent->_right = node;

    delete [] dtreeArray;

}

/**
 * Overloaded << operator for an Account to print out the account details
 * @param sout ostream object
 * @param acct Account objec to print
 * @return ostream object containing stream of account details
 */
ostream& operator<<(ostream& sout, const Account& acct) {
    sout << "Account name: " << acct.getUsername() <<
         "\n\tDiscriminator: " << acct.getDiscriminator() <<
         "\n\tNitro: " << acct.hasNitro() <<
         "\n\tBadge: " << acct.getBadge() <<
         "\n\tStatus: " << acct.getStatus();
    return sout;
}

void DNode::clear(DNode* node) {
    if (!node)
        return;

    clear(node->_left);
    clear(node->_right);

    node->_account._badge = "";
    node->_account._disc = 0;
    node->_account._nitro = false;
    node->_account._status = "";
    node->_account._username = "";

    node->_numVacant = 0;
    node->_size = 0;
    node->_vacant = false;

    delete node;
    node = nullptr;
}

void DNode::copy(DNode* copy) {
    _vacant = copy->_vacant;
    _numVacant = copy->_numVacant;
    _size = copy->_size;

    if (copy->_left) {
        _left = new DNode(copy->_left->_account);
        _left->copy(copy->_left);
    }
    if (copy->_right) {
        _right = new DNode(copy->_right->_account);
        _right->copy(copy->_right);
    }
}

bool DTree::insertHelper(int discToInsert, Account acctToInsert, DNode* node) {
    bool temp = false;

    if (discToInsert == node->_account._disc)
        return false;

    if (discToInsert > node->_account._disc){

      // go to the right
        if (node->_right){

	  //insert at vacant node
            if (node->_right->isVacant()) {
	        DNode* right = node->_right->_right;
	        DNode* left = node->_right->_left;

	        delete node->_right;

	        node->_right = new DNode(acctToInsert);
	        node->_right->_right = right;
	        node->_right->_left = left;

	        node->_right->_vacant = false;
	        node->_right->_numVacant--;

	        updateSize(node);
	        updateNumVacant(node);
	        if (checkImbalance(node))
		  rebalance(node);

	        return true;
            }
            else {
	      //recursion to the right
                temp =  insertHelper(discToInsert, acctToInsert, node->_right);

                updateSize(node);
                updateNumVacant(node);
                if (checkImbalance(node))
                  rebalance(node);
            }
        }
	//insert new leaf
	else{
            node->_right = new DNode(acctToInsert);

            updateSize(node);
            updateNumVacant(node);
            if (checkImbalance(node))
                rebalance(node);
            return true;
        }
    }
    else{
        // go to the left
        if ( node->_left){
            //if the left is vacant, insert
            if ( node->_left->isVacant()) {
               	DNode* right = node->_left->_right;
		DNode* left = node->_left->_left;

		delete node->_left;

		node->_left = new DNode(acctToInsert);
		node->_left->_right = right;
		node->_left->_left = left;

		node->_left->_vacant = false;
		node->_left->_numVacant--;

		updateSize(node);
		updateNumVacant(node);
		if (checkImbalance(node))
		  rebalance(node);

		return true;
            }
            else {
	      //recursion to the left
                temp =  insertHelper(discToInsert, acctToInsert, node->_left);

                updateSize(node);
                updateNumVacant(node);
                if (checkImbalance(node)) {
                    rebalance(node);
                }
            }
        }

	//insert new leaf
	else{
            node->_left = new DNode(acctToInsert);

            updateSize(node);
            updateNumVacant(node);
            if (checkImbalance(node))
                rebalance(node);

            return true;
        }
    }
    
    return temp;
}

DNode *DNode::retrieve(int disc, DNode* node) {
    DNode* temp;

    //node's disc matches
    if (node && node->_account._disc == disc && !(node->_vacant))
      return node;
  
    //left node matches disc
    else if (node->_left && node->_left->_account._disc == disc && !(node->_left->_vacant))
      return node->_left;

    //right node matches disc
    else if (node->_right && node->_right->_account._disc == disc && !(node->_right->_vacant))
      return node->_right;

    //disc is on left side
    else if (node->_left && disc < node->_account._disc)
      temp = node->retrieve(disc, node->_left);

    //disc is on right side
    else if (node->_right && disc > node->_account._disc)
      temp = node->retrieve(disc, node->_right);

    //disc not in tree
    else
      temp = nullptr;

    return temp;
}

void DNode::print(DNode* nodeToPrint) {
    if (!nodeToPrint || nodeToPrint->isVacant())
        return;

    print(nodeToPrint->_left);

    cout << nodeToPrint->_account << endl;

    print(nodeToPrint->_right);
}

//add nodes to an array from smallest disc to largest
void DNode::rebalance(DNode*& node, DNode* dtreeArray[], int &i) {
    if (!node) {
        return;
    }

    rebalance(node->_left, dtreeArray, i);

    if (!node->isVacant()) {
        node->_size = 1;
        node->_numVacant = 0;
        dtreeArray[i] = node;
        i++;
    }

    rebalance(node->_right, dtreeArray, i);

    if (node->isVacant()) {
        delete node;
    }
}

DNode *DTree::rebuild(DNode* dtreeArray[], int start, int end, DNode*& node) {

    if (start > end)
        return nullptr;
    
    int middle = ((start + end)) / 2;

    node = dtreeArray[middle];

    node->_left = rebuild(dtreeArray, start, middle - 1, node->_left);
    //updateSize(node);
    //updateNumVacant(node);


    node->_right = rebuild(dtreeArray, middle + 1, end, node->_right);
    updateSize(node);
    updateNumVacant(node);

    return node;
}

DNode* DTree::removeHelper(int disc, DNode* node) {
    DNode* temp;

    //node matches the disc
    if (node && node->_account._disc == disc && !(node->_vacant)){
      node->_vacant = true;
      node->_numVacant += 1;
      updateNumVacant(node);
      return node;
    }
    
    //left node matches disc
    else if (node->_left && node->_left->_account._disc == disc && !(node->_left->_vacant)) {
        node->_left->_vacant = true;
        node->_left->_numVacant += 1;
        updateNumVacant(node);
        return node->_left;
    }

        //right node matches disc
    else if (node->_right && node->_right->_account._disc == disc && !(node->_right->_vacant)) {
        node->_right->_vacant = true;
        node->_right->_numVacant += 1;
        updateNumVacant(node);
        return node->_right;
    }

        //disc is on left side
    else if (node->_left && disc < node->_account._disc) {
        temp = removeHelper(disc, node->_left);
        updateNumVacant(node);
    }

        //disc is on right side
    else if (node->_right && disc > node->_account._disc) {
        temp = removeHelper(disc, node->_right);
        updateNumVacant(node);
    }

        //disc not in tree
    else
        return nullptr;

    return temp;
}
