# V3 Colorimeter (cuvette) enclosure

Cuvette-based colorimeter variant using an M5StickC Plus and an AS7341 board.

| Part | Source | STL |
|---|---|---|
| Base (cuvette holder + M5 pocket) | `OpenScad/m5stick_colorometer_ada_base6.scad` | `STLs/m5stick_colorometer_ada_base6.stl` |
| Hinged cover and lid (v13) | `OpenScad/m5stick_colorometer_ada_cover_v13.scad` | `STLs/m5stick_colorometer_ada_cover_v13_cover.stl`, `..._lid.stl` |
| Enclosure sleeve over the lid | `FreeCAD/m5spectrometer_cuvette.FCStd` | `STLs/Enclosure_Sleeve.stl` |

`FreeCAD/Colorimeter_Cover.FCStd` is the cover model.

## Notes
- Screws are M2.5x6. All screw holes in the base are O2.7 (`hole_d`), matching the sleeve.
- The sleeve is a plain cuboid with 1.5 mm walls, 1.0 mm gap to the lid, open +X end, and a -Y slot for the USB-C and Grove plugs.
- The base STL exports with an OpenSCAD "may not be a valid 2-manifold" warning; check it in your slicer.
