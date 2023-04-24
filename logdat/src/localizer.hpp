#ifndef LOCALIZER_HPP
#define LOCALIZER_HPP

#define LOCALIZER_VER				"localizer, localizer_property for SnowRemoval_Update: 2023/03/07"
#define LOCALIZER_SNAME				"localizer"
#define REMOTE_LOCALIZER_SNAME		"remote_localizer"

// +++++++ 2023年の実験では、以下の２つはコメントアウト ++++++
//#define Localizer_SnowRemoval_Version		1	// 以前のlocalizerデータを再生して他のプログラムで使用できるようにする
//#define Use_GIAJ_Localizer					1	// 以前のGNSSデータが使えるように、Localizerで国土地理院の計算式でENZ座標へ変換

#define _CORRET_DRIFT_ORIENTATION				1	// 地磁気センサを利用して、停止時の方位補正
#define Calc_RepresentativePoint				1	// 車両代表点を計算
#define Replace_First_initYaw					1	// 最初の2m程度の直進後に、Yawを置き換える（ただし、前進する条件つき）

typedef struct
{
	bool status;
	double estPos[ 3 ];		// 推定した位置
	double estAng[ 3 ];		// 推定した方位

	double estVel[ 3 ];
	double estVel_xy;
	double estAngvel[ 3 ];
#ifdef Localizer_SnowRemoval_Version
	bool dir;		// 前後退のフラグに使用(前進:true, 後退:false)
#else
	int dir;
#endif
	double imu_offset;
	double estYaw_gnss;
#ifdef Localizer_SnowRemoval_Version
#else
	int posStatus;		// FIX or FLOAT, etc
#endif
} localizer;

#ifdef Localizer_SnowRemoval_Version
#else
typedef struct {
	char ver[ 256 ];
} localizer_property;
#endif

#endif
