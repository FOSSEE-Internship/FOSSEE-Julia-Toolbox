if [[ $1 != '' ]]; then 
	echo "Pkg.add(\"$1\")" > tempfile.jl;
	echo "using $1" >> tempfile.jl;
else
	echo "Pkg.update()" > tempfile.jl;
fi
cat tempfile.jl;
thirdparty/$(uname)/julia/bin/julia tempfile.jl;
rm tempfile.jl;