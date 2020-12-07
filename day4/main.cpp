#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <regex>
#include <string>

#include "passport.h"

const std::string INPUT_FILENAME = "input.txt";
const std::string BREAK_TOKEN = "BREAK";
const std::string PROPERTY_REGEX = "^([a-z]{3}):(\\S+)$";

// "token" vector.
// I'm cheating a bit here by using a special string to represent
// the space between passports. I don't like parsing very much :(
std::vector<std::string> Tokens;

// this probably is unnecessary
std::vector<Passport> Passports;

void tokenize(std::string Line)
{
    std::string currentToken = "";

    for (char c : Line)
    {
        // separator - don't expect one at the beginning of the line.
        // push the token we have so far
        if (c == ' ')
        {
            Tokens.push_back(std::move(currentToken));
            currentToken = "";
            continue;
        }

        currentToken.push_back(c);
    }

    // push anything remaining
    if (currentToken != "")
    {
        Tokens.push_back(std::move(currentToken));
    }
}

void readInput()
{
    std::ifstream ifs;
    ifs.open(INPUT_FILENAME);

    while (!ifs.eof())
    {
        std::string Line;
        std::getline(ifs, Line);

        // End of file, finish and push the current token,
        // add a break token for convenience and fall through.
        if (ifs.eof())
        {
            tokenize(Line);
            Tokens.push_back(BREAK_TOKEN);
            return;
        }

        // empty line - end of the passport so push a break
        if (Line == "")
        {
            Tokens.push_back(BREAK_TOKEN);
            continue;
        }

        // not end of file or break, so tokenize the line
        tokenize(Line);
    }
}

// Parse the tokens which represent properties of a passport, into
// the passports themselves.
void parsePassports()
{
    std::regex PropRE(PROPERTY_REGEX);

    Passport currentPassport;
    for (auto Token : Tokens)
    {
        // assume we don't get a break without a previous passport
        if (Token == BREAK_TOKEN)
        {
            Passports.push_back(std::move(currentPassport));
            Passport currentPassport; // re-construct
            continue;
        }

        // use regular expression to split the property key from value
        std::cmatch match;
        std::regex_match(Token.c_str(), match, PropRE);

        currentPassport.AddProp(match[1].str(), match[2].str());
    }
}

void validatePassports()
{
    int numLooselyValidPassports = 0;
    int numStrictlyValidPassports = 0;

    for (auto CurrentPassport : Passports)
    {
        CurrentPassport.IsValid(false) && numLooselyValidPassports++;
        CurrentPassport.IsValid(true) && numStrictlyValidPassports++;
    }

    std::cout << "Number of non-strict valid passports is: " << numLooselyValidPassports << std::endl;
    std::cout << "Number of strictly valid passports is: " << numStrictlyValidPassports << std::endl;
}

int main(int argc, char** argv)
{
    readInput();
    parsePassports();
    validatePassports();
}