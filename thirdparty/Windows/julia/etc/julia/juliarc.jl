# This file should contain site-specific commands to be executed on Julia startup
# Users should store their own personal commands in homedir(), in a file named .juliarc.jl

# Set up environment for Julia Windows binary distribution
ENV["PATH"] = JULIA_HOME*";"*ENV["PATH"]
