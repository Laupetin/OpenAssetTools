catch2 = {}

function catch2:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "catch2", "single_include")
		}
	end
end

function catch2:link()
end

function catch2:use()
	
end

function catch2:name()
    return "catch2"
end

function catch2:project()
end
