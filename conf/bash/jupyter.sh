

function start_jupyter ()
{
    local LAB="$HOME/.jupyter/lab"
    local log="$LAB/log"
    touch $log
    cd $HOME/projects
    jupyter lab > $log 2>&1 &
}

function kill_jupyter ()
{
    local pid=`ps aux | grep jupyter-lab | grep -v grep | awk '{print $2}'`
    if [ -n "$pid" ]; then
        echo "Killing $pid"
        kill $pid
    else
        echo "No Jupyter process found"
    fi
}

