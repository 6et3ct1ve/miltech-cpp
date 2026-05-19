#pragma once

struct DropPoint { double fireX, fireY; };

struct DroneState { float xd, yd, zd, targetX, targetY, attackSpeed, accelerationPath; char ammo_name[32]; };

DropPoint compute_drop_point ( DroneState dronState, bool& ok );
