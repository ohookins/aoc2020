#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

const std::string INPUT_FILENAME = "input.txt";
const std::string PASSWORD_REGEX = "^(\\d+)-(\\d+) ([a-z]): (\\w+)$";

// password policy and corresponding password
struct PasswordEntity
{
    int Min;
    int Max;
    char Letter;
    std::string Password;
};

// storage
std::vector<PasswordEntity> Passwords;

void parsePolicy(const std::string policy)
{
    std::regex policyRegex(PASSWORD_REGEX);
    std::cmatch matchResult;

    // not sure why this insists on a C string instead of a std::string...
    bool didMatch = std::regex_match(policy.c_str(), matchResult, policyRegex);
    if (didMatch)
    {
        PasswordEntity p;

        p.Min = std::stoi(matchResult[1]);
        p.Max = std::stoi(matchResult[2]);
        p.Letter = matchResult[3].str().c_str()[0]; // UGH
        p.Password = matchResult[4];

        Passwords.push_back(p);
    }
}

void readInput()
{
    std::ifstream ifs;
    ifs.open(INPUT_FILENAME);

    // current value to be read
    std::string value;

    while (!ifs.eof())
    {
        char c;
        ifs.get(c);

        // end of current value
        if (c == '\n' || ifs.eof())
        {
            parsePolicy(value);
            value = ""; // reset line value
            continue;
        }

        value.push_back(c);
    }

    ifs.close();
}

void validatePasswordsPart1()
{
    int numValidPasswords = 0;

    for (auto Password : Passwords)
    {
        int actualLetterCount = 0;

        // welp, naming clearly was not great
        for (char Letter : Password.Password)
        {
            if (Letter == Password.Letter) actualLetterCount++;
        }

        if (actualLetterCount >= Password.Min && actualLetterCount <= Password.Max)
        {
            numValidPasswords++;
        }
    }

    std::cout << "Part1 valid passwords is " << numValidPasswords << std::endl;   
}

void validatePasswordsPart2()
{
    int numValidPasswords = 0;

    for (auto Password : Passwords)
    {
        // these are indexed from 1, so adjust them
        int Position1 = Password.Min - 1;
        int Position2 = Password.Max - 1;

        // only one position can be filled, which means effectively XOR
        if (
            Password.Password[Position1] == Password.Letter ^
            Password.Password[Position2] == Password.Letter)
        {
            numValidPasswords++;
        }
    }

    std::cout << "Part2 valid passwords is " << numValidPasswords << std::endl;   
}

int main(int argc, char** argv)
{
    readInput();
    validatePasswordsPart1();
    validatePasswordsPart2();
}