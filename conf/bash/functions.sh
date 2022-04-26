alias tl='tmux ls'
alias ta='tmux attach -t'
alias tn='tmux new -s'
alias ca='conda activate'
alias cl='conda env list'
alias cr='conda env remove --all --name'

alias today='date "+%a %b %d, %Y"'

alias seed='python -c "import time; print(int(time.time()*1000) % (2**32-1))";'

alias abspath='realpath'
alias relpath='realpath --relative-to=.'

function li ()
{
    local num=${1:?"Missing line number"};
    local path=${2:?"Missing file path"};
    sed -n ${num}p $path
}

function findgrep ()
{
    local dir=${1:?"Missing directory"};
    local wc=${2:?"Missing wildcard pattern"};
    local pat=${3:?"Missing pattern to grep for"};
    find $dir -name "$wc" -exec grep --color=auto -Hn $pat {} \;
}

function findpy ()
{
    if [ "$#" == "1"  ]; then
        findgrep . "*.py" $1
    elif [ "$#" == "2"  ]; then
        findgrep $1 "*.py" $2
    else
        echo "Usage: findgrep <PATTERN> or findgrep <DIRECTORY> <PATTERN>"
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
