#include <vector>

using namespace std;

//This class originally inherited from the Player class
//But once I added in the splitting functionality
//and each player could hold multiple hands
//the classes had diverged enough that I stopped using inheritance

//I could still use inheritance and make special overloaded methods for the dealer
//These overloaded methods would call the player methods with hand number as 0
//However, the only methods that would be truly duplicated are getHand and clearHand
//The reason is that the dealer's getHandSum could be overridden for a hit on soft 17 rule
//If this is done, then the dealer would not want to use the Player's addCardToHand method
//Since getHand and clearHand are trivial methods, it was acceptable to duplicate them

//If you disagree with my choice here, I'd love to discuss!
//I spent a fair amount of time going back and forth on whether or not to have the Dealer inherit from the Player
class Dealer
{
public:
	//There should only be one dealer, so make it a Singleton
	static Dealer* getInstance();

	//Destructor
	~Dealer();

	//Add a card to the dealer's hand and return the sum of the hand
	int addCardToHand(int card);

	//Return a vector of all the cards in the dealer's hand
	const vector<int> &getHand() const;

	//Return the sum of the hand
	int getHandSum();

	//Clear the vector that represents the hand
	void clearHand();

private:
	//Private constructor for singleton.
	//Initializes m_hand
	Dealer();

	static Dealer* m_instance;

	vector<int> m_hand;
};