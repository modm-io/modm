#ifndef _ROS_SERVICE_QueryTrajectoryState_h
#define _ROS_SERVICE_QueryTrajectoryState_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "ros/time.h"

namespace control_msgs
{

static const char QUERYTRAJECTORYSTATE[] = "control_msgs/QueryTrajectoryState";

  class QueryTrajectoryStateRequest : public ros::Msg
  {
    public:
      typedef ros::Time _time_type;
      _time_type time;

    QueryTrajectoryStateRequest():
      time()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->time.sec >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->time.sec >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->time.sec >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->time.sec >> (8 * 3)) & 0xFF;
      offset += sizeof(this->time.sec);
      *(outbuffer + offset + 0) = (this->time.nsec >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->time.nsec >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->time.nsec >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->time.nsec >> (8 * 3)) & 0xFF;
      offset += sizeof(this->time.nsec);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      this->time.sec =  ((uint32_t) (*(inbuffer + offset)));
      this->time.sec |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->time.sec |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      this->time.sec |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      offset += sizeof(this->time.sec);
      this->time.nsec =  ((uint32_t) (*(inbuffer + offset)));
      this->time.nsec |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->time.nsec |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      this->time.nsec |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      offset += sizeof(this->time.nsec);
     return offset;
    }

    const char * getType(){ return QUERYTRAJECTORYSTATE; };
    const char * getMD5(){ return "556a4fb76023a469987922359d08a844"; };

  };

  class QueryTrajectoryStateResponse : public ros::Msg
  {
    public:
      uint32_t name_length;
      typedef char* _name_type;
      _name_type st_name;
      _name_type * name;
      uint32_t position_length;
      typedef double _position_type;
      _position_type st_position;
      _position_type * position;
      uint32_t velocity_length;
      typedef double _velocity_type;
      _velocity_type st_velocity;
      _velocity_type * velocity;
      uint32_t acceleration_length;
      typedef double _acceleration_type;
      _acceleration_type st_acceleration;
      _acceleration_type * acceleration;

    QueryTrajectoryStateResponse():
      name_length(0), name(NULL),
      position_length(0), position(NULL),
      velocity_length(0), velocity(NULL),
      acceleration_length(0), acceleration(NULL)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->name_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->name_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->name_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->name_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->name_length);
      for( uint32_t i = 0; i < name_length; i++){
      uint32_t length_namei = strlen(this->name[i]);
      varToArr(outbuffer + offset, length_namei);
      offset += 4;
      memcpy(outbuffer + offset, this->name[i], length_namei);
      offset += length_namei;
      }
      *(outbuffer + offset + 0) = (this->position_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->position_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->position_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->position_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->position_length);
      for( uint32_t i = 0; i < position_length; i++){
      union {
        double real;
        uint64_t base;
      } u_positioni;
      u_positioni.real = this->position[i];
      *(outbuffer + offset + 0) = (u_positioni.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_positioni.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_positioni.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_positioni.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_positioni.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_positioni.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_positioni.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_positioni.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->position[i]);
      }
      *(outbuffer + offset + 0) = (this->velocity_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->velocity_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->velocity_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->velocity_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->velocity_length);
      for( uint32_t i = 0; i < velocity_length; i++){
      union {
        double real;
        uint64_t base;
      } u_velocityi;
      u_velocityi.real = this->velocity[i];
      *(outbuffer + offset + 0) = (u_velocityi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_velocityi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_velocityi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_velocityi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_velocityi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_velocityi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_velocityi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_velocityi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->velocity[i]);
      }
      *(outbuffer + offset + 0) = (this->acceleration_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->acceleration_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->acceleration_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->acceleration_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->acceleration_length);
      for( uint32_t i = 0; i < acceleration_length; i++){
      union {
        double real;
        uint64_t base;
      } u_accelerationi;
      u_accelerationi.real = this->acceleration[i];
      *(outbuffer + offset + 0) = (u_accelerationi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_accelerationi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_accelerationi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_accelerationi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_accelerationi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_accelerationi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_accelerationi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_accelerationi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->acceleration[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint32_t name_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      name_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      name_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      name_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->name_length);
      if(name_lengthT > name_length)
        this->name = (char**)realloc(this->name, name_lengthT * sizeof(char*));
      name_length = name_lengthT;
      for( uint32_t i = 0; i < name_length; i++){
      uint32_t length_st_name;
      arrToVar(length_st_name, (inbuffer + offset));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_st_name; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_st_name-1]=0;
      this->st_name = (char *)(inbuffer + offset-1);
      offset += length_st_name;
        memcpy( &(this->name[i]), &(this->st_name), sizeof(char*));
      }
      uint32_t position_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      position_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      position_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      position_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->position_length);
      if(position_lengthT > position_length)
        this->position = (double*)realloc(this->position, position_lengthT * sizeof(double));
      position_length = position_lengthT;
      for( uint32_t i = 0; i < position_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_position;
      u_st_position.base = 0;
      u_st_position.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_position.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_position.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_position.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_position.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_position.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_position.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_position.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_position = u_st_position.real;
      offset += sizeof(this->st_position);
        memcpy( &(this->position[i]), &(this->st_position), sizeof(double));
      }
      uint32_t velocity_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      velocity_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      velocity_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      velocity_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->velocity_length);
      if(velocity_lengthT > velocity_length)
        this->velocity = (double*)realloc(this->velocity, velocity_lengthT * sizeof(double));
      velocity_length = velocity_lengthT;
      for( uint32_t i = 0; i < velocity_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_velocity;
      u_st_velocity.base = 0;
      u_st_velocity.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_velocity.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_velocity.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_velocity.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_velocity.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_velocity.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_velocity.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_velocity.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_velocity = u_st_velocity.real;
      offset += sizeof(this->st_velocity);
        memcpy( &(this->velocity[i]), &(this->st_velocity), sizeof(double));
      }
      uint32_t acceleration_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      acceleration_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      acceleration_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      acceleration_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->acceleration_length);
      if(acceleration_lengthT > acceleration_length)
        this->acceleration = (double*)realloc(this->acceleration, acceleration_lengthT * sizeof(double));
      acceleration_length = acceleration_lengthT;
      for( uint32_t i = 0; i < acceleration_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_acceleration;
      u_st_acceleration.base = 0;
      u_st_acceleration.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_acceleration.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_acceleration.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_acceleration.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_acceleration.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_acceleration.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_acceleration.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_acceleration.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_acceleration = u_st_acceleration.real;
      offset += sizeof(this->st_acceleration);
        memcpy( &(this->acceleration[i]), &(this->st_acceleration), sizeof(double));
      }
     return offset;
    }

    const char * getType(){ return QUERYTRAJECTORYSTATE; };
    const char * getMD5(){ return "1f1a6554ad060f44d013e71868403c1a"; };

  };

  class QueryTrajectoryState {
    public:
    typedef QueryTrajectoryStateRequest Request;
    typedef QueryTrajectoryStateResponse Response;
  };

}
#endif
