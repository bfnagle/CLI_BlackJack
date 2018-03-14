#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "Player.h"
#include "Deck.h"
#include "Dealer.h"

using namespace std;

int getNumberOfPlayers();

int receiveBet(Player *person);

bool askToPlayHand(Player *player);

void deal(vector<Player*> &players, Dealer *dealer, Deck *deck);

void printPlayerCards(Player *player, int handNum);

void printCardName(int card);

bool playRound(Player *player, Deck *deck, int handNum);

void dealerPlays(Dealer *dealer, Deck *deck);

char getHitOrStay();

char getHitStayDoubleOrSplit(Player *player, Deck *deck, int handNum);

int addCardToPlayer(Player *player, Deck *deck, int handNum);

void calcPayoutAndUpdatePlayer(Player *player, Dealer *dealer);

int main()
{
	vector<Player*> players;
	cout << "Welcome to Blackjack by Brendan!" << endl;

	//Get the number of players in the game, and initialize
	//that number of players, as well as the dealer and deck
	int numPlayers = getNumberOfPlayers();

	for (int i = 0; i < numPlayers; i++)
	{
		Player *player = new Player();
		player->setPlayerNumber(i + 1);
		players.push_back(player);
	}
	Dealer *dealer = Dealer::getInstance();
	Deck *deck = Deck::getInstance();

	//Play rounds until all players have withdrawn
	bool playing = true;

	while (playing == true)
	{
		//Collect a bet amount from each player
		for (int i = 0; i < players.size(); i++)
		{
			if (players.at(i)->getAtTheTable() == true)
			{
				players.at(i)->addHand();
				cout << "Player " << players.at(i)->getPlayerNumber() << "'s turn to bet." << endl << endl;
				receiveBet(players.at(i));
			}
			else
			{
				cout << "Player " << players.at(i)->getPlayerNumber() << " has withdrawn from the game." << endl << endl;
			}
		}

		cout << "\nAll bets have been placed! Time to play." << endl << endl << endl << endl;

		//Deal the cards
		deal(players, dealer, deck);

		//Have the players play their hands
		bool allBusted = true;

		for (int i = 0; i < players.size(); i++)
		{
			if (players.at(i)->getAtTheTable() == true)
			{
				cout << "\nPlayer " << players.at(i)->getPlayerNumber() << ". Your turn!" << endl;
				allBusted = playRound(players.at(i), deck, 0);
				for (int j = 0; j < players.at(i)->getNumberOfHands(); j++)
				{
					cout << "Player " << players.at(i)->getPlayerNumber() << "'s turn is over. End score for hand " << (j + 1) << ": " << players.at(i)->getHandSum(j) << endl << endl;
				}
			}
		}

		//Have the dealer play his/her hand
		if (allBusted == false)
		{
			dealerPlays(dealer, deck);
		}
		else
		{
			cout << "Everyone busted, so the dealer doesn't need to draw." << endl;
		}


		//Go through each player and update their bank and clear than hands
		for (int i = 0; i < players.size(); i++)
		{
			if (players.at(i)->getAtTheTable() == true)
			{
				calcPayoutAndUpdatePlayer(players.at(i), dealer);
			}
		}

		//Reset the dealer and shuffle the deck
		dealer->clearHand();

		deck->shuffle();

		cout << endl << endl;


		//See if anyone still wants to play if they have money or haven't already quit
		playing = false;

		for (int i = 0; i < players.size(); i++)
		{
			if (players.at(i)->getAtTheTable() == true)
			{
				if (askToPlayHand(players.at(i)) == true)
				{
					playing = true;
				}
			}
		}

		//If everyone has withdrawn, stop playing the game
		if (playing == false)
		{
			cout << "No players left. Ending the game." << endl;
		}
	}

	return 1;
}

void calcPayoutAndUpdatePlayer(Player *player, Dealer *dealer)
{
	for (int i = 0; i < player->getNumberOfHands(); i++)
	{
		int playerSum = player->getHandSum(i);

		//If the player busted, they lose their money
		if (playerSum > 21)
		{
			player->updateBank(false, i);
		}
		else
		{
			//If the dealer busted and the player didn't, the player wins
			int dealerSum = dealer->getHandSum();
			if (dealerSum > 21)
			{
				player->updateBank(true, i);
			}
			else
			{
				//If no one busted and the player has a higher score, player wins
				if (playerSum > dealerSum)
				{
					player->updateBank(true, i);
				}
				//If player didnt but and the dealer has a higher score, dealer wins
				else if (playerSum < dealerSum)
				{
					player->updateBank(false, i);
				}
				//Check for dealer 2 card 21 versus player 3+ card. Then dealer wins
				else if ((dealerSum == 21) && (dealer->getHand().size() == 2) && (player->getHand(i).size() > 2))
				{
					player->updateBank(false, i);
				}
				//The remaining case is a push, wherein nothing happens.
			}
		}
	}

	player->clearHand();
	cout << "Player " << player->getPlayerNumber() << "'s remaining cash: $" << player->getBank() << endl;

	if (player->getBank() == 0)
	{
		cout << "\nPlayer " << player->getPlayerNumber() << " is out of money. They have left the table." << endl;
		player->setAtTheTable(false);
	}
}

//This function could be updated to take in playing information
//Then the dealer could, for example, hit on 18 if all players have 19s or 20s
//However, it seems that Vegas doesn't allow this behavior for some reason
//Since Vegas doesn't allow it, I won't either
void dealerPlays(Dealer *dealer, Deck* deck)
{
	//Added sleep statements to improve the feel of playing the game
	//It gives the player a chance to process what the dealer is doing
	//and builds suspense
	this_thread::sleep_for(chrono::seconds(2));

	int handSum = dealer->getHandSum();

	cout << "Dealer playing now." << endl << endl;

	cout << "Dealer's two cards are ";
	printCardName(dealer->getHand().front());
	cout << " and ";
	printCardName(dealer->getHand().at(1));
	cout << ". Score is " << handSum << "." << endl << endl;

	this_thread::sleep_for(chrono::seconds(1));

	//I am going to have the CPU dealer play according to guidelines I found here:
	//http://healy.econ.ohio-state.edu/blackjack/table/dealing.html
	//Basically, the strategy is to hit until you get to 17 or higher.
	//The dealer cannot hit on a soft hand in this case, which I think is a normal rule.
	//It seems like Vegas casinos are split on soft 17 hitting, based on my research
	//If this is wrong, it would be super easy to override the inherited getHandSum function for the Dealer
	//Then if the score is 17 or higher, convert an Ace's value to a 1, instead of an 11.
	while (handSum < 17)
	{
		this_thread::sleep_for(chrono::seconds(1));
		int card = deck->drawCard();
		handSum = dealer->addCardToHand(card);

		cout << "Dealer drew a ";
		printCardName(card);
		cout << ". New score is " << handSum << "." << endl;
		this_thread::sleep_for(chrono::seconds(2));
	}

	if (handSum < 21)
	{
		cout << endl << "Dealer stays." << endl << endl << endl;
	}
	else if (handSum == 21)
	{
		cout << endl <<  "Dealer has 21!" << endl << endl << endl;
	}
	else
	{
		cout << endl << "Dealer busted!" << endl << endl << endl;
	}
}

bool playRound(Player *player, Deck *deck, int handNum)
{
	printPlayerCards(player, handNum);

	bool firstCard = true;
	while (player->getHandSum(handNum) < 21)
	{
		char hitOrStay = 'n';
		if (firstCard == true)
		{
			hitOrStay = getHitStayDoubleOrSplit(player, deck, handNum);
		}
		else
		{
			hitOrStay = getHitOrStay();
		}

		//s indicates staying
		if (hitOrStay == 's')
		{
			return false;
		}
		//h indicates hitting
		else if (hitOrStay == 'h')
		{
			addCardToPlayer(player, deck, handNum);
		}
		//d indicates doubling down
		else if (hitOrStay == 'd')
		{
			int score = addCardToPlayer(player, deck, handNum);
			if (score < 21)
			{
				cout << "Your turn is over now because you doubled down." << endl;
				return false;
			}
		}
		else if (hitOrStay == 'p')
		{
			//Splitting returns a p (for sPlit) if at least one hand does not bust
			return false;
		}
		else if (hitOrStay == 'b')
		{
			//Splitting returns a b if all hands bust
			return true;
		}
	
		firstCard = false;
	}

	//Return true if the hand busted
	if (player->getHandSum(handNum) <= 21)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int addCardToPlayer(Player *player, Deck *deck, int handNum)
{
	int card = deck->drawCard();
	cout << "You drew a ";
	printCardName(card);
	cout << endl;

	int score = player->addCardToHand(card, handNum);
	cout << "Your new hand value is " << score << endl << endl;

	if (score > 21)
	{
		cout << "BUSTED!!!" << endl;
		cout << "Better luck next time!" << endl << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
	else if (score == 21)
	{
		cout << "21! Great work!" << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}

	return score;
}

char getHitStayDoubleOrSplit(Player *player, Deck *deck, int handNum)
{
	while (true)
	{
		cout << "Would you like to hit, stay, double down, or split?" << endl;
		cout << "Shortcut: enter h for hit, s for stay, d for double down, or p for split." << endl;
		string input;
		cin >> input;

		if ((input == "Hit") || (input == "hit") || (input == "h") || (input =="H"))
		{
			return 'h';
		}
		else if ((input == "Stay") || (input == "stay") || (input == "s") || (input == "S"))
		{
			return 's';
		}
		else if ((input == "Double") || (input == "double") || (input == "d") || (input == "D") ||
				 (input == "dd") || (input == "DD") || (input == "Double down") || (input == "double down") ||
				 (input == "Double Down"))
		{
			if (player->placeBet((player->getBet(handNum) * 2), handNum) == true)
			{
				return 'd';
			}
			else
			{
				cout << "Can't double down. Insufficient funds." << endl << endl;
			}
		}
		else if ((input == "Split") || (input == "split") || (input == "p") || (input == "P"))
		{
			if (player->getHand(handNum).front() != player->getHand(handNum).at(1))
			{
				cout << "You cannot split unless your two cards match." << endl << endl;
			}
			else
			{
				//If the player splits, split their hand
				//That function handles the bet allocation too
				int newHandNum = player->splitHand(handNum);

				//splitHand returns an error code of -2 if it couldn't place the bet due to insufficient funds
				if (newHandNum == -2)
				{
					cout << "You cannot split because you cannot double your bet for this hand. Insufficient funds." << endl << endl;
				}
				else
				{
					cout << "Hand split. Playing first hand." << endl << endl;
					this_thread::sleep_for(chrono::seconds(1));

					int card = deck->drawCard();
					player->addCardToHand(card, handNum);
					bool firstSplitBusted = playRound(player, deck, handNum);
					
					card = deck->drawCard();
					player->addCardToHand(card, newHandNum);
					bool secondSplitBusted = playRound(player, deck, newHandNum);

					if ((firstSplitBusted && secondSplitBusted) == true)
					{
						return 'b';
					}
					else
					{
						return 'p';
					}
				}
			}
		}
		else
		{
			cout << "Invalid choice. Please choose to either Hit or Stay." << endl;
		}
	}
}

char getHitOrStay()
{
	while (true)
	{
		cout << "Would you like to hit or stay?" << endl;
		string input;
		cin >> input;

		if ((input == "Hit") || (input == "hit") || (input == "h") || (input =="H"))
		{
			return 'h';
		}
		else if ((input == "Stay") || (input == "stay") || (input == "s") || (input == "S"))
		{
			return 's';
		}
		else
		{
			cout << "Invalid choice. Please choose to either Hit or Stay." << endl;
		}
	}
}

void deal(vector<Player*> &players, Dealer *dealer, Deck *deck)
{
	//Going through and dealing cards the way they would in Vegas.
	//Give one card to earch player, then a second
	//Probably unncessary since this is a computer program, but it's fun

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < players.size(); j++)
		{
			if (players.at(j)->getAtTheTable() == true)
			{
				int card = deck->drawCard();
				players.at(j)->addCardToHand(card, 0);
			}
		}
		int card = deck->drawCard();
		dealer->addCardToHand(card);
	}

	cout << "Dealer's second card is ";
	printCardName(dealer->getHand().at(1));
	cout << endl << endl;

	for (int i = 0; i < players.size(); i++)
	{
		if (players.at(i)->getAtTheTable() == true)
		{
			printPlayerCards(players.at(i), 0);
		}
	}
}

void printPlayerCards(Player *player, int handNum)
{
	cout << "Player " << player->getPlayerNumber() << "'s cards are ";
	printCardName(player->getHand(handNum).front());
	cout << " and ";
	printCardName(player->getHand(handNum).at(1));
	cout << ". Totaling " << player->getHandSum(handNum);
	cout << endl << endl;
}

void printCardName(int card)
{
	if (card <= 10)
	{
		cout << card;
	}
	else
	{
		switch (card)
		{
			case 11:
				cout << "Jack";
				break;
			case 12:
				cout << "Queen";
				break;
			case 13:
				cout << "King";
				break;
			case 14:
				cout << "Ace";
				break;
			default:
				cout << "Unknown error";
		}
	}
}

int receiveBet(Player *player)
{
	int bet = 0;
	bool validValue = false;

	while (validValue == false)
	{
		try
		{
			cout << "Please place a bet." << endl << "$";
			string input;
			cin >> input;
	  		bet = stoi(input);
	  		validValue = player->placeBet(bet, 0);
	  		if (validValue == false)
	  		{
	  			cout << "You can't bet more than you have! Please reduce your bet amount." << endl;
	  		}
	  		if (bet < 0)
	  		{
	  			validValue = false;
	  			cout << "You can't bet against yourself!" << endl;
	  		}
	  		else if (bet == 0)
	  		{
	  			validValue = false;
	  			cout << "You can't play for free!" << endl;
	  		}
		}
		catch(invalid_argument& e)
		{
			cout << "Invalid input. Please enter a number." << endl;
		}
		catch(out_of_range& e)
		{
			cout << "Bet amount is comically large. Please enter a reasonable number." << endl;
		}
		catch(...)
		{
			cout << "Unknown issue with your input. Please try again." << endl;
		}
	}

	cout << "Great. You have placed a bet of $" << bet << endl << endl << endl;

	return bet;
}

bool askToPlayHand(Player *player)
{
	bool playing = true;
	string playChoice = "";
	while (playChoice == "")
	{
		cout << "Would Player " << player->getPlayerNumber() << " like to play another hand?" << endl;
		cin >> playChoice;
		if ((playChoice == "yes") || (playChoice == "Yes") || (playChoice == "y") || (playChoice == "Y"))
		{
			cout << "\nPlayer " << player->getPlayerNumber() << " will play another round!" << endl << endl;
			player->setAtTheTable(true);
			playing = true;
		}
		else if ((playChoice == "no") || (playChoice == "No") || (playChoice == "n") || (playChoice == "N"))
		{
			cout << "\nPlayer " << player->getPlayerNumber() << " has left the table." << endl << endl;
			player->setAtTheTable(false);
			playing = false;
		}
		else
		{
			playChoice = "";
			cout << "Please enter yes or no." << endl;
		}
	}

	return playing;
}

int getNumberOfPlayers()
{
	int numPlayers = 0;
	bool invalidValue = true;

	while (invalidValue == true)
	{
		try
		{
			cout << "How many people will be playing today?" << endl;
			string input;
			cin >> input;
	  		numPlayers = stoi(input);
	  		invalidValue = false;
	  		if (numPlayers < 0)
	  		{
	  			cout << "Negative people can't play! Please try again." << endl;
	  			invalidValue = true;
	  		}
	  		else if (numPlayers == 0)
	  		{
	  			cout << "Can't play with zero people at the table. The dealer will be lonely!" << endl;
	  			invalidValue = true;
	  		}
		}
		catch(invalid_argument& e)
		{
			cout << "Invalid input. Please enter a number." << endl;
		}
		catch(out_of_range& e)
		{
			cout << "Number of players too large. Please enter a reasonable number." << endl;
		}
		catch(...)
		{
			cout << "Unknown issue with your input. Please try again." << endl;
		}
	}

	cout << "\nGreat. This game will have " << numPlayers << " players and 1 dealer!" << endl << endl << endl;

	return numPlayers;
}