#ifndef _ROS_SERVICE_SetLightProperties_h
#define _ROS_SERVICE_SetLightProperties_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/ColorRGBA.h"

namespace gazebo_msgs
{

static const char SETLIGHTPROPERTIES[] = "gazebo_msgs/SetLightProperties";

  class SetLightPropertiesRequest : public ros::Msg
  {
    public:
      typedef const char* _light_name_type;
      _light_name_type light_name;
      typedef std_msgs::ColorRGBA _diffuse_type;
      _diffuse_type diffuse;
      typedef double _attenuation_constant_type;
      _attenuation_constant_type attenuation_constant;
      typedef double _attenuation_linear_type;
      _attenuation_linear_type attenuation_linear;
      typedef double _attenuation_quadratic_type;
      _attenuation_quadratic_type attenuation_quadratic;

    SetLightPropertiesRequest():
      light_name(""),
      diffuse(),
      attenuation_constant(0),
      attenuation_linear(0),
      attenuation_quadratic(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      uint32_t length_light_name = strlen(this->light_name);
      varToArr(outbuffer + offset, length_light_name);
      offset += 4;
      memcpy(outbuffer + offset, this->light_name, length_light_name);
      offset += length_light_name;
      offset += this->diffuse.serialize(outbuffer + offset);
      union {
        double real;
        uint64_t base;
      } u_attenuation_constant;
      u_attenuation_constant.real = this->attenuation_constant;
      *(outbuffer + offset + 0) = (u_attenuation_constant.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_attenuation_constant.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_attenuation_constant.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_attenuation_constant.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_attenuation_constant.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_attenuation_constant.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_attenuation_constant.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_attenuation_constant.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->attenuation_constant);
      union {
        double real;
        uint64_t base;
      } u_attenuation_linear;
      u_attenuation_linear.real = this->attenuation_linear;
      *(outbuffer + offset + 0) = (u_attenuation_linear.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_attenuation_linear.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_attenuation_linear.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_attenuation_linear.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_attenuation_linear.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_attenuation_linear.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_attenuation_linear.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_attenuation_linear.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->attenuation_linear);
      union {
        double real;
        uint64_t base;
      } u_attenuation_quadratic;
      u_attenuation_quadratic.real = this->attenuation_quadratic;
      *(outbuffer + offset + 0) = (u_attenuation_quadratic.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_attenuation_quadratic.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_attenuation_quadratic.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_attenuation_quadratic.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_attenuation_quadratic.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_attenuation_quadratic.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_attenuation_quadratic.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_attenuation_quadratic.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->attenuation_quadratic);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint32_t length_light_name;
      arrToVar(length_light_name, (inbuffer + offset));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_light_name; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_light_name-1]=0;
      this->light_name = (char *)(inbuffer + offset-1);
      offset += length_light_name;
      offset += this->diffuse.deserialize(inbuffer + offset);
      union {
        double real;
        uint64_t base;
      } u_attenuation_constant;
      u_attenuation_constant.base = 0;
      u_attenuation_constant.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_attenuation_constant.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_attenuation_constant.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_attenuation_constant.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_attenuation_constant.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_attenuation_constant.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_attenuation_constant.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_attenuation_constant.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->attenuation_constant = u_attenuation_constant.real;
      offset += sizeof(this->attenuation_constant);
      union {
        double real;
        uint64_t base;
      } u_attenuation_linear;
      u_attenuation_linear.base = 0;
      u_attenuation_linear.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_attenuation_linear.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_attenuation_linear.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_attenuation_linear.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_attenuation_linear.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_attenuation_linear.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_attenuation_linear.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_attenuation_linear.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->attenuation_linear = u_attenuation_linear.real;
      offset += sizeof(this->attenuation_linear);
      union {
        double real;
        uint64_t base;
      } u_attenuation_quadratic;
      u_attenuation_quadratic.base = 0;
      u_attenuation_quadratic.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_attenuation_quadratic.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_attenuation_quadratic.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_attenuation_quadratic.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_attenuation_quadratic.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_attenuation_quadratic.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_attenuation_quadratic.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_attenuation_quadratic.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->attenuation_quadratic = u_attenuation_quadratic.real;
      offset += sizeof(this->attenuation_quadratic);
     return offset;
    }

    const char * getType(){ return SETLIGHTPROPERTIES; };
    const char * getMD5(){ return "73ad1ac5e9e312ddf7c74f38ad843f34"; };

  };

  class SetLightPropertiesResponse : public ros::Msg
  {
    public:
      typedef bool _success_type;
      _success_type success;
      typedef const char* _status_message_type;
      _status_message_type status_message;

    SetLightPropertiesResponse():
      success(0),
      status_message("")
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_success;
      u_success.real = this->success;
      *(outbuffer + offset + 0) = (u_success.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->success);
      uint32_t length_status_message = strlen(this->status_message);
      varToArr(outbuffer + offset, length_status_message);
      offset += 4;
      memcpy(outbuffer + offset, this->status_message, length_status_message);
      offset += length_status_message;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_success;
      u_success.base = 0;
      u_success.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->success = u_success.real;
      offset += sizeof(this->success);
      uint32_t length_status_message;
      arrToVar(length_status_message, (inbuffer + offset));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_status_message; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_status_message-1]=0;
      this->status_message = (char *)(inbuffer + offset-1);
      offset += length_status_message;
     return offset;
    }

    const char * getType(){ return SETLIGHTPROPERTIES; };
    const char * getMD5(){ return "2ec6f3eff0161f4257b808b12bc830c2"; };

  };

  class SetLightProperties {
    public:
    typedef SetLightPropertiesRequest Request;
    typedef SetLightPropertiesResponse Response;
  };

}
#endif
