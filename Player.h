#include <vector>

using namespace std;

class Player
{
public:
	//Constructor - Initialize variables
	Player();

	//Destructor  No need to do memory cleanup, as no dynamic memory used
	~Player();

	//Adds a card to the hand
	//Returns the value of the hand
	int addCardToHand(int card, int handNum);

	//Returns the number of hands each player is playing
	int getNumberOfHands();

	//Adds a hand for the player
	//Returns the index of that hand
	int addHand();

	//Returns the current hand of the player
	const vector<int> &getHand(int handNum) const;

	//Returns the current score of the player's hand
	int getHandSum(int handNum);

	//Clears all hands of the player for a new game
	void clearHand();

	//Sets the amount that the player has bet
	//Returns false if the bet is greater than the player's bank
	bool placeBet(int amount, int handNum);

	//Returns the total amount bet across all hands
	int getTotalBet();

	//Returns the amount that the player has bet
	int getBet(int handNum);

	//Updates the amount of money the player has
	void updateBank(bool won, int handNum);

	//Returns the amount of money the player has
	int getBank();

	//Returns whether a player has withdrawn or lost all their money and left the game
	bool getAtTheTable();

	//Sets whether a player has withdrawn or lost all their money and left the game
	void setAtTheTable(bool playing);

	//Move the second card to a separatae hand
	//Returns the index of the newly created hand
	int splitHand(int handNum);

	//Assigns the player a number
	void setPlayerNumber(int num);

	//Returns the player's number
	int getPlayerNumber();

private:
	//Players start with $1000.
	//Could change this to be configureable per game, if desired
	//To do this, take it as an input in main and add it as an argument to the constructor
	static const int STARTING_MONEY;

	vector<vector<int> > m_hands;
	int m_money;
	vector<int> m_currentBet;
	bool m_atTheTable;
	int m_playerNumber;
};