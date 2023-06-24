#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ctime>
#include "FileWriter.h"
#include "DataPuffer.h"

using namespace je;

class Controller{
	protected:
		FileWriter * filewriter;
		CommBuffer<std::array<double,4>> * databuffer;

	public:
		Controller(FileWriter * filewriter, CommBuffer<std::array<double,4>> * databuffer);
		~Controller();

		virtual void PublishValues();

		virtual void run();
};

#endif