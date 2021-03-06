//include needed libraries
#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <vector>
#include <cstdlib>
#include <random>
#include <string>
#include <Windows.h>

using namespace std;
//using random numbers from <random> since the rand(time(NULL)) method
//didn't work in the threads well
default_random_engine generator;
uniform_int_distribution<int> itemNumberDistribution(1, 100);

mutex theMutex;//make mutex
//item class
class item {
public:

	string itemID;//name of item
	string producedBy;//producer which made it
	
	//constructor
	item(string itemIDIn, string producedByIn) {
		itemID = itemIDIn;
		producedBy = producedByIn;
	}

	//getter for ID (even though I made it all public)
	string getID() {
		return this->itemID;
	}
	
	string getProducer() {//getter for Producer
		return this->producedBy;
	}
};


class Producer {//producer class
public:

	string prodName;//name of producer
	string itemType;//type of item made
	vector<item> tempVec;//to initialze &sharedVec
	vector<item>& sharedVec = tempVec;//reference to shared vector

	//contructor
	Producer(string prodNameIn, string itemMadeIn, vector<item>& vec) 
		:prodName(prodNameIn), itemType(itemMadeIn), sharedVec(vec){
	}
	
	//put new item into shared vector
	void produce() {

		//generate new item below using a concacated string
		//as the "random"ness in the item with a random number
		//generator
		item itemMade = item(this->itemType + " " + 
			to_string(itemNumberDistribution(generator)) , this->prodName);
		//lock mutex in critical section
		theMutex.lock();
		//put new item at end of shared vector
		sharedVec.push_back(itemMade);
		//critical section over- unlock mutex
		theMutex.unlock();
	}
	//this part makes a thread of the produce() function
	//allows for multithreading
	thread produceThread() {
		return thread([=] {produce(); });
	}
};

//consumer class
class Consumer {
public:
	string consName;//name of consumer
	vector<item> tempVec;//vectors same as in Producer()
	vector<item>& sharedVec = tempVec;

	//constructor
	Consumer(string consNameIn, vector<item>& vec)
		:consName(consNameIn), sharedVec(vec) {
	}

	
	void consume() {//consume function- consumes last item
		//shared vector

		//lock mutex for ccritical section
		theMutex.lock();
		//print info about last item in array
		cout << consName << " just consumed the item '" <<
			sharedVec[sharedVec.size() - 1].getID() <<
			"' which was made by " <<
			sharedVec[sharedVec.size() - 1].getProducer()<<endl;
		//remove last item in array
		sharedVec.pop_back();
		//unlock mutex now that critical section is over
		theMutex.unlock();
	}

	//this makes a thread of the consume() function
	thread consumeThread() {
		return thread([=] {consume(); });
	}

};


int main() {

	vector<item> itemStack;//LIFO shared vector shared data structure

	//4 producer objects initialized
	Producer Producer1("Producer1", "Sock",itemStack);
	Producer Producer2("Producer2", "Shoe", itemStack);
	Producer Producer3("Producer3", "Mitten", itemStack);
	Producer Producer4("Producer4", "Watch", itemStack);

	//3 consumer objects initialized
	Consumer Consumer1("Consumer1", itemStack);
	Consumer Consumer2("Consumer2", itemStack);
	Consumer Consumer3("Consumer3", itemStack);

	//infinite loop created
	while (true){
		//threads to produce items (for each producer)
		thread Prod1Thread = Producer1.produceThread();
		thread Prod2Thread = Producer2.produceThread();
		thread Prod3Thread = Producer3.produceThread();
		thread Prod4Thread = Producer4.produceThread();
		//threads to consume items (for each consumer)
		thread Cons1Thread = Consumer1.consumeThread();
		thread Cons2Thread = Consumer2.consumeThread();
		thread Cons3Thread = Consumer3.consumeThread();

		//threads joined (ensure completion)
		Prod1Thread.join();
		Prod2Thread.join();
		Prod3Thread.join();
		Prod4Thread.join();

		Cons1Thread.join();
		Cons2Thread.join();
		Cons3Thread.join();

		//three second pause to view outputs
		Sleep(3000);
	}
	

	return 0;
}