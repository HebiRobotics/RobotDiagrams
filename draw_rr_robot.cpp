#include "robot_diagrams_0.0.hpp"

int main()
{
  // TODO: messy! figure out the C++03 way to do this right...
  // NOTE: in C++11, these could be unique_ptrs...
  // Build robot:
  // Link 1:
  //  l = 100
  //  theta = pi/4
  // Line 2:
  //  l = 50
  //  theta = pi/8
  // End effector:
  rob_diag::Robot robot;
  rob_diag::Base e0(10);
  rob_diag::RJoint e1;
  rob_diag::Link e2(100, M_PI / 4);
  rob_diag::RJoint e3;
  rob_diag::Link e4(50, M_PI / 8);
  robot.elements_.push_back((rob_diag::RobotElement*)&e0);
  robot.elements_.push_back((rob_diag::RobotElement*)&e1);
  robot.elements_.push_back((rob_diag::RobotElement*)&e2);
  robot.elements_.push_back((rob_diag::RobotElement*)&e3);
  robot.elements_.push_back((rob_diag::RobotElement*)&e4);

  // Compute dimensions
  rob_diag::Rect bounds = robot.compute_dimensions();
  double width = bounds.right_ - bounds.left_;
  double height = bounds.top_ - bounds.bottom_;
  double margin = 10;
  svg::Dimensions dimensions(width + margin * 2.0, height + margin * 2.0);

  // Draw to file:
  Document doc("robot.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));
  rob_diag::Pose origin(-bounds.left_ + margin, -bounds.bottom_ + margin, 0);
  robot.draw_at(doc, origin);

  // Save and quit
  doc.save();
}
