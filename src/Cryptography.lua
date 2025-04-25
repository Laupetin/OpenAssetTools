Cryptography = {}

function Cryptography:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Cryptography")
		}
	end
end

function Cryptography:link(links)
	links:add(self:name())
	links:linkto(libtomcrypt)
	links:linkto(libtommath)
	links:linkto(salsa20)
end

function Cryptography:use()
	
end

function Cryptography:name()
	return "Cryptography"
end

function Cryptography:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Cryptography/**.h"), 
			path.join(folder, "Cryptography/**.cpp") 
		}
		
		self:include(includes)
		libtomcrypt:include(includes)
		libtommath:include(includes)
        salsa20:include(includes)
end
