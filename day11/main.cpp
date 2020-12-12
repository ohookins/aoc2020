#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <numeric>
#include <vector>

const std::string INPUT_FILENAME = "input.txt";

// Add two cells in either dimension to make boundary checking easier.
// I'm having trouble figuring out if the input file is actually 98x98 or 99x99.
const int DIM = 100;

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

int RunSimulation(Layout* Ferry)
{
    bool HaveMadeChanges = false;

    for (int y = 1; y < DIM; y++)
    {
        for (int x = 1; x < DIM; x++)
        {
            // floor, do nothing
            if (Ferry->IsFloor(x, y)) continue;

            int Neighbours = Ferry->CountNeighbours(x, y);

            // Empty seat with no neighbours - fill
            if (!Ferry->IsFilled(x, y) && Neighbours == 0)
            {
                Ferry->Fill(x, y);
                HaveMadeChanges = true;
            }

            // Filled with four or more neighbours - empty
            if (Ferry->IsFilled(x, y) && Neighbours >= 4)
            {
                Ferry->Empty(x, y);
                HaveMadeChanges = true;
            }

            // everything else stays the same
        }
    }

    Ferry->Commit();
    if (HaveMadeChanges) return RunSimulation(Ferry) + 1;
    return 1;
}

int main(int argc, char** argv)
{
    // Storage
    Layout Ferry;

    // Fill all cells except one line around the outside
    int row = 0;
    ReadInput([&Ferry, &row](std::string Line) {
        for (int col = 0; col < Line.size(); col++)
        {
            Ferry.Set(col+1, row+1, Line[col]);
        }
        row++;
    });

    // switch storage pointers since we used the Set interface to
    // it above, which only writes to the new storage location.
    Ferry.Commit();
    int Iterations = RunSimulation(&Ferry);
    std::cout << "Simulation stabilised after " << Iterations << " iterations." << std::endl;
    std::cout << Ferry.OccupiedSeatsCount() << " seats were occupied." << std::endl;
}