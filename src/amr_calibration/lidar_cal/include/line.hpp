#pragma once

#include <cmath>
#include <pcl/point_types.h>

// Represents a line as 2 points
class Line
{
  using PointType = pcl::PointXYZ;

public:
  typedef struct
  {
    float A;
    float B;
    float C;
  } line_coefs;

public:
  PointType m_p1;
  PointType m_p2;
  line_coefs coefs;

public:
  Line(PointType p1, PointType p2) : m_p1{p1}, m_p2{p2}
  {
    coefs.A = p2.y - p1.y;
    coefs.B = p1.x - p2.x;
    coefs.C = p1.y * (p2.x - p1.x) - (p2.y - p1.y) * p1.x;
  }

  // Find point of intersection with another line.
  PointType intersection_with(const Line &other) const
  {
    // https://stackoverflow.com/questions/13244666/how-can-i-find-the-intersection-of-two-lines-in-general-equation-form
    PointType intersection_point;
    auto t_c = this->coefs;
    auto t_o = other.coefs;
    intersection_point.x =
        (t_c.C * t_o.B - t_c.B * t_o.C) / (t_c.B * t_o.A - t_c.A * t_o.B);
    intersection_point.y =
        (t_c.A * t_o.C - t_o.A * t_c.C) / (t_c.B * t_o.A - t_c.A * t_o.B);
    return intersection_point;
  }

  float angle() const { return std::atan(-coefs.A / coefs.B); }

  // Find the angle between two lines.
  float angle_between(const Line &other) const
  {
    return (this->angle() - other.angle());
  }

  // Return the absolute of the smallest angle between two lines.
  float smallest_angle_between(const Line &other) const
  {
    float ang = std::abs(this->angle_between(other));
    if (ang > M_PI)
    {
      return (2 * M_PI - ang);
    }
    else
    {
      return ang;
    }
  }

  // Find point along line where normal extends through another point
  PointType point_proj(const PointType &p) const
  {
    // https://stackoverflow.com/questions/37197987/find-point-along-line-where-normal-extends-through-another-point

    auto A = m_p1.getVector3fMap();
    auto B = m_p2.getVector3fMap();
    auto P = p.getVector3fMap();
    auto AB = B - A;
    auto AP = P - A;

    auto t = AB.dot(AP) / (AB.dot(AB));
    auto projection = A + AB * t;

    PointType out;
    out.getVector3fMap() = projection;
    return out;
  }
};