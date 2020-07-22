
cd `dirname $0`

cp -r .gitconfig .inputrc .tmux .tmux.conf .vimrc .vim_bash_env.sh $HOME

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

echo "for sh in $BASH_CONF/*; do source \$sh; done" > $tmpfile
cat $HOME/.bash_profile >> $tmpfile
echo "" >> $tmpfile
cp $tmpfile $HOME/.bash_profile
rm $tmpfile

cd -
