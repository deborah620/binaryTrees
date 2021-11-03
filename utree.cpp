/**
 * CMSC 341 - Spring 2021
 * Project 2 - Binary Trees
 * UserTree.h
 * Implementation for the UTree class.
 */

#include "utree.h"

/**
 * Destructor, deletes all dynamic memory.
 */
UTree::~UTree() {
    clear();
    _root = nullptr;
}

/**
 * Sources a .csv file to populate Account objects and insert them into the UTree.
 * @param infile path to .csv file containing database of accounts
 * @param append true to append to an existing tree structure or false to clear before importing
 */
void UTree::loadData(string infile, bool append) {
    std::ifstream instream(infile);
    string line;
    char delim = ',';
    const int numFields = 5;
    string fields[numFields];

    /* Check to make sure the file was opened */
    if(!instream.is_open()) {
        std::cerr << __FUNCTION__ << ": File " << infile << " could not be opened or located" << endl;
        exit(-1);
    }

    /* Should we append or clear? */
    if(!append) this->clear();

    /* Read in the data from the .csv file and insert into the UTree */
    while(std::getline(instream, line)) {
        std::stringstream buffer(line);

        /* Quick check to make sure each line is formatted correctly */
        int delimCount = 0;
        for(unsigned int c = 0; c < buffer.str().length(); c++) if(buffer.str()[c] == delim) delimCount++;
        if(delimCount != numFields - 1) {
            throw std::invalid_argument("Malformed input file detected - ensure each line contains 5 fields deliminated by a ','");
        }

        /* Populate the account attributes -
         * Each line always has 5 sections of data */
        for(int i = 0; i < numFields; i++) {
            std::getline(buffer, line, delim);
            fields[i] = line;
        }
        Account newAcct = Account(fields[0], std::stoi(fields[1]), std::stoi(fields[2]), fields[3], fields[4]);
        this->insert(newAcct);
    }
}

/**
 * Dynamically allocates a new UNode in the tree and passes insertion into DTree.
 * Should also update heights and detect imbalances in the traversal path after
 * an insertion.
 * @param newAcct Account object to be inserted into the corresponding DTree
 * @return true if the account was inserted, false otherwise
 */
bool UTree::insert(Account newAcct) {
    if (!_root) {
      //inserting the root
        _root = new UNode();
        if (_root->getDTree()->insert(newAcct)){

            updateHeight(_root);
            if (checkImbalance(_root))
                rebalance(_root);

            return true;
        }
        return false;
    }

    //if user already exists
    else if (retrieveUser(newAcct.getUsername(), newAcct.getDiscriminator()))
        return false;

    //insert user if doesn't exist
    else if (insertHelper(newAcct.getUsername(), newAcct, _root)){
        return true;
    }
    else
        return false;
}

bool UTree::insertHelper(string username, Account account, UNode *node) {
    bool temp = false;

    //if username is the root node
    if (node->getDTree()->getUsername() == account.getUsername()) {
        node->getDTree()->insert(account);
        updateHeight(node);
        if (checkImbalance(node))
            rebalance(node);
        return true;
    }
    //if username is greater than the root node
    else if (account.getUsername() > node->getDTree()->getUsername()) {
        //go to the right
        if (node->_right) {
            temp = insertHelper(username, account, node->_right);

            updateHeight(node);
            if (checkImbalance(node))
                rebalance(node);
        }
	//insert node
	else {
            node->_right = new UNode();
            node->_right->getDTree()->insert(account);

            updateHeight(node);
            if (checkImbalance(node))
                rebalance(node);
            return true;
        }
    }
    //if username is less than the root node
    else{
        //go to the left
        if (node->_left) {
            temp = insertHelper(username, account, node->_left);

            updateHeight(node);
            if (checkImbalance(node))
                rebalance(node);
        }
	//insert node
	else {
            node->_left = new UNode();
            node->_left->getDTree()->insert(account);

            updateHeight(node);
            if (checkImbalance(node))
                rebalance(node);
            return true;
        }
    }
        return temp;
}

/**
 * Removes a user with a matching username and discriminator.
 * @param username username to match
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
bool UTree::removeUser(string username, int disc, DNode*& removed) {
    bool remove = false;
    UNode* temp = retrieve(username);
    if (temp) {

        //remove the node from the dtree
        if(temp->getDTree()->remove(disc, removed))
	    remove = true;
      
	//if the dtree is empty, remove it
        if (temp->getDTree()->getNumUsers() <= 0) {
            removeUNode(temp);
        }
    }
    return remove;
}

void UTree::removeUNode(UNode*& node) {
    if (node) {
      //delete root
      if (node == _root && !node->_left && !node->_right){
	delete node;
	node = nullptr;
	_root = nullptr;
	return;
      }
      
      bool isLeft = false;
      UNode *parent = _root;

      if (node != _root) {

	//get the parent
	while (parent->_left != node && parent->_right != node){

	  if (parent->getUsername() > node->getUsername())
	    parent = parent->_left;

	  else if (parent->getUsername() < node->getUsername())
	    parent = parent->_right;
	}
	if (parent->_left == node)
	  isLeft = true;
      }
      
      //if there's a left, go to this function
        if (node->_left) {
            removeUNodeLeft(node, node->_left);

	    updateHeight(node);
	    if (checkImbalance(node))
	      rebalance(node);

	    updateHeight(parent);
	    if (checkImbalance(parent))
	      rebalance(parent);

	    return;
        }
	//if there's a right, that node becomes the root
	else if (node->_right) {
	    *node->_dtree = *node->_right->_dtree;
	    delete node->_right;
	    node->_right = nullptr;

	    updateHeight(node);
	    if (checkImbalance(node))
	      rebalance(node);
	}
	//else the node is a leaf
	else{
            delete node;
	    node = nullptr;
	}
	//reconnect parent to new root
	if (isLeft)
	  parent->_left = node;

	else
	  parent->_right = node;

	updateHeight(node);
	if (checkImbalance(node))
	  rebalance(node);

	updateHeight(parent);
	if (checkImbalance(parent))
	  rebalance(parent);
    }
}

void UTree::removeUNodeLeft(UNode*& node, UNode* nodeX) {
  bool isLeft = false;
  UNode* parent = _root;

  //find parent node
  while (parent->_left != nodeX && parent->_right != nodeX){
    if (parent->getUsername() > nodeX->getUsername())
      parent = parent->_left;
    else if (parent->getUsername() < nodeX->getUsername())
      parent = parent->_right;
  }
  if (parent->_left == nodeX)
    isLeft = true;

  //find largest node in node's left subtree
  if (nodeX->_right){
    nodeX = nodeX->_right;
    removeUNodeLeft(node, nodeX);

    updateHeight(node);
    if (checkImbalance(node))
      rebalance(node);

    updateHeight(parent);
    if (checkImbalance(parent))
      rebalance(parent);

    return;
  }

  *node->_dtree = *nodeX->_dtree;

  //nodeX has a left child, they switch places and the child is deleted
  if (nodeX->_left){
    *nodeX->_dtree = *nodeX->_left->_dtree;
    delete nodeX->_left;
    nodeX->_left = nullptr;

    updateHeight(nodeX);
    if (checkImbalance(nodeX))
      rebalance(nodeX);
    return;
  }
  //nodeX is a leaf, delete it
  else{
    delete nodeX;
    nodeX = nullptr;
  }

  //reconnect parent to new root
  if (isLeft)
    parent->_left = nullptr;
  else
    parent->_right = nullptr;

  updateHeight(node);
  if (checkImbalance(node))
    rebalance(node);

  updateHeight(parent);
  if (checkImbalance(parent))
    rebalance(parent);
}

/**
 * Retrieves a set of users within a UNode.
 * @param username username to match
 * @return UNode with a matching username, nullptr otherwise
 */
UNode* UTree::retrieve(string username) {
    if (_root) {
      //username wanted is the root
        if (_root->getUsername() == username)
            return _root;

        else
	  //get the username
	  return (_root->retrieve(username, _root));
    }
    return nullptr;
}
UNode *UNode::retrieve(string username, UNode* node) {
    UNode* temp;

    //username matches the desired username
    if (node->getDTree()->getUsername() == username)
        return node;

    //left node matches username
    if (node->_left && node->_left->getUsername() == username) {
      return node->_left;
    }

    //right node matches username
    else if (node->_right && node->_right->getUsername() == username) {
      return node->_right;
    }

    //username is on left side
    else if (node->_left && username < node->getUsername())
      temp = node->_left->retrieve(username, node->_left);

    //username is on right side
    else if (node->_right && username > node->getUsername())
      temp = node->_right->retrieve(username, node->_right);

    //username not in tree
    else
      temp = nullptr;

    return temp;
}

/**
 * Retrieves the specified Account within a DNode.
 * @param username username to match
 * @param disc discriminator to match
 * @return DNode with a matching username and discriminator, nullptr otherwise
 */
DNode* UTree::retrieveUser(string username, int disc) {
    if (_root){
      //desired username is the root, so search the root's dtree
        if (_root->getUsername() == username)
            return _root->getDTree()->retrieve(disc);

        UNode* temp = _root->retrieve(username, _root);

	//if the username is in the tree, retreive the dnode from that dtree
        if (temp){
           return temp->getDTree()->retrieve(disc);
        }
    }
    return nullptr;
}

/**
 * Returns the number of users with a specific username.
 * @param username username to match
 * @return number of users with the specified username
 */
int UTree::numUsers(string username) {
    UNode* temp = retrieve(username);
    if (temp){
        return temp->getDTree()->getNumUsers();
    }
    return 0;
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void UTree::clear() {
    if (_root) {
        _root->clear(_root);
    }
}
void UNode::clear(UNode *node) {
    if (!node)
        return;
    clear(node->_left);
    clear(node->_right);

    delete node;
    node = nullptr;
}

/**
 * Prints all accounts' details within every DTree.
 */
void UTree::printUsers() const {
    if (_root) {
        _root->print(_root);
    }
}
void UNode::print(UNode* node) {
    if (node->_left) {
        print(node->_left);
        node->_left->getDTree()->printAccounts();
    }

    if (node->_right) {
        print(node->_right);
        node->_right->getDTree()->printAccounts();
    }
}


/**
 * Dumps the UTree in the '()' notation.
 */
void UTree::dump(UNode* node) const {
    if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getUsername() << ":" << node->getHeight() << ":" << node->getDTree()->getNumUsers();
    dump(node->_right);
    cout << ")";
}

/**
 * Updates the height of the specified node.
 * @param node UNode object in which the height will be updated
 */
void UTree::updateHeight(UNode* node) {
    int left = 0;
    int right = 0;

    if (node){
      //node is a leaf
        if (!node->_left && !node->_right) {
            node->_height = 0;
            return;
        }
	//get the left node height
        if (node->_left)
            left = node->_left->getHeight();

	//get the right node height
        if (node->_right)
            right = node->_right->getHeight();

	//right side is taller
        if (right > left)
            node->_height = right + 1;

	//left side is taller
        else
            node->_height = left + 1;
    }

}

/**
 * Checks for an imbalance, defined by AVL rules, at the specified node.
 * @param node UNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
int UTree::checkImbalance(UNode* node) {
  //start at -1 bec the height of a null child
    int left = -1;
    int right = -1;

    if (node){
      //get the left height
        if (node->_left){
            left = node->_left->_height;
        }
	//right height
        if (node->_right){
            right = node->_right->_height;
        }
	//there's an imbalance
	if (abs(right - left) > 1)
            return true;
    }
    return false;
}

//----------------
/**
 * Begins and manages the rebalance procedure for an AVL tree (pass by reference).
 * @param node UNode object where an imbalance occurred
 */
void UTree::rebalance(UNode*& node) {
  //start at -1 bec the height of a null child
    int right = -1; 
    int left = -1;
    int rightLeft = -1;
    int leftRight = -1;
    int leftLeft = -1;
    int rightRight = -1;

    if (node->_right) {
        right = node->_right->getHeight();

	if (node->_right->_left)
            rightLeft = node->_right->_left->getHeight();

	if (node->_right->_right)
            rightRight = node->_right->_right->getHeight();
    }
    if (node->_left) {
        left = node->_left->getHeight();

	if (node->_left->_left)
            leftLeft = node->_left->_left->getHeight();

	if (node->_left->_right)
            leftRight = node->_left->_right->getHeight();
    }

    //left heavy
    if ((left - right) > 1){
        if (leftLeft > leftRight)
             rightRotation(node);
        else {
            //double rotation
            leftRightRotation(node);
        }
    }
    //right heavy
    else{
        //double rotation
        if (rightLeft > rightRight)
            rightLeftRotation(node);
        else {
            leftRotation(node); 
        }
    }

}

UNode *UTree::rightRotation(UNode*& Z) {
  UNode* parent = _root;
  bool isRoot = false;
  bool isLeft = false;

  if (Z == _root)
    isRoot = true;

  else{
    //find parent node
    while (parent->_left != Z && parent->_right != Z){

      if (parent->getUsername() > Z->getUsername())
	parent = parent->_left;

      else if (parent->getUsername() < Z->getUsername())
	parent = parent->_right;
    }

    if (parent->_left == Z)
      isLeft = true;
  }

  UNode *Y = Z->_left;
  UNode *T2 = Y->_right;

  // Perform rotation
  Y->_right = Z;
  Z->_left = T2;

  //reconnect parent node to new root
  if (isRoot)
    _root = Y;

  else if (isLeft)
    parent->_left = Y;

  else
    parent->_right = Y;

  // Update heights
  updateHeight(Y);
  updateHeight(Z);
  updateHeight(parent);

  // Return new root
  return Y;
}

UNode *UTree::leftRotation(UNode *&Z) {
  UNode* parent = _root;
  bool isRoot = false;
  bool isLeft = false;

  if (Z == _root)
    isRoot = true;

  else{
    //find parent node

    while (parent->_left != Z && parent->_right != Z){

      if (parent->getUsername() > Z->getUsername())
	parent = parent->_left;

      else if (parent->getUsername() < Z->getUsername())
	parent = parent->_right;
    }

    if (parent->_left == Z)
      isLeft = true;
  }

  UNode *Y = Z->_right;
  UNode *T2 = Y->_left;

  // Perform rotation
  Y->_left = Z;
  Z->_right = T2;

  //reconnect parent node to new root
  if (isRoot)
    _root = Y;

  else if (isLeft)
    parent->_left = Y;

  else
    parent->_right = Y;

  // Update heights
  updateHeight(Z);
  updateHeight(Y);
  updateHeight(parent);

  // Return new root
  return Y;
}

UNode *UTree::leftRightRotation(UNode *&Z) {
  UNode* parent = _root;
  bool isRoot = false;
  bool isLeft = false;

  if (Z == _root)
    isRoot = true;

  else{

    //find parent node
    while (parent->_left != Z && parent->_right != Z){

      if (parent->getUsername() > Z->getUsername())
	parent = parent->_left;

      else if (parent->getUsername() < Z->getUsername())
	parent = parent->_right;
    }

    if (parent->_left == Z)
      isLeft = true;
  }

  UNode *Y = Z->_left;
  UNode *X = Y->_right;
  UNode *T2 = X->_left;

  // Perform rotation
  Z->_left = X;
  X->_left = Y;
  Y->_right = T2;

  //reconnect parent node to new root
  if (isRoot)
    _root = Z;

  else if (isLeft)
    parent->_left = Z;

  else
    parent->_right = Z;

  //update heights
  updateHeight(X);
  updateHeight(Y);
  updateHeight(Z);
  updateHeight(parent);

  //do second rotation
  return rightRotation(Z);
}

UNode *UTree::rightLeftRotation(UNode *&Z) {
  UNode* parent = _root;
  bool isRoot = false;
  bool isLeft = false;

  if (Z == _root)
    isRoot = true;

  else{

    //find parent node
    while (parent->_left != Z && parent->_right != Z){

      if (parent->getUsername() > Z->getUsername())
	parent = parent->_left;

      else if (parent->getUsername() < Z->getUsername())
	parent = parent->_right;
    }

    if (parent->_left == Z)
      isLeft = true;
  }

  UNode *Y = Z->_right;
  UNode *X = Y->_left;
  UNode *T2;

  if (X->_right)
    T2 = X->_right;
  else
    T2 = nullptr;

  // Perform rotation
  Z->_right = X;
  X->_right = Y;
  Y->_left = T2;

  //reconnect new root to parent
  if (isRoot)
    _root = Z;

  else if (isLeft)
    parent->_left = Z;

  else
    parent->_right = Z;

  //update heights
  updateHeight(X);
  updateHeight(Y);
  updateHeight(Z);
  updateHeight(parent);

  //do second rotation
  return leftRotation(Z);
}

// -- OR --
/**
 * Begins and manages the rebalance procedure for an AVL tree (returns a pointer).
 * @param node UNode object where an imbalance occurred
 * @return UNode object replacing the unbalanced node's position in the tree
 */
//UTree* UTree::rebalance(UNode* node) {

//}
//----------------
