#include "robot_diagrams_0.0.hpp"

#include <iostream>
#include <fstream>
#include <string>

bool add_element(rob_diag::Robot, std::string line)
{
  // TODO!
  return true;
}

void draw_robot(rob_diag::Robot robot, std::string filename)
{
  // TODO!
}

bool draw_robot(const char* filename)
{
  std::string file_string(filename);
  if (file_string.size() < 6 || file_string.substr(file_string.size() - 6, 6) != ".robot")
  {
    std::cerr << filename << " is not a valid .robot filename." << std::endl;
    return false;
  }
  std::string file_base = file_string.substr(0, file_string.size() - 6);
  std::string file_ext = file_string.substr(0, file_string.size() - 6);

  std::string line;
  std::ifstream robot_config (filename);
  if (robot_config.is_open())
  {
    rob_diag::Robot robot;
    while ( getline (robot_config,line) )
    {
      if (!add_element(robot, line))
      {
        std::cerr << "Bad configuration line for " << filename << ":" << std::endl << line << std::endl;
      }
    }
    robot_config.close();
    draw_robot(robot, file_base + ".svg");
    return true;
  }
  else
  {
    std::cout << "Unable to open " << filename << std::endl; 
    return false;
  }

  std::cout << "Unknown error with " << filename << std::endl;
  return false;
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: ./generate_robots <list of .robot files>" << std::endl;
    return -1;
  }

  int num_good = 0;
  std::cout << "Converting files..." << std::endl;
  for (int i = 1; i < argc; i++)
  {
    if (draw_robot(argv[i]))
    {
      ++num_good;
      std::cout << "  " << argv[i] << ": success" << std::endl;
    }
    // NOTE: error from failure will be displayed in the 'draw_robot' function.
  }

  std::cout << "Converted " << num_good << "/" << argc - 1 << " files." << std::endl; 
  return 0; 
}
