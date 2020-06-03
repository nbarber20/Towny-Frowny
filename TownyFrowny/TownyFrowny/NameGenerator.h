#pragma once
#include "rapidcsv.h"
#include <string>
class NameGenerator {
public:
	NameGenerator() {
		rapidcsv::Document doc("Data/Names.csv", rapidcsv::LabelParams(0, -1));
		fnames = doc.GetColumn<std::string>("First Name");
		lnames = doc.GetColumn<std::string>("Last Name");	
	};
	static NameGenerator& Instance()
	{
		static NameGenerator INSTANCE;
		return INSTANCE;
	}
	std::string GenName() {
		srand(time(NULL));
		return fnames[rand() % (fnames.size() - 1)] + " " + lnames[rand() % (lnames.size() - 1)];
	}
	std::string GenNameFirstName() {
		srand(time(NULL));
		return fnames[rand() % (fnames.size() - 1)];
	}
private:
	std::vector<std::string> fnames;
	std::vector<std::string> lnames;
};