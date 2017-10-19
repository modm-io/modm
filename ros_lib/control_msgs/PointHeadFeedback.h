#ifndef _ROS_control_msgs_PointHeadFeedback_h
#define _ROS_control_msgs_PointHeadFeedback_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace control_msgs
{

  class PointHeadFeedback : public ros::Msg
  {
    public:
      typedef double _pointing_angle_error_type;
      _pointing_angle_error_type pointing_angle_error;

    PointHeadFeedback():
      pointing_angle_error(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        double real;
        uint64_t base;
      } u_pointing_angle_error;
      u_pointing_angle_error.real = this->pointing_angle_error;
      *(outbuffer + offset + 0) = (u_pointing_angle_error.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_pointing_angle_error.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_pointing_angle_error.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_pointing_angle_error.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_pointing_angle_error.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_pointing_angle_error.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_pointing_angle_error.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_pointing_angle_error.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->pointing_angle_error);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        double real;
        uint64_t base;
      } u_pointing_angle_error;
      u_pointing_angle_error.base = 0;
      u_pointing_angle_error.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_pointing_angle_error.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_pointing_angle_error.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_pointing_angle_error.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_pointing_angle_error.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_pointing_angle_error.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_pointing_angle_error.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_pointing_angle_error.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->pointing_angle_error = u_pointing_angle_error.real;
      offset += sizeof(this->pointing_angle_error);
     return offset;
    }

    const char * getType(){ return "control_msgs/PointHeadFeedback"; };
    const char * getMD5(){ return "cce80d27fd763682da8805a73316cab4"; };

  };

}
#endif