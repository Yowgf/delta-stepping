//===----------------------------------------------------------===//
// File purpose: main function (normal execution).
//
// Description: the main function is simply responsible to call the
// init class constructor, and let it do the heavy-lifting in
// processing the arguments and handing them over to the algorithm.
//
// Also, we include some exception catches and error handling.
//===----------------------------------------------------------===//


#include "Interface/init.hpp"

#include <cerrno>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
  try {
    Interface::init(argc, argv);
  }
  catch (std::exception& e) {
    cerr << "Uncaught exception:\n"\
      "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"\
         << e.what() << "\n"\
      "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
  }
    
  return errno;
}
