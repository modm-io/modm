#ifndef _ROS_hector_mapping_HectorDebugInfo_h
#define _ROS_hector_mapping_HectorDebugInfo_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "hector_mapping/HectorIterData.h"

namespace hector_mapping
{

  class HectorDebugInfo : public ros::Msg
  {
    public:
      uint32_t iterData_length;
      typedef hector_mapping::HectorIterData _iterData_type;
      _iterData_type st_iterData;
      _iterData_type * iterData;

    HectorDebugInfo():
      iterData_length(0), iterData(NULL)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->iterData_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->iterData_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->iterData_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->iterData_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->iterData_length);
      for( uint32_t i = 0; i < iterData_length; i++){
      offset += this->iterData[i].serialize(outbuffer + offset);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint32_t iterData_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      iterData_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      iterData_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      iterData_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->iterData_length);
      if(iterData_lengthT > iterData_length)
        this->iterData = (hector_mapping::HectorIterData*)realloc(this->iterData, iterData_lengthT * sizeof(hector_mapping::HectorIterData));
      iterData_length = iterData_lengthT;
      for( uint32_t i = 0; i < iterData_length; i++){
      offset += this->st_iterData.deserialize(inbuffer + offset);
        memcpy( &(this->iterData[i]), &(this->st_iterData), sizeof(hector_mapping::HectorIterData));
      }
     return offset;
    }

    const char * getType(){ return "hector_mapping/HectorDebugInfo"; };
    const char * getMD5(){ return "4d33c0696c0c536f5c1447c260756674"; };

  };

}
#endif