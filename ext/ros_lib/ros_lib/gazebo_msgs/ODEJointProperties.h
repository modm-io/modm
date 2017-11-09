#ifndef _ROS_gazebo_msgs_ODEJointProperties_h
#define _ROS_gazebo_msgs_ODEJointProperties_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace gazebo_msgs
{

  class ODEJointProperties : public ros::Msg
  {
    public:
      uint32_t damping_length;
      typedef double _damping_type;
      _damping_type st_damping;
      _damping_type * damping;
      uint32_t hiStop_length;
      typedef double _hiStop_type;
      _hiStop_type st_hiStop;
      _hiStop_type * hiStop;
      uint32_t loStop_length;
      typedef double _loStop_type;
      _loStop_type st_loStop;
      _loStop_type * loStop;
      uint32_t erp_length;
      typedef double _erp_type;
      _erp_type st_erp;
      _erp_type * erp;
      uint32_t cfm_length;
      typedef double _cfm_type;
      _cfm_type st_cfm;
      _cfm_type * cfm;
      uint32_t stop_erp_length;
      typedef double _stop_erp_type;
      _stop_erp_type st_stop_erp;
      _stop_erp_type * stop_erp;
      uint32_t stop_cfm_length;
      typedef double _stop_cfm_type;
      _stop_cfm_type st_stop_cfm;
      _stop_cfm_type * stop_cfm;
      uint32_t fudge_factor_length;
      typedef double _fudge_factor_type;
      _fudge_factor_type st_fudge_factor;
      _fudge_factor_type * fudge_factor;
      uint32_t fmax_length;
      typedef double _fmax_type;
      _fmax_type st_fmax;
      _fmax_type * fmax;
      uint32_t vel_length;
      typedef double _vel_type;
      _vel_type st_vel;
      _vel_type * vel;

    ODEJointProperties():
      damping_length(0), damping(NULL),
      hiStop_length(0), hiStop(NULL),
      loStop_length(0), loStop(NULL),
      erp_length(0), erp(NULL),
      cfm_length(0), cfm(NULL),
      stop_erp_length(0), stop_erp(NULL),
      stop_cfm_length(0), stop_cfm(NULL),
      fudge_factor_length(0), fudge_factor(NULL),
      fmax_length(0), fmax(NULL),
      vel_length(0), vel(NULL)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->damping_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->damping_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->damping_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->damping_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->damping_length);
      for( uint32_t i = 0; i < damping_length; i++){
      union {
        double real;
        uint64_t base;
      } u_dampingi;
      u_dampingi.real = this->damping[i];
      *(outbuffer + offset + 0) = (u_dampingi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_dampingi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_dampingi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_dampingi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_dampingi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_dampingi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_dampingi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_dampingi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->damping[i]);
      }
      *(outbuffer + offset + 0) = (this->hiStop_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->hiStop_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->hiStop_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->hiStop_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->hiStop_length);
      for( uint32_t i = 0; i < hiStop_length; i++){
      union {
        double real;
        uint64_t base;
      } u_hiStopi;
      u_hiStopi.real = this->hiStop[i];
      *(outbuffer + offset + 0) = (u_hiStopi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_hiStopi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_hiStopi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_hiStopi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_hiStopi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_hiStopi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_hiStopi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_hiStopi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->hiStop[i]);
      }
      *(outbuffer + offset + 0) = (this->loStop_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->loStop_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->loStop_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->loStop_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->loStop_length);
      for( uint32_t i = 0; i < loStop_length; i++){
      union {
        double real;
        uint64_t base;
      } u_loStopi;
      u_loStopi.real = this->loStop[i];
      *(outbuffer + offset + 0) = (u_loStopi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_loStopi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_loStopi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_loStopi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_loStopi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_loStopi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_loStopi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_loStopi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->loStop[i]);
      }
      *(outbuffer + offset + 0) = (this->erp_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->erp_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->erp_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->erp_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->erp_length);
      for( uint32_t i = 0; i < erp_length; i++){
      union {
        double real;
        uint64_t base;
      } u_erpi;
      u_erpi.real = this->erp[i];
      *(outbuffer + offset + 0) = (u_erpi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_erpi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_erpi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_erpi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_erpi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_erpi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_erpi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_erpi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->erp[i]);
      }
      *(outbuffer + offset + 0) = (this->cfm_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->cfm_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->cfm_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->cfm_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->cfm_length);
      for( uint32_t i = 0; i < cfm_length; i++){
      union {
        double real;
        uint64_t base;
      } u_cfmi;
      u_cfmi.real = this->cfm[i];
      *(outbuffer + offset + 0) = (u_cfmi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_cfmi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_cfmi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_cfmi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_cfmi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_cfmi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_cfmi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_cfmi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->cfm[i]);
      }
      *(outbuffer + offset + 0) = (this->stop_erp_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->stop_erp_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->stop_erp_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->stop_erp_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->stop_erp_length);
      for( uint32_t i = 0; i < stop_erp_length; i++){
      union {
        double real;
        uint64_t base;
      } u_stop_erpi;
      u_stop_erpi.real = this->stop_erp[i];
      *(outbuffer + offset + 0) = (u_stop_erpi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_stop_erpi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_stop_erpi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_stop_erpi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_stop_erpi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_stop_erpi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_stop_erpi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_stop_erpi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->stop_erp[i]);
      }
      *(outbuffer + offset + 0) = (this->stop_cfm_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->stop_cfm_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->stop_cfm_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->stop_cfm_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->stop_cfm_length);
      for( uint32_t i = 0; i < stop_cfm_length; i++){
      union {
        double real;
        uint64_t base;
      } u_stop_cfmi;
      u_stop_cfmi.real = this->stop_cfm[i];
      *(outbuffer + offset + 0) = (u_stop_cfmi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_stop_cfmi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_stop_cfmi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_stop_cfmi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_stop_cfmi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_stop_cfmi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_stop_cfmi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_stop_cfmi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->stop_cfm[i]);
      }
      *(outbuffer + offset + 0) = (this->fudge_factor_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->fudge_factor_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->fudge_factor_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->fudge_factor_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->fudge_factor_length);
      for( uint32_t i = 0; i < fudge_factor_length; i++){
      union {
        double real;
        uint64_t base;
      } u_fudge_factori;
      u_fudge_factori.real = this->fudge_factor[i];
      *(outbuffer + offset + 0) = (u_fudge_factori.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_fudge_factori.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_fudge_factori.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_fudge_factori.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_fudge_factori.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_fudge_factori.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_fudge_factori.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_fudge_factori.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->fudge_factor[i]);
      }
      *(outbuffer + offset + 0) = (this->fmax_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->fmax_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->fmax_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->fmax_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->fmax_length);
      for( uint32_t i = 0; i < fmax_length; i++){
      union {
        double real;
        uint64_t base;
      } u_fmaxi;
      u_fmaxi.real = this->fmax[i];
      *(outbuffer + offset + 0) = (u_fmaxi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_fmaxi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_fmaxi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_fmaxi.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_fmaxi.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_fmaxi.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_fmaxi.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_fmaxi.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->fmax[i]);
      }
      *(outbuffer + offset + 0) = (this->vel_length >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->vel_length >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (this->vel_length >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (this->vel_length >> (8 * 3)) & 0xFF;
      offset += sizeof(this->vel_length);
      for( uint32_t i = 0; i < vel_length; i++){
      union {
        double real;
        uint64_t base;
      } u_veli;
      u_veli.real = this->vel[i];
      *(outbuffer + offset + 0) = (u_veli.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_veli.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_veli.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_veli.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_veli.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_veli.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_veli.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_veli.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->vel[i]);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      uint32_t damping_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      damping_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      damping_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      damping_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->damping_length);
      if(damping_lengthT > damping_length)
        this->damping = (double*)realloc(this->damping, damping_lengthT * sizeof(double));
      damping_length = damping_lengthT;
      for( uint32_t i = 0; i < damping_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_damping;
      u_st_damping.base = 0;
      u_st_damping.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_damping.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_damping.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_damping.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_damping.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_damping.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_damping.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_damping.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_damping = u_st_damping.real;
      offset += sizeof(this->st_damping);
        memcpy( &(this->damping[i]), &(this->st_damping), sizeof(double));
      }
      uint32_t hiStop_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      hiStop_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      hiStop_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      hiStop_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->hiStop_length);
      if(hiStop_lengthT > hiStop_length)
        this->hiStop = (double*)realloc(this->hiStop, hiStop_lengthT * sizeof(double));
      hiStop_length = hiStop_lengthT;
      for( uint32_t i = 0; i < hiStop_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_hiStop;
      u_st_hiStop.base = 0;
      u_st_hiStop.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_hiStop.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_hiStop.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_hiStop.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_hiStop.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_hiStop.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_hiStop.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_hiStop.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_hiStop = u_st_hiStop.real;
      offset += sizeof(this->st_hiStop);
        memcpy( &(this->hiStop[i]), &(this->st_hiStop), sizeof(double));
      }
      uint32_t loStop_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      loStop_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      loStop_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      loStop_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->loStop_length);
      if(loStop_lengthT > loStop_length)
        this->loStop = (double*)realloc(this->loStop, loStop_lengthT * sizeof(double));
      loStop_length = loStop_lengthT;
      for( uint32_t i = 0; i < loStop_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_loStop;
      u_st_loStop.base = 0;
      u_st_loStop.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_loStop.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_loStop.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_loStop.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_loStop.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_loStop.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_loStop.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_loStop.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_loStop = u_st_loStop.real;
      offset += sizeof(this->st_loStop);
        memcpy( &(this->loStop[i]), &(this->st_loStop), sizeof(double));
      }
      uint32_t erp_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      erp_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      erp_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      erp_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->erp_length);
      if(erp_lengthT > erp_length)
        this->erp = (double*)realloc(this->erp, erp_lengthT * sizeof(double));
      erp_length = erp_lengthT;
      for( uint32_t i = 0; i < erp_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_erp;
      u_st_erp.base = 0;
      u_st_erp.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_erp.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_erp.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_erp.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_erp.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_erp.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_erp.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_erp.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_erp = u_st_erp.real;
      offset += sizeof(this->st_erp);
        memcpy( &(this->erp[i]), &(this->st_erp), sizeof(double));
      }
      uint32_t cfm_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      cfm_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      cfm_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      cfm_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->cfm_length);
      if(cfm_lengthT > cfm_length)
        this->cfm = (double*)realloc(this->cfm, cfm_lengthT * sizeof(double));
      cfm_length = cfm_lengthT;
      for( uint32_t i = 0; i < cfm_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_cfm;
      u_st_cfm.base = 0;
      u_st_cfm.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_cfm.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_cfm.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_cfm.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_cfm.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_cfm.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_cfm.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_cfm.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_cfm = u_st_cfm.real;
      offset += sizeof(this->st_cfm);
        memcpy( &(this->cfm[i]), &(this->st_cfm), sizeof(double));
      }
      uint32_t stop_erp_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      stop_erp_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      stop_erp_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      stop_erp_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->stop_erp_length);
      if(stop_erp_lengthT > stop_erp_length)
        this->stop_erp = (double*)realloc(this->stop_erp, stop_erp_lengthT * sizeof(double));
      stop_erp_length = stop_erp_lengthT;
      for( uint32_t i = 0; i < stop_erp_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_stop_erp;
      u_st_stop_erp.base = 0;
      u_st_stop_erp.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_stop_erp.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_stop_erp.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_stop_erp.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_stop_erp.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_stop_erp.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_stop_erp.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_stop_erp.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_stop_erp = u_st_stop_erp.real;
      offset += sizeof(this->st_stop_erp);
        memcpy( &(this->stop_erp[i]), &(this->st_stop_erp), sizeof(double));
      }
      uint32_t stop_cfm_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      stop_cfm_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      stop_cfm_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      stop_cfm_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->stop_cfm_length);
      if(stop_cfm_lengthT > stop_cfm_length)
        this->stop_cfm = (double*)realloc(this->stop_cfm, stop_cfm_lengthT * sizeof(double));
      stop_cfm_length = stop_cfm_lengthT;
      for( uint32_t i = 0; i < stop_cfm_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_stop_cfm;
      u_st_stop_cfm.base = 0;
      u_st_stop_cfm.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_stop_cfm.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_stop_cfm.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_stop_cfm.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_stop_cfm.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_stop_cfm.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_stop_cfm.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_stop_cfm.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_stop_cfm = u_st_stop_cfm.real;
      offset += sizeof(this->st_stop_cfm);
        memcpy( &(this->stop_cfm[i]), &(this->st_stop_cfm), sizeof(double));
      }
      uint32_t fudge_factor_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      fudge_factor_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      fudge_factor_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      fudge_factor_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->fudge_factor_length);
      if(fudge_factor_lengthT > fudge_factor_length)
        this->fudge_factor = (double*)realloc(this->fudge_factor, fudge_factor_lengthT * sizeof(double));
      fudge_factor_length = fudge_factor_lengthT;
      for( uint32_t i = 0; i < fudge_factor_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_fudge_factor;
      u_st_fudge_factor.base = 0;
      u_st_fudge_factor.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_fudge_factor.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_fudge_factor.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_fudge_factor.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_fudge_factor.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_fudge_factor.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_fudge_factor.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_fudge_factor.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_fudge_factor = u_st_fudge_factor.real;
      offset += sizeof(this->st_fudge_factor);
        memcpy( &(this->fudge_factor[i]), &(this->st_fudge_factor), sizeof(double));
      }
      uint32_t fmax_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      fmax_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      fmax_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      fmax_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->fmax_length);
      if(fmax_lengthT > fmax_length)
        this->fmax = (double*)realloc(this->fmax, fmax_lengthT * sizeof(double));
      fmax_length = fmax_lengthT;
      for( uint32_t i = 0; i < fmax_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_fmax;
      u_st_fmax.base = 0;
      u_st_fmax.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_fmax.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_fmax.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_fmax.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_fmax.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_fmax.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_fmax.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_fmax.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_fmax = u_st_fmax.real;
      offset += sizeof(this->st_fmax);
        memcpy( &(this->fmax[i]), &(this->st_fmax), sizeof(double));
      }
      uint32_t vel_lengthT = ((uint32_t) (*(inbuffer + offset))); 
      vel_lengthT |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1); 
      vel_lengthT |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2); 
      vel_lengthT |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3); 
      offset += sizeof(this->vel_length);
      if(vel_lengthT > vel_length)
        this->vel = (double*)realloc(this->vel, vel_lengthT * sizeof(double));
      vel_length = vel_lengthT;
      for( uint32_t i = 0; i < vel_length; i++){
      union {
        double real;
        uint64_t base;
      } u_st_vel;
      u_st_vel.base = 0;
      u_st_vel.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_st_vel.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_st_vel.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_st_vel.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_st_vel.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_st_vel.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_st_vel.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_st_vel.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->st_vel = u_st_vel.real;
      offset += sizeof(this->st_vel);
        memcpy( &(this->vel[i]), &(this->st_vel), sizeof(double));
      }
     return offset;
    }

    const char * getType(){ return "gazebo_msgs/ODEJointProperties"; };
    const char * getMD5(){ return "1b744c32a920af979f53afe2f9c3511f"; };

  };

}
#endif