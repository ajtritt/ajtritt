
function new_env() 
{
    local envname=${1:?"Please provide environment name"}
    local pyvers=${2:?"Please provide Python version"}
    shift
    shift
    
    conda create -n $envname python=$pyvers
    conda activate $envname
    conda install -n $envname -c clawpack -c conda-forge hdf5-parallel
    conda install -n $envname -c conda-forge mpich
    conda install -n $envname -c conda-forge mpi4py
    conda install -n $envname -c conda-forge gxx_linux-64
    mkdir $HOME/mytmp
    export TMPDIR=$HOME/mytmp
    export CC=mpicc
    export HDF5_MPI="ON"
    pip install --no-binary=h5py h5py

    for pkg in $@; do
        pip install $pkg
    done
}
