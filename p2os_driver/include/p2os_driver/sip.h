/*
 *  P2OS for ROS
 *  Copyright (C) 2009
 *     David Feil-Seifer, Brian Gerkey, Kasper Stoy,
 *      Richard Vaughan, & Andrew Howard
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _SIP_H
#define _SIP_H

#include <limits.h>
#include <stdint.h>
#include <p2os_driver/p2os.h>
#include <boost/assign/list_of.hpp>
typedef struct ArmJoint
{
	char speed;
	unsigned char home;
	unsigned char min;
	unsigned char centre;
	unsigned char max;
	unsigned char ticksPer90;
} ArmJoint;

enum PlayerGripperStates {
    PLAYER_GRIPPER_STATE_OPEN = 1,
    PLAYER_GRIPPER_STATE_CLOSED,
    PLAYER_GRIPPER_STATE_MOVING,
    PLAYER_GRIPPER_STATE_ERROR
};

enum PlayerActArrayStates {
    PLAYER_ACTARRAY_ACTSTATE_IDLE = 1,
    PLAYER_ACTARRAY_ACTSTATE_MOVING,
    PLAYER_ACTARRAY_ACTSTATE_STALLED
};

class SIP
{
  private:
    int PositionChange( unsigned short, unsigned short );
    int param_idx; // index of our robot's data in the parameter table

  public:
    // these values are returned in every standard SIP
    bool lwstall, rwstall;
	unsigned char  motors_enabled, sonar_flag;
    unsigned char status, battery, sonarreadings, analog, digin, digout;
    unsigned short ptu, compass, timer, rawxpos;
    unsigned short rawypos, frontbumpers, rearbumpers;
    short angle, lvel, rvel, control;
    unsigned short *sonars;
    int xpos, ypos;
    int x_offset,y_offset,angle_offset;
    std::string odom_frame_id;
    std::string base_link_frame_id;

	 double pstd_dev_speed, pstd_dev_angular_speed;

    ///
    //boost::array<double, 36> pose_cov, twist_cov;
    //    double pose_cov[36], twist_cov[36];

    // these values are returned in a CMUcam serial string extended SIP
    // (in host byte-order)
    unsigned short blobmx, blobmy;	// Centroid
    unsigned short blobx1, blobx2, bloby1, bloby2;	// Bounding box
    unsigned short blobarea, blobconf;	// Area and confidence
    unsigned int	 blobcolor;

    // This value is filled by ParseGyro()
    int32_t gyro_rate;

    // This information comes from the ARMpac and ARMINFOpac packets
    bool armPowerOn, armConnected;
    bool armJointMoving[6];
    unsigned char armJointPos[6];
    double armJointPosRads[6];
    unsigned char armJointTargetPos[6];
    char *armVersionString;
    unsigned char armNumJoints;
    ArmJoint *armJoints;

    // Need this value to calculate approx position of lift when in between up
    // and down
    double lastLiftPos;

    //Timestamping SIP packets
    //double timeStandardSIP, timeGyro, timeSERAUX, timeArm;

    /* returns 0 if Parsed correctly otherwise 1 */
    void ParseStandard( unsigned char *buffer );
    void ParseSERAUX( unsigned char *buffer );
    void ParseGyro(unsigned char* buffer);
    void ParseArm (unsigned char *buffer);
    void ParseArmInfo (unsigned char *buffer);
    void Print();
    void PrintSonars();
    void PrintArm ();
    void PrintArmInfo ();
    void FillStandard(ros_p2os_data_t* data);

	 void setTwistCov(double pstd_dev_speed, double pstd_dev_angular_speed);
   //  void setOdomCov( boost::array<double, 36> pose_cov,
	// 	     boost::array<double, 36> twist_cov);
    //void FillSERAUX(player_p2os_data_t* data);
    //void FillGyro(player_p2os_data_t* data);
    //void FillArm(player_p2os_data_t* data);

    SIP(int idx) :
            param_idx(idx), sonarreadings(0), sonars(NULL),
            xpos(0), ypos(0), x_offset(0), y_offset(0), angle_offset(0),
            blobmx(0), blobmy(0), blobx1(0), blobx2(0), bloby1(0), bloby2(0),
            blobarea(0), blobconf(0), blobcolor(0),
            armPowerOn(false), armConnected(false), armVersionString(NULL),
            armNumJoints(0), armJoints(NULL),
            lastLiftPos(0.0f)
    {
        for (int i = 0; i < 6; ++i)
        {
            armJointMoving[i] = false;
            armJointPos[i] = 0;
            armJointPosRads[i] = 0;
            armJointTargetPos[i] = 0;
        }
	// pose_cov = boost::assign::list_of
	//   (1e-3) (0)    (0)   (0)   (0)   (0)
	//   (0)    (1e-3) (0)   (0)   (0)   (0)
	//   (0)    (0)    (1e6) (0)   (0)   (0)
	//   (0)    (0)    (0)   (1e6) (0)   (0)
	//   (0)    (0)    (0)   (0)   (1e6) (0)
	//   (0)    (0)    (0)   (0)   (0)   (1e3);
	//
	// twist_cov = boost::assign::list_of
	//   (1e-3) (0)    (0)   (0)   (0)   (0)
	//   (0)    (1e-3) (0)   (0)   (0)   (0)
	//   (0)    (0)    (1e6) (0)   (0)   (0)
	//   (0)    (0)    (0)   (1e6) (0)   (0)
	//   (0)    (0)    (0)   (0)   (1e6) (0)
	//   (0)    (0)    (0)   (0)   (0)   (1e3) ;

    }

    ~SIP(void)
    {
        delete[] sonars;
    }
};

#endif
