#ifndef _ROS_control_msgs_JointControllerState_h
#define _ROS_control_msgs_JointControllerState_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"

namespace control_msgs
{

  class JointControllerState : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef double _set_point_type;
      _set_point_type set_point;
      typedef double _process_value_type;
      _process_value_type process_value;
      typedef double _process_value_dot_type;
      _process_value_dot_type process_value_dot;
      typedef double _error_type;
      _error_type error;
      typedef double _time_step_type;
      _time_step_type time_step;
      typedef double _command_type;
      _command_type command;
      typedef double _p_type;
      _p_type p;
      typedef double _i_type;
      _i_type i;
      typedef double _d_type;
      _d_type d;
      typedef double _i_clamp_type;
      _i_clamp_type i_clamp;
      typedef bool _antiwindup_type;
      _antiwindup_type antiwindup;

    JointControllerState():
      header(),
      set_point(0),
      process_value(0),
      process_value_dot(0),
      error(0),
      time_step(0),
      command(0),
      p(0),
      i(0),
      d(0),
      i_clamp(0),
      antiwindup(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      union {
        double real;
        uint64_t base;
      } u_set_point;
      u_set_point.real = this->set_point;
      *(outbuffer + offset + 0) = (u_set_point.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_set_point.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_set_point.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_set_point.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_set_point.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_set_point.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_set_point.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_set_point.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->set_point);
      union {
        double real;
        uint64_t base;
      } u_process_value;
      u_process_value.real = this->process_value;
      *(outbuffer + offset + 0) = (u_process_value.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_process_value.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_process_value.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_process_value.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_process_value.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_process_value.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_process_value.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_process_value.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->process_value);
      union {
        double real;
        uint64_t base;
      } u_process_value_dot;
      u_process_value_dot.real = this->process_value_dot;
      *(outbuffer + offset + 0) = (u_process_value_dot.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_process_value_dot.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_process_value_dot.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_process_value_dot.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_process_value_dot.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_process_value_dot.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_process_value_dot.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_process_value_dot.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->process_value_dot);
      union {
        double real;
        uint64_t base;
      } u_error;
      u_error.real = this->error;
      *(outbuffer + offset + 0) = (u_error.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_error.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_error.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_error.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_error.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_error.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_error.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_error.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->error);
      union {
        double real;
        uint64_t base;
      } u_time_step;
      u_time_step.real = this->time_step;
      *(outbuffer + offset + 0) = (u_time_step.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_time_step.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_time_step.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_time_step.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_time_step.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_time_step.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_time_step.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_time_step.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->time_step);
      union {
        double real;
        uint64_t base;
      } u_command;
      u_command.real = this->command;
      *(outbuffer + offset + 0) = (u_command.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_command.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_command.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_command.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_command.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_command.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_command.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_command.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->command);
      union {
        double real;
        uint64_t base;
      } u_p;
      u_p.real = this->p;
      *(outbuffer + offset + 0) = (u_p.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_p.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_p.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_p.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_p.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_p.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_p.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_p.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->p);
      union {
        double real;
        uint64_t base;
      } u_i;
      u_i.real = this->i;
      *(outbuffer + offset + 0) = (u_i.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_i.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_i.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_i.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_i.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_i.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_i.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_i.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->i);
      union {
        double real;
        uint64_t base;
      } u_d;
      u_d.real = this->d;
      *(outbuffer + offset + 0) = (u_d.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_d.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_d.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_d.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_d.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_d.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_d.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_d.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->d);
      union {
        double real;
        uint64_t base;
      } u_i_clamp;
      u_i_clamp.real = this->i_clamp;
      *(outbuffer + offset + 0) = (u_i_clamp.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_i_clamp.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_i_clamp.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_i_clamp.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_i_clamp.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_i_clamp.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_i_clamp.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_i_clamp.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->i_clamp);
      union {
        bool real;
        uint8_t base;
      } u_antiwindup;
      u_antiwindup.real = this->antiwindup;
      *(outbuffer + offset + 0) = (u_antiwindup.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->antiwindup);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      union {
        double real;
        uint64_t base;
      } u_set_point;
      u_set_point.base = 0;
      u_set_point.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_set_point.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_set_point.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_set_point.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_set_point.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_set_point.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_set_point.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_set_point.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->set_point = u_set_point.real;
      offset += sizeof(this->set_point);
      union {
        double real;
        uint64_t base;
      } u_process_value;
      u_process_value.base = 0;
      u_process_value.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_process_value.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_process_value.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_process_value.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_process_value.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_process_value.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_process_value.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_process_value.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->process_value = u_process_value.real;
      offset += sizeof(this->process_value);
      union {
        double real;
        uint64_t base;
      } u_process_value_dot;
      u_process_value_dot.base = 0;
      u_process_value_dot.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_process_value_dot.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_process_value_dot.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_process_value_dot.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_process_value_dot.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_process_value_dot.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_process_value_dot.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_process_value_dot.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->process_value_dot = u_process_value_dot.real;
      offset += sizeof(this->process_value_dot);
      union {
        double real;
        uint64_t base;
      } u_error;
      u_error.base = 0;
      u_error.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_error.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_error.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_error.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_error.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_error.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_error.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_error.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->error = u_error.real;
      offset += sizeof(this->error);
      union {
        double real;
        uint64_t base;
      } u_time_step;
      u_time_step.base = 0;
      u_time_step.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_time_step.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_time_step.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_time_step.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_time_step.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_time_step.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_time_step.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_time_step.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->time_step = u_time_step.real;
      offset += sizeof(this->time_step);
      union {
        double real;
        uint64_t base;
      } u_command;
      u_command.base = 0;
      u_command.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_command.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_command.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_command.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_command.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_command.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_command.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_command.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->command = u_command.real;
      offset += sizeof(this->command);
      union {
        double real;
        uint64_t base;
      } u_p;
      u_p.base = 0;
      u_p.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_p.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_p.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_p.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_p.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_p.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_p.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_p.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->p = u_p.real;
      offset += sizeof(this->p);
      union {
        double real;
        uint64_t base;
      } u_i;
      u_i.base = 0;
      u_i.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_i.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_i.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_i.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_i.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_i.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_i.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_i.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->i = u_i.real;
      offset += sizeof(this->i);
      union {
        double real;
        uint64_t base;
      } u_d;
      u_d.base = 0;
      u_d.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_d.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_d.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_d.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_d.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_d.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_d.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_d.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->d = u_d.real;
      offset += sizeof(this->d);
      union {
        double real;
        uint64_t base;
      } u_i_clamp;
      u_i_clamp.base = 0;
      u_i_clamp.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_i_clamp.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_i_clamp.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_i_clamp.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_i_clamp.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_i_clamp.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_i_clamp.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_i_clamp.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->i_clamp = u_i_clamp.real;
      offset += sizeof(this->i_clamp);
      union {
        bool real;
        uint8_t base;
      } u_antiwindup;
      u_antiwindup.base = 0;
      u_antiwindup.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->antiwindup = u_antiwindup.real;
      offset += sizeof(this->antiwindup);
     return offset;
    }

    const char * getType(){ return "control_msgs/JointControllerState"; };
    const char * getMD5(){ return "987ad85e4756f3aef7f1e5e7fe0595d1"; };

  };

}
#endif