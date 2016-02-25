// -*-c++-*-
#ifndef __CAMERAS_H__
#define __CAMERAS_H__

// Local lan camera in our office.
#define LAN_CAMERA_DESK_URL     "rtsp://192.168.1.40:554/media/4/video/1?commport=80&Warped=False"
#define LAN_CAMERA_DESK_USER    "admin"
#define LAN_CAMERA_DESK_PASS    "admin"

#define TYCO_PAUSE1_ISSUE_URL    "rtsp://192.168.1.66/media/7/video/1?commport=80&Warped=False"
#define TYCO_PAUSE1_ISSUE_USER   "support"
#define TYCO_PAUSE1_ISSUE_PASS   "troppus"

#define TYCO_CAM1_URL    "rtsp://213.106.250.126:554/media/10/video/1?commport=80&Warped=False"
#define TYCO_CAM1_USER   "admin"
#define TYCO_CAM1_PASS   "admin"

#define TYCO_CAM2_URL    "rtsp://108.179.8.109:554/media/9/video/1?commport=80&Warped=False"
#define TYCO_CAM2_USER   "demo"
#define TYCO_CAM2_PASS   "demo"

#define TYCO_CAM3_URL    "rtsp://192.168.1.220/media/10/video/1?commport=80&Warped=False"
#define TYCO_CAM3_USER   "admin"
#define TYCO_CAM3_PASS   "admin"

#define TYCO_ESPN_URL    "rtsp://108.179.8.109:554/media/5/video/1?commport=80&Warped=False"
#define TYCO_ESPN_USER   "demo"
#define TYCO_ESPN_PASS   "demo"

// This is the camera all tests will use
#define CAMERA_URL   LAN_CAMERA_DESK_URL
#define CAMERA_USER  LAN_CAMERA_DESK_USER
#define CAMERA_PASS  LAN_CAMERA_DESK_PASS

#endif //__CAMERAS_H__
