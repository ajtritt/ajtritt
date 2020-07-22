
cd `dirname $0`

for conf in .gitconfig .inputrc .tmux .tmux.conf .vimrc .vim_bash_env.sh; do
    echo "Copying $conf"
    cp -r $conf $HOME
done

BASH_CONF="$HOME/.bash_config"
if [ ! -d "$BASH_CONF" ]; then
    echo "Making $BASH_CONF"
    mkdir $BASH_CONF
fi

for sh in bash/*.sh; do
    echo "Copying $sh"
    cp $sh $BASH_CONF
done

if [ $# -gt 0 ]; then
    sys_conf=bash/systems/$1.sh
    echo $sys_conf
    if [ -f $sys_conf ]; then
        echo "Copying $sys_conf"
        cp $sys_conf $BASH_CONF
    else
        echo "No system config found for $1"
    fi
fi

tmpfile=$(mktemp /tmp/ajtritt.XXXXXX)
MD5=`command -v md5sum || command -v md5`
line="for sh in $BASH_CONF/*; do source \$sh; done"
PROFILE=$HOME/.bash_profile

expected=`echo $line | $MD5`
received=`head -n 1 $PROFILE | $MD5`

if [ "$expected" != "$received" ]; then
    echo "adding line to $PROFILE"
    echo $line > $tmpfile
    cat $PROFILE >> $tmpfile
    echo "" >> $tmpfile
    cp $tmpfile $PROFILE
    rm $tmpfile
fi

cd -
