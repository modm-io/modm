#ifndef _ROS_SERVICE_GetRecoveryInfo_h
#define _ROS_SERVICE_GetRecoveryInfo_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "geometry_msgs/PoseStamped.h"
#include "ros/time.h"
#include "nav_msgs/Path.h"

namespace hector_nav_msgs
{

static const char GETRECOVERYINFO[] = "hector_nav_msgs/GetRecoveryInfo";

  class GetRecoveryInfoRequest : public ros::Msg
  {
    public:
      typedef ros::Time _request_time_type;
      _request_time_type request_time;
      typedef double _request_radius_type;
      _request_radius_type request_radius;

    GetRecoveryInfoRequest():
      request_time(),
      request_radius(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->request_time.sec >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->request_time.sec >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->request_time.sec >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->request_time.sec >> (8 * 3)) & 0xFF;
      offset += sizeof(this->request_time.sec);
      *(outbuffer + offset + 0) = (this->request_time.nsec >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->request_time.nsec >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->request_time.nsec >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->request_time.nsec >> (8 * 3)) & 0xFF;
      offset += sizeof(this->request_time.nsec);
      union {
        double real;
        uint64_t base;
      } u_request_radius;
      u_request_radius.real = this->request_radius;
      *(outbuffer + offset + 0) = (u_request_radius.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_request_radius.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_request_radius.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_request_radius.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_request_radius.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_request_radius.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_request_radius.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_request_radius.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->request_radius);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      this->request_time.sec =  ((uint32_t) (*(inbuffer + offset)));
      this->request_time.sec |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->request_time.sec |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      this->request_time.sec |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      offset += sizeof(this->request_time.sec);
      this->request_time.nsec =  ((uint32_t) (*(inbuffer + offset)));
      this->request_time.nsec |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      this->request_time.nsec |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      this->request_time.nsec |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      offset += sizeof(this->request_time.nsec);
      union {
        double real;
        uint64_t base;
      } u_request_radius;
      u_request_radius.base = 0;
      u_request_radius.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_request_radius.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_request_radius.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_request_radius.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_request_radius.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_request_radius.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_request_radius.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_request_radius.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->request_radius = u_request_radius.real;
      offset += sizeof(this->request_radius);
     return offset;
    }

    const char * getType(){ return GETRECOVERYINFO; };
    const char * getMD5(){ return "3916a0c55958d5dd43204cd2fe5608f6"; };

  };

  class GetRecoveryInfoResponse : public ros::Msg
  {
    public:
      typedef nav_msgs::Path _trajectory_radius_entry_pose_to_req_pose_type;
      _trajectory_radius_entry_pose_to_req_pose_type trajectory_radius_entry_pose_to_req_pose;
      typedef geometry_msgs::PoseStamped _radius_entry_pose_type;
      _radius_entry_pose_type radius_entry_pose;
      typedef geometry_msgs::PoseStamped _req_pose_type;
      _req_pose_type req_pose;

    GetRecoveryInfoResponse():
      trajectory_radius_entry_pose_to_req_pose(),
      radius_entry_pose(),
      req_pose()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->trajectory_radius_entry_pose_to_req_pose.serialize(outbuffer + offset);
      offset += this->radius_entry_pose.serialize(outbuffer + offset);
      offset += this->req_pose.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->trajectory_radius_entry_pose_to_req_pose.deserialize(inbuffer + offset);
      offset += this->radius_entry_pose.deserialize(inbuffer + offset);
      offset += this->req_pose.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return GETRECOVERYINFO; };
    const char * getMD5(){ return "a93581be8e34e3c09aeafc6b9b990ad5"; };

  };

  class GetRecoveryInfo {
    public:
    typedef GetRecoveryInfoRequest Request;
    typedef GetRecoveryInfoResponse Response;
  };

}
#endif
