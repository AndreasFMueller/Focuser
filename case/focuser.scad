/*
 * focuser.stl -- case for the focuser 
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */

board_width = 60.96 + 2 * 0.2;
board_height = 41.28 + 2 * 0.2;

w = board_width + 2 * 8;
h = board_height;
thickness = 1.5;
d = 10;

wall = 2;
W = w + 2 * wall;
H = h + 2 * wall;
D = d + 2 * wall;

dl = wall + 2 + thickness;
du = wall + d + 2;

module screwhole() {
    cylinder(r = 1.2, h = 20, center = true);
}

module bottom() {
	difference() {
		translate([0, 0, dl/2]) cube([W, H, dl], center = true);
		union() {
			translate([0, 0, wall + 2 + 1.5])
				cube([board_width, board_height, 3], center = true);
            translate([0, 0, wall + 5])
				cube([board_width - 1, board_height - 1, 10], center = true);
            // screw holes
            translate([board_width/2 + 6, 0, 0])
                screwhole();
            translate([-board_width/2 - 6, board_height/2 - 2, 0])
                screwhole();
            translate([-board_width/2 - 6, -board_height/2 + 2, 0])
                screwhole();
//            translate([0, 0, 5 + wall + 2 + 1.5 - 1])
//            difference() {
//                cube([W + 2, H + 2, 10], center = true);
//                cube([W - wall, H - wall, 12], center = true);
//            }
		}
	}
}

module screw() {
    union() {
        cylinder(r = 1.7, h = 30, center = true, $fn = 20);
        translate([0, 0, d-1])
            cylinder(r1 = 0, r2 = 3, h = 3, $fn = 20);
        translate([0, 0, d+2])
            cylinder(r = 3, h = 10, $fn = 20);
    }
}

module top() {
    difference() {
        union() {
            difference() {
                translate([0, 0, du / 2]) cube([W, H, du], center = true);
                union() {
                    translate([-3, 0, d/2 + 1])
                        cube([w - 6, h, d + 2], center = true);
                    // USB connector
                    translate([w / 2, 10.5, 10 - d + 2])
                        cube([20, 14, 20], center = true);
                    // motor cable opening
                    translate([w/2, -10, du/2 -d + 2.5])
                        cube([20, 5, 6], center = true);
                    // motor connector
                    //translate([w / 2, -10, du/2 - d + 2])
                    //    cube([20, 12, du], center = true);
                    // power cable opening
                    translate([w / 2 - 2, 12 + 4, 3.5 - d])
                        cube([10, 10, 20], center = true);
                }
            }
            translate([-board_width/2 - 6, board_height/2 - 2, du/2])
                cube([7, 7, du], center = true);
            translate([-board_width/2 - 6, -board_height/2 + 2, du/2])
                cube([7, 7, du], center = true);
            // board clamps bottom
            translate([0, -board_height/2, du/2])
                cube([4, 2, du], center = true);
            // board clamps top
            translate([board_width / 3, board_height/2, du/2])
                cube([4, 2, du], center = true);
            translate([-board_width / 3, board_height/2, du/2])
                cube([4, 2, du], center = true);
        }
        union() {
            // screw holes
            translate([board_width/2 + 6, 0, 0]) screw();
            translate([-board_width/2 - 6, board_height/2 - 2, 0]) screw();
            translate([-board_width/2 - 6, -board_height/2 + 2, 0]) screw();
            // LED holes
            translate([6, -16, 0])
                cylinder(r = 2.0, h = 30, center = true, $fn = 20);
            translate([27, -2, 0])
                cylinder(r = 2.0, h = 30, center = true, $fn = 20);
            translate([-11.5, 10.5, 0])
                cylinder(r = 2.0, h = 30, center = true, $fn = 20);
        }
    }
}

translate([0, 50, d + 4]) rotate([180, 0, 0]) top();
bottom();