# This file should contain site-specific commands to be executed on Julia startup
# Users may store their own personal commands in the user home directory `homedir()`, in a file named `.juliarc.jl`

# Set up environment for Julia Windows binary distribution
ENV["PATH"] = JULIA_HOME*";"*ENV["PATH"]
