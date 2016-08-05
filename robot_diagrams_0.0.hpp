#include <cmath>
#include <algorithm>
#include <memory>

#include "simple_svg_1.0.0.hpp"

// Since M_PI isn't part of the C++ standard, we define it here to be more
// portable.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace svg;

namespace rob_diag
{

class Pose
{
public:
  Pose(double x, double y, double theta)
    : x_(x), y_(y), theta_(theta)
  {}
  double x_, y_, theta_;
};

class Rect
{
public:
  Rect()
    : left_(0), top_(0), right_(0), bottom_(0)
  {}
  Rect(double left, double top, double right, double bottom)
    : left_(left), top_(top), right_(right), bottom_(bottom)
  {}
  void extend(const Rect& other)
  {
    left_ = std::min(left_, other.left_);
    right_ = std::max(right_, other.right_);
    top_ = std::max(top_, other.top_);
    bottom_ = std::min(bottom_, other.bottom_);
  }
  double left_, top_, right_, bottom_;
};

class RobotElement
{
public:
  // Computes the ending pose and a bounding box given a starting pose
  virtual void measure(const Pose& start, Pose& end, Rect& bounds) = 0;
  // Draws the link and computes the output pose given a starting pose
  virtual void draw_at(Document& doc, const Pose& start, Pose& end) = 0;
  virtual ~RobotElement() {};
};

class Link : public RobotElement
{
public:
  Link(double length, double default_theta = 0)
    : length_(length), default_theta_(default_theta)  
  {}
  virtual void measure(const Pose& start, Pose& end, Rect& bounds)
  {
    double x_offset = std::cos(start.theta_ + default_theta_) * length_;
    double y_offset = std::sin(start.theta_ + default_theta_) * length_;
    end.theta_ = start.theta_ + default_theta_;
    end.x_ = start.x_ + x_offset;
    end.y_ = start.y_ + y_offset;
    bounds.left_ = std::min(start.x_, end.x_);
    bounds.right_ = std::max(start.x_, end.x_);
    bounds.top_ = std::max(start.y_, end.y_);
    bounds.bottom_ = std::min(start.y_, end.y_);
  }
  virtual void draw_at(Document& doc, const Pose& start, Pose& end)
  {
    double x_offset = std::cos(start.theta_ + default_theta_) * length_;
    double y_offset = std::sin(start.theta_ + default_theta_) * length_;
    Point p1(start.x_, start.y_);
    Point p2(start.x_ + x_offset, start.y_ + y_offset);
    Stroke s(0.5, Color::Black);
    doc << Line(p1, p2, s);
    end.theta_ = start.theta_ + default_theta_;
    end.x_ = start.x_ + x_offset;
    end.y_ = start.y_ + y_offset;
  }
  virtual ~Link() {};
  double length_, default_theta_;
};

class RJoint : public RobotElement
{
public:
  RJoint(double radius = 5, double default_theta = 0)
    : radius_(radius), default_theta_(default_theta)  
  {}
  virtual void measure(const Pose& start, Pose& end, Rect& bounds)
  {
    end = start;
    end.theta_ += default_theta_;
    bounds.left_ = start.x_ - radius_;
    bounds.right_ = start.x_ + radius_;
    bounds.top_ = start.y_ + radius_;
    bounds.bottom_ = start.y_ - radius_;
  }
  virtual void draw_at(Document& doc, const Pose& start, Pose& end)
  {
    end.theta_ = start.theta_ + default_theta_;
    end.x_ = start.x_;
    end.y_ = start.y_;
    doc << Circle(svg::Point(start.x_, start.y_), radius_ * 2, Fill(Color::Transparent), Stroke(0.5, Color::Black));
  }
  virtual ~RJoint() {};
  double radius_, default_theta_;
};

class Base : public RobotElement
{
public:
  Base(double width = 10, double default_theta = 0)
    : width_(width), default_theta_(default_theta)
  {}
  virtual void measure(const Pose& start, Pose& end, Rect& bounds)
  {
    double theta = start.theta_ + default_theta_;
    double w_x = std::cos(theta) * width_;
    double w_y = std::sin(theta) * width_;
    double h_x = std::sin(theta) * width_ * 0.5;
    double h_y = -std::cos(theta) * width_ * 0.5;
    double left = std::min(-w_x, h_x);
    double right = std::max(w_x, h_x);
    double top = std::max(w_y, h_y);
    double bottom = std::min(-w_y, h_y);

    end = start;
    end.theta_ += default_theta_;
    bounds.left_ = start.x_ + left;
    bounds.right_ = start.x_ + right;
    bounds.top_ = start.y_ + top;
    bounds.bottom_ = start.y_ + bottom;
  }
  virtual void draw_at(Document& doc, const Pose& start, Pose& end)
  {
    double theta = start.theta_ + default_theta_;
    double w_x = std::cos(theta) * width_;
    double w_y = std::sin(theta) * width_;
    double h_x = std::sin(theta) * width_ * 0.5;
    double h_y = -std::cos(theta) * width_ * 0.5;

    end = start;
    end.theta_ += default_theta_;
    // Horizontal "ground"
    Point p1(start.x_ - w_x, start.y_ - w_y);
    Point p2(start.x_ + w_x, start.y_ + w_y);
    Stroke s(0.5, Color::Black);
    doc << Line(p1, p2, s);
    // Slanted "fixed" lines
    int total_lines = 5;
    for (int i = 0; i < total_lines; ++i)
    {
      // 0 to 1, from left to right, of the top of the lines along the "ground"
      // line
      double frac_top = ((double)i + 1) / (((double)total_lines) + 0.5);
      double frac_bot = ((double)i) / (((double)total_lines) + 0.5);
      frac_top = frac_top * 2 - 1;
      frac_bot = frac_bot * 2 - 1;
      Point p1_tmp(start.x_ + w_x * frac_top, start.y_ + w_y * frac_top);
      Point p2_tmp(start.x_ + w_x * frac_bot + h_x, start.y_ + w_y * frac_bot + h_y);
      Stroke s(0.5, Color::Black);
      doc << Line(p1_tmp, p2_tmp, s);
    }
  }
  virtual ~Base() {};
  double width_, default_theta_;
};

class EndEffector : public RobotElement
{
public:
  EndEffector(double width = 10, double default_theta = 0)
    : width_(width), default_theta_(default_theta)
  {}
  virtual void measure(const Pose& start, Pose& end, Rect& bounds)
  {
    double theta = start.theta_ + default_theta_;
    double w_x = -std::sin(theta) * width_;
    double w_y = std::cos(theta) * width_;
    double h_x = std::cos(theta) * width_;
    double h_y = std::sin(theta) * width_;
    double left = std::min(-w_x, h_x);
    double right = std::max(w_x, h_x);
    double top = std::max(w_y, h_y);
    double bottom = std::min(-w_y, h_y);

    end = start;
    end.theta_ += default_theta_;
    bounds.left_ = start.x_ + left;
    bounds.right_ = start.x_ + right;
    bounds.top_ = start.y_ + top;
    bounds.bottom_ = start.y_ + bottom;
  }
  virtual void draw_at(Document& doc, const Pose& start, Pose& end)
  {
    double theta = start.theta_ + default_theta_;
    double w_x = -std::sin(theta) * width_;
    double w_y = std::cos(theta) * width_;
    double h_x = std::cos(theta) * width_;
    double h_y = std::sin(theta) * width_;

    end = start;
    end.theta_ += default_theta_;
    // Horizontal "ground"
    Point p1(start.x_ - w_x, start.y_ - w_y);
    Point p2(start.x_ + w_x, start.y_ + w_y);
    Point p1_e(start.x_ - w_x + h_x, start.y_ - w_y + h_y);
    Point p2_e(start.x_ + w_x + h_x, start.y_ + w_y + h_y);
    Stroke s(0.5, Color::Black);
    doc << Line(p1, p2, s)
        << Line(p1, p1_e, s)
        << Line(p2, p2_e, s);
  }
  virtual ~EndEffector() {};
  double width_, default_theta_;
};

// A "robot" (restricted to a simple kinematic chain)
// TODO: different name?
class Robot
{
public:
  std::vector<RobotElement*> elements_;

  Rect compute_dimensions()
  {
    Pose current(0,0,0);
    Rect bounds;
    for (int i = 0; i < elements_.size(); i++)
    {
      Rect tmp_bounds;
      Pose out(0,0,0);
      elements_[i]->measure(current, out, tmp_bounds);
      current = out;
      bounds.extend(tmp_bounds);
    }
    return bounds;
  }

  void draw_at(Document& doc, const Pose& start)
  {
    Pose current = start;
    for (int i = 0; i < elements_.size(); i++)
    {
      Pose out(0,0,0);
      elements_[i]->draw_at(doc, current, out);
      current = out;
    }
  }
};

}
