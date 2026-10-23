//translate([3.1, 0.0, 0.5])    rotate([90, 0, 0]) import ("m5stick_colorometer_ada_v2_large_holes.stl");

//translate([3.1, 0.0, 1.5])    rotate([90, 0, 0]) spectrometer ();

height_pd1=12;
height_pd=12;

Screwdia=3.2;
 
$fn = 100; 
 w_m5=26.0;// width of m5stick
 h_m5=48.6;// length of m5stick pocket (base6); was 45
 dx=h_m5-45; // +X end grows by dx, the -X end stays
 t_m5=16; // thickness of m5stick
 wall=1.5; //wall of the body
 tol=0.5; //tolerance for printing
 floor_extra=2.25; // lowers the floor: 1.0 mm gap below the AS7341 board (board bottom Z -14.5, floor inner -15.5, outer -16.5)
 cover_z0=-16.5; // outer floor face (-14.25 - floor_extra)
 side_wall=2.0; // +Y and -Y wall thickness (was 1.5); the cavity stays 28.9 wide, so the wall grows outward
 side_gap=0.2; // clearance per side between the base (28.5 wide in Y) and the cavity
 cav_w=w_m5+tol+2+2*side_gap; // cavity width in Y = 28.9
 cover_w=cav_w+2*side_wall; // outer width in Y = 32.9
 hinge_x=32.1+dx;  // hinge axis along Y: 6.5 mm beyond the +X end (25.6), as in the iorodeo2 lid
 hinge_z=-13.5; // axis 3 mm above the floor face, so the knuckles are flush with the floor
 hinge_t=side_wall; // knuckle thickness along the axis = wall thickness, flush with the outer wall faces; the lid hinge part fills the rest
 hinge_gap=0.75; // clearance between knuckle and lid barrel
 cuv_y=0; cuv_z=-1.0; // cuvette axis in the cover frame
 part="both"; // "cover", "lid" or "both"


module enclosure_cover(){    
  difference(){
 union() {


     translate([dx/2,0,-floor_extra/2]) 
     cube ([h_m5+6.2,cover_w,w_m5+tol+2+floor_extra], center=true);  //cuvette holder, floor lowered, thick side walls

     hinge_double(); // hinge lugs on the -Z (floor) edge, flush with the side walls and the floor
     

 }
 
 union() {}
 translate([0,0,0])
   enclosure_screws(); 

     
     translate([0.6+dx/2,0,1.0-floor_extra/2]) 
     cube ([h_m5+5.2,cav_w,w_m5+tol+2+floor_extra], center=true);  // cavity, floor lowered; ends 0.1 mm past the +X face so the hinge lugs stay solid
 }
 }
 // translate([55.0,0,2]) 
// enclosure_cover();
 module enclosure_screws(){    
rotate([90,0,0])
translate([18,9,-20])
cylinder(d=2, h=10);// Side screw1
 
 
rotate([90,0,0])
translate([18,9,14])
cylinder(d=2, h=10);// Side screw2
 
rotate([0,90,0])
translate([2.0,10,-30])
cylinder(d=2, h=10);// bottom screw1
 

rotate([0,90,0])
translate([2.0,-10,-30])
cylinder(d=2, h=10);// Bottom screw2
     
rotate([90,0,0])
translate([18,-9,-20])
cylinder(d=2, h=10);// Side screw3
 
 
 rotate([90,0,0])
translate([18,-9,14])
cylinder(d=2, h=10);// Side screw4    


 }
 

 // one knuckle: axis along Y through the origin, arm runs back toward -X into the wall
 module hinge_single(){
     difference(){
 union(){
     rotate([-90,0,0]) cylinder(r=3,h=hinge_t); 
      translate([-11,0,-3])
     cube ([11,hinge_t,6]); 
 }
       translate([0,-0.1,0])
 rotate([-90,0,0]) cylinder(r=1,h=hinge_t+0.2); 
 }
 }

 module hinge_double(){
   translate([hinge_x,-cover_w/2,hinge_z])
   hinge_single();
   translate([hinge_x,cover_w/2-hinge_t,hinge_z])
   hinge_single();
     }

 // lid (door) closing the +X end. Cap dimensions from m5stick_colorometer_iorodeo2_atom7_lid_only_v3.scad:
 // cup d24 outside, d22 inside, 2 mm end wall, cup starts 8.5 mm before the hinge axis (2 mm before the body face)
 cap_od=23.5; cap_id=22; cap_start=8.5; cap_end=3; cap_wall=2;

 module lid(){
   y0=-cover_w/2+hinge_t+hinge_gap;
   y1=cover_w/2-hinge_t-hinge_gap;
   difference(){
 union() {
       // the wide barrel blends into the round cup end wall (no square plate, no step)
       hull(){
         translate([hinge_x,y0,hinge_z])
        rotate([-90,0,0]) cylinder(r=3,h=y1-y0); 
         translate([hinge_x-3,cuv_y,cuv_z])
        rotate([0,90,0]) cylinder(d=cap_od,h=6);
       }
     translate([hinge_x-cap_start,cuv_y,cuv_z])
     rotate([0,90,0])
     cylinder(d=cap_od, h=cap_start+cap_end);
 }
  translate([hinge_x,y0-0.1,hinge_z])
       rotate([-90,0,0]) cylinder(r=1.2,h=y1-y0+0.2); 
  translate([hinge_x-cap_start-0.1,cuv_y,cuv_z])
     rotate([0,90,0])
     cylinder(d=cap_id, h=cap_start+cap_end-cap_wall+0.1);
   }
   }

if (part=="cover" || part=="both") enclosure_cover();
if (part=="lid" || part=="both") lid();
 //translate([0, 0.0, -1]) 
   // enclosure_screws(); 
 
