#include "Dealer.h"

using namespace std;

Dealer* Dealer::m_instance = 0;

Dealer* Dealer::getInstance()
{
	if (m_instance == 0)
    {
        m_instance = new Dealer();
    }

    return m_instance;
}

Dealer::Dealer()
{
	m_hand.clear();
}

Dealer::~Dealer(){}

int Dealer::addCardToHand(int card)
{
	//Card mapping: J = 11, Q = 12, K = 13, A = 14
	//Put aces at the back of the hand
	//Will make calculating the sum easier
	if (card == 14)
	{
		m_hand.push_back(card);
	}
	else
	{
		std::vector<int>::iterator it = m_hand.begin();
		m_hand.insert(it,card);
	}

	return getHandSum();
}

const vector<int> &Dealer::getHand() const
{
	return m_hand;
}

int Dealer::getHandSum()
{
	int sum = 0;

	//Calculate the value of the hand
	for (int i = 0; i < m_hand.size(); i++)
	{
		//Add a number card normally
		if (m_hand.at(i) <= 10)
		{
			sum += m_hand.at(i);
		}
		//Add a face card as 10
		else if (m_hand.at(i) <= 13)
		{
			sum += 10;
		}
		//Add an ace as 11
		else
		{
			sum += 11;
		}

		//If the ace as an 11 caused a bust, recalc with ace = 1
		if ((m_hand.at(i) == 14) && (sum > 21))
		{
			sum -= 11;
			sum += 1;
		}
	}

	return sum;
}

void Dealer::clearHand()
{
	m_hand.clear();
}