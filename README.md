# Filament-Nvidia-Physx-wxWidget-Integration
 A Demo showing Google Filament PBR Renderer Integration with Nvidia Physx Engine and using wxWidget as UI backend.
 The integration is using latest filament build with Nvidia Physx v4.1 and wxWidget 3.2.1 as a windowing framework.
### How to build:
Define the following environment variables that points to your installation of filament:<br/>
- <b>FILAMENT_INCLUDE_DIR:</b> should point to root directory where filament is installed. <br/>
- <b>FILAMENT_LIB_DIR:</b> should point to root directory where filament static libs is installed. <br/>
- <b>PHYSX_DIR:</b> should point to root directory where Nvidia Physx is installed. <br/>
- Download pre-built wxWidget for your version of VisualC++ and add it the visual studio project of the demo
see instructions here: https://docs.wxwidgets.org/3.2.1/plat_msw_binaries.html  <br/>
- Open Filament-wxWidget-Integration solution and build the app.<br/>
- All compiled materials and Textures are pachaged in this repo.<br/>
- The Physx integration is in separate branch,<b>Physx-Integration</b>.

The demo shows some animated, Physics-controlled, spheres, cubes and lights with textured and plain materials along with Controlable camera.
- Move the orbit camera using the mouse.
- Press SPACE to shoot a box
- Press ENTER to shoot a sphere
- Press F1 to Add a Pyramid Stack to the scene.
- Right click an object to pick it (Raycasting).
 
 <img src="https://github.com/mahmoudgalal/Filament-wxWidget-Integration/blob/main/screenshot/Filament-wxWidget%20Integration%20Demo.png"/>
<img src="https://github.com/mahmoudgalal/Filament-Physx-wxWidget-Integration/blob/PhysX-Integration/screenshot/screenshot2.png"/>
