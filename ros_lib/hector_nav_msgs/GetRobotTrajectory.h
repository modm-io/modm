#ifndef _ROS_SERVICE_GetRobotTrajectory_h
#define _ROS_SERVICE_GetRobotTrajectory_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "nav_msgs/Path.h"

namespace hector_nav_msgs
{

static const char GETROBOTTRAJECTORY[] = "hector_nav_msgs/GetRobotTrajectory";

  class GetRobotTrajectoryRequest : public ros::Msg
  {
    public:

    GetRobotTrajectoryRequest()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
     return offset;
    }

    const char * getType(){ return GETROBOTTRAJECTORY; };
    const char * getMD5(){ return "d41d8cd98f00b204e9800998ecf8427e"; };

  };

  class GetRobotTrajectoryResponse : public ros::Msg
  {
    public:
      typedef nav_msgs::Path _trajectory_type;
      _trajectory_type trajectory;

    GetRobotTrajectoryResponse():
      trajectory()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->trajectory.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->trajectory.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return GETROBOTTRAJECTORY; };
    const char * getMD5(){ return "c7bd40129c5786fc26351edbd33b8d33"; };

  };

  class GetRobotTrajectory {
    public:
    typedef GetRobotTrajectoryRequest Request;
    typedef GetRobotTrajectoryResponse Response;
  };

}
#endif
