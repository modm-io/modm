#ifndef _ROS_SERVICE_GetSearchPosition_h
#define _ROS_SERVICE_GetSearchPosition_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "geometry_msgs/PoseStamped.h"

namespace hector_nav_msgs
{

static const char GETSEARCHPOSITION[] = "hector_nav_msgs/GetSearchPosition";

  class GetSearchPositionRequest : public ros::Msg
  {
    public:
      typedef geometry_msgs::PoseStamped _ooi_pose_type;
      _ooi_pose_type ooi_pose;
      typedef float _distance_type;
      _distance_type distance;

    GetSearchPositionRequest():
      ooi_pose(),
      distance(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->ooi_pose.serialize(outbuffer + offset);
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
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->ooi_pose.deserialize(inbuffer + offset);
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
     return offset;
    }

    const char * getType(){ return GETSEARCHPOSITION; };
    const char * getMD5(){ return "a579e8b989d066f6c17946ab060a6ff9"; };

  };

  class GetSearchPositionResponse : public ros::Msg
  {
    public:
      typedef geometry_msgs::PoseStamped _search_pose_type;
      _search_pose_type search_pose;

    GetSearchPositionResponse():
      search_pose()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->search_pose.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->search_pose.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return GETSEARCHPOSITION; };
    const char * getMD5(){ return "6cc90c22cc49490b4dd1ef4798491231"; };

  };

  class GetSearchPosition {
    public:
    typedef GetSearchPositionRequest Request;
    typedef GetSearchPositionResponse Response;
  };

}
#endif
