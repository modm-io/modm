#ifndef _ROS_control_msgs_GripperCommand_h
#define _ROS_control_msgs_GripperCommand_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace control_msgs
{

  class GripperCommand : public ros::Msg
  {
    public:
      typedef double _position_type;
      _position_type position;
      typedef double _max_effort_type;
      _max_effort_type max_effort;

    GripperCommand():
      position(0),
      max_effort(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        double real;
        uint64_t base;
      } u_position;
      u_position.real = this->position;
      *(outbuffer + offset + 0) = (u_position.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_position.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_position.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_position.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_position.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_position.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_position.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_position.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->position);
      union {
        double real;
        uint64_t base;
      } u_max_effort;
      u_max_effort.real = this->max_effort;
      *(outbuffer + offset + 0) = (u_max_effort.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_max_effort.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_max_effort.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_max_effort.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_max_effort.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_max_effort.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_max_effort.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_max_effort.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->max_effort);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        double real;
        uint64_t base;
      } u_position;
      u_position.base = 0;
      u_position.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_position.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_position.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_position.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_position.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_position.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_position.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_position.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->position = u_position.real;
      offset += sizeof(this->position);
      union {
        double real;
        uint64_t base;
      } u_max_effort;
      u_max_effort.base = 0;
      u_max_effort.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_max_effort.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_max_effort.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_max_effort.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_max_effort.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_max_effort.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_max_effort.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_max_effort.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->max_effort = u_max_effort.real;
      offset += sizeof(this->max_effort);
     return offset;
    }

    const char * getType(){ return "control_msgs/GripperCommand"; };
    const char * getMD5(){ return "680acaff79486f017132a7f198d40f08"; };

  };

}
#endif