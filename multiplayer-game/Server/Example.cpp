#include "Example.h"

#include <Common/CommonHeader.h>
#include <Common/MyPacket.h>
#include <Common/Serializer.h>
#include "PacketHandler.h"

#include <Common/Unicode.h>

#include <Windows.h>

void Example::AllDemo()
{
	Example::SerializationDemo();
	Example::UnicodeDemo();
	Example::ProducerConsumerDemo();
}

void Example::SerializationDemo()
{
	std::cout << "Serialization Demo begin\n\n";

	constexpr size_t IterationCount = 2;

	for (size_t i = 0; i < IterationCount; ++i)
	{
		MyPacket clientPacket{};
		clientPacket.myInt = 8457;
		clientPacket.myEnum = MyEnum::eOption2;
		clientPacket.myString = u8"Xin chào thế giới 세계를 향한 Use 不但 而且 for sentences contain both 不但 and 而且";
		clientPacket.myFloats = { -10.0f, 3.14f, 0.0001f, -202.202f };

		MySmallerStruct smaller{};
		for (uint64 i = 0; i < 20; ++i)
		{
			smaller.multipleInts.push_back(i);
		}
		clientPacket.mySmallerStruct = std::move(smaller);

		clientPacket.myIntStringMap.emplace(std::make_pair(213, "Value String One"));
		clientPacket.myIntStringMap.emplace(std::make_pair(-10, "Minus Ten"));

		auto start = now();
		auto buffer = Serialize<MyPacket>(clientPacket);
		auto end = now();
		std::cout << "Serialize time: " << (end - start).count() << "ns.\n";

		auto type = IdentifyPacket(buffer.get());
		if (PacketHandlerMap.count(type))
		{
			// invoke packet handler
			auto& packetHandler = PacketHandlerMap.at(type);
			packetHandler(buffer.get());
		}
	}

	std::cout << "\nSerialization Demo end\n\n";
}

void Example::UnicodeDemo()
{
	std::cout << "Unicode Demo begin\n\n";

	#pragma execution_character_set( "utf-8" )
	// Set console code page to UTF-8 so console known how to interpret string data
	SetConsoleOutputCP(CP_UTF8);

	
	std::string utf8String = "Xin chào thế giới🎀💝🦊";
	uint32 graphemeCounter = 0;
	ForEachGraphemeCluster(utf8String, [&graphemeCounter](std::string grapheme) {
		std::cout << grapheme;
		++graphemeCounter;
	});

	std::cout << "\nNumber of grapheme clusters: " << graphemeCounter << "\n";


	std::cout << "\nUnicode Demo end\n\n";

}



// These global variables (scoped in a namespace) will be used to demonstate the Producer Consumer demo.
namespace ProducerConsumerDemo
{
	std::chrono::steady_clock::time_point clockInit{};		// Marks the timepoint which the producer thread starts to run.

	std::queue<int32> myQueue{};							// The queue in which the producer insert incoming data, and the consumer consume data inserted by the producer.
	std::mutex mtx{};										// Mutex to control the queue data read/write operation.
	std::condition_variable cond_var{};						// Condition variable to help with the waiting of consumer thread until data exists in the queue.

	std::atomic<bool> demoFinished{false};					// The main thread will change to true if the producer has finished and the queue is empty.
}
namespace PCDemo = ProducerConsumerDemo;					// Shorthand namespace name for ease of reading!

void producer(size_t ThreadID)
{
	PCDemo::clockInit = now();

	// These two variables are initialize here to void re-initialize in the loop
	int32 producedValue = 0;
	std::chrono::steady_clock::time_point clockNow{};

	constexpr size_t IterationCount = 30;

	for(size_t i = 0; i < IterationCount; ++i)
	{
		// Simulating producing the value, 
		std::this_thread::sleep_for(std::chrono::milliseconds(100));	// Simulation of incoming data delay as if we receive it from the internet!
		producedValue = i;
		clockNow = now();
		auto elapsedSinceBegin = ((clockNow - PCDemo::clockInit).count()) / 1'000'000;

		std::string s = "[Producer]"  "Produced value: " + std::to_string(producedValue) + " [" + std::to_string(elapsedSinceBegin) + "ms]\n";
		std::cout << s;
		{
			std::unique_lock<std::mutex> lock(PCDemo::mtx);				// Starts the mutex lock
			PCDemo::myQueue.push(producedValue);
			PCDemo::cond_var.notify_all();								// Notify other consumer threads that the data is ready!
		}
	}

	std::cout << "Producer thread stopping...\n";
}

void consumer(size_t ThreadID)
{
	// These two variables are initialize here to void re-initialize in the loop
	int32 consumedValue{};
	std::chrono::steady_clock::time_point clockNow{};

	while (!PCDemo::demoFinished)
	{
		{
			std::unique_lock<std::mutex> lock(PCDemo::mtx);

			//PCDemo::cond_var.wait(lock, []() {return !PCDemo::myQueue.empty(); });
			auto isDataReady = PCDemo::cond_var.wait_for(lock, std::chrono::milliseconds(1000), []() {return !PCDemo::myQueue.empty(); });
			if (isDataReady == false)
			{
				continue;
			}

			consumedValue = PCDemo::myQueue.front();
			PCDemo::myQueue.pop();
		}
		clockNow = now();
		auto elapsedSinceBegin = ((clockNow - PCDemo::clockInit).count()) / 1'000'000;
		std::string s = "[Consumer][ID:" + std::to_string(ThreadID) + "]" + "Consumed: " + std::to_string(consumedValue) + " [" + std::to_string(elapsedSinceBegin) + "ms]\n";
		std::cout << s;

		std::this_thread::sleep_for(std::chrono::milliseconds(700));	// Simulation of calculation delay
	}

	std::string s = "[Consumer][ID:" + std::to_string(ThreadID) + "]" + "Consumer thread stopping...\n";
	std::cout << s;
}

void Example::ProducerConsumerDemo()
{
	std::cout << "ProducerConsumer Demo begin\n\n";


	size_t OriginalThreadID = 1;
	auto ProducerThreadID = 0;
	auto ConsumerThreadID = 0;

	std::jthread consumerThread(consumer, ++ConsumerThreadID);
	std::jthread consumerThread2(consumer, ++ConsumerThreadID);
	std::jthread consumerThread3(consumer, ++ConsumerThreadID);
	std::jthread consumerThread4(consumer, ++ConsumerThreadID);
	std::jthread consumerThread5(consumer, ++ConsumerThreadID);


	std::jthread producerThread(producer, ++ProducerThreadID);


	// We want to wait until the producerThread is joinable
	producerThread.join();
	// Then wait until the queue is empty
	while (true)
	{
		if (PCDemo::myQueue.empty())
		{
			PCDemo::demoFinished = true;	// Signal consumer threads to stop.
			break;
		}
	}

	consumerThread.join();
	consumerThread2.join();
	consumerThread3.join();
	consumerThread4.join();
	consumerThread5.join();
	std::cout << "\nProducerConsumer Demo end\n\n";

}

