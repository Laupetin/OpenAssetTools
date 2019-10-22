Crypto = {}

function Crypto:include()
	includedirs {
		path.join(ProjectFolder(), "Crypto")
	}
end

function Crypto:link()
    libtomcrypt:link()
	libtommath:link()
	salsa20:link()
	links {
		"Crypto"
	}
end

function Crypto:project()
	local folder = ProjectFolder();

	project "Crypto"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src"
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
