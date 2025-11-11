
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Record.h"
#include "AVLTree.h"

typedef struct node *Node;
struct node {
    Record rec;
    Node   left;
    Node   right;
    int    height;
};

struct tree {
    Node    root;
    int     (*compare)(Record, Record);
};

////////////////////////////////////////////////////////////////////////
// Auxiliary functions

static void doTreeFree(Node n, bool freeRecords);
static Node newNode(Record rec);
static Record doTreeSearch(Tree t, Node n, Record rec);

////////////////////////////////////////////////////////////////////////

static Node newNode(Record rec) {
    Node n = malloc(sizeof(*n));
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    n->rec = rec;
    n->left = NULL;
    n->right = NULL;
    n->height = 0;
    return n;
}

////////////////////////////////////////////////////////////////////////

Tree TreeNew(int (*compare)(Record, Record)) {
    Tree t = malloc(sizeof(*t));
    if (t == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    t->root = NULL;
    t->compare = compare;
    return t;
}

////////////////////////////////////////////////////////////////////////

void TreeFree(Tree t, bool freeRecords) {
    doTreeFree(t->root, freeRecords);
    free(t);
}

static void doTreeFree(Node n, bool freeRecords) {
    if (n != NULL) {
        doTreeFree(n->left, freeRecords);
        doTreeFree(n->right, freeRecords);
        if (freeRecords) {
            RecordFree(n->rec);
        }
        free(n);
    }
}

////////////////////////////////////////////////////////////////////////

Record TreeSearch(Tree t, Record rec) {
    return doTreeSearch(t, t->root, rec);
}

static Record doTreeSearch(Tree t, Node n, Record rec) {
    if (n == NULL) {
        return NULL;
    }

    int cmp = t->compare(rec, n->rec);
    if (cmp < 0) {
        return doTreeSearch(t, n->left, rec);
    } else if (cmp > 0) {
        return doTreeSearch(t, n->right, rec);
    } else {
        return n->rec;
    }
}


////////////////////////////////////////////////////////////////////////
/* IMPORTANT: 
   Do NOT modify the code above this line. 
   You must not modify the 'node' and 'tree' structures defined above.
   You must not modify the functions defined above.
*/
////////////////////////////////////////////////////////////////////////
// Auxiliary functions
static Node doTreeInsert(Tree t, Node n, Record rec, bool *res);
static Node rotateLeft(Node n);
static Node rotateRight(Node n);
static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l);
static void doTreeNext(Tree t, Node n, Record r, Record *temp);
////////////////////////////////////////////////////////////////////////

bool TreeInsert(Tree t, Record rec) {
    // TODO: Complete this function
    bool res = false; // If the record was inserted
    t->root = doTreeInsert(t, t->root, rec, &res);
    return res;
}

static Node doTreeInsert(Tree t, Node n, Record rec, bool *res) {
    if (n == NULL) { //Base case
        *res = true; // Means insert successfully
        return newNode(rec); 
    }
    int cmp = t->compare(rec, n->rec); // To compare the (want)insert record and the record in tree
    if (cmp < 0) { // Do the recursion
        n->left = doTreeInsert(t, n->left, rec, res);
    } else if (cmp > 0) {
        n->right = doTreeInsert(t, n->right, rec, res);
    } else { //If insert node is already in the tree
        *res = false; // *res still need to be fasle because there can not contain two completely records
        return n; // No need to do the recursion, just return n instead of calling the doTreeInsert function
    }

    int LeftHeight = -1; //Create two ints to express n->left/right->height, and also seen the NULL node has -1 height
    int RightHeight = -1;

    if (n->left != NULL) { // If not NULL node, Left/RightHeight can gets the correct height. If NULL, it still stay -1
        LeftHeight = n->left->height;
    } 
    if (n->right != NULL) {
        RightHeight = n->right->height;
    }
    
    if (abs(LeftHeight - RightHeight) > 0) { // Method to update the node height after insertion before entering rotation
        n->height = n->height + 1; // If(n->left && n->right)is NULL, after insertion, n->height should increse 1
    }                              // If only n->left NULL or n->right NULL, after insertion, abs value=0!>0 so node maintains its height

    if (LeftHeight - RightHeight > 1) { // Not balence for AVL tree, should rotate to keep balence(make the middle node to be the root node)
        int cmpleft = t->compare(rec, n->left->rec); // To make sure the inserted node is in the left or right side of n->left
        if (cmpleft > 0) {
            n->left = rotateLeft(n->left);
        }
        n = rotateRight(n);
        n->height = 1; // After rotation, the three rotated nodes' height should be updated
        n->left->height = 0; // By analysis, in AVL tree insertion, the heights of the three nodes only has this situation 
        n->right->height = 0; 
    } else if (RightHeight - LeftHeight > 1) {
        int cmpright = t->compare(rec, n->right->rec); // To make sure the inserted node is in the left or right side of n->right
        if (cmpright < 0) {
            n->right = rotateRight(n->right);
        }
        n = rotateLeft(n);
        n->height = 1;
        n->left->height = 0;
        n->right->height = 0;
    }
    return n;
}

static Node rotateLeft(Node n) { // Help to do the left rotation but not contain height updating
    Node nodeR = n->right;
    n->right = nodeR->left;
    nodeR->left = n;
    return nodeR;
}

static Node rotateRight(Node n) { // Help to do the right rotation but not contain height updating
    Node nodeL = n->left;
    n->left = nodeL->right;
    nodeL->right = n;
    return nodeL;
}
////////////////////////////////////////////////////////////////////////

List TreeSearchBetween(Tree t, Record lower, Record upper) {
    // TODO: Complete this function
    List l = ListNew();
    doTreeSearchBetween(t, t->root, lower, upper, l);
    return l;
}

static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l) {
    // TODO: Complete this function
    if (n == NULL){ return; } //Base case
    int cmp1 = t->compare(lower, n->rec); //This can compare by FlightNumber, DepartureAirport and by Timea
    int cmp2 = t->compare(upper, n->rec);
    if (cmp1 <= 0) {
        doTreeSearchBetween(t, n->left, lower, upper, l); //Follow the LNR, which can gets the list out in ordered
        if (cmp2 >= 0) {
            ListAppend(l, n->rec); //N if it statisfies >= lower and <= upper then it will be appended into List l
            doTreeSearchBetween(t, n->right, lower, upper, l); //R
        }
    } else {
            doTreeSearchBetween(t, n->right, lower, upper, l); //> upper
    }

}
////////////////////////////////////////////////////////////////////////

Record TreeNext(Tree t, Record r) {
    // TODO: Complete this function
    Record temp = NULL; // If find next flight, temp will be the next flight
    doTreeNext(t, t->root, r, &temp);
    return temp;
}

static void doTreeNext(Tree t, Node n, Record r, Record *temp) {
    if (n == NULL) {  // Base case
        return;  // Void function return nothing
    }
    int cmp = t->compare(r, n->rec);
    if (cmp < 0) {
        *temp = n->rec; // n->rec > r and to be the temporary next flight of r, then go n->left to do the recursion
        doTreeNext(t, n->left, r, temp);
    } else if (cmp > 0){ // If r > n->rec, temp will maintain its value until find cmp < 0
        doTreeNext(t, n->right, r, temp);
    } else {
        *temp = n->rec; // If we find the exactly same record as r, let temp be n->rec and then returned in TreeNext function
    }
}