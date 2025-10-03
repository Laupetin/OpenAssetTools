webview = {}

function webview:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "webview/core/include"),
			path.join(self:msWebviewDir(), "build/native/include")
		}
	end
end

function webview:link(links)

	if os.host() == "windows" then
		links:add("WebView2LoaderStatic")
		
		filter "platforms:x86"
			libdirs {
				path.join(self:msWebviewDir(), "build/native/x86")
			}
		filter {}
		filter "platforms:x64"
			libdirs {
				path.join(self:msWebviewDir(), "build/native/x64")
			}
		filter {}
	end

	links:add(self:name())
end

function webview:use()
	
end

function webview:name()
	return "webview"
end

function webview:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files { 
			path.join(folder, "webview/core/include/**.h"),
			path.join(folder, "webview/core/include/**.hh"),
			path.join(folder, "webview/core/src/**.cc")
		}
		
		defines {
			"WEBVIEW_STATIC"
		}

		if os.host() == "windows" then
			self:installWebview2()
		end

		filter { "system:linux", "action:gmake" }
  			buildoptions { "`pkg-config --cflags gtk4 webkitgtk-6.0`" }
  			linkoptions { "`pkg-config --libs gtk4 webkitgtk-6.0`" }
		filter {}
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end

function webview:msWebviewDir()
	return path.join(BuildFolder(), "thirdparty/ms-webview2")
end

function webview:installWebview2()
	local version = "1.0.3485.44"
	local webviewDir = self:msWebviewDir()
	local versionFile = path.join(webviewDir, "ms-webview2.txt")
	local nuspecFile = path.join(webviewDir, "Microsoft.Web.WebView2.nuspec")
	local nupkgFile = path.join(webviewDir, "microsoft.web.webview2.nupkg.zip")
	local url = "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/" .. version

	if not os.isdir(webviewDir) then
		os.mkdir(webviewDir)
	end

	local installedVersion = io.readfile(versionFile)
	if installedVersion == version and os.isfile(nuspecFile) then
		return
	end

	function progress(total, current)		
		local ratio = current / total;
		ratio = math.min(math.max(ratio, 0), 1);
		local percent = math.floor(ratio * 100);
		io.write("\rDownload progress (" .. percent .. "%/100%)")
	end

	print("Downloading Microsoft.Web.WebView2 " .. version .. "...")
	local result_str, response_code = http.download(url, nupkgFile, {
		progress = progress
	})

	io.write("\n")
	
	if result_str ~= "OK" then
		premake.error("Failed to download Microsoft.Web.WebView2")
	end

	-- local hash = string.sha1(io.readfile(nupkgFile))
	-- print(hash)

	print("Extracting Microsoft.Web.WebView2 " .. version .. "...")
	zip.extract(nupkgFile, webviewDir)

	os.remove(nupkgFile)

	io.writefile(versionFile, version)
end
