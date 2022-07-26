alias switch='conda deactivate; conda activate'

function new_env() 
{
    local envname=${1:?"Please provide environment name"}
    local pyvers=${2:?"Please provide Python version"}
    shift
    shift

    conda create -y -n $envname python=$pyvers
    conda activate $envname

    install_h5py_parallel $envname

    for pkg in $@; do
        pip install $pkg
    done

    conda deactivate
    echo "Done building conda environment $envname"
}

function install_h5py_parallel()
{
    local envname=${1:?"Please provide environment name"}
    shift

    conda activate $envname
    conda install -y -n $envname -c clawpack -c conda-forge hdf5-parallel
    conda install -y -n $envname -c conda-forge mpich
    conda install -y -n $envname -c conda-forge mpi4py
    conda install -y -n $envname -c conda-forge gxx_linux-64
    CC=mpicc HDF5_MPI="ON" pip install --no-binary=h5py h5py
}
