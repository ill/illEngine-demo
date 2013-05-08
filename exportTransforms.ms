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
			if classof i == TargetObject or i.name == "__VolumeLight__" or i.name == "__Start__" then (
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
			yzSwap2 =  matrix3 [1,0,0] [0,0,1] [0,-1,0] [0,0,0]
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
				if i.name == "__VolumeLight__" then (
					format "__VolumeLight__\n" to:output_file
				)
				else if i.name == "__Start__" then (
					format "__Start__\n" to:output_file
				)
				else (				
					--output name, omitting last 3 characters to get around Max duplicate name numbering, a slight hack
					--Be sure the object names always have some redundant 3 characters at the end, usually from being cloned
					format "%\n" (substring i.name 1 (i.name.count-3)) to:output_file
				)
				
				illTransform = yzSwap2 * illTransform * yzSwap
			)
			
			--output 4x3 transform matrix in column major readable format
			format "% % % %\n" illTransform.row1[1] illTransform.row2[1] illTransform.row3[1] (illTransform.row4[1] - extentsMin.x) to:output_file
			format "% % % %\n" illTransform.row1[2] illTransform.row2[2] illTransform.row3[2] (illTransform.row4[2] - extentsMin.y) to:output_file
			format "% % % %\n" illTransform.row1[3] illTransform.row2[3] illTransform.row3[3] (illTransform.row4[3] - extentsMin.z) to:output_file
			
			if i.name == "__Start__" then (
				continue
			)
			
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
				
				if i.name == "__VolumeLight__" then (
					--volume light parameters
					
					--light color					
					lightColorR = getUserProp i "colorR"
					
					if lightColorR == undefined then (
						format "% " 1 to:output_file
					)
					else (
						format "% " lightColorR to:output_file
					)
					
					lightColorG = getUserProp i "colorG"
					
					if lightColorG == undefined then (
						format "% " 1 to:output_file
					)
					else (
						format "% " lightColorG to:output_file
					)
					
					lightColorB = getUserProp i "colorB"
					
					if lightColorB == undefined then (
						format "%" 1 to:output_file
					)
					else (
						format "%" lightColorB to:output_file
					)
					
					format "\n" intensity to:output_file
					
					--light intensity
					intensity = getUserProp i "intensity"
					
					if intensity == undefined then (
						format "%\n" 1 to:output_file
					)
					else (
						format "%\n" intensity to:output_file
					)
					
					--use specular
					specularEnabled = getUserProp i "specular"
					
					if specularEnabled == true or specularEnabled == undefined then (
						format "1\n" to:output_file
					)
					else (
						format "0\n" to:output_file
					)
					
					--point or directional
					directional = getUserProp i "directional"
					
					if directional == undefined then (
						directional = false
						format "0\n" to:output_file
					)
					else (
						format "1\n" to:output_file
					)
					
					--light vector
					lightVectorX = getUserProp i "vectorX"
					
					if lightVectorX == undefined then (
						lightVectorX = illTransform.row4[1] - extentsMin.x
					)
					
					lightVectorY = getUserProp i "vectorY"
					
					if lightVectorY == undefined then (
						lightVectorY = illTransform.row4[2] - extentsMin.y
					)
					
					lightVectorZ = getUserProp i "vectorZ"
					
					if lightVectorZ == undefined then (
						lightVectorZ = illTransform.row4[3] - extentsMin.z
					)
					
					lightVector = [lightVectorX, lightVectorY, lightVectorZ]
					
					if directional == true then (
						lightVector = normalize lightVector
					)
					
					format "% % %\n" lightVector[1] lightVector[2] lightVector[3] to:output_file
					
					--number of "planes"  This should be 12 or less
					numPlanes = 0
										
					format "%\n" (polyop.getNumFaces i) to:output_file
					
					for face = 1 to (polyop.getNumFaces i) do (
						--get 3 points
						/*test = polyop.getVert i (polyop.getFaceVerts i face)[1]
						test2 =(polyop.getFaceVerts i face)[1]
						
						format "% %\n" test test2 to:output_file*/

						pos = #(polyop.getVert i (polyop.getFaceVerts i face)[3], 
							polyop.getVert i (polyop.getFaceVerts i face)[2],
							polyop.getVert i (polyop.getFaceVerts i face)[1])
						
						/*pos = #([1, 2, 3], 
							[1, 2, 3],
							[1, 2, 3])*/
						
						swap pos[1].y pos[1].z
						pos[1].z = -pos[1].z
						pos[1] -= extentsMin
						
						swap pos[2].y pos[2].z
						pos[2].z = -pos[2].z
						pos[2] -= extentsMin
						
						swap pos[3].y pos[3].z
						pos[3].z = -pos[3].z
						pos[3] -= extentsMin
						
						--http://easycalculation.com/analytical/cartesian-plane-equation.php (I really don't wanna think right now)
						
						/*a = (By-Ay)(Cz-Az)-(Cy-Ay)(Bz-Az)
						b = (Bz-Az)(Cx-Ax)-(Cz-Az)(Bx-Ax)
						c = (Bx-Ax)(Cy-Ay)-(Cx-Ax)(By-Ay)
						d = -(aAx+bAy+cAz).*/
							
						--format "POS (%) (%) (%)\n" pos[1] pos[2] pos[3] to:output_file
							
						a = (pos[2].y-pos[1].y) * (pos[3].z-pos[1].z)-(pos[3].y-pos[1].y) * (pos[2].z - pos[1].z)
						b = (pos[2].z-pos[1].z) * (pos[3].x-pos[1].x)-(pos[3].z-pos[1].z) * (pos[2].x - pos[1].x)
						c = (pos[2].x-pos[1].x) * (pos[3].y-pos[1].y)-(pos[3].x-pos[1].x) * (pos[2].y - pos[1].y)
						d= -(a * pos[1].x + b * pos[1].y + c * pos[1].z)
						
						denom = 1.0 / length([a, b, c])
						a *= denom
						b *= denom
						c *= denom
						d *= denom
						
						format "% % % %\n" a b c d to:output_file
						
						/*
						--face normal
						faceNorm = polyop.getFaceNormal i face						
						format "Plane % % %\n" faceNorm[1] faceNorm[2] faceNorm[3] to:output_file						
						swap faceNorm[2] faceNorm[3]
						faceNorm[3] = -faceNorm[3]						
						format "Plane % % %\n" faceNorm[1] faceNorm[2] faceNorm[3] to:output_file						
						faceNorm = -faceNorm						
						format "Plane % % %\n" faceNorm[1] faceNorm[2] faceNorm[3] to:output_file
						
						--plane distance
						pos = polyop.getVert i (polyop.getFaceVerts i face)[1]						
						format "Plane  Pos % % %\n" pos[1] pos[2] pos[3] to:output_file						
						swap pos[2] pos[3]
						pos[3] = -pos[3]						
						format "Plane Ext % % % Pos % % %\n" extentsMin[1] extentsMin[2] extentsMin[3] pos[1] pos[2] pos[3] to:output_file						
						pos -= extentsMin						
						format "Plane % % % Pos % % %\n" faceNorm[1] faceNorm[2] faceNorm[3] pos[1] pos[2] pos[3] to:output_file						
						dist = dot pos faceNorm
						
						--plane
						format "% % % %\n" faceNorm[1] faceNorm[2] faceNorm[3] dist to:output_file*/
						
						--face start attenuation reciprocal (stored as material id - 1 trollol, I couldn't think of a better way to store per face data)
						faceAtten = ((polyop.getFaceMatId i face) - 1.0)
						
						if(faceAtten == 0) then (
							format "100\n" to:output_file
						)
						else (
							format "%\n" (1.0 / faceAtten) to:output_file
						)
					)
				)
				else (				
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
		)
		
		close output_file
		edit output_name
	)
)
else (
	rollout thing "You have no objects selected. Only the selections are exported." ( )
)