# V3 Colorimeter (cuvette) enclosure

Cuvette-based colorimeter variant using an M5StickC Plus and an AS7341 board.

| Part | Source | STL |
|---|---|---|
| Base (cuvette holder + M5 pocket) | `OpenScad/m5stick_colorometer_ada_base6.scad` | `STLs/m5stick_colorometer_ada_base6.stl` |
| Cover with print-in-place hinge (v14, no screws) | `OpenScad/m5stick_colorometer_ada_cover_v14.scad` | `STLs/m5stick_colorometer_ada_cover_v14_print.stl` (cover + folded-open lid, print as is) |
| Hinged cover and lid (v13) | `OpenScad/m5stick_colorometer_ada_cover_v13.scad` | `STLs/m5stick_colorometer_ada_cover_v13_cover.stl`, `..._lid.stl` |
| Enclosure sleeve over the lid | `FreeCAD/m5spectrometer_cuvette.FCStd` | `STLs/Enclosure_Sleeve.stl` |

`FreeCAD/Colorimeter_Cover.FCStd` is the cover model.

## Notes
- Screws are M2.5x6. All screw holes in the base are O2.7 (`hole_d`), matching the sleeve.
- The sleeve is a plain cuboid with 1.5 mm walls, 1.0 mm gap to the lid, open +X end, and a -Y slot for the USB-C and Grove plugs.
- The base STL exports with an OpenSCAD "may not be a valid 2-manifold" warning; check it in your slicer.
- v14 hinge: the lid has an integral O1.8 pin in O2.6 lug holes (`pin_clear` = 0.4). Print the cover floor-down with the lid folded open; no supports. If the hinge fuses raise `pin_clear` to 0.5, if loose lower it to 0.3. Untested print.
- base6: the lid screw holes and lid screw windows on the -Y face are removed, and the buttonB cutout runs out through the +X end so no floating wall piece is left.
- `STLs/m5stick_colorometer_ada_cover_v14_cover.stl` and `..._lid.stl` are the v14 parts in the closed pose (for the FreeCAD assembly); print `..._v14_print.stl`.
