#include <map>
#include <stdlib.h>

using namespace std;

//Making this a class, not just a function so that future implementations
//could choose whether or not to use a 2 deck game or a multi-deck game
//They could then swap out the Deck object to decide
//This deck is going to be 2 deck

class Deck
{
public:
	//Implementing the deck as a Singleton, so we only have one deck
	static Deck* getInstance();

	//Destructor
	~Deck();

	//Produces a random card from the deck
	//If the deck has already drawn all of that number/face card
	//create another random number and check that one
	//Keep going until a valid card is drawn
	//Shuffle the deck if out of cards
	virtual int drawCard();

	//Inserts all the cards back in the deck
	//Does this by resetting the map to have all the cards
	virtual void shuffle();

private:
	//Private constructor, as we are using a singleton
	Deck();

	static Deck* m_instance;

	map<int, int> m_remainingCards;
	int m_numberOfDecks;
};