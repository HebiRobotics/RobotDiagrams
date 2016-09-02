#include <cmath>
#include <algorithm>
#include <memory>

#include "simple_svg_1.0.0.hpp"

// Since M_PI isn't part of the C++ standard, we define it here to be more
// portable.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace svg; // TODO: remove me!

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
  virtual Rect measure(const Pose& start, Pose& end) = 0;
  // Draws the element given an offset from the measure pass
  virtual void draw(Document& doc, const Point& offset) = 0;
  virtual ~RobotElement() {};
protected:
  // A set of points that are used to draw the given element.  These are
  // computed during the 'measure' pass, and used for rendering during the
  // 'draw' pass.
  std::vector<Point> points_;
  // Extend the current 'bounds' object to include the (x,y) values of each
  // point.
  virtual Rect point_bounds()
  {
    if (points_.size() == 0)
    {
      std::cerr << "Invalid use of point_bounds!" << std::endl;
      return Rect();
    }
    Rect bounds(points_[0].x, points_[0].y, points_[0].x, points_[0].y);
    for (unsigned int i = 1; i < points_.size(); ++i)
    {
      Point p = points_[i];
      bounds.left_   = std::min(p.x, bounds.left_);
      bounds.right_  = std::max(p.x, bounds.right_);
      bounds.top_    = std::max(p.y, bounds.top_);
      bounds.bottom_ = std::min(p.y, bounds.bottom_);
    }
    return bounds;
  }
};

// TODO: could think about ensuring measure pass has been run before calling
// draw?
// TODO: debug by drawing bounds? And rotating things?

class Vector : public RobotElement
{
public:
  Vector(double length, std::string label = "")
    : length_(length), arrow_len_(4), label_(label)
  {}
  virtual Rect measure(const Pose& start, Pose& end)
  {
    // Note: the points for Link are { start, end, arrowhead end 1,
    // arrowhead end 2 }
    points_.clear();
    end = start;
    double c = std::cos(end.theta_);
    double s = std::sin(end.theta_);
    end.x_ = start.x_ + c * length_;
    end.y_ = start.y_ + s * length_;
    points_.push_back(Point(start.x_, start.y_));
    points_.push_back(Point(end.x_, end.y_));
    points_.push_back(Point(end.x_ - c * arrow_len_ + s * arrow_len_, end.y_ - s * arrow_len_ - c * arrow_len_ ));
    points_.push_back(Point(end.x_ - c * arrow_len_ - s * arrow_len_, end.y_ - s * arrow_len_ + c * arrow_len_ ));
    return point_bounds();
  }
  virtual void draw(Document& doc, const Point& offset)
  {
    doc << Line(points_[0] + offset, points_[1] + offset, Stroke(0.5, Color::Black));
    doc << Line(points_[1] + offset, points_[2] + offset, Stroke(0.5, Color::Black));
    doc << Line(points_[1] + offset, points_[3] + offset, Stroke(0.5, Color::Black));
    if (label_.size() > 0)
      doc << Text(points_[0] * 0.5 + points_[1] * 0.5, label_, Fill(Color::Black));
  }
  virtual ~Vector() {};
  double length_;
  double arrow_len_;
  std::string label_;
};

// TODO: change this to 'point', and don't pull in svg namespace!
class RobPoint : public RobotElement
{
public:
  RobPoint(double radius = 2, std::string label = "")
    : radius_(radius), label_(label)
  {}
  virtual Rect measure(const Pose& start, Pose& end)
  {
    // Note: the points for Point are { center }
    end = start;
    points_.push_back(Point(start.x_, start.y_));
    return Rect(start.x_ - radius_, start.y_ + radius_,
                start.x_ + radius_, start.y_ - radius_);
  }
  virtual void draw(Document& doc, const Point& offset)
  {
    doc << Circle(points_[0] + offset, radius_ * 2, Fill(Color::Black));
    if (label_.size() > 0)
      doc << Text(points_[0], label_, Fill(Color::Black));
  }
  virtual ~RobPoint() {};
  double radius_;
  std::string label_;
};


class Frames : public RobotElement
{
public:
  Frames()
    : frame_scale_(25), arrow_len_(4)
  {}
  virtual Rect measure(const Pose& start, Pose& end)
  {
    // Set end frame
    end = start;
    // Note: the points for are { center, x axis, x arrowheads, y axis,
    // y arrowheads}
    double c = std::cos(end.theta_);
    double s = std::sin(end.theta_);
    // X axis:
    points_.push_back(Point(start.x_, start.y_));
    Point p_x(points_[0] + Point(c, s) * frame_scale_);
    points_.push_back(p_x);
    points_.push_back(p_x + (Point(-c, -s) + Point(-s, c)) * arrow_len_);
    points_.push_back(p_x + (Point(-c, -s) - Point(-s, c)) * arrow_len_);
    // Y axis:
    Point p_y(points_[0] + Point(-s, c) * frame_scale_);
    points_.push_back(p_y);
    points_.push_back(p_y + (Point(s, -c) + Point(c, s)) * arrow_len_);
    points_.push_back(p_y + (Point(s, -c) - Point(c, s)) * arrow_len_);
    return point_bounds();
  }
  virtual void draw(Document& doc, const Point& offset)
  {
    Stroke s_r(1.35, Color::Red);
    Stroke s_b(1.35, Color::Blue);
    doc << Line(points_[0] + offset, points_[1] + offset, s_r);
    doc << Line(points_[1] + offset, points_[2] + offset, s_r);
    doc << Line(points_[1] + offset, points_[3] + offset, s_r);
    doc << Line(points_[0] + offset, points_[4] + offset, s_b);
    doc << Line(points_[4] + offset, points_[5] + offset, s_b);
    doc << Line(points_[4] + offset, points_[6] + offset, s_b);
  }
  double frame_scale_;
  double arrow_len_;
};

class Link : public RobotElement
{
public:
  Link(double length, std::string label = "")
    : length_(length), label_(label), visible_(true)
  {}
  virtual Rect measure(const Pose& start, Pose& end)
  {
    // Note: the points for Link are { start, end }
    points_.clear();
    end = start;
    end.x_ = start.x_ + std::cos(end.theta_) * length_;
    end.y_ = start.y_ + std::sin(end.theta_) * length_;
    points_.push_back(Point(start.x_, start.y_));
    points_.push_back(Point(end.x_, end.y_));
    return point_bounds();
  }
  virtual void draw(Document& doc, const Point& offset)
  {
    if (visible_)
      doc << Line(points_[0] + offset, points_[1] + offset, Stroke(0.5, Color::Black));
    if (label_.size() > 0)
      doc << Text(points_[0] * 0.5 + points_[1] * 0.5, label_, Fill(Color::Black));
  }
  virtual ~Link() {};
  double length_;
  std::string label_;
  bool visible_;
};
// TODO: add label!


class RJoint : public RobotElement
{
public:
  RJoint(double default_theta = 0, double radius = 4)
    : radius_(radius), default_theta_(default_theta), visible_(true)
  {}
  virtual Rect measure(const Pose& start, Pose& end)
  {
    // Note: the points for RJoint are { center }
    end = start;
    end.theta_ += default_theta_;
    points_.push_back(Point(start.x_, start.y_));
    return Rect(start.x_ - radius_, start.y_ + radius_,
                start.x_ + radius_, start.y_ - radius_);
  }
  virtual void draw(Document& doc, const Point& offset)
  {
    if (visible_)
      doc << Circle(points_[0] + offset, radius_ * 2, Fill(Color::Transparent), Stroke(0.5, Color::Black));
  }
  virtual ~RJoint() {};
  double radius_, default_theta_;
  bool visible_;
};

// (x to the right, y up, origin halfway between pts. 1 and 2)
//
// 1_______0
//         |
// 4----5  |
// 2_______|3
//
// width is distance from 1 to 2.
// length is distance from midpoint of 0-3 to 4.
class PJoint : public RobotElement
{
public:
  PJoint()
    : width_(10), length_(30)
  {}
  virtual Rect measure(const Pose& start, Pose& end)
  {
    // Note: the points for PJoint are labeled in the above diagram
    points_.clear();
    end.theta_ = start.theta_;
    double l_x = std::cos(end.theta_) * length_;
    double l_y = std::sin(end.theta_) * length_;
    double w_x = std::sin(end.theta_) * width_ * 0.5;
    double w_y = -std::cos(end.theta_) * width_ * 0.5;
    end.x_ = start.x_ + l_x;
    end.y_ = start.y_ + l_y;
    points_.push_back(Point(start.x_ - w_x + l_x, start.y_ - w_y + l_y));
    points_.push_back(Point(start.x_ - w_x, start.y_ - w_y));
    points_.push_back(Point(start.x_ + w_x, start.y_ + w_y));
    points_.push_back(Point(start.x_ + w_x + l_x, start.y_ + w_y + l_y));
    points_.push_back(Point(start.x_, start.y_));
    points_.push_back(Point(start.x_, start.y_) * (1.0 / 3.0) + Point(end.x_, end.y_) * (2.0 / 3.0));
    return point_bounds();
  }
  virtual void draw(Document& doc, const Point& offset)
  {
    Stroke s(0.5, Color::Black);
    doc << Line(points_[0] + offset, points_[1] + offset, s)
        << Line(points_[2] + offset, points_[3] + offset, s)
        << Line(points_[4] + offset, points_[5] + offset, s)
        << Line(points_[0] + offset, points_[3] + offset, s);
  }
  virtual ~PJoint() {};
  double width_, length_;
};

class Base : public RobotElement
{
public:
  Base(double width = 20, double default_theta = 0)
    : width_(width), default_theta_(default_theta), visible_(true)
  {}
  virtual Rect measure(const Pose& start, Pose& end)
  {
    // Start/end at same point...almost.  Set 'end' at end :)
    end = start;
    end.theta_ += default_theta_;

    //    |
    //  -----
    //   \\\\
    //
    // Points: { left of ground, right of ground, bottom left of "fixed" lines,
    // end of "fixed" lines, center, top }
    points_.clear();
    double theta = start.theta_ + default_theta_;
    double w_x = std::cos(theta) * width_ * 0.5;
    double w_y = std::sin(theta) * width_ * 0.5;
    double h_x = std::sin(theta) * width_ * 0.3;
    double h_y = -std::cos(theta) * width_ * 0.3;
    double left = std::min(-w_x, h_x);
    double right = std::max(w_x, h_x);
    double top = std::max(w_y, h_y);
    double bottom = std::min(-w_y, h_y);

    end.x_ -= h_x;
    end.y_ -= h_y;

    // Horizontal "ground"
    points_.push_back(Point(start.x_ - w_x, start.y_ - w_y));
    points_.push_back(Point(start.x_ + w_x, start.y_ + w_y));
    // Slanted "fixed" lines
    points_.push_back(Point(start.x_ - w_x + h_x, start.y_ - w_y + h_y));
    points_.push_back(Point(start.x_ + w_x + h_x, start.y_ + w_y + h_y));
    // Small "pole"/base link
    points_.push_back(Point(start.x_, start.y_));
    points_.push_back(Point(start.x_ - h_x, start.y_ - h_y));

    // Compute bounds
    return point_bounds();
  }
  virtual void draw(Document& doc, const Point& offset)
  {
    if (!visible_)
      return;
    Stroke s(0.5, Color::Black);
    // Horizontal "ground"
    doc << Line(points_[0] + offset, points_[1] + offset, s);
    // Slanted "fixed" lines
    int total_lines = 5;
    for (int i = 0; i < total_lines; ++i)
    {
      // 0 to 1, from left to right, of the top of the lines along the "ground"
      // line
      double frac_top = ((double)i + 1) / (((double)total_lines) + 0.5);
      double frac_bot = ((double)i) / (((double)total_lines) + 0.5);
      doc << Line(
        points_[0] * frac_top + points_[1] * (1 - frac_top) + offset,
        points_[2] * frac_bot + points_[3] * (1 - frac_bot) + offset,
        s);
    }
    // Small pole/base link
    doc << Line(points_[4] + offset, points_[5] + offset, s);
  }
  virtual ~Base() {};
  double width_, default_theta_;
  bool visible_;
};

// (x to the right, y up, origin halfway between pts. 1 and 2)
//
// 1____0
//  |
//  |  
// 2|___3
//
// width is distance from 1 to 2.
// length is distance from 1 to 0 or 2 to 3; set as 0.5 * width.
class EndEffector : public RobotElement
{
public:
  EndEffector(double width = 20, double default_theta = 0)
    : width_(width), default_theta_(default_theta)
  {}
  virtual Rect measure(const Pose& start, Pose& end)
  {
    end = start;
    end.theta_ += default_theta_;

    // Note: the points for PJoint are labeled in the above diagram
    points_.clear();
    double theta = start.theta_ + default_theta_;
    double w_x = std::sin(end.theta_) * width_ * 0.5;
    double w_y = -std::cos(end.theta_) * width_ * 0.5;
    double l_x = std::cos(end.theta_) * width_ * 0.5;
    double l_y = std::sin(end.theta_) * width_ * 0.5;
    points_.push_back(Point(start.x_ - w_x + l_x, start.y_ - w_y + l_y));
    points_.push_back(Point(start.x_ - w_x, start.y_ - w_y));
    points_.push_back(Point(start.x_ + w_x, start.y_ + w_y));
    points_.push_back(Point(start.x_ + w_x + l_x, start.y_ + w_y + l_y));
    return point_bounds();
  }
  virtual void draw(Document& doc, const Point& offset)
  {
    Stroke s(0.5, Color::Black);
    doc << Line(points_[0] + offset, points_[1] + offset, s)
        << Line(points_[1] + offset, points_[2] + offset, s)
        << Line(points_[2] + offset, points_[3] + offset, s);
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
      Pose out(0,0,0);
      bounds.extend(elements_[i]->measure(current, out));
      current = out;
    }
    return bounds;
  }

  void draw_at(Document& doc, const Pose& start)
  {
    for (int i = 0; i < elements_.size(); i++)
    {
      elements_[i]->draw(doc, Point(start.x_, start.y_));
    }
  }
};

}
