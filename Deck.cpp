#include "Deck.h"

using namespace std;

Deck* Deck::m_instance = 0;

Deck* Deck::getInstance()
{
	if (m_instance == 0)
    {
        m_instance = new Deck();
    }

    return m_instance;
}

Deck::Deck()
{
	m_numberOfDecks = 2;
	m_remainingCards.clear();

	for (int i = 2; i <= 14; i++)
	{
		m_remainingCards.insert(pair<int, int>(i, (m_numberOfDecks * 4)));
	}
}

Deck::~Deck(){}

int Deck::drawCard()
{
	bool keepRunning = true;
	int card;

	while (keepRunning == true)
	{
		//Generates a random number between 2 and 14
		card = rand() % 13 + 2;

		if (m_remainingCards[card] > 0)
		{
			keepRunning = false;
			m_remainingCards[card]--;
		}
	}

	bool deckIsEmpty = true;
	//If we are out of cards, we need to reshuffle
	for (map<int, int>::iterator it = m_remainingCards.begin(); it != m_remainingCards.end(); it++)
	{
		if (it->second > 0)
		{
			deckIsEmpty = false;
			break;
		}
	}

	if (deckIsEmpty == true)
	{
		shuffle();
	}
	
	return card;
}


void Deck::shuffle()
{
	for (int i = 2; i <= 14; i++)
	{
		m_remainingCards[i] = (m_numberOfDecks * 4);
	}
}