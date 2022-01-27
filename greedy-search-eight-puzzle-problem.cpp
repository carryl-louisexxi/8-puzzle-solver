#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>


class Node{ // node class : puzzle state
public:
    std::array<int, 9> puzzle;
    Node* parent;
    int fval, gcost, hcost;
    Node(std::array<int, 9>, Node*, int);
};
Node::Node(std::array<int, 9> puzzle, Node* parent, int gcost) : puzzle(puzzle), parent(parent), gcost(gcost) {}

class Solver{ //solver class: responsible for moving/swapping tiles
private:
    int col = 3, size = 9;
public:
    std::list<Node*> children;
    void expandNode(Node*);
    void moveUp(std::array<int,9>, Node*, int);
    void moveDown(std::array<int,9>, Node*, int);
    void moveRight(std::array<int,9>, Node*, int);
    void moveLeft(std::array<int,9>, Node*, int);
};

void Solver::expandNode(Node* curr){ //expand the
    std::array<int, 9> temp = curr->puzzle;
    int zero;

    for(int i = 0; i < curr->puzzle.size(); i++){
        if(curr->puzzle[i] == 0) zero = i;
    }
    
    moveUp(temp, curr, zero); 
    moveDown(temp, curr, zero);
    moveRight(temp, curr, zero); 
    moveLeft(temp, curr, zero); 
}

void Solver::moveUp(std::array<int, 9> temp, Node *curr, int zero){ //move to up
    if((zero - col) >= 0){

        int data = temp[zero - 3];
        temp[zero - 3] = temp[zero];
        temp[zero] = data;

        Node *child = new Node(temp, curr, curr->gcost+1);
        children.push_back(child);
    }
}

void Solver::moveDown(std::array<int, 9> temp, Node *curr, int zero){ //move to down
    if((zero + col) < size){

        int data = temp[zero + 3];
        temp[zero + 3] = temp[zero];
        temp[zero] = data;

        Node *child = new Node(temp, curr, curr->gcost+1);
        children.push_back(child);
    }
}

void Solver::moveRight(std::array<int, 9> temp, Node *curr, int zero){ //move to right
    if((zero % col) > 0){

        int data = temp[zero -1];
        temp[zero - 1] = temp[zero];
        temp[zero] = data;

        Node *child = new Node(temp, curr, curr->gcost+1);
        children.push_back(child);
    }
}

void Solver::moveLeft(std::array<int, 9> temp, Node *curr, int zero){ //move to left
    if((zero % col) < (col - 1)){

        int data = temp[zero + 1];
        temp[zero + 1] = temp[zero];
        temp[zero] = data;

        Node *child = new Node(temp, curr, curr->gcost+1);
        children.push_back(child);
    }
}

class Puzzle{ //puzzle class : responsible for a* search steps
private:
    int col = 3, size = 9;
    std::array<int, 9> start {7, 2, 4, 5, 0, 6, 8, 3, 1};
    std::array<int, 9> goal {0, 1, 2, 3, 4, 5, 6, 7, 8};
public:
    int manhattan(Node);
    bool contains(std::unordered_multimap<int,Node>, Node, int);
    bool samePuzzle(Node,Node);
    std::list<Node*> pathTrace(std::list<Node*>, Node*);
    void printPuzzle(Node);
    std::list<Node*> process();
};

int Puzzle::manhattan(Node curr){ //evaluating manhattan value
    int distance = 0, m = 0;

    for(int i = 0; i < col; i++){
        for(int j = 0; j < col; j++){
            if(curr.puzzle[m] != 0 && curr.puzzle[m] != goal[m]){
                int val = curr.puzzle[m];      
                distance += abs((round(((double) val - 1) / 3)) - i) + abs((val % 3) -j); //manhattan formula
            }
            m++;
        }
    }
    return distance;
}

bool Puzzle::contains(std::unordered_multimap<int,Node> closelist, Node curr, int hcost){ //checking if the the node is in the closed list
    auto range = closelist.equal_range(hcost);
    for(auto it = range.first; it != range.second; ++it){
        if(samePuzzle(curr, it->second)) return true;
    }  
    return false;
}

bool Puzzle::samePuzzle(Node curr, Node cls){ // checking if the state has same values
    for(int i = 0; i< curr.puzzle.size(); i++){
        if(curr.puzzle[i] != cls.puzzle[i]) return false;
    }
    return true;
}

std::list<Node*> Puzzle::pathTrace(std::list<Node*> path, Node* curr){ // path tracing
    std::cout<<"Path Tracing...."<<std::endl;
    Node *c = curr;
    path.push_front(c);

    while(c->parent != NULL){
        c = c->parent;
        path.push_front(c);
    }
    return path;
}

void Puzzle::printPuzzle(Node n){ // puzzle printing
    int m = 0;
    for(int i= 0; i < col; i++){
        for(int j =0; j < col; j++){
            std::cout<<n.puzzle[m]<<" ";
            m++;
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

class CompareFLevel{ //compare manhattan class: used in openlist for sorting out the lowest hcost
public:
    bool operator()(Node const *curr, Node const *opList) { 
        return curr->hcost > opList->hcost;
    }
};

std::list<Node*> Puzzle::process(){ // a* search process
    std::priority_queue<Node* , std::vector<Node*>, CompareFLevel> openlist;
    std::unordered_multimap<int, Node> closedlist;
    std::list<Node*> path;

    Node *root = new Node(start, NULL, 0);
    root->hcost = manhattan(*root);
    root->fval = root->gcost + root->hcost;
    openlist.push(root); //push the root node to the openlist

    bool goalFound = false;

    while(true){
        Node *currentNode = openlist.top(); //getting the top/lowest openlist value
        openlist.pop();

        closedlist.insert(closedlist.begin(), std::make_pair(currentNode->hcost, *currentNode)); //putting in closed list

        Solver solve;
        solve.expandNode(currentNode); //expand node

        for(auto children: solve.children){

            if(manhattan(*children) == 0){ // goal checking
                std::cout<<"Goal Found!"<<std::endl;
                return pathTrace(path, children); //if goal, return the path
            }

            if(!contains(closedlist, *children, manhattan(*children))){ // closedlist checking: if the generated node has been evaluated
                children->hcost = manhattan(*children);
                children->fval = children->gcost + children->hcost;
                openlist.push(children); //if not in closedlist; push the generated node to the openlist
            }
        }
    }
    return path;
}

int main(){
    Puzzle puzzle;

    std::list<Node*> solution = puzzle.process();
    if(!solution.empty()){
        std::cout<<std::endl;
        for(auto i : solution){
            puzzle.printPuzzle(*i);
        }
    }else{
        std::cout<<"no solution found!";
    }
}