#include "utree.h"
#include "dtree.h"

#include <random>

#define NUMACCTS 20
#define RANDDISC (distAcct(rng))

std::mt19937 rng(10);
std::uniform_int_distribution<> distAcct(0, 9999);

class Tester {
public:
    bool testBasicDTreeInsert(DTree& dtree);
    bool testBasicDTreeRemoval(int disc, DTree& dtree);
    bool testDTreeInsertRemove(DTree& dteeRoot);
    bool testSmallDTreeInsert(DTree& dtree);
    bool testBasicDTreeRetrieval(int disc, DTree& dtree);
    bool testOverloadedAssignment(DTree& assign, DTree& dtree);
    bool overload(DNode* lhs, DNode* rhs);
    bool testBalance(DTree& dtree);
    bool balance(DNode* balanceNode);
    bool testRemoveInsert(DTree &dtree);
    bool testBSTProperty(DTree &dtree);
    bool checkBST(DNode *node);

    bool testBasicUTreeInsert(UTree& utree);
    bool testUTreeBalance(UTree& uTree);
    bool balanceUTree(UNode *node);
    bool testUTreeBST(UTree &utree);
    bool checkUTreeBST(UNode *node);
    bool testUTreeRemoveRoot(UTree& utree);
    bool testUTreeRemoval(UTree& utree);
};


bool Tester::testBasicDTreeInsert(DTree& dtree) {
    bool allInserted = true;
    for(int i = 0; i < NUMACCTS; i++) {
        int disc = RANDDISC;
        Account newAcct = Account("", disc, 0, "", "");
        if(!dtree.insert(newAcct)) {
            cout << "Insertion on node " << disc << " did not return true" << endl;
            allInserted = false;
        }
    }
    return allInserted;
}

bool Tester::testSmallDTreeInsert(DTree& dtree) {
    bool allInserted = true;

    Account testAcct = Account("", 16, 0, "", "");
    dtree.insert(testAcct);

    Account test1Acct = Account("", 8, 0, "", "");
    dtree.insert(test1Acct);

    Account test2Acct = Account("", 20, 0, "", "");
    dtree.insert(test2Acct);

    Account test3Acct = Account("", 6, 0, "", "");
    dtree.insert(test3Acct);

    Account test4Acct = Account("", 10, 0, "", "");
    dtree.insert(test4Acct);

    Account test5Acct = Account("", 5, 0, "", "");
    dtree.insert(test5Acct);

    if (!dtree.retrieve(dtree._root->getDiscriminator())) {
        cout << "root not properly inserted" << endl;
        allInserted = false;
    }

    return allInserted;
}

bool Tester::testBasicDTreeRetrieval(int disc, DTree& dtree) {
    bool retrieve = true;
    if (!dtree.retrieve(disc)) {
        retrieve = false;
    }
    return retrieve;
}

bool Tester::testBasicDTreeRemoval(int disc, DTree& dtree) {
    DNode* node;
    bool removal = true;

    if (!dtree.remove(disc, node)) {
        cout << "removal of disc " << disc << " failed" << endl;
        removal = false;
    }

    return removal;
}

bool Tester::testDTreeInsertRemove(DTree& dtreeRoot){
    DNode* removed;
    Account account = Account("", 100, 0, "", "");
    if (!dtreeRoot.insert(account) || !dtreeRoot.remove(dtreeRoot._root->getDiscriminator(), removed))
        return false;
    return true;
}


bool Tester::testOverloadedAssignment(DTree& assign, DTree& dtree) {
    bool deepCopy = true;
    if (!overload(assign._root, dtree._root))
        return false;

    return deepCopy;
}

bool Tester::overload(DNode* lhs, DNode* rhs) {
    bool deepCopy = true;

    if (lhs == rhs || lhs->_account._disc != rhs->_account._disc)
        return false;

    if (rhs->_left)
        deepCopy = overload(lhs->_left, rhs->_left);

    if (rhs->_right)
        deepCopy = overload(lhs->_right, rhs->_right);

    return deepCopy;
}

bool Tester::testBalance(DTree& dtree) {
    if(!balance(dtree._root))
        return false;
    return true;
}

bool Tester::balance(DNode* balanceNode) {
    if (balanceNode) {

        int right = 0;
        int left = 0;

        if (balanceNode->_left)
            left = balanceNode->_left->getSize();
        if (balanceNode->_right)
            right = balanceNode->_right->getSize();

        //check the size of left compared to the right
        if ((left * 0.5) >= right || (right * 0.5) >= left) {
            if (left >= 4 || right >= 4)
                return false;
        }

        if (balanceNode->_left)
            balance(balanceNode->_left);
        if (balanceNode->_right)
            balance(balanceNode->_left);

        return true;
    }
    return false;
}

bool Tester::testRemoveInsert(DTree &dtree) {
    Account testAcct = Account("", 16, 0, "", "");
    dtree.insert(testAcct);

    Account test1Acct = Account("", 8, 0, "", "");
    dtree.insert(test1Acct);
    
    Account test2Acct = Account("", 20, 0, "", "");
    dtree.insert(test2Acct);
    
    Account test3Acct = Account("", 6, 0, "", "");
    dtree.insert(test3Acct);
    
    Account test4Acct = Account("", 10, 0, "", "");
    dtree.insert(test4Acct);
    
    Account test5Acct = Account("", 5, 0, "", "");
    dtree.insert(test5Acct);
    
    //test removing some nodes
    DNode* remove;
    if (!dtree.remove(6, remove) || remove->getDiscriminator() != 6)
        return false;

    if (!dtree.remove(20, remove)|| remove->getDiscriminator() != 20)
        return false;

    if (!dtree.remove(8, remove)|| remove->getDiscriminator() != 8)
        return false;

    //insert 25, should be inserted to root since root was removed
    Account test6Acct = Account("", 9, 0, "", "");
    if (!dtree.insert(test6Acct) || dtree._root->getDiscriminator() != 9)
        return false;
    
    //insert removed node so should work
    if (!dtree.insert(test3Acct))
        return false;

    //inserting but already exists
    if (dtree.insert(test4Acct))
        return false;

    if (!testBalance(dtree))
        return false;

    //trying to retrieve removed node
    if (dtree.retrieve(20))
        return false;

    dtree.remove(10, remove);
    //insert so now tree is imbalanced, check if vacant nodes decrease by 1
    int numVacant = dtree._root->getNumVacant();

    Account test7Acct = Account("", 11, 0, "", "");
    dtree.insert(test7Acct);

    Account test8Acct = Account("", 12, 0, "", "");
    dtree.insert(test8Acct);

    Account test9Acct = Account("", 13, 0, "", "");
    dtree.insert(test9Acct);

    if (dtree._root->getNumVacant() != (numVacant - 1))
        return false;

    return true;
}

bool Tester::testBSTProperty(DTree &dtree) {
    Account new1Acct = Account("Brackle",9550,0,"","This is a status");
    dtree.insert(new1Acct);
    Account new8Acct = Account("Brackle",2725,0,"HypeSquad Brilliance","");
    dtree.insert(new8Acct);
    Account new11Acct = Account("Brackle",4957,0,"Early Supporter","");
    dtree.insert(new11Acct);
    Account new17Acct = Account("Brackle",2724,1,"","recursion(recursion(recursion()))");
    dtree.insert(new17Acct);
    Account new20Acct = Account("Brackle",46,1,"","This is a status");
    dtree.insert(new20Acct);

    //checking to make sure the left is smaller than the right and the root
    if (checkBST(dtree._root)) {
        //making sure the tree is balanced
        if (!dtree.checkImbalance(dtree._root))
            return true;
    }

    return false;
}

bool Tester::checkBST(DNode *node) {

    if (node->_right && node->_left && node->_left->getDiscriminator() < node->getDiscriminator() &&
        node->_left->getDiscriminator() < node->_right->getDiscriminator()) {
        //making sure the tree is balanced
        checkBST(node->_left);
        checkBST(node->_right);
        return true;
    }
    else
        return false;
}

bool Tester::testBasicUTreeInsert(UTree& utree) {
    string dataFile = "accounts.csv";
   
    try {
        utree.loadData(dataFile);
    } catch(std::invalid_argument e) {
        std::cerr << e.what() << endl;
        return false;
    }

    return true;
}

bool Tester::testUTreeBalance(UTree &uTree) {
    if(!balanceUTree(uTree._root))
        return false;
    return true;
}

bool Tester::balanceUTree(UNode* balanceNode) {
    bool isBalanced = true;

    int right = -1;
    int left = -1;

    //check the size of left compared to the right
    if (balanceNode->_left)
        left = balanceNode->_left->getHeight();
    if (balanceNode->_right)
        right = balanceNode->_right->getHeight();

    if (abs(right - left) > 1)
        return false;

    if (balanceNode->_left)
        isBalanced = balanceUTree(balanceNode->_left);
    if (balanceNode->_right)
        isBalanced = balanceUTree(balanceNode->_left);

    return isBalanced;
}

bool Tester::testUTreeBST(UTree &utree) {
    Account new1Acct = Account("Brackle",9550,0,"","This is a status");
    utree.insert(new1Acct);
    Account new2Acct = Account("Kippage",9918,0,"","This is a status");
    utree.insert(new2Acct);
    Account new3Acct = Account("Cinnamon",7713,1,"Subscriber","");
    utree.insert(new3Acct);
    Account new4Acct = Account("Aqua5Seemly",7510,1,"","");
    utree.insert(new4Acct);
    Account new5Acct = Account("Allegator",9408,0,"Subscriber","");
    utree.insert(new5Acct);

    //checking to make sure the left is smaller than the right and the root
    if (checkUTreeBST(utree._root)) {
        //making sure the tree is balanced
        if (!utree.checkImbalance(utree._root))
            return true;
    }

    return false;
}

bool Tester::checkUTreeBST(UNode *node) {
    if (node->_right && node->_left && node->_left->getUsername() < node->getUsername() &&
        node->_left->getUsername() < node->_right->getUsername()) {
        //making sure the tree is balanced
        checkUTreeBST(node->_left);
        checkUTreeBST(node->_right);
        return true;
    }
    else
        return false;
}

bool Tester::testUTreeRemoveRoot(UTree& utree){
  DNode* remove;

  Account newAcct = Account("Aqua",3922,1,"Subscriber","");
  utree.insert(newAcct);

  //remove the root and it's a leaf
  utree.removeUser("Aqua",3922, remove);
  if (utree.retrieve("Aqua"))
    return false;
  return true;
}

bool Tester::testUTreeRemoval(UTree& utree) {
  Account newAcct = Account("Aqua5Seemly",3922,1,"Subscriber","");
  utree.insert(newAcct);
  Account new1Acct = Account("Brackle",9550,0,"","This is a status");
  utree.insert(new1Acct);
  Account new2Acct = Account("Capstan",4962,1,"Subscriber","proj2 :100:");
  utree.insert(new2Acct);
  Account new3Acct = Account("Capstan",7383,1,"","");
  utree.insert(new3Acct);
  Account new4Acct = Account("Cinnamon",5319,0,"Subscriber","");
  utree.insert(new4Acct);
  Account new5Acct = Account("Capstan",604,0,"","recursion(recursion(recursion()))");
  utree.insert(new5Acct);
  Account new6Acct = Account("Cinnamon",1180,0,"HypeSquad Brilliance","");
  utree.insert(new6Acct);
  Account new7Acct = Account("Pika",6130,0,"","");
  utree.insert(new7Acct);
  Account new8Acct = Account("Brackle",2725,0,"HypeSquad Brilliance","");
  utree.insert(new8Acct);
  Account new9Acct = Account("Aqua5Seemly",7300,0,"","");
  utree.insert(new9Acct);
  Account new10Acct = Account("Sulkyreal",8550,0,"Subscriber","");
  utree.insert(new10Acct);
  Account new11Acct = Account("A",7300,0,"","");
  utree.insert(new11Acct);

  DNode* removedNode;
  //remove username with a left and right node and a left grandchild
  utree.removeUser("Brackle", 9550, removedNode);
  utree.removeUser("Brackle", 2725, removedNode);
  
  if (utree.retrieve("Brackle"))
    return false;
  
  //remove username with a left and right node
  utree.removeUser("Aqua5Seemly", 7300, removedNode);
  utree.removeUser("Aqua5Seemly", 3922, removedNode);

  //tests retrieve
  if (utree.retrieve("Aqua5Seemly"))
    return false;
  
  //remove username with a right and no left node
  utree.removeUser("A", 7300, removedNode);

  //tests retrieve
  if (utree.retrieve("A"))
    return false;

  //remove username leaf
  utree.removeUser("Capstan",4962, removedNode);
  utree.removeUser("Capstan",7383,removedNode);
  utree.removeUser( "Capstan",604,removedNode);

  //tests retrieve
  if (utree.retrieve("Capstan"))
    return false;

  //make sure tree is balanced
  if (utree.checkImbalance(utree._root))
    return false;

  //remove root node, two right traversals to get to nodeX
  Account new12Acct = Account("D",7300,0,"","");
  utree.insert(new12Acct);
  Account new13Acct = Account("Q",7300,0,"","");
  utree.insert(new13Acct);
  Account new14Acct = Account("E",7300,0,"","");
  utree.insert(new14Acct);
  Account new15Acct = Account("F",7300,0,"","");
  utree.insert(new15Acct);
  utree.removeUser( "Pika",6130,removedNode);

  //tests retrieve
  if (utree.retrieve("Pika"))
    return false;
  
  return true;
}

int main() {
    Tester tester;

    /* Basic dtree tests */
    DTree dtree;

    cout << "Testing DTree insertion...";
    if(tester.testBasicDTreeInsert(dtree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "Resulting DTree:" << endl;
    dtree.dump();
    cout << endl;

    cout << "Testing DTree removal" << endl;
    Account newAcct = Account("",207,0,"","");
    dtree.insert(newAcct);
    if(tester.testBasicDTreeRemoval(207, dtree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "Testing DTree retrieval" << endl;
    Account new1Acct = Account("",2987,0,"","");
    dtree.insert(new1Acct);
    if(tester.testBasicDTreeRetrieval(2987, dtree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "Testing DTree retrieval of vacant node" << endl;
    if(!(tester.testBasicDTreeRetrieval(207, dtree))) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "Testing DTree retrieval, node far down tree" << endl;
    Account new2Acct = Account("",883,0,"","");
    dtree.insert(new2Acct);
    if(tester.testBasicDTreeRetrieval(883, dtree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "Test DTree, insert and removal of root" << endl;
    DTree dtreeRoot;
    if(tester.testDTreeInsertRemove(dtreeRoot)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "Testing DTree retrieval, farthest right node" << endl;
    Account farRight = Account("",8319,0,"","");
    dtree.insert(farRight);
    if(tester.testBasicDTreeRetrieval(8319, dtree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "Testing DTree print accounts, with 1 vacant node" << endl;
    dtree.printAccounts();

     //testing balance
    cout << "\nTesting if tree is balanced, using dtree" << endl;
    if (tester.testBalance(dtree)){
        cout << "test passed" << endl;
    }
    else
        cout << "test failed" << endl;

    //dtree1 is a smaller insertion
    DTree dtree1;
    cout << "Testing a smaller DTree insertion" << endl;
    if(tester.testSmallDTreeInsert(dtree1)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }


    cout << "Testing a smaller DTree removal" << endl;
    if(tester.testBasicDTreeRemoval(20, dtree1)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "Testing smaller DTree print accounts, with 1 vacant node" << endl;
    dtree1.printAccounts();

    dtree1.dump();

    cout << "Testing overloaded assignment operator" << endl;
    DTree assign;
    assign = dtree1;
    if (tester.testOverloadedAssignment(assign, dtree1)){
        cout << "test passed" << endl;
    }
    else
        cout << "test failed" << endl;

    //test overload assignment operator that already has value
    cout << "Testing overloaded assignment operator, has previous value" << endl;
    DTree assign2;
    Account testAcct = Account("", 16, 0, "", "");
    assign2.insert(testAcct);
    assign2 = dtree1;
    if (tester.testOverloadedAssignment(assign2, dtree1)){
        cout << "test passed" << endl;
    }
    else
        cout << "test failed" << endl;

    //testing balance
    cout << "Testing if tree is balanced, using dtree1" << endl;
    if (tester.testBalance(dtree1)){
        cout << "test passed" << endl;
    }
    else
        cout << "test failed" << endl;

    //testing removal and reinsert
    cout << "Testing removal then reinsert" << endl;
    DTree removeInsert;
    if (tester.testRemoveInsert(removeInsert))
        cout << "test passed" << endl;
    else
        cout << "test failed" << endl;

    //testing DTree BST property
    cout << "Testing Dtree, BST proptery maintained" << endl;
    DTree dtreeBST;
    if (tester.testBSTProperty(dtreeBST))
        cout << "test passed" << endl;
    else
        cout << "test failed" << endl;


    /* Basic UTree tests */
    UTree utree;

    cout << "\n\nTesting UTree insertion...";
    if(tester.testBasicUTreeInsert(utree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    cout << "print accounts in UTree" << endl;
    utree.printUsers();

    cout << "Testing balanced UTree" << endl;
    if(tester.testUTreeBalance(utree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    //testing UTree BST property
    cout << "Testing UTree, BST proptery maintained" << endl;
    UTree utreeBST;
    if (tester.testUTreeBST(utreeBST))
        cout << "test passed" << endl;
    else
        cout << "test failed" << endl;

    cout << "Testing utree removal functionality of root node" << endl;
    UTree utree1;
    if (tester.testUTreeRemoveRoot(utree1))
      cout << "test passed" << endl;
    else
      cout << "test failed" << endl;

    cout << "Testing removal functionality and some other utree properties" << endl;
    UTree utree2;
    if (tester.testUTreeRemoval(utree2))
      cout << "test passed" << endl;
    else
        cout << "test failed" << endl;

    cout << "Resulting UTree:" << endl;

    utree.dump();
    cout << endl;

    return 0;
}
