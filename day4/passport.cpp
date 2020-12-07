#include "passport.h"

#include <regex>

BirthYearProperty::BirthYearProperty(std::string value)
{
    Year = std::stoi(value);
}

bool BirthYearProperty::IsValid() const
{
    return Year >= 1920 && Year <= 2002;
}

IssueYearProperty::IssueYearProperty(std::string value)
{
    Year = std::stoi(value);
}

bool IssueYearProperty::IsValid() const
{
    return Year >= 2010 && Year <= 2020;
}

ExpirationYearProperty::ExpirationYearProperty(std::string value)
{
    Year = std::stoi(value);
}

bool ExpirationYearProperty::IsValid() const
{
    return Year >= 2020 && Year <= 2030;
}

HeightProperty::HeightProperty(std::string value)
{
    std::basic_regex HeightRE("^(\\d+)(cm|in)$");
    std::cmatch match;

    bool didMatch = std::regex_match(value.c_str(), match, HeightRE);

    // there ARE actually some errors in the input file >:(
    if (!didMatch)
    {
        Height = 0;
        Units = "bad";
        return;
    }

    Height = std::stoi(match[1].str());
    Units = match[2].str();
}

bool HeightProperty::IsValid() const
{
    if (Units == "cm")
    {
        return Height >= 150 && Height <= 193;
    }

    if (Units == "in")
    {
        return Height >= 59 && Height <= 76;
    }

    return false;
}

bool HairColorProperty::IsValid() const
{
    std::basic_regex ColorRE("^#[0-9a-f]{6}$");
    return std::regex_match(Color, ColorRE);
}

bool EyeColorProperty::IsValid() const
{
    std::basic_regex ColorRE("^(amb|blu|brn|gry|grn|hzl|oth)$");
    return std::regex_match(Color, ColorRE);
}

bool PassIDProperty::IsValid() const
{
    std::basic_regex IDRE("^[0-9]{9}$");
    return std::regex_match(ID, IDRE);
}

bool Passport::IsValid(bool strictMode) const
{
    for (auto PropCode : MandatoryProperties)
    {
        if (Properties.count(PropCode) != 1) return false;

        // enables or disables further validations
        if (!strictMode) continue;

        auto Property = Properties.at(PropCode);
        if (!Property->IsValid()) return false;
    }

    return true;
};

void Passport::AddProp(const std::string& Key, const std::string& Value)
{
    PassportProperty* prop;

    // UGH this whole thing is horrible
    if (Key == "byr") prop = new BirthYearProperty(Value);
    if (Key == "iyr") prop = new IssueYearProperty(Value);
    if (Key == "eyr") prop = new ExpirationYearProperty(Value);
    if (Key == "hgt") prop = new HeightProperty(Value);
    if (Key == "hcl") prop = new HairColorProperty(Value);
    if (Key == "ecl") prop = new EyeColorProperty(Value);
    if (Key == "pid") prop = new PassIDProperty(Value);
    if (Key == "cid") return; // does nothing

    Properties.insert({Key, prop});
}