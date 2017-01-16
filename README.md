THOR ENGINE
	Simple 3D game engine for edutational purposes

USAGE
	- Animation:
		- Sample scene is prepared. Just press "2" for run loop or "1" for attack.
		- You can play around with the values in the component Animation in the panel inspector.
		- More animations can be added at the end of the Animation zone. Button "AddAnimation".
		- To see animation bones go to Debugging Tools -> Display -> GameObjects box.
		- Bones are not scaled, and in the sample scene are in a verly little scale so they overlap.
			To see the sample bones properly, click on animTest GameObject, right click on transform component -> reset.
		- There is another animation imported in the project called "simpleAnimation". See "Resources" to add it into the scene.
		- StreetEnvironment.FBX has an animation too, but its not supported yet since it has no bones.

	- File explorer:
		- File explorer is just a demo that could come very handy in the future. Files can't be edited from here,
			but all assets files can be seen.
	- Debugging tools:
		- Use debugging tools in the main menu for testing purposes

	- Camera movement:
		Use clicked mouse wheel to move camera position
		Use mouse right button to rotate camera
		Use mouse wheel to zoom in and out
		Double click on an item on the hierarchy to focus the camera
		on it

	- Resources:
		- All files in /Assets/ are loaded at te beggining of the execution.
		- If new files are addded in /Assets/, they will be imported after a while
		- If a texture is changed in /Assets/, it will be reimported after a while
			- The blue texture in Street_Environment IS NOT A BUG: it is for testing purposes.
			  Changing that texture file is an easy way to see the changes in the scene.
		- If a material is changed in the editor, it will be saved after a while.

		- To load an imported asset in the current open scene use the menu Assets->LoadImportedScene
		  in the main menu bar

		- WARNING: Do not erase nor modify the following files:
			- Any .meta file in /Assets/
			- /Library/ folder
			- /ProjectSettings/ folder or its files

			- If a .meta file or /Library/ file is erased, remove /Library/, all .meta files
			  and /ProjectSettings/Resources.JSON and all scenes.
	
	- Camera culling:
		- To test camera culling create a new camera in the menu Assets->Create->Camera

	- Mouse GameObject selection:
		- To test picking correctly, remove "Plane002" or any plane in scene. It causes trouble in
		  detecting the ray collision

	- GameObjects:
		- Can be deleted using "Delete" keyboard key. Yet this can cause problems sometimes
		- Can be multiple-selected using "Ctrl" + click in hierarchy

LICENCE

	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or
	distribute this software, either in source code form or as a compiled
	binary, for any purpose, commercial or non-commercial, and by any
	means.

	In jurisdictions that recognize copyright laws, the author or authors
	of this software dedicate any and all copyright interest in the
	software to the public domain. We make this dedication for the benefit
	of the public at large and to the detriment of our heirs and
	successors. We intend this dedication to be an overt act of
	relinquishment in perpetuity of all present and future rights to this
	software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

	For more information, please refer to <http://unlicense.org/>
