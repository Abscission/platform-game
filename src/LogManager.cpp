#include"LogManager.h"
#include<fstream>

Log::Log(std::string FileName){
	std::ofstream FileStream;
	FileStream.open(FileName, std::ofstream::out);
	FileStream << "This is a place holder top line for log files\n";
	FileStream.close();
	this->FileName = FileName;
}

void Log::Write(std::string String){
	std::ofstream FileStream;
	FileStream.open(FileName, std::ofstream::app);
	FileStream << String << "\n";
	FileStream.close();

}
