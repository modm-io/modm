#ifndef _ROS_SERVICE_GetDistanceToObstacle_h
#define _ROS_SERVICE_GetDistanceToObstacle_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "geometry_msgs/PointStamped.h"

namespace hector_nav_msgs
{

static const char GETDISTANCETOOBSTACLE[] = "hector_nav_msgs/GetDistanceToObstacle";

  class GetDistanceToObstacleRequest : public ros::Msg
  {
    public:
      typedef geometry_msgs::PointStamped _point_type;
      _point_type point;

    GetDistanceToObstacleRequest():
      point()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->point.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->point.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return GETDISTANCETOOBSTACLE; };
    const char * getMD5(){ return "47dfdbd810b48d0a47b7ad67e4191bcc"; };

  };

  class GetDistanceToObstacleResponse : public ros::Msg
  {
    public:
      typedef float _distance_type;
      _distance_type distance;
      typedef geometry_msgs::PointStamped _end_point_type;
      _end_point_type end_point;

    GetDistanceToObstacleResponse():
      distance(0),
      end_point()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        float real;
        uint32_t base;
      } u_distance;
      u_distance.real = this->distance;
      *(outbuffer + offset + 0) = (u_distance.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_distance.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_distance.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_distance.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->distance);
      offset += this->end_point.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        float real;
        uint32_t base;
      } u_distance;
      u_distance.base = 0;
      u_distance.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_distance.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_distance.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_distance.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->distance = u_distance.real;
      offset += sizeof(this->distance);
      offset += this->end_point.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return GETDISTANCETOOBSTACLE; };
    const char * getMD5(){ return "019a8fc3bf7fd73c014dc08523397f1c"; };

  };

  class GetDistanceToObstacle {
    public:
    typedef GetDistanceToObstacleRequest Request;
    typedef GetDistanceToObstacleResponse Response;
  };

}
#endif
