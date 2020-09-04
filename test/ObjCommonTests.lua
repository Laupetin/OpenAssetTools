ObjCommonTests = {}

function ObjCommonTests:include()
    
end

function ObjCommonTests:link()
	if References:link("ObjCommonTests") then
		links "ObjCommonTests"
	end
end

function ObjCommonTests:use()
	
end

function ObjCommonTests:project()
	References:reset()
	local folder = TestFolder();

	project "ObjCommonTests"
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "SharedLib"
		language "C++"
		
		files {
			path.join(folder, "ObjCommonTests/**.h"), 
			path.join(folder, "ObjCommonTests/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjCommonTests"),
				path.join(BuildFolder(), "src/ZoneCode")
			}
		}
		
		self:include()
		ObjCommon:include()

		ObjCommon:link()
end
