#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <regex>
#include <map>
#include <set>

const std::string INPUT_FILENAME = "input.txt";
const std::string TARGET_BAG = "shiny gold";

struct Node
{
    std::string Name;
    std::vector<std::pair<int, Node*>> Children;
    std::vector<Node*> Parents;
};

// store all nodes for later lookup when constructing the graph
std::map<std::string, Node*> NodeMap;

// map of name to children counts and names to resolve once we have everything parsed
std::map<std::string, std::vector<std::pair<int, std::string>>> UnresolvedNodeMap;

// add a single leaf node
// has no dependencies so it is easy
void addLeafNode(std::string Name)
{
    Node* CurrentNode = new Node();
    CurrentNode->Name = Name;
    NodeMap.insert({Name, CurrentNode});
}

// Returns true if the node was successfully added to the NodeMap,
// false otherwise.
bool addBranchNode(std::string Name, std::vector<std::pair<int, std::string>> Children, bool AddToUnresolved = true)
{
    // check that all children are known, otherwise put this node on the
    // UnresolvedNodeList "queue" to be added to the graph later.
    for (const auto& [Count, ChildName] : Children)
    {
        if (NodeMap.count(ChildName) != 1)
        {
            // Child wasn't found, can't put this node and its children
            // into the node list yet.
            // We skip this if we are resolving everything.
            if (AddToUnresolved) UnresolvedNodeMap.insert({Name, Children});
            return false;
        }
    }

    // All children are present - add the node!
    // This also means adding this node to the parent list of
    // each child to facilitate navigation of the graph.
    Node* CurrentNode = new Node();
    CurrentNode->Name = Name;
    for (const auto& [Count, ChildName] : Children)
    {
        Node* ChildNode = NodeMap[ChildName];

        // add up and down relationship pointers
        ChildNode->Parents.push_back(CurrentNode);
        CurrentNode->Children.push_back({Count, ChildNode});
    }

    // finally add this node to the list of resolved nodes
    NodeMap.insert({Name, CurrentNode});
    return true;
}

void parseLine(std::string Line)
{
    // Parse leaf nodes that have no dependencies.
    std::regex LeafNodeRE("^(\\w+ \\w+) bags contain no other bags\\.$");
    std::cmatch cmatch;
    if (std::regex_match(Line.c_str(), cmatch, LeafNodeRE))
    {
        addLeafNode(cmatch[1].str());
        return;
    }

    // Parse "branch" nodes in two parts: the prefix (containing bag)
    // and then the bags contained within. I can't figure out how to retrieve values with ()+
    // so this is the only way for now.
    std::regex ContainingRE("^(\\w+ \\w+) bags contain (.*)$");
    std::regex_match(Line.c_str(), cmatch, ContainingRE);
    
    std::string Name = cmatch[1].str();
    std::string Suffix = cmatch[2].str();

    std::regex BagsRE("((\\d+) (\\w+ \\w+) bags?[,. ]+)");
    std::smatch smatch;
    std::vector<std::pair<int, std::string>> Children;
    while (std::regex_search(Suffix, smatch, BagsRE))
    {
        // 2 & 3 are the submatches of the bag count and two word name of the child bag
        Children.push_back({std::stoi(smatch[2].str()), smatch[3].str()});
        Suffix = smatch.suffix();
    }

    addBranchNode(Name, Children);
}

void readInput()
{
    std::ifstream ifs;
    ifs.open(INPUT_FILENAME);

    if (ifs.fail())
    {
        std::cout << std::filesystem::current_path() << std::endl;
        throw("Failed to open input file");
    }

    while (!ifs.eof())
    {
        std::string Line;
        std::getline(ifs, Line);

        parseLine(Line);
    }
}

// Iteratively resolve "branch" nodes until the UnresolvedNodeMap is empty.
// This is probably a very expensive way to do it.
void resolveBranchNodes()
{
    while (UnresolvedNodeMap.size() > 0)
    {
        for (const auto& [Name, Children] : UnresolvedNodeMap)
        {
            bool success = addBranchNode(Name, Children, false);

            // was inserted to the NodeMap successfully
            if (success)
            {
                // Does this invalidate this for loop iterator???
                // Answer: YES.
                // Should do this better with an explicit iterator but I'm lazy.
                UnresolvedNodeMap.erase(Name);
                break;
            }
        }
    }
}

// set of node names that we've visited in graph traversal, to
// avoid doubling up on all the kinds of bags that could contain
// the target bag.
std::set<std::string> VisitedNodeNames;

// Starting from a given bag name, do a search to find all ancestor
// nodes and return the count. Hooray recursion.
void findContainingBagCount(Node* StartingNode)
{
    for (auto Parent : StartingNode->Parents)
    {
        // add parent to set
        VisitedNodeNames.emplace(Parent->Name);

        // visit all the parents recursively
        findContainingBagCount(Parent);
    }
}

// Starting from a given bag name, do a search to find all child
// nodes and return the total count, multiplied by how many bags
// of each child type this current bag can hold.
int calculateChildBagCount(Node* StartingNode)
{
    // include this bag as it will be needed by the caller's calculation
    int bagCount = 1;

    for (const auto& [ChildCount, ChildNode] : StartingNode->Children)
    {
        bagCount += calculateChildBagCount(ChildNode) * ChildCount;
    }

    return bagCount;
}

int main(int argc, char** argv)
{
    readInput();
    resolveBranchNodes();

    // Find and count all unique ancestors ("containing bags") of this bag.
    findContainingBagCount(NodeMap[TARGET_BAG]);
    std::cout << TARGET_BAG << " bag has " << VisitedNodeNames.size() << " containing bags." << std::endl;

    // Find and count all of the child bags of this bag.
    // This one is different as we need to multiply all of the bag counts up the chain.
    int count = calculateChildBagCount(NodeMap[TARGET_BAG]);
    count--; // don't count the starting bag itself!
    std::cout << TARGET_BAG << " bag contains " << count << " other bags." << std::endl;
}