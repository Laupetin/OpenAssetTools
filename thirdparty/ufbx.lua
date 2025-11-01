ufbx = {}

function ufbx:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "ufbx")
		}
	end
end

function ufbx:link()
end

function ufbx:use()
	
end

function ufbx:name()
    return "ufbx"
end

function ufbx:project()
end
