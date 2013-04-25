if selection.count > 0 then (
	for i in selection do (
		output_name = getSaveFileName caption:"illEditor Object Transforms" types:"illEditor Object Transforms (*.txt)|*.txt|All Files (*.*)|*.*|"
	
		if output_name != undefined then (
			output_file = createfile output_name
		)
	)
)
	
	
	 
		
		--output number of objects
		format "%\n" selection.count to:output_file
		
		




curObject  = $
curObjectName = $.name
curObjectPos = curObject.position

rollout roll_FBXport "FBXport" width:232 height:160
(

	
	checkbox chk_ReMat "Rename material to match object name?" pos:[8,40] width:216 height:16 checked:true
	label lbl1 "Export Path" pos:[8,64] width:64 height:16
	edittext edt_ExportPath "" pos:[0,80] width:224 height:16 text:exportfolderPath
	label lbl5 "Material Name" pos:[8,8] width:72 height:16 
	edittext edt_MaterialName "" pos:[0,24] width:224 height:16 text:materialname
	button btn_Export "Export" pos:[48,112] width:128 height:32


	
	fn ExportFolder = (
		if doesFileExist exportfolderPath  == false do (-- Create Exports folder if it doesn't exsist
			makedir exportfolderPath
		) 
	)
	
	fn MaterialNameMatch = (
		if curObject.material.name != CurObject.name do (--Changes the material name to match the object name if it doesn't already
			curObject.material.name = CurObject.name
			print "material name changed to match object name"
		)
	)
	
	fn MoveandExport = (
		curObject.pos = [0,0,0] -- Moves the object to the world 0 0 0 node
		exportFile exportpathFull selectedOnly:true -- Exports the currently selected object as fbx
		move curObject curObjectPos -- moves the object back to original position
	)
	
	on btn_export pressed do (
		ExportFolder()
		MaterialNameMatch()
		MoveandExport()
	)
)
createdialog roll_FBXport
