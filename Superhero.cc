#include "Superhero.h"

// Constructor
Superhero::Superhero(std::string* fn, std::string* ln, std::string* hn) {
    firstName = new std::string(*fn);
    lastName = new std::string(*ln);
    heroName = new std::string(*hn);
}

// Copy Constructor
Superhero::Superhero(const Superhero& other) {
    firstName = new std::string(*other.firstName);
    lastName = new std::string(*other.lastName);
    heroName = new std::string(*other.heroName);
}

// Destructor
Superhero::~Superhero() {
    delete firstName;
    delete lastName;
    delete heroName;
}

// Assignment Operator
Superhero& Superhero::operator=(const Superhero& other) {
    if (this == &other) {
        return *this; // Handle self-assignment
    }
    *firstName = *other.firstName;
    *lastName = *other.lastName;
    *heroName = *other.heroName;
    return *this;
}

// Comparison Operator
bool Superhero::operator==(const Superhero& other) const {
    return *firstName == *other.firstName && *lastName == *other.lastName;
}

// Less than Operator
bool Superhero::operator<(const Superhero& other) const {
    if (*lastName != *other.lastName) {
        return *lastName < *other.lastName;
    }
    return *firstName < *other.firstName;
}

// Getter for heroName
std::string Superhero::getHeroName() const {
    return *heroName;
}

// Output Operator
std::ostream& operator<<(std::ostream& os, const Superhero& hero) {
    os << *hero.firstName << " " << *hero.lastName;
    return os;
}
