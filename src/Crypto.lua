Crypto = {}

function Crypto:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Crypto")
		}
	end
end

function Crypto:link(links)
	links:add(self:name())
	links:linkto(libtomcrypt)
	links:linkto(libtommath)
	links:linkto(salsa20)
end

function Crypto:use()
	
end

function Crypto:name()
	return "Crypto"
end

function Crypto:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Crypto/**.h"), 
			path.join(folder, "Crypto/**.cpp") 
		}
		
		self:include(includes)
		libtomcrypt:include(includes)
		libtommath:include(includes)
        salsa20:include(includes)
end
