#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <numeric>
#include <vector>

const std::string INPUT_FILENAME = "input.txt";

// Add two cells in either dimension to make boundary checking easier.
// I'm having trouble figuring out if the input file is actually 98x98 or 99x99.
const int DIM = 101;

enum class Direction
{
    N, NE, E, SE, S, SW, W, NW
};

class Layout
{
public:
    Layout() {
        // Initialize both storages
        for (int i = 0; i < DIM*DIM; i++)
        {
            StorageA.push_back('\0');
            StorageB.push_back('\0');
        }

        CurrentStorage = &StorageA;
        NewStorage = &StorageB;
    }

    // Only count the neighbours in the immediately surrounding 8 positions
    int CountNeighbours(int x, int y)
    {
        // Indexes in the current storage to the surrounding 8
        // positions around the given location.
        std::vector<int> Indices = {
            (y-1)*DIM + x - 1,
            (y-1)*DIM + x,
            (y-1)*DIM + x + 1,
            y*DIM + x - 1,
            y*DIM + x + 1,
            (y+1)*DIM + x - 1,
            (y+1)*DIM + x,
            (y+1)*DIM + x + 1
        };

        // Was previously using std::accumulate here but I think it was a bit
        // too clever and didn't return the right thing. The following is less
        // code anyway.
        int Neighbours = 0;
        for (auto Index : Indices)
        {
            if ((*CurrentStorage)[Index] == '#') Neighbours++;
        }
        return Neighbours;
    }

    // Count the visible neighbours in the surrounding 8 directions, irrespective of
    // distance. If an empty chair is visible first, don't examine further.
    int CountVisibleNeighbours(int x, int y)
    {
        std::vector<Direction> Directions = {
            Direction::N, Direction::NE, Direction::E, Direction::SE,
            Direction::S, Direction::SW, Direction::W, Direction::NW
        };

        int Count = 0;
        for (auto d : Directions)
        {
            if (SeeNeighboursInDirection(x, y, d)) Count++;
        }

        return Count;
    }

    // convenience functions
    // should really check for '.' but I suspect something funky is going on with
    // boundary cells.
    bool IsFloor(int x, int y) const { return Get(x, y) != '#' && Get(x, y) != 'L'; };
    bool IsFilled(int x, int y) const { return Get(x, y) == '#'; };
    
    void Fill(int x, int y) { Set(x, y, '#'); };
    void Empty(int x, int y) { Set(x, y, 'L'); };

    // end the current simulation iteration and swap active storage
    void Commit()
    {
        std::swap(CurrentStorage, NewStorage);

        // copy current storage to new storage so that they
        // are now mirrored for the next set of operations
        for (int i = 0; i < (*CurrentStorage).size(); i++)
        {
            (*NewStorage)[i] = (*CurrentStorage)[i];
        }
    }

    // Set a given cell's contents directly - normally only used by convenience
    // functions or during input reading.
    void Set(const int x, const int y, const char value)
    {
        // don't allow setting the outside rows/columns
        assert(x > 0 && x < DIM);
        assert(y > 0 && y < DIM);

        // This has to set the new value in the new storage
        // to ensure the simulation is consistent across all cells.
        (*NewStorage)[y*DIM + x] = value;
    }

    // debugging
    void PrintLayout() const
    {
        for (int y = 1; y < DIM-1; y++)
        {
            for (int x = 1; x < DIM-1; x++)
            {
                std::cout << (*CurrentStorage)[y*DIM+x];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    int OccupiedSeatsCount() const
    {
        int Count = 0;
        for (auto c : (*CurrentStorage))
        {
            if (c == '#') Count++;
        }
        return Count;
    }

private:
    // Two vectors of DIM*DIM "cells" - one for the current
    // layout, and the other for the next iteration.
    // We swap between them on each iteration.
    std::vector<char> StorageA;
    std::vector<char> StorageB;

    // Pointers to the current layout and next layout
    // so we can make consistent changes.
    std::vector<char>* CurrentStorage;
    std::vector<char>* NewStorage;

    char Get(int x, int y) const
    {
        // don't allow setting the outside rows/columns
        assert(x > 0 && x < DIM);
        assert(y > 0 && y < DIM);

        return (*CurrentStorage)[y*DIM + x];
    }

    bool SeeNeighboursInDirection(const int x, const int y, const Direction LookDirection)
    {
        // "velocities" of travel
        // in retrospect, this is a horrible way of doing it.
        int xVel = 0, yVel = 0;
        switch (LookDirection)
        {
            case Direction::N:
                yVel = -1;
                break;
            case Direction::NW:
                yVel = -1;
                xVel = -1;
                break; 
            case Direction::NE:
                yVel = -1;
                xVel = 1;
                break;
            case Direction::S:
                yVel = 1;
                break;
            case Direction::SW:
                yVel = 1;
                xVel = -1;
                break;
            case Direction::SE:
                yVel = 1;
                xVel = 1;
                break;
            case Direction::W:
                xVel = -1;
                break;
            case Direction::E:
                xVel = 1;
                break;
        }

        // iterate through layout in the view direction until we fall off
        // or see an empty chair or neighbour
        int currentX = x, currentY = y;
        while (true)
        {
            currentX += xVel;
            currentY += yVel;

            // bounds check
            if (currentX <= 0 || currentX >= DIM - 1 || currentY <= 0 || currentY >= DIM - 1) return false;

            char VisibleObject = Get(currentX, currentY);

            // see an empty chair, we don't care about what is beyond it
            if (VisibleObject == 'L') return false;
        
            // see a neighbour
            if (VisibleObject == '#') return true;
        }
    }
};

void ReadInput(std::function<void(std::string)> function)
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
        function(Line);
    }
}

int RunSimulation(Layout* Ferry, bool RelaxedRuleset = false)
{
    int MaxNeighbourTolerance = RelaxedRuleset ? 5 : 4;

    bool HaveMadeChanges = false;

    for (int y = 1; y < DIM; y++)
    {
        for (int x = 1; x < DIM; x++)
        {
            // floor, do nothing
            if (Ferry->IsFloor(x, y)) continue;

            int Neighbours = 0;
            if (RelaxedRuleset) Neighbours = Ferry->CountVisibleNeighbours(x, y);
            else Neighbours = Ferry->CountNeighbours(x, y);

            // Empty seat with no neighbours - fill
            if (!Ferry->IsFilled(x, y) && Neighbours == 0)
            {
                Ferry->Fill(x, y);
                HaveMadeChanges = true;
            }

            // Filled with X or more neighbours - empty
            if (Ferry->IsFilled(x, y) && Neighbours >= MaxNeighbourTolerance)
            {
                Ferry->Empty(x, y);
                HaveMadeChanges = true;
            }

            // everything else stays the same
        }
    }

    Ferry->Commit();
    if (HaveMadeChanges) return RunSimulation(Ferry, RelaxedRuleset) + 1;
    return 1;
}

int main(int argc, char** argv)
{
    // Two temporary storages for the layout since I'm lazy and the memory
    // is mutated after the simulation (could copy but I closed off that
    // path early).
    Layout FerryPart1;
    Layout FerryPart2;

    // Fill all cells except one line around the outside
    int row = 0;
    ReadInput([&FerryPart1, &FerryPart2, &row](std::string Line) {
        for (int col = 0; col < Line.size(); col++)
        {
            FerryPart1.Set(col+1, row+1, Line[col]);
            FerryPart2.Set(col+1, row+1, Line[col]);
        }
        row++;
    });

    // switch storage pointers since we used the Set interface to
    // it above, which only writes to the new storage location.
    FerryPart1.Commit();
    FerryPart2.Commit();

    // Part1
    int Iterations = RunSimulation(&FerryPart1);
    std::cout << "(Part1) Simulation stabilised after " << Iterations << " iterations." << std::endl;
    std::cout << FerryPart1.OccupiedSeatsCount() << " seats were occupied." << std::endl;

    // Part2
    Iterations = RunSimulation(&FerryPart2, true);
    std::cout << std::endl << "(Part2) Simulation stabilised after " << Iterations << " iterations." << std::endl;
    std::cout << FerryPart2.OccupiedSeatsCount() << " seats were occupied." << std::endl;
}