#include "robot_diagrams_0.0.hpp"

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <cstdlib>

std::vector<std::string> split(std::string s)
{
  std::stringstream ss(s);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> vstrings(begin, end);
  std::copy(vstrings.begin(), vstrings.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
}

bool add_element(rob_diag::Robot, std::string line)
{
  try
  {
    std::vector<std::string> words = split(line);
    if (words[0] == "base")
    {
      if (words.size() == 1)
      {
        rob_diag::RobotElement* base = (rob_diag::RobotElement*)new rob_diag::Base();
      }
      else if (words.size() == 2)
      {
        // TODO: do this better...std::stof is only C++11...
        double width = std::strtod (words[1].c_str(), NULL);
        rob_diag::RobotElement* base = (rob_diag::RobotElement*)new rob_diag::Base(width);
      }
    }
    else if (words[0] == "link")
    {
    }
    else if (words[0] == "link_frames")
    {
      std::cerr << words[0] << " not yet supported!" << std::endl;
      return false;
    }
    else if (words[0] == "rjoint")
    {
      std::cerr << words[0] << " not yet supported!" << std::endl;
      return false;
    }
    else if (words[0] == "pjoint")
    {
      std::cerr << words[0] << " not yet supported!" << std::endl;
      return false;
    }
    else if (words[0] == "effector")
    {
      std::cerr << words[0] << " not yet supported!" << std::endl;
      return false;
    }
    else
    {
      std::cerr << words[0] << " not recognized!" << std::endl;
      return false;
    }
  }
  catch (...)
  {
    return false;
  }
  
  return true;
}

void draw_robot(rob_diag::Robot robot, std::string filename)
{
  // TODO!
}

void delete_robot(rob_diag::Robot robot)
{
  // TODO: move to a destructor? Don't store pointers?
  for (int i = 0; i < robot.elements_.size(); ++i)
    delete robot.elements_[i];
  robot.elements_.clear();
  // TODO! NOTE: ensure this is called on any failure, even after a bad 'add element'
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
        delete_robot(robot);
        robot_config.close();
        return false;
      }
    }
    delete_robot(robot);
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
