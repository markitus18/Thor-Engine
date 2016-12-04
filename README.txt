THOR ENGINE
	Simple 3D game engine for edutational purposes

USAGE
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
			- The red texture in Street_Environment IS NOT A BUG: it is for testing purposes.
			  Changing that texture file is an easy way to see the changes in the scene.
		- If a material is changed in the editor, it will be saved after a while
		  Not handling FBX modifications yet.

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
