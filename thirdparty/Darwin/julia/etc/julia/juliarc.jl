# This file should contain site-specific commands to be executed on Julia startup
# Users may store their own personal commands in the user home directory `homedir()`, in a file named `.juliarc.jl`

# This file is a part of Julia. License is MIT: https://julialang.org/license

# Set up environment for Julia OSX binary distribution
let
    ROOT = abspath(JULIA_HOME,"..")
    ENV["PATH"]="$JULIA_HOME:$(ENV["PATH"])"
    ENV["FONTCONFIG_PATH"] = joinpath(ROOT, "etc", "fonts")
    ENV["TK_LIBRARY"] = "/System/Library/Frameworks/Tk.framework/Versions/8.5/Resources/Scripts"
end
