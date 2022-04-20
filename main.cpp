#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono>

using namespace std;

// MapNode implemented as red/black tree node
struct MapNode {
    string state; // Name of state
    int cases; // Total cases in state from cases.csv
    bool color; // 1 is red, 0 is black
    MapNode* left; // Pointer to left child
    MapNode* right; // Pointer to right child
    MapNode* parent; // Pointer to parent
public:
    MapNode(string state, int cases); // Constructor, initializes node with data, pointers to nullptr, and color to red
};

MapNode::MapNode(string state, int cases) {
    this->state = state;
    this->cases = cases;
    this->left = nullptr;
    this->right = nullptr;
    this->parent = nullptr;
    this->color = 1;
}

// Map implemented as red/black tree
class Map {
    MapNode* root; // Root of tree
public:
    int totalCases;

    Map() { root = nullptr; totalCases = 0; } // Constructor, initializes root to nullptr and total cases to 0

    void rotateLeft(MapNode* &root, MapNode* &newNode); // Perform a left rotation to balance tree, used in balance()
    void rotateRight(MapNode* &root, MapNode* &newNode); // Perform a right rotation to balance tree, used in balance()
    void balance(MapNode* &root, MapNode* &newNode); // Balance the tree, called after insertion of a new node/vertex
    void insert(string state, int cases); // Insert vertex into tree/map, sorting by state name
    int getCases(string state); // Return the number of cases in a given state
    bool contains(string state, int cases); // Check if tree/map contains a state already, if it does, add cases to existing node, otherwise, continue insert method
};

// Recursive helper function to check if state exists in map already
bool containsHelper(MapNode* root, string state, int cases) {
    if(root == nullptr)
        return false;

    if(state == root->state) {
        root->cases += cases;
        return true;
    }
    else if(state < root->state)
        return containsHelper(root->left, state, cases);
    else
        return containsHelper(root->right, state, cases);

}

bool Map::contains(string state, int cases) {
    bool result = containsHelper(this->root, state, cases);
    if(result)
        this->totalCases += cases;
    return result;
}

// Recursive helper function to insert vertex into map
MapNode* insertHelper(MapNode* root, MapNode* newNode) {
    if(root == nullptr)
        return newNode;

    if(newNode->state < root->state) {
        root->left = insertHelper(root->left, newNode);
        root->left->parent = root;
    }

    else if(newNode->state > root->state) {
        root->right = insertHelper(root->right, newNode);
        root->right->parent = root;
    }

    return root;
}

void Map::rotateLeft(MapNode* &root, MapNode* &newNode) {
    MapNode* newNode_right = newNode->right;

    newNode->right = newNode_right->left;

    if(newNode->right != nullptr)
        newNode->right->parent = newNode;

    newNode_right->parent = newNode->parent;

    if(newNode->parent == nullptr)
        root = newNode_right;

    else if(newNode == newNode->parent->left)
        newNode->parent->left = newNode_right;

    else
        newNode->parent->right = newNode_right;

    newNode_right->left = newNode;
    newNode->parent = newNode_right;
}

void Map::rotateRight(MapNode* &root, MapNode* &newNode) {
    MapNode* newNode_left = newNode->left;

    newNode->left = newNode_left->right;

    if(newNode->left != nullptr)
        newNode->left->parent = newNode;

    newNode_left->parent = newNode->parent;

    if(newNode->parent == nullptr)
        root = newNode_left;

    else if(newNode == newNode->parent->left)
        newNode->parent->left = newNode_left;

    else
        newNode->parent->right = newNode_left;

    newNode_left->right = newNode;
    newNode->parent = newNode_left;
}

void Map::balance(MapNode* &root, MapNode* &newNode) {
    MapNode* newNode_parent = nullptr;
    MapNode* newNode_grand_parent = nullptr;

    while((newNode != root) && (newNode->color != 0) && (newNode->parent != nullptr) && (newNode->parent->color == 1)) {
        newNode_parent = newNode->parent;
        newNode_grand_parent = newNode->parent->parent;

        if(newNode_parent == newNode_grand_parent->left) {
            MapNode* newNode_uncle = newNode_grand_parent->right;

            if(newNode_uncle != nullptr && newNode_uncle->color == 1) {
                newNode_grand_parent->color = 1;
                newNode_parent->color = 0;
                newNode_uncle->color = 0;
                newNode = newNode_grand_parent;
            }
            else {
                if(newNode == newNode_parent->right) {
                    rotateLeft(root, newNode_parent);
                    newNode = newNode_parent;
                    newNode_parent = newNode->parent;
                }

                rotateRight(root, newNode_grand_parent);
                swap(newNode_parent->color, newNode_grand_parent->color);
                newNode = newNode_parent;
            }
        }
        else {
            MapNode* newNode_uncle = newNode_grand_parent->left;

            if((newNode_uncle != nullptr) && (newNode_uncle->color == 1)) {
                newNode_grand_parent->color = 1;
                newNode_parent->color = 0;
                newNode_uncle->color = 0;
                newNode = newNode_grand_parent;
            }
            else {
                if (newNode == newNode_parent->left) {
                    rotateRight(root, newNode_parent);
                    newNode = newNode_parent;
                    newNode_parent = newNode->parent;
                }

                rotateLeft(root, newNode_grand_parent);
                swap(newNode_parent->color, newNode_grand_parent->color);
                newNode = newNode_parent;
            }
        }
    }
    root->color = 0;
}

void Map::insert(string state, int cases) {
    if(contains(state, cases)) // Will add cases to existing state node
        return;
    MapNode* newNode = new MapNode(state, cases);
    root = insertHelper(root, newNode);
    this->totalCases += cases;
    balance(root, newNode);
}

// Recursive helper function to return cases in a given state in the map
int getCasesHelper(MapNode* &root, string state) {
    if(state == root->state)
        return root->cases;
    else if(state < root->state)
        return getCasesHelper(root->left, state);
    else
        return getCasesHelper(root->right, state);

    cout << "Invalid state." << endl;
    return -1;
}

int Map::getCases(string state) {
    return getCasesHelper(this->root, state);
}

class StackNode {
public:
    string state; // Stores state of vertex
    int cases; // Stores number of cases in state

    StackNode() { this->state = ""; this->cases = -1; } // Default constructor for stack array creation with default values
    StackNode(string state, int cases) { this->state = state; this->cases = cases; } // Constructor, initializes state and number of cases of vertex
    void operator=(StackNode* other) { this->state = other->state; this->cases = other->cases; } // Copy constructor for push function
};

class Stack {
    int topIndex; // Index of top vertex in array, initialized to -1 before any vertices are pushed into stack
    StackNode* arr = new StackNode[712000]; // Num vertices: 711650 < 712000
public:
    int totalCases = 0;

    Stack() { topIndex = -1; } // Constructor, initializes topIndex to -1
    ~Stack() { delete[] arr; } // Destructor, deletes array from memory

    void push(string state, int cases); // Add vertex to top of stack
    void pop(); // Remove top vertex from stack
    StackNode* top(); // Return top vertex in stack
    int size() { return topIndex + 2; } // Return size of stack
    bool isEmpty() { return topIndex < 0; } // Return whether stack is empty or not
};

void Stack::push(string state, int cases) {
    if(this->size() < 712000) {
        this->topIndex++;
        this->arr[topIndex] = new StackNode(state, cases);
        this->totalCases += cases;
    }
    // Will fail to push if stack is full (this shouldn't happen, but is implemented just in case)
}

void Stack::pop() {
    if(this->size() > 0) {
        this->arr[topIndex] = new StackNode("Empty", -1);
        this->topIndex--;
    }
    // Will only pop if stack is not empty
}

StackNode* Stack::top() {
    if(topIndex > -1) {
        return &this->arr[topIndex];
    }
    else {
        return new StackNode("Failed", -1);
    }
    // Will return -1 if stack is empty (this shouldn't happen, but is implemented just in case)
}
sf::Color heatIntensity(int stateCases, int totalCases){//function to output a sfml color to be displayed
    double weight = (double) stateCases / totalCases; //find a weight
    unsigned char red,blue,green;
    if(weight > 0.05){//make the color a darker red if the weight is more than 5%
        blue = 0;
        green = 0;
        red = ((1-weight)*.3) * 255;//just some formula to create a color, can be adjusted
    }
    else{//make the color a lighter red if the weight is less than 5%
        red = 255;
        blue = ((1-weight * 20)) * 255;
        green = ((1-weight * 20)) * 255;//just some formula to create a color, can be adjusted
    }
    return {red,blue,green};
}
int main(){
    const int width = 1762;//constant map width to help with sfml adjustments
    const int height = 1271;//as above
    const int STATES = 50;
    sf::Sprite heatMapSprite;//sprite to hold the current desired map output
    sf::Sprite mapSprite;//sprite to hold the original blank map
    sf::Texture mapTexture;//texture to load images into
    mapTexture.loadFromFile("images/BWMap.png");
    mapSprite.setTexture(mapTexture);//load the texture into the sprite
    heatMapSprite.setTexture(mapTexture);
    vector<string> stateStrings = {"Alabama","Alaska","Arizona","Arkansas","California","Colorado","Connecticut"
    ,"Delaware","Florida","Georgia","Hawaii","Idaho","Illinois","Indiana","Iowa","Kansas","Kentucky","Louisiana"
    ,"Maine","Maryland","Massachusetts","Michigan","Minnesota","Mississippi","Missouri","Montana","Nebraska"
    ,"Nevada","New Hampshire","New Jersey","New Mexico","New York","North Carolina","North Dakota","Ohio"
    ,"Oklahoma","Oregon","Pennsylvania","Rhode Island","South Carolina","South Dakota","Tennessee","Texas"
    ,"Utah","Vermont","Virginia","Washington","West Virginia","Wisconsin","Wyoming"};
    vector<pair<int,int>> stateLocations = {{1156,625},{85,752},{299,550},{955,585},{69,323}
            ,{493,424},{1543,289},{1499,389},{1199,756},{1235,614},{473,905}
            ,{288,108},{1029,362},{1143,382},{878,332},{710,466},{1114,468},{982,717}
            ,{1563,72},{1398,396},{1539,247},{1040,184},{850,144},{1065,635}
            ,{908,448},{357,111},{657,362},{181,343},{1551,166},{1502,331}
            ,{464,561},{1360,197},{1287,503},{664,141},{1223,351},{675,567}
            ,{87,174},{1343,313},{1590,287},{1315,592},{662,255},{1094,547}
            ,{537,584},{339,371},{1505,171},{1295,426},{134,76},{1313,394}
            ,{979,217},{455,279}};//holds found locations so that individual state sprites line up with the map

    sf::RenderTexture renderTexture;//2d unseen render for adding to the heatmap sprite
    renderTexture.create(width,height);//designate the rendertexture with a width and height
    sf::RenderWindow window(sf::VideoMode(width,height), "Covid-19 Heatmap");//window where the GUI is displayed
    while(window.isOpen()){//gui loop, ends when the window is closed
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left) {
                if((event.mouseButton.x > 707 && event.mouseButton.x < 909) && (event.mouseButton.y > 995 && event.mouseButton.y < 1088)) {
                    //on left click within the "Stack" button region:
                    Stack s;//create stack to store data in
                    ifstream casesFile("cases.csv");//create an input stream for data to be read from

                    string line;
                    getline(casesFile, line); // Skip header line

                    if(casesFile.is_open()) {
                        while(getline(casesFile, line)) {//read needed data and skip unwanted data line by line
                            istringstream stream(line);
                            string skip;
                            getline(stream, skip, ',');
                            getline(stream, skip, ',');

                            string state;
                            getline(stream, state, ',');

                            getline(stream, skip, ',');

                            string cases;
                            getline(stream, cases, ',');

                            getline(stream, skip);

                            s.push(state, stoi(cases));//add current line to the stack
                        }
                    }
                    else
                        cout << "Error opening cases.csv, please rerun the program and try again." << endl;
                    casesFile.close();
                    renderTexture.clear();//clear the current drawing render so it can be re-drawn
                    sf::Texture newMap;//texture to load in the stack version of the map
                    newMap.loadFromFile("images/BWMapStack.png");//stack version has a timer and red stack button
                    sf::Sprite newMapSprite;
                    newMapSprite.setTexture(newMap);
                    renderTexture.draw(newMapSprite);//draw the new map to the render
                    int totalCases = s.totalCases;//get total cases for usage later
                    vector<int> stateCases(50,0);
                    int stackSize = s.size();
                    auto start = chrono::high_resolution_clock::now();//start microsecond timer
                    /* Find the total number of cases in each state by popping off each data entry and adding it
                     * to the correct state total.
                     */
                    for(int i = 0; i < stackSize -1; i++) {
                        for(int j = 0; j < stateStrings.size(); j++){
                            if(stateStrings[j] == s.top()->state) {
                                stateCases[j] += s.top()->cases;
                                break;
                            }
                        }
                        s.pop();
                    }
                    auto stop = chrono::high_resolution_clock::now();//end microsecond timer
                    for (int i = 0; i < STATES; i++) {//loop to find and apply the new weighted color to each state's sprite
                        string stateString = "images/state" + to_string(i) + ".png";
                        sf::Texture tempTexture;
                        tempTexture.loadFromFile(stateString);//load the state texture from the image files
                        sf::Sprite tempSprite;
                        tempSprite.setTexture(tempTexture);//set the state sprite
                        tempSprite.setColor(heatIntensity(stateCases[i], totalCases));//set the state sprite's color
                        tempSprite.setPosition(stateLocations[i].first, stateLocations[i].second);
                        renderTexture.draw(tempSprite);//render texture flips pixels
                        heatMapSprite.setTexture(renderTexture.getTexture());
                        heatMapSprite.setTextureRect(sf::IntRect(0, height, width, -height));//flip texture back
                        window.draw(heatMapSprite);//draw the updated map sprite
                        window.display();//display current sprite
                    }

                    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);//compute the difference in time
                    sf::Font font;//font for creating text
                    font.loadFromFile("Cave-Story.ttf");
                    sf::Text timer(to_string(duration.count()),font);//text for displaying the timer
                    timer.setFillColor({0,0,0});
                    timer.setPosition(818,1113);//found position
                    renderTexture.draw(timer);//draw the new timer to the render
                    heatMapSprite.setTexture(renderTexture.getTexture());
                    heatMapSprite.setTextureRect(sf::IntRect(0, height, width, -height));
                }
                else if((event.mouseButton.x > 920 && event.mouseButton.x < 1122) && (event.mouseButton.y > 995 && event.mouseButton.y < 1088)) {
                    //On left click of the "Map" button
                    Map m;//create map object

                    ifstream casesFile("cases.csv");//as in the stack section

                    string line;
                    getline(casesFile, line); // Skip header line

                    if(casesFile.is_open()) {
                        while(getline(casesFile, line)) {
                            istringstream stream(line);
                            string skip;
                            getline(stream, skip, ',');
                            getline(stream, skip, ',');

                            string state;
                            getline(stream, state, ',');

                            getline(stream, skip, ',');

                            string cases;
                            getline(stream, cases, ',');

                            getline(stream, skip);

                            m.insert(state, stoi(cases));
                        }
                    }
                    else
                        cout << "Error opening cases.csv, please rerun the program and try again." << endl;
                    int totalCases = m.totalCases;
                    vector<int> stateCases(50);
                    auto start = chrono::high_resolution_clock::now();
                    /* Get number of cases in each state by using the map's getCases function
                     * since the map uses a balanced tree data can efficiently be grabbed
                     */
                    for(int i = 0; i < stateCases.size(); i++){
                        stateCases[i] = m.getCases(stateStrings[i]);
                    }
                    auto stop = chrono::high_resolution_clock::now();
                    renderTexture.clear();
                    sf::Texture newMap;
                    newMap.loadFromFile("images/BWMapMap.png");
                    sf::Sprite newMapSprite;
                    newMapSprite.setTexture(newMap);
                    renderTexture.draw(newMapSprite);
                    for (int i = 0; i < STATES; i++) {
                        string imgString = "images/state" + to_string(i) + ".png";
                        sf::Texture tempTexture;
                        tempTexture.loadFromFile(imgString);
                        sf::Sprite tempSprite;
                        tempSprite.setTexture(tempTexture);
                        tempSprite.setColor(heatIntensity(stateCases[i], totalCases));
                        tempSprite.setPosition(stateLocations[i].first, stateLocations[i].second);
                        renderTexture.draw(tempSprite);
                        heatMapSprite.setTexture(renderTexture.getTexture());
                        heatMapSprite.setTextureRect(sf::IntRect(0, height, width, -height));
                        window.draw(heatMapSprite);
                        window.display();
                    }

                    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
                    sf::Font font;
                    font.loadFromFile("Cave-Story.ttf");
                    sf::Text timer(to_string(duration.count()),font);
                    timer.setFillColor({0,0,0});
                    timer.setPosition(818,1113);
                    renderTexture.draw(timer);
                    heatMapSprite.setTexture(renderTexture.getTexture());
                    heatMapSprite.setTextureRect(sf::IntRect(0, height, width, -height));
                }
            }
            else if(event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Right) {
                heatMapSprite = mapSprite;//reload a blank map into the heatmap sprite on right click
            }
            else if(event.type == sf::Event::Closed)//end loop and program if the window is closed
                window.close();
        }
        window.clear();//clear the window so it can be redrawn
        window.draw(heatMapSprite);//draw whatever is in the heatMapSprite to the window
        window.display();//display the current view of the window
    }
    return 0;
}