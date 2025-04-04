#ifndef SUPERHERO_H
#define SUPERHERO_H

#include <string>
#include <iostream>

class Superhero {
private:
    std::string* firstName;
    std::string* lastName; 
    std::string* heroName;

public:
    // Constructor
    Superhero(std::string* fn, std::string* ln, std::string* hn);

    // Copy Constructor
    Superhero(const Superhero& other);

    // Destructor
    ~Superhero();

    // Assignment Operator
    Superhero& operator=(const Superhero& other);

    // Comparison Operator
    bool operator==(const Superhero& other) const;

    // Less than Operator
    bool operator<(const Superhero& other) const;

    // Getter for heroName
    std::string getHeroName() const;

    // Friend declaration for output operator
    friend std::ostream& operator<<(std::ostream& os, const Superhero& hero);
};

#endif
