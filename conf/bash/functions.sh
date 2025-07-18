alias tl='tmux ls'
alias ta='tmux attach -t'

function tn ()
{
    if [ $# -eq 1  ]; then
        tmux new -s $1
    else
        tmux new
    fi
}

function tk ()
{
    echo -n "Are you sure you want me to kill the tmux server? [Y/n] "
    read conf
    if [ "$conf" == "Y" ]; then
        echo "Killing tmux server"
        tmux kill-server
    elif [ "$conf" == "n" ]; then
        echo "Okay then, I will do nothing"
    else
        echo "Unrecognized argument: '$conf'. Doing nothing"
    fi
}


function gl()
{
        line_no=${1:?"Provide line number"}
        file=${2:?"Provide file"}
        awk "NR==$line_no" $file
}

function check_python ()
{
    # If I'm on a NERSC system, make sure the Python module is loaded
    if [ `hostname | egrep -c '(MB14|dhcp)'` -eq 0 ]; then
        local PY=`module list -t 2>&1 | grep -c ^python`
        if [ $PY -eq 0 ]; then
            echo "Loading python module";
            module load python
        fi
    fi
}

alias ca='check_python; conda activate'
alias cl='check_python; conda env list'
alias cr='conda env remove --all --name'
alias alert="printf '\a'"

alias today='date "+%a %b %d, %Y"'

alias seed='python -c "import time; print(int(time.time()*1000) % (2**32-1))";'

alias abspath='realpath'
alias relpath='realpath --relative-to=.'

function last_tmux () {
    local system="perlmutter"
    this_host=`hostname`
    if [[ $this_host == "cori"* ]]; then
        system="cori"
    fi
    last_host=`cat ~/.tmux/resurrect/$system/last_host`
    if [[ $this_host == $last_host ]]; then
        echo "$this_host is the last host, doing nothing"
    else
        ssh $last_host
    fi

}

function install_conda_env_kernel() {
    local ENV=$CONDA_DEFAULT_ENV
    local TARGET_ENV=""

    # Check if we're in base environment
    if [[ "$ENV" == "base" ]]; then
        # Check if command line argument is provided
        if [[ -n "$1" ]]; then
            TARGET_ENV=$1
        else
            echo "Error: You are in the base environment."
            echo "Please either:"
            echo "  1. Activate the conda environment you want to add as a kernel, or"
            echo "  2. Provide the environment name as an argument to this function"
            return 1
        fi
    else
        # We're already in a non-base environment
        TARGET_ENV=$ENV
    fi

    # Verify that the target environment exists
    if conda env list | grep -q " $TARGET_ENV "; then
        echo "Error: Conda environment '$TARGET_ENV' does not exist."
        return 1
    fi

    # Install the kernel
    echo "Installing IPython kernel for conda environment: $TARGET_ENV"
    conda install -n $TARGET_ENV --yes ipykernel
    conda run -n $TARGET_ENV ipython kernel install --user --name="$TARGET_ENV"

    if [[ $? -eq 0 ]]; then
        echo "Successfully installed kernel for $TARGET_ENV environment."
    else
        echo "Failed to install kernel for $TARGET_ENV environment."
        return 1
    fi
}

function new_env() {
    local env_name=${1:?"Missing environment name"};
    conda create -n $env_name -c conda-forge --yes seaborn scikit-learn ipykernel
    conda run -n $env_name ipython kernel install --user --name=$env_name
}


function li ()
{
    local num=${1:?"Missing line number"};
    local path=${2:?"Missing file path"};
    sed -n ${num}p $path
}

function findgrep ()
{
    local dir=${1:?"Missing directory"}
    local wc=${2:?"Missing wildcard pattern"}
    local pat=${3:?"Missing pattern to grep for"}

    find "$dir" -name "$wc" -exec grep --color=auto -Hn -e "$pat" {} +
}

function findpy ()
{
    if [ "$#" == "1" ]; then
        findgrep . "*.py" "$1"
    elif [ "$#" == "2" ]; then
        findgrep "$1" "*.py" "$2"
    else
        echo "Usage: findpy <PATTERN> or findpy <DIRECTORY> <PATTERN>"
    fi
}

function colnames ()
{
    local path=${1:?"Missing CSV"};
    local delim=${2:-","};
    head -n1 $path | tr $delim '\n' | nl
}

function transpos ()
{

    local path=${1:?"Missing file"};
    awk '
    {
        for (i=1; i<=NF; i++)  {
            a[NR,i] = $i
        }
    }
    NF>p { p = NF }
    END {
        for(j=1; j<=p; j++) {
            str=a[1,j]
            for(i=2; i<=NR; i++){
                str=str" "a[i,j];
            }
            print str
        }
    }' $path
}

function npls()
{
    local npz=${1:?"Please provide the path to the NPZ file"};
    python -c 'import numpy; import sys; print("\n".join(numpy.load(sys.argv[1])))' $npz
}
