json = {}

function json:include(includes)
	if includes:handle(self:name()) then
		defines {
			"JSON_DIAGNOSTICS=1",
			"HAS_NLOHMANN_JSON"
		}
		includedirs {
			path.join(ThirdPartyFolder(), "json", "single_include")
		}
	end
end

function json:link()
end

function json:use()
	
end

function json:name()
    return "json"
end

function json:project()
end
