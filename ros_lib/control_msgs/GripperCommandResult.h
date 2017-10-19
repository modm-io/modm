#ifndef _ROS_control_msgs_GripperCommandResult_h
#define _ROS_control_msgs_GripperCommandResult_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace control_msgs
{

  class GripperCommandResult : public ros::Msg
  {
    public:
      typedef double _position_type;
      _position_type position;
      typedef double _effort_type;
      _effort_type effort;
      typedef bool _stalled_type;
      _stalled_type stalled;
      typedef bool _reached_goal_type;
      _reached_goal_type reached_goal;

    GripperCommandResult():
      position(0),
      effort(0),
      stalled(0),
      reached_goal(0)
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
      } u_effort;
      u_effort.real = this->effort;
      *(outbuffer + offset + 0) = (u_effort.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_effort.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_effort.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_effort.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_effort.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_effort.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_effort.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_effort.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->effort);
      union {
        bool real;
        uint8_t base;
      } u_stalled;
      u_stalled.real = this->stalled;
      *(outbuffer + offset + 0) = (u_stalled.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->stalled);
      union {
        bool real;
        uint8_t base;
      } u_reached_goal;
      u_reached_goal.real = this->reached_goal;
      *(outbuffer + offset + 0) = (u_reached_goal.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->reached_goal);
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
      } u_effort;
      u_effort.base = 0;
      u_effort.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_effort.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_effort.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_effort.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_effort.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_effort.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_effort.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_effort.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->effort = u_effort.real;
      offset += sizeof(this->effort);
      union {
        bool real;
        uint8_t base;
      } u_stalled;
      u_stalled.base = 0;
      u_stalled.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->stalled = u_stalled.real;
      offset += sizeof(this->stalled);
      union {
        bool real;
        uint8_t base;
      } u_reached_goal;
      u_reached_goal.base = 0;
      u_reached_goal.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->reached_goal = u_reached_goal.real;
      offset += sizeof(this->reached_goal);
     return offset;
    }

    const char * getType(){ return "control_msgs/GripperCommandResult"; };
    const char * getMD5(){ return "e4cbff56d3562bcf113da5a5adeef91f"; };

  };

}
#endif