$fn=360;

difference() {
    translate([0, 0, 16]){
        cube([38, 90, 30], true);
    }
    translate([0, 0, 17]){
        rotate([90, 0, 0]) {
            cylinder(76, d=34, center=true);
            translate([0, 5, 0]) {
                cube([34, 20, 76], true);
            }
        }
    }
}