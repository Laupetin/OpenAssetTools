eigen = {}

function eigen:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "eigen", "Eigen")
		}
	end
end

function eigen:link()
end

function eigen:use()
	
end

function eigen:name()
    return "eigen"
end

function eigen:project()
end
