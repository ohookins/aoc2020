#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

const std::string INPUT_FILENAME = "input.txt";
const char SPACE = '.';
const char TREE = '#';

// vector of vectors of chars for the terrain
// which should hopefully not making indexed access too painful
std::vector<std::vector<char> > Terrain;

void readInput()
{
    std::ifstream ifs;
    ifs.open(INPUT_FILENAME);

    // current line to be read
    std::vector<char> TerrainLine;

    while (!ifs.eof())
    {
        char c;
        ifs.get(c);

        // end of current value
        if (c == '\n' || ifs.eof())
        {
            Terrain.push_back(std::move(TerrainLine));
            TerrainLine.clear(); // re-initialize for next line
            continue;
        }

        TerrainLine.push_back(c);
    }

    ifs.close();
}

int calculateTreeCollisions(const int RightSpeed, const int DownSpeed)
{
    int x = 0, y = 0;
    int treeCollisions = 0;

    // width of the input terrain, which we use with modulo to wrap around
    int width = Terrain[0].size();
    int length = Terrain.size();

    // Loop over terrain. We could move more than one unit down per iteration
    // so we can't use an iterator.
    while (true)
    {
        // check to see if we've fallen off the end of the terrain
        if (y >= length) break;

        char currentLocationContents = Terrain[y][x % width];

        if (currentLocationContents == TREE)
        {
            treeCollisions++;
        }

        // move by the direction speeds for the next loop
        x += RightSpeed;
        y += DownSpeed;
    }

    return treeCollisions;
}

int main(int argc, char** argv)
{
    readInput();

    // Part one
    int answerOne = calculateTreeCollisions(3, 1);
    std::cout << "Tree collisions in first part: " << answerOne << std::endl;

    // Part two
    int answerTwo = calculateTreeCollisions(1, 1);
    int answerThree = calculateTreeCollisions(5, 1);
    int answerFour = calculateTreeCollisions(7, 1);
    int answerFive = calculateTreeCollisions(1, 2);
    int collisionProduct = answerOne * answerTwo * answerThree * answerFour * answerFive;

    std::cout << "Remaining path collisions: " <<
        answerTwo << ", " << answerThree << ", " <<
        answerFour << ", " << answerFive << std::endl;
    std::cout << "Product of collisions: " << collisionProduct << std::endl;
}