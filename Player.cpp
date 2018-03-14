#include "Player.h"

using namespace std;

const int Player::STARTING_MONEY = 1000;

Player::Player()
{
	m_hands.clear();
	m_money = STARTING_MONEY;
	m_currentBet.clear();
	m_atTheTable = true;
}

Player::~Player(){}

int Player::addCardToHand(int card, int handNum)
{
	if (handNum >= m_hands.size())
	{
		return -1;
	}

	//Card mapping: J = 11, Q = 12, K = 13, A = 14
	//Put aces at the back of the hand
	//Will make calculating the sum easier
	if (card == 14)
	{
		m_hands.at(handNum).push_back(card);
	}
	else
	{
		std::vector<int>::iterator it = m_hands.at(handNum).begin();
		m_hands.at(handNum).insert(it,card);
	}

	return getHandSum(handNum);
}

int Player::getNumberOfHands()
{
	return m_hands.size();
}

int Player::addHand()
{
	vector<int> hand;
	m_hands.push_back(hand);
	m_currentBet.push_back(0);
	return (m_hands.size() - 1);
}

const vector<int> &Player::getHand(int handNum) const
{
	return m_hands.at(handNum);
}

int Player::getHandSum(int handNum)
{
	int sum = 0;

	//Calculate the value of the hand
	for (int i = 0; i < m_hands.at(handNum).size(); i++)
	{
		//Add a number card normally
		if (m_hands.at(handNum).at(i) <= 10)
		{
			sum += m_hands.at(handNum).at(i);
		}
		//Add a face card as 10
		else if (m_hands.at(handNum).at(i) <= 13)
		{
			sum += 10;
		}
		//Add an ace as 11
		else
		{
			sum += 11;
		}

		//If the ace as an 11 caused a bust, recalc with ace = 1
		if ((m_hands.at(handNum).at(i) == 14) && (sum > 21))
		{
			sum -= 11;
			sum += 1;
		}

		//Could break here if the player is over 21 to save computational cycles
		//However, a hand will never be big enough for that to matter
		//and players might get bothered by incorrect sums
		//so keep computing for the correct sum
	}

	return sum;
}

void Player::clearHand()
{
	m_hands.clear();
	m_currentBet.clear();
}

bool Player::placeBet(int amount, int handNum)
{
	if (handNum >= m_currentBet.size())
	{
		return -1;
	}

	int totalBet = getTotalBet();

	//Subtract out the bet for this hand, then add the attempted bet
	//This accounts for for doubling down
	totalBet -= m_currentBet.at(handNum);
	totalBet += amount;

	if (totalBet > m_money)
	{
		return false;
	}

	//When a hand is added, a bet is added, so this indexing should not seg fault
	m_currentBet.at(handNum) = amount;
	return true;
}

int Player::getTotalBet()
{
	int sum = 0;
	for (int i = 0; i < m_currentBet.size(); i++)
	{
		sum += m_currentBet.at(i);
	}

	return sum;
}

int Player::getBet(int handNum)
{
	if (handNum >= m_currentBet.size())
	{
		return -1;
	}

	return m_currentBet.at(handNum);
}

void Player::updateBank(bool won, int handNum)
{
	if (won == true)
	{
		if ((m_hands.at(handNum).size() == 2) && (getHandSum(handNum) == 21))
		{
			m_money += (m_currentBet.at(handNum) * 1.5);
		}
		else
		{
			m_money += m_currentBet.at(handNum);
		}
	}
	else
	{
		m_money -= m_currentBet.at(handNum);
	}
}

int Player::getBank()
{
	return m_money;
}

bool Player::getAtTheTable()
{
	return m_atTheTable;
}

void Player::setAtTheTable(bool playing)
{
	m_atTheTable = playing;
}

int Player::splitHand(int handNum)
{
	if (handNum >= m_hands.size())
	{
		return -1;
	}

	//When splitting, add a new hand.
	//Move card from original hand to that hand
	//Delete that card from the original hand
	//The new hand gets the same bet amount as the first hand
	//If the bet can't be placed due to insufficient funds
	//delete the new hand and don't edit the old hand
	//Then, return an error code to main to let it know that splitting is disallowed.
	int newHand = addHand();
	m_hands.at(newHand).push_back(m_hands.at(handNum).at(1));

	if (placeBet(m_currentBet.at(handNum), newHand) == true)
	{
		m_hands.at(handNum).pop_back();
	}
	else
	{
		m_hands.pop_back();
		m_currentBet.pop_back();
		return -2;
	}	
	return newHand;
}

void Player::setPlayerNumber(int num)
{
	m_playerNumber = num;
}

int Player::getPlayerNumber()
{
	return m_playerNumber;
}