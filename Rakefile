require 'fileutils'
require 'json'
require 'colorize'
require 'pathname'

task :default => [:setup]

#task :setup => [:getSource, :buildSource, :setupLinks]
task :setup => [ :buildSource, :setupLinks]

# Answers true if the directory did not exist and had to be created
def ensureDirExists(path)
	if !File.directory?(path)
		FileUtils.mkdir_p(path)
		return true
	end
	return false
end

# Fork and exec the given command
# Couldn't get hg to print share stdout any other way
def executeShellCommand(cmd)
	pid = fork do
		exec cmd
	end

	Process.wait pid
end

def loadJsonFile(filename)
	file = File.open(filename, "r")
	contents = file.read()
	file.close()

	return JSON.parse(contents)
end

################################################################################
#	Source Retrieval Phase
################################################################################
rakeDir = Pathname.new(__FILE__).dirname
vendorDir = rakeDir + 'vendor'

task :getSource do
	pkgSpec = loadJsonFile("package.json")

	# Make sure the vendor directory exists
	ensureDirExists(vendorDir)
	Dir.chdir(vendorDir)

	dependencies = pkgSpec["dependencies"]
	if dependencies == nil
		puts("No dependencies found".red)
		next
	end

	dependencies.each do |name, spec|
		# Always move back to the base of the vendor lib
		Dir.chdir(vendorDir)

		# Figure out what type of vcs we should use for the dependency
		repoType = spec["type"] ? spec["type"] : "git"
		repoUrl = spec["repo"]
		if !repoUrl
			puts("Dependency \"#{name}\" does not have a valid repo url".red)
			return
		end

		# Clone the repo if the directory does not exist
		if !File.directory?(name)
			cmd = "#{repoType} clone #{repoUrl}"
			print("Downloading dependecy...".ljust(30).green)
			puts(name)
			puts(cmd)
			executeShellCommand(cmd)

		# Make sure we're up to date with the latest
		else
			Dir.chdir(name)

			case repoType
			when "git"
				cmd = "git pull origin master"
			when "hg"
				cmd = "hg pull -u"
			end

			print("Updating dependency...".ljust(30).green)
			puts(name)
			puts(cmd)
			executeShellCommand(cmd)
		end
	end

	Dir.chdir(rakeDir)
	puts("Done getting source".green)
end

################################################################################
#	Source Building Phase
################################################################################
sdlDyLib = "vendor/SDL/local/lib/libSDL2.dylib"

file sdlDyLib do
	# Build SDL
	sdlDir = vendorDir + "SDL"
	sdlBuildDir = vendorDir + "SDL/local"
	ensureDirExists(sdlBuildDir)

	Dir.chdir(sdlDir)
	executeShellCommand("./configure --prefix=#{sdlBuildDir}")
	executeShellCommand("make && make install")
end

task :buildSource => [sdlDyLib] do
	Dir.chdir(rakeDir)
	puts("Done building source".green)
end

################################################################################
#	Link Building Phase
################################################################################

def relativeSymlink(source, target)
	# Save the workin dir to be restored when we leave the function
	origDir = Dir.pwd

	ensureDirExists(target.dirname)

	# Change to where the link source is and build a relative path to the target
	Dir.chdir(target.dirname)
	linkRelativePath = source.relative_path_from(target.dirname)

	# Setup the symbolic link
	FileUtils.ln_s(linkRelativePath, target, {:noop=>false, :verbose=>true})

	Dir.chdir(origDir)
end

task :setupLinks do
	pkgSpec = loadJsonFile("package.json")
	dependencies = pkgSpec["dependencies"]

	dependencies.each do |name, spec|
		linkSpec = spec["createLink"]
		if linkSpec == nil
			next
		end

		pkgDir = vendorDir + name
		Dir.chdir(rakeDir)

		case linkSpec.class.name
		when "String"
			# This means we want to create a link from the vendor directory
			# to another directory somewhere in the project dir
			linkSource = pkgDir
			linkTarget = rakeDir + linkSpec
			next if linkTarget.exist?

			relativeSymlink(linkSource, linkTarget)
		when "Hash"
			linkSpec.each do |source, target|
				linkSource = pkgDir + source
				linkTarget = rakeDir + target
				relativeSymlink(linkSource, linkTarget)
			end
		end
	end

	Dir.chdir(rakeDir)
	puts("Done setting up links".green)
end

