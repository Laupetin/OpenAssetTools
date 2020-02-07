Crypto = {}

function Crypto:include()
	if References:include("Crypto") then
		includedirs {
			path.join(ProjectFolder(), "Crypto")
		}
	end
end

function Crypto:link()
	if References:link("Crypto") then
		libtomcrypt:link()
		libtommath:link()
		salsa20:link()
		links "Crypto"
	end
end

function Crypto:use()
	
end

function Crypto:project()
	References:reset()
	local folder = ProjectFolder();

	project "Crypto"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Crypto/**.h"), 
			path.join(folder, "Crypto/**.cpp") 
		}
		
		self:include()
		libtomcrypt:include()
		libtommath:include()
        salsa20:include()
end
