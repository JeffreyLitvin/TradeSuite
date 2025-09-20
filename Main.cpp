#include <iostream>
#include <cxxopts/cxxopts.hpp>

int main(int argc, char *argv[])
{
    cxxopts::Options options("TradeEntry", "Testing how cxxopts works");
    options.add_options()
        ("f,file", "File name", cxxopts::value<std::string>())
        ("h,help", "Print usage")
        ;
    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
     if (result.count("file"))
     {
         std::string f = result["file"].as<std::string>();
         std::cout << "File given is " << f << std::endl;
     }
     else
     {
         std::cout << "File argument required!" << std::endl;
         exit(0);
     }

    std::cout << "Test done" <<  std::endl;
}
