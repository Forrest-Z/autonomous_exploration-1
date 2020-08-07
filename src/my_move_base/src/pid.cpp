//
// Created by parallels on 4/18/20.
//

#include "../include/my_move_base/pid.h"
#include <math.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#define PI 3.1415926
#define length_from_baselink_frontsteering 0.62 //0.75*3/4
#define width_left_right_wheel 0.69
void Pid::ConfigInit() {
/**
    const char ConfigFile[]= "./src/my_move_base/Configuration/config.txt";
    Config configSettings(ConfigFile);
    p_vel_ = configSettings.Read("p_vel_",0.45);
    i_vel_ = configSettings.Read("i_vel_",0.05);
    p_rot_vel_ = configSettings.Read("p_rot_vel_",1);
    upper_limit_ = configSettings.Read("upper_limit_",0.3);
**/    
    //p_vel_ = 0.45;
    //i_vel_ = 0.05;
    //p_rot_vel_ = 1;
   // upper_limit_ = 0.1;

}
vector<double> Pid::SetVel(vector<vector<double>> baselinkposori,vector<vector<double>> frontsteeringposori,vector<double> goalpos){
    double vel = 0;// m/s
    double poserror = sqrt(pow((frontsteeringposori[0][0] - goalpos[0]),2) + pow((frontsteeringposori[0][1] - goalpos[1]),2));
    double rotationvel = 0;
    double reached = 0.0;
    bool goal_under_car = false;
    vector<double> res;
    res.push_back(vel);
    res.push_back(rotationvel);
    res.push_back(reached);
    cout<<" start pid ........................................"<<endl;
    //std::cout<<"pos error : "<<poserror<<endl;
    if (poserror >= pos_threshold_){
cout<<" poserror : "<<poserror<<endl;
        //sum_error_ = sum_error_ + poserror;
        vel  = 0.07;
        //cout<<" poserror * p_vel_ :"<< poserror * p_vel_ <<endl;
        //cout<<" sum_error_ * i_vel_ :"<< sum_error_ * i_vel_ <<endl;
       // cout<<"vel :"<<vel <<endl;
        if(vel >= upper_limit_){
            vel = upper_limit_;
        }else if(vel< 0){
            vel = 0;
        }
        // this vector represents the direction of x axis of front steering joint
        vector<double> referenceori ;// from current pos to goal pos
        referenceori.push_back(goalpos[0] - baselinkposori[0][0]);
        referenceori.push_back(goalpos[1] - baselinkposori[0][1]);

        //Eigen::Quaterniond q(frontsteeringposori[1][3] ,frontsteeringposori[1][0],frontsteeringposori[1][1],frontsteeringposori[1][2]);// w x y z
        //Eigen::Matrix3d rotationmatrix = q.toRotationMatrix();
        // this vector represents the direction from front steering joint postion to goal
        //Eigen::Vector2d vector_front_steering ;
        //vector_front_steering<<rotationmatrix(0,0),rotationmatrix(1,0);

        //calculate the angle between two vectors
        //double relative_angle_cos = (referenceori[0]*vector_front_steering(0)+referenceori[1]*vector_front_steering(1))/
               // (sqrt(pow(referenceori[0],2)+pow(referenceori[1],2))*sqrt(pow(vector_front_steering(0),2)+pow(vector_front_steering(1),2)));
       // double relative_angle = acos(relative_angle_cos);
        //if(relative_angle >=  1.53){count_keep_rotating_++ ;}
        //af - eb: calculate the cross production of twu vector to decide the relative direction
        //because their z value is always 0, it cross produciton can be simplified
        //double z_direction =vector_front_steering(0)*referenceori[1] - referenceori[0]*vector_front_steering(1);
        //if(z_direction <  0){//vector_front_steering is at the right side of the reference vector, error should be negative
        //    relative_angle = -relative_angle;
        //}
        //cout<<"relative angle : "<<relative_angle<<endl;
        // if the waypoint is under the car, it should be ignored
        double goal_center_distance = sqrt(pow(((baselinkposori[0][0]+frontsteeringposori[0][0])/2.0 - goalpos[0]),2)+pow(((baselinkposori[0][1]+frontsteeringposori[0][1])/2.0 - goalpos[1]),2));
        if(goal_center_distance <= 0.5){
            goal_under_car = true;
        }
	


	// ***********************
	//
	//
	//
	Eigen::Quaterniond q_bl(baselinkposori[1][3] ,baselinkposori[1][0],baselinkposori[1][1],baselinkposori[1][2]);// w x y z
        Eigen::Matrix3d rotationmatrix_bl = q_bl.toRotationMatrix();
	// this vector represents the direction from front steering joint postion to goal
        Eigen::Vector2d vector_base_link ;
        vector_base_link<<rotationmatrix_bl(0,0),rotationmatrix_bl(1,0);
        double relative_angle_cos_bl = (referenceori[0]*vector_base_link(0)+referenceori[1]*vector_base_link(1))/
                (sqrt(pow(referenceori[0],2)+pow(referenceori[1],2))*sqrt(pow(vector_base_link(0),2)+pow(vector_base_link(1),2)));
        double relative_angle_bl = acos(relative_angle_cos_bl);
        //if(relative_angle >=  1.53){count_keep_rotating_++ ;}
        //af - eb: calculate the cross production of twu vector to decide the relative direction
        //because their z value is always 0, it cross produciton can be simplified
        double z_direction_bl =vector_base_link(0)*referenceori[1] - referenceori[0]*vector_base_link(1);
        if(z_direction_bl >  0){//vector_front_steering is at the right side of the reference vector, error should be negative
            relative_angle_bl = -relative_angle_bl;
        }
	
	
	// **********************







	if(relative_angle_bl >= 2.355){//2.0(65) 2.2(53) 1.22(110) 1.39(100)
		cout<<"##############################greater than 2.355 , keep 2.355 "<<endl;
		relative_angle_bl = 2.355;
	}else if(relative_angle_bl <=  -2.355){
		cout<<"smaller than -2.355 , keep -2.355$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
		relative_angle_bl = -2.355;
	}else{
	
        // sum_error_rotation_ += relative_angle;
         // rotationvel = relative_angle_bl * p_rot_vel_;// + sum_error_rotation_ * i_rot_;
	}
	cout<<"relative_angle_bl"<<relative_angle_bl<<endl;
	if(relative_angle_bl > 1.57){
	cout<<"bainibnanbiajian"<<endl;
		relative_angle_bl = 3.14 - relative_angle_bl ;
		vel = -vel;
	
	}else if(relative_angle_bl < -1.57){
	cout<<"bainibnanbiajian"<<endl;
		relative_angle_bl = 3.14 + relative_angle_bl ;
		vel = -vel;
	}

	rotationvel = relative_angle_bl * p_rot_vel_;
         //sum_error_rotation_ += relative_angle;
         // rotationvel = relative_angle * p_rot_vel_ + sum_error_rotation_ * i_rot_;
      /*  if(rotationvel>=1.57){
             rotationvel = 1.57;         
         }else if(rotationvel <= -1.57){
             rotationvel = -1.57;
        
	 }*/

	//cout<<"sun_error_ratotation : "<<sum_error_rotation_<<endl;
	cout<<"rotationvel : "<<rotationvel<<endl;
        res[0] = vel;
	cout<<" vel !!!!!!!!! : "<<vel<<endl;
        res[1] = rotationvel;
	/**
        if(count_keep_rotating_ >= 15  ){//car gets stuck
            cout<<"car gets stuck ; skip to next point"<<endl;
            res[2] = 1.0;
            count_keep_rotating_ = 0;
            sum_error_ = 0;
	    //sum_error_rotation_ = 0;
        }**/


        if(goal_under_car == true){
            cout<<"*****goal under car; skip to next point"<<endl;
            res[2] = 1.0;
            count_keep_rotating_ = 0;
            //sum_error_ = 0;
	    //sum_error_rotation_ = 0;
        }
        return res;
    } else{// if reached
        res[2] = 1.0;
       // sum_error_ = 0;
	//sum_error_rotation_ = 0;
    }
    return res;
};



