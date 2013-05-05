if selection.count > 0 then (
	 output_name = getSaveFileName caption:"illEditor Object Transforms" types:"illEditor Object Transforms (*.txt)|*.txt|All Files (*.*)|*.*|"
	
	 if output_name != undefined then (
		output_file = createfile output_name
		
		--output number of objects
		--format "%\n" selection.count to:output_file
		
		--first find extents
		extentsMin = [9999999.0, 9999999.0, 9999999.0]
		extentsMax = [0.0, 0.0, 0.0]
		 
		for i in selection do (
			if classof i == TargetObject then (
				continue
			)
			
			yzSwap =  matrix3 [1,0,0] [0,0,-1] [0,1,0] [0,0,0]
			illTransform = i.objectTransform
						
			if classof i == OmniLight then (				
				illTransform = illTransform * yzSwap
			)
			else if classof i == FreeSpot or classof i == TargetSpot then (				
				illTransform = illTransform * yzSwap
			)
			else (					
				yzSwap2 =  matrix3 [1,0,0] [0,0,1] [0,-1,0] [0,0,0]
				illTransform = yzSwap2 * illTransform * yzSwap
				
				box = nodeGetBoundingBox i (transMatrix i.objectTransform.translationpart)
				
				if box[1][1] + illTransform.translationpart.x > extentsMax.x then extentsMax.x = box[1][1] + illTransform.translationpart.x
				if box[1][1] + illTransform.translationpart.x < extentsMin.x then extentsMin.x = box[1][1] + illTransform.translationpart.x
					
				if box[2][1] + illTransform.translationpart.x > extentsMax.x then extentsMax.x = box[2][1] + illTransform.translationpart.x
				if box[2][1] + illTransform.translationpart.x < extentsMin.x then extentsMin.x = box[2][1] + illTransform.translationpart.x
					
				
				if box[1][3] + illTransform.translationpart.y > extentsMax.y then extentsMax.y = box[1][3] + illTransform.translationpart.y
				if box[1][3] + illTransform.translationpart.y < extentsMin.y then extentsMin.y = box[1][3] + illTransform.translationpart.y
					
				if box[2][3] + illTransform.translationpart.y > extentsMax.y then extentsMax.y = box[2][3] + illTransform.translationpart.y
				if box[2][3] + illTransform.translationpart.y < extentsMin.y then extentsMin.y = box[2][3] + illTransform.translationpart.y
					
				
				if -box[2][2] + illTransform.translationpart.z  > extentsMax.z then extentsMax.z = -box[2][2] + illTransform.translationpart.z
				if -box[2][2] + illTransform.translationpart.z  < extentsMin.z then extentsMin.z = -box[2][2] + illTransform.translationpart.z
					
				if -box[1][2] + illTransform.translationpart.z > extentsMax.z then extentsMax.z = -box[1][2] + illTransform.translationpart.z
				if -box[1][2] + illTransform.translationpart.z < extentsMin.z then extentsMin.z = -box[1][2] + illTransform.translationpart.z
			)
		)
		 
		for i in selection do (
			if classof i == TargetObject then (
				continue
			)
			
			yzSwap =  matrix3 [1,0,0] [0,0,-1] [0,1,0] [0,0,0]
			illTransform = i.objectTransform
			
			if classof i == OmniLight then (
				format "__PointLight__\n" to:output_file
				
				illTransform = illTransform * yzSwap
			)
			else if classof i == FreeSpot or classof i == TargetSpot then (
				format "__SpotLight__\n" to:output_file
				
				illTransform = illTransform * yzSwap
			)
			else (
				--output name, omitting last 3 characters to get around Max duplicate name numbering, a slight hack
				--Be sure the object names always have some redundant 3 characters at the end, usually from being cloned
				format "%\n" (substring i.name 1 (i.name.count-3)) to:output_file
					
				yzSwap2 =  matrix3 [1,0,0] [0,0,1] [0,-1,0] [0,0,0]
				illTransform = yzSwap2 * illTransform * yzSwap
			)
			
			--output 4x3 transform matrix in column major readable format
			format "% % % %\n" illTransform.row1[1] illTransform.row2[1] illTransform.row3[1] (illTransform.row4[1] - extentsMin.x) to:output_file
			format "% % % %\n" illTransform.row1[2] illTransform.row2[2] illTransform.row3[2] (illTransform.row4[2] - extentsMin.y) to:output_file
			format "% % % %\n" illTransform.row1[3] illTransform.row2[3] illTransform.row3[3] (illTransform.row4[3] - extentsMin.z) to:output_file
			
			if classof i == OmniLight or classof i == FreeSpot or classof i == TargetSpot then (
				--light color
				format "% % %\n" (i.rgb.r/255.0) (i.rgb.g/255.0) (i.rgb.b/255.0) to:output_file
				
				--light intensity
				format "%\n" i.multiplier to:output_file
				
				--use specular
				specularEnabled = getUserProp i "specular"
				
				if specularEnabled == true or specularEnabled == undefined then (
					format "%\n" 1 to:output_file
				)
				else (
					format "%\n" 0 to:output_file
				)
				
				--atten start
				format "%\n" i.farAttenStart to:output_file
				
				--atten end
				format "%\n" i.farAttenEnd to:output_file						
				
				if classof i == FreeSpot or classof i == TargetSpot then (
					--cone start
					format "%\n" (cos(i.hotspot * 0.5)) to:output_file
				
					--cone end
					format "%\n" (cos(i.falloff * 0.5)) to:output_file
				)
			)			
			else (
				--output AABB relative to transform position
				box = nodeGetBoundingBox i (transMatrix i.objectTransform.translationpart)
				
				--swap y and z, and negate the z			
				format "% % %\n" box[1][1] box[1][3] -box[2][2] to:output_file
				format "% % %\n" box[2][1] box[2][3] -box[1][2] to:output_file
				
				--materials
				if getNumSubMtls i.material > 0 then (
					
					--number of materials
					format "%\n" (getNumSubMtls i.material) to:output_file
					
					--material names
					for subMat = 1 to (getNumSubMtls i.material) do (
						format "%\n" (getSubMtl i.material subMat).name to:output_file
					)
				)
				else (
					--number of materials
					format "1\n" to:output_file
					
					--material names
					format "%\n" i.material.name to:output_file
				)
			)
		)
		
		close output_file
		edit output_name
	)
)
else (
	rollout thing "You have no objects selected. Only the selections are exported." ( )
)