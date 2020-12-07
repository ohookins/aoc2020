#include <map>
#include <string>
#include <vector>

class PassportProperty
{
public:
    virtual bool IsValid() const = 0;
    virtual std::string Type() const = 0;
};

class BirthYearProperty : public PassportProperty
{
public:
    BirthYearProperty(std::string value);
    bool IsValid() const override;
    std::string Type() const override { return "byr"; };

private:
    int Year;
};

class IssueYearProperty : public PassportProperty
{
public:
    IssueYearProperty(std::string value);
    bool IsValid() const override;
    std::string Type() const override { return "iyr"; };

private:
    int Year;
};

class ExpirationYearProperty : public PassportProperty
{
public:
    ExpirationYearProperty(std::string value);
    bool IsValid() const override;
    std::string Type() const override { return "eyr"; };

private:
    int Year;
};

class HeightProperty : public PassportProperty
{
public:
    HeightProperty(std::string value);
    bool IsValid() const override;
    std::string Type() const override { return "hgt"; };

private:
    int Height;
    std::string Units;
};

class HairColorProperty : public PassportProperty
{
public:
    HairColorProperty(std::string value) : Color(value) {};
    bool IsValid() const override;
    std::string Type() const override { return "hcl"; };

private:
    std::string Color;
};

class EyeColorProperty : public PassportProperty
{
public:
    EyeColorProperty(std::string value) : Color(value) {};
    bool IsValid() const override;
    std::string Type() const override { return "ecl"; };

private:
    std::string Color;
};

class PassIDProperty : public PassportProperty
{
    public:
    PassIDProperty(std::string value) : ID(value) {};
    bool IsValid() const override;
    std::string Type() const override { return "pid"; };

private:
    std::string ID;
};

class Passport
{
private:
    std::vector<std::string> MandatoryProperties;

public:
    Passport();

    // This doesn't feel right. I want to make a set or vector of
    // properties but can't think of an elegant way to evaluate them
    // all and figure out the end result of whether we have all of them
    // and they are all valid. Maybe a list of missing properties that
    // we take away from after looking at each one? It would ideally
    // be determined at construction time but that doesn't work with
    // how we evaluate tokens earlier in the program.
    //
    // I also made a big mess here and realised too late I'd need pointers
    // in this map (due to polymorphic functions) and paid zero attention
    // to memory lifetime. Nothing is deleting properties.
    std::map<std::string, PassportProperty*> Properties;

    bool IsValid(const bool strictMode) const;

    void AddProp(const std::string& Key, const std::string& Value);
};