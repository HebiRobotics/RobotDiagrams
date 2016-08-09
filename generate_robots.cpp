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
  return vstrings;
}

// TODO: delete copy constructor of robot!
bool add_element(rob_diag::Robot& robot, std::string line)
{
  // TODO: do float parsing better...std::stof is only C++11...std::strtod sucks for error conditions.
  try
  {
    std::vector<std::string> words = split(line);
    if (words.size() == 0)
    {
      std::cerr << "No arguments on line!" << std::endl;
      return false;
    }
    if (words[0] == "base")
    {
      if (words.size() == 1)
      {
        rob_diag::RobotElement* base = (rob_diag::RobotElement*)new rob_diag::Base();
        robot.elements_.push_back(base);
        return true;
      }
      else if (words.size() == 2)
      {
        double width = std::strtod (words[1].c_str(), NULL);
        rob_diag::RobotElement* base = (rob_diag::RobotElement*)new rob_diag::Base(width);
        robot.elements_.push_back(base);
        return true;
      }
      std::cerr << "Invalid arguments for " << words[0] << std::endl;
      return false;
    }
    else if (words[0] == "link")
    {
      if (words.size() == 2)
      {
        double length = std::strtod (words[1].c_str(), NULL);
        rob_diag::RobotElement* link = (rob_diag::RobotElement*)new rob_diag::Link(length);
        robot.elements_.push_back(link);
        return true;
      }
      else if (words.size() == 3)
      {
        double length = std::strtod (words[1].c_str(), NULL);
        double theta = std::strtod (words[2].c_str(), NULL);
        rob_diag::RobotElement* link = (rob_diag::RobotElement*)new rob_diag::Link(length, theta);
        robot.elements_.push_back(link);
        return true;
      }
      else if (words.size() == 4)
      {
        double length = std::strtod (words[1].c_str(), NULL);
        double theta = std::strtod (words[2].c_str(), NULL);
        rob_diag::RobotElement* link = (rob_diag::RobotElement*)new rob_diag::Link(length, theta, words[3]);
        robot.elements_.push_back(link);
        return true;
      }
      std::cerr << "Invalid (or not yet supported) arguments for " << words[0] << std::endl;
      return false;
    }
    else if (words[0] == "link_frames")
    {
      if (words.size() == 2)
      {
        double length = std::strtod (words[1].c_str(), NULL);
        rob_diag::RobotElement* link = (rob_diag::RobotElement*)new rob_diag::LinkWithFrames(length);
        robot.elements_.push_back(link);
        return true;
      }
      else if (words.size() == 3)
      {
        double length = std::strtod (words[1].c_str(), NULL);
        double theta = std::strtod (words[2].c_str(), NULL);
        rob_diag::RobotElement* link = (rob_diag::RobotElement*)new rob_diag::LinkWithFrames(length, theta);
        robot.elements_.push_back(link);
        return true;
      }
      else if (words.size() == 4)
      {
        double length = std::strtod (words[1].c_str(), NULL);
        double theta = std::strtod (words[2].c_str(), NULL);
        rob_diag::RobotElement* link = (rob_diag::RobotElement*)new rob_diag::LinkWithFrames(length, theta, words[3]);
        robot.elements_.push_back(link);
        return true;
      }
      std::cerr << "Invalid (or not yet supported) arguments for " << words[0] << std::endl;
      return false;
    }
    else if (words[0] == "rjoint")
    {
      if (words.size() == 1)
      {
        rob_diag::RobotElement* rjoint = (rob_diag::RobotElement*)new rob_diag::RJoint();
        robot.elements_.push_back(rjoint);
        return true;
      }
      std::cerr << "Invalid arguments for " << words[0] << std::endl;
      return false;
    }
    else if (words[0] == "pjoint")
    {
      if (words.size() == 1)
      {
        rob_diag::RobotElement* pjoint = (rob_diag::RobotElement*)new rob_diag::PJoint();
        robot.elements_.push_back(pjoint);
        return true;
      }
      std::cerr << "Invalid arguments for " << words[0] << std::endl;
      return false;
    }
    else if (words[0] == "effector")
    {
      if (words.size() == 1)
      {
        rob_diag::RobotElement* ee = (rob_diag::RobotElement*)new rob_diag::EndEffector();
        robot.elements_.push_back(ee);
        return true;
      }
      std::cerr << "Invalid (or not yet supported) arguments for " << words[0] << std::endl;
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
  
  return false;
}

void draw_robot(rob_diag::Robot& robot, std::string filename)
{
  // Compute dimensions
  rob_diag::Rect bounds = robot.compute_dimensions();
  double width = bounds.right_ - bounds.left_;
  double height = bounds.top_ - bounds.bottom_;
  double margin = 10;
  svg::Dimensions dimensions(width + margin * 2.0, height + margin * 2.0);

  // Draw to file:
  Document doc(filename, svg::Layout(dimensions, svg::Layout::BottomLeft));
  rob_diag::Pose origin(-bounds.left_ + margin, -bounds.bottom_ + margin, 0);
  robot.draw_at(doc, origin);

  // Save and quit
  doc.save();
}

void delete_robot(rob_diag::Robot& robot)
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
        robot_config.close();
        delete_robot(robot);
        return false;
      }
    }
    robot_config.close();
    draw_robot(robot, file_base + ".svg");
    delete_robot(robot);
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
      std::cout << argv[i] << ": success" << std::endl;
    }
    // NOTE: error from failure will be displayed in the 'draw_robot' function.
  }

  std::cout << "Converted " << num_good << "/" << argc - 1 << " files." << std::endl; 
  return 0; 
}
