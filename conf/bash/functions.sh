alias tl='tmux ls'
alias ta='tmux attach -t'
alias tn='tmux new -s'
alias ca='source activate'

alias today='date "+%a %b %d, %Y"'

function findgrep ()
{
    local dir=${1:?"Missing directory"};
    local wc=${2:?"Missing wildcard pattern"};
    local pat=${3:?"Missing pattern to grep for"};
    find $dir -name "$wc" -exec grep -Hn $pat {} \;
}

function colnames ()
{
    local path=${1:?"Missing CSV"};
    local delim=${2:-","};
    head -n1 $path | tr $delim '\n' | nl
}

function start_jupyter ()
{
    tmux new-session -d -s jupyter -n run
    tmux send-keys -t jupyter:run "cd ~/projects" Enter
    tmux send-keys -t jupyter:run "jupyter lab" Enter
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
